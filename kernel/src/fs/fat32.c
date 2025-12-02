#include "fat32.h"
#include "../storage/storage.h"
#include "../gfx/printf.h"
#include "../libs/string.h"
#include "../mem/new/pmm.h"

static fat32_info_t fs;
static uint8_t sector_buf[512];
static uint32_t current_dir_cluster = 0;  // Current working directory cluster

static uint32_t cluster_to_lba(uint32_t cluster) {
    return fs.start_lba + fs.reserved_sector_count + 
           (fs.num_fats * fs.fat_size_sectors) + 
           ((cluster - 2) * fs.sectors_per_cluster);
}

static uint32_t get_fat_entry(uint32_t cluster) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fs.start_lba + fs.reserved_sector_count + (fat_offset / 512);
    uint32_t entry_offset = fat_offset % 512;
    
    storage_read_sectors(fat_sector, 1, sector_buf);
    return *(uint32_t*)&sector_buf[entry_offset] & 0x0FFFFFFF;
}

static void set_fat_entry(uint32_t cluster, uint32_t value) {
    uint32_t fat_offset = cluster * 4;
    uint32_t fat_sector = fs.start_lba + fs.reserved_sector_count + (fat_offset / 512);
    uint32_t entry_offset = fat_offset % 512;
    
    storage_read_sectors(fat_sector, 1, sector_buf);
    *(uint32_t*)&sector_buf[entry_offset] = (*(uint32_t*)&sector_buf[entry_offset] & 0xF0000000) | (value & 0x0FFFFFFF);
    storage_write_sectors(fat_sector, 1, sector_buf);
}

static uint32_t allocate_cluster(void) {
    uint8_t fat_buf[512];
    uint32_t fat_start = fs.start_lba + fs.reserved_sector_count;
    
    // Search first 128 FAT sectors (16384 clusters)
    for (uint32_t sector = 0; sector < 128; sector++) {
        storage_read_sectors(fat_start + sector, 1, fat_buf);
        
        for (uint32_t offset = 0; offset < 512; offset += 4) {
            uint32_t cluster = (sector * 512 + offset) / 4;
            if (cluster < 2) continue;
            
            uint32_t entry = *(uint32_t*)&fat_buf[offset] & 0x0FFFFFFF;
            if (entry == 0) {
                // Mark as EOF
                *(uint32_t*)&fat_buf[offset] = (*(uint32_t*)&fat_buf[offset] & 0xF0000000) | 0x0FFFFFFF;
                storage_write_sectors(fat_start + sector, 1, fat_buf);
                
                // Clear cluster
                uint32_t lba = cluster_to_lba(cluster);
                memset(sector_buf, 0, 512);
                for (uint32_t j = 0; j < fs.sectors_per_cluster; j++) {
                    storage_write_sectors(lba + j, 1, sector_buf);
                }
                
                return cluster;
            }
        }
    }
    return 0;
}

bool fat32_mount(uint32_t lba, bool ahci) {
    (void)ahci;
    
    serial_io_printf("fat32_mount: reading LBA %u\n", lba);
    if (!storage_read_sectors(lba, 1, sector_buf)) {
        serial_io_printf("fat32_mount: read failed\n");
        return false;
    }
    serial_io_printf("fat32_mount: read OK\n");
    
    fs.start_lba = lba;
    fs.bytes_per_sector = *(uint16_t*)&sector_buf[11];
    fs.sectors_per_cluster = sector_buf[13];
    fs.reserved_sector_count = *(uint16_t*)&sector_buf[14];
    fs.num_fats = sector_buf[16];
    fs.fat_size_sectors = *(uint32_t*)&sector_buf[36];
    fs.root_cluster = *(uint32_t*)&sector_buf[44];
    
    serial_io_printf("FAT32: bps=%u spc=%u rsc=%u nf=%u fss=%u rc=%u\n",
        fs.bytes_per_sector, fs.sectors_per_cluster, fs.reserved_sector_count,
        fs.num_fats, fs.fat_size_sectors, fs.root_cluster);
    serial_io_printf("FAT32: first bytes: %02x %02x %02x\n", sector_buf[0], sector_buf[1], sector_buf[2]);
    
    return fs.bytes_per_sector == 512;
}

// Extract characters from LFN entry
static void lfn_extract(uint8_t* entry, char* lfn, int seq) {
    int base = ((seq & 0x1F) - 1) * 13;
    int offsets[] = {1,3,5,7,9, 14,16,18,20,22,24, 28,30};
    for (int i = 0; i < 13; i++) {
        uint16_t c = *(uint16_t*)&entry[offsets[i]];
        if (c == 0 || c == 0xFFFF) break;
        if (base + i < 255) lfn[base + i] = (char)(c & 0x7F);
    }
}

// Calculate LFN checksum from 8.3 name
static uint8_t lfn_checksum(uint8_t* shortname) {
    uint8_t sum = 0;
    for (int i = 0; i < 11; i++) {
        sum = ((sum & 1) ? 0x80 : 0) + (sum >> 1) + shortname[i];
    }
    return sum;
}

// Create LFN entry
static void lfn_create_entry(uint8_t* entry, const char* name, int seq, uint8_t checksum, bool last) {
    memset(entry, 0, 32);
    entry[0] = seq | (last ? 0x40 : 0);
    entry[11] = 0x0F;  // LFN attribute
    entry[13] = checksum;
    
    int offsets[] = {1,3,5,7,9, 14,16,18,20,22,24, 28,30};
    int base = (seq - 1) * 13;
    int namelen = strlen(name);
    
    for (int i = 0; i < 13; i++) {
        int idx = base + i;
        uint16_t c;
        if (idx < namelen) {
            c = (uint16_t)(uint8_t)name[idx];
        } else if (idx == namelen) {
            c = 0;
        } else {
            c = 0xFFFF;
        }
        *(uint16_t*)&entry[offsets[i]] = c;
    }
}

// Generate 8.3 short name from long name
static void generate_short_name(const char* longname, uint8_t* shortname) {
    memset(shortname, ' ', 11);
    
    const char* dot = strrchr(longname, '.');
    int baselen = dot ? (dot - longname) : strlen(longname);
    if (baselen > 8) baselen = 6;  // Leave room for ~1
    
    int j = 0;
    for (int i = 0; i < baselen && j < 8; i++) {
        char c = longname[i];
        if (c == ' ' || c == '.') continue;
        shortname[j++] = k_toupper(c);
    }
    
    // Add ~1 if name was truncated or has lowercase
    bool needs_lfn = (strlen(longname) > 11) || dot;
    for (int i = 0; longname[i] && !needs_lfn; i++) {
        if (longname[i] >= 'a' && longname[i] <= 'z') needs_lfn = true;
    }
    if (needs_lfn && j <= 6) {
        shortname[j++] = '~';
        shortname[j++] = '1';
    }
    
    // Extension
    if (dot) {
        dot++;
        for (int i = 0; i < 3 && dot[i]; i++) {
            shortname[8 + i] = k_toupper(dot[i]);
        }
    }
}

void fat32_list_root(void) {
    uint32_t lba = cluster_to_lba(fs.root_cluster);
    char lfn[256];
    memset(lfn, 0, 256);
    
    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        if (!storage_read_sectors(lba + sec, 1, sector_buf)) break;
        
        for (int i = 0; i < 16; i++) {
            uint8_t* entry = &sector_buf[i * 32];
            if (entry[0] == 0) return;
            if (entry[0] == 0xE5) continue;
            
            // LFN entry
            if (entry[11] == 0x0F) {
                lfn_extract(entry, lfn, entry[0]);
                continue;
            }
            
            // Regular entry - use LFN if available, else 8.3
            char name[256];
            if (lfn[0]) {
                strcpy(name, lfn);
                memset(lfn, 0, 256);
            } else {
                int pos = 0;
                for (int j = 0; j < 8 && entry[j] != ' '; j++) {
                    name[pos++] = entry[j];
                }
                if (entry[8] != ' ') {
                    name[pos++] = '.';
                    for (int j = 8; j < 11 && entry[j] != ' '; j++) {
                        name[pos++] = entry[j];
                    }
                }
                name[pos] = 0;
            }
            
            uint32_t size = *(uint32_t*)&entry[28];
            char type = (entry[11] & 0x10) ? 'D' : 'F';
            
            printf("[%c] %-30s %10u bytes\n", type, name, size);
        }
    }
}

// Find directory cluster by name in given parent cluster
// Case-insensitive string compare
static int strcasecmp_local(const char* a, const char* b) {
    while (*a && *b) {
        char ca = (*a >= 'A' && *a <= 'Z') ? *a + 32 : *a;
        char cb = (*b >= 'A' && *b <= 'Z') ? *b + 32 : *b;
        if (ca != cb) return ca - cb;
        a++; b++;
    }
    return *a - *b;
}

static uint32_t find_dir_cluster(uint32_t parent_cluster, const char* dirname) {
    uint32_t lba = cluster_to_lba(parent_cluster);
    char lfn[256];
    memset(lfn, 0, 256);
    
    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        if (!storage_read_sectors(lba + sec, 1, sector_buf)) return 0;
        
        for (int i = 0; i < 16; i++) {
            uint8_t* entry = &sector_buf[i * 32];
            if (entry[0] == 0) return 0;
            if (entry[0] == 0xE5) continue;
            
            // LFN entry
            if (entry[11] == 0x0F) {
                lfn_extract(entry, lfn, entry[0]);
                continue;
            }
            
            if (!(entry[11] & 0x10)) {
                memset(lfn, 0, 256);  // Reset LFN for non-dirs
                continue;
            }
            
            // Get name (LFN or 8.3)
            char name[256];
            if (lfn[0]) {
                strcpy(name, lfn);
                memset(lfn, 0, 256);
            } else {
                int pos = 0;
                for (int j = 0; j < 8 && entry[j] != ' '; j++) {
                    name[pos++] = entry[j];
                }
                name[pos] = 0;
            }
            
            if (strcasecmp_local(name, dirname) == 0) {
                uint32_t cluster = (*(uint16_t*)&entry[20] << 16) | *(uint16_t*)&entry[26];
                return cluster;
            }
        }
    }
    return 0;
}

// Resolve path to cluster (e.g., "/boot/limine" -> cluster)
uint32_t fat32_path_to_cluster(const char* path) {
    if (!path || path[0] == 0 || strcmp(path, "/") == 0) {
        return fs.root_cluster;
    }
    
    uint32_t cluster = fs.root_cluster;
    char pathcopy[256];
    strncpy(pathcopy, path, 255);
    pathcopy[255] = 0;
    
    char* token = pathcopy;
    if (*token == '/') token++;
    
    while (*token) {
        char* next = strchr(token, '/');
        if (next) *next = 0;
        
        if (strlen(token) > 0) {
            cluster = find_dir_cluster(cluster, token);
            if (!cluster) return 0;
        }
        
        if (next) token = next + 1;
        else break;
    }
    return cluster;
}

bool fat32_dir_exists(const char* path) {
    return fat32_path_to_cluster(path) != 0;
}

void fat32_list_dir(const char* path) {
    uint32_t cluster = fat32_path_to_cluster(path);
    if (!cluster) {
        printf("Directory not found: %s\n", path);
        return;
    }
    
    uint32_t lba = cluster_to_lba(cluster);
    char lfn[256];
    memset(lfn, 0, 256);
    
    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        if (!storage_read_sectors(lba + sec, 1, sector_buf)) break;
        
        for (int i = 0; i < 16; i++) {
            uint8_t* entry = &sector_buf[i * 32];
            if (entry[0] == 0) return;
            if (entry[0] == 0xE5) continue;
            
            if (entry[11] == 0x0F) {
                lfn_extract(entry, lfn, entry[0]);
                continue;
            }
            
            char name[256];
            if (lfn[0]) {
                strcpy(name, lfn);
                memset(lfn, 0, 256);
            } else {
                int pos = 0;
                for (int j = 0; j < 8 && entry[j] != ' '; j++) {
                    name[pos++] = entry[j];
                }
                if (entry[8] != ' ') {
                    name[pos++] = '.';
                    for (int j = 8; j < 11 && entry[j] != ' '; j++) {
                        name[pos++] = entry[j];
                    }
                }
                name[pos] = 0;
            }
            
            uint32_t size = *(uint32_t*)&entry[28];
            char type = (entry[11] & 0x10) ? 'D' : 'F';
            
            printf("[%c] %-30s %10u bytes\n", type, name, size);
        }
    }
}

void fat32_set_cwd(const char* path) {
    uint32_t cluster = fat32_path_to_cluster(path);
    if (cluster) {
        current_dir_cluster = cluster;
    }
}

uint32_t fat32_get_cwd_cluster(void) {
    return current_dir_cluster ? current_dir_cluster : fs.root_cluster;
}

bool fat32_read_file_simple(const char* filename, void* buffer, uint32_t* size) {
    uint32_t lba = cluster_to_lba(fat32_get_cwd_cluster());
    char lfn[256];
    memset(lfn, 0, 256);
    
    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        if (!storage_read_sectors(lba + sec, 1, sector_buf)) return false;
        
        for (int i = 0; i < 16; i++) {
            uint8_t* entry = &sector_buf[i * 32];
            if (entry[0] == 0) return false;
            if (entry[0] == 0xE5) continue;
            
            // LFN entry
            if (entry[11] == 0x0F) {
                lfn_extract(entry, lfn, entry[0]);
                continue;
            }
            
            if (entry[11] & 0x10) {
                memset(lfn, 0, 256);  // Reset for dirs
                continue;
            }
            
            // Get name (LFN or 8.3)
            char name[256];
            if (lfn[0]) {
                strcpy(name, lfn);
                memset(lfn, 0, 256);
            } else {
                int pos = 0;
                for (int j = 0; j < 8 && entry[j] != ' '; j++) {
                    name[pos++] = entry[j];
                }
                if (entry[8] != ' ') {
                    name[pos++] = '.';
                    for (int j = 8; j < 11 && entry[j] != ' '; j++) {
                        name[pos++] = entry[j];
                    }
                }
                name[pos] = 0;
            }
            
            if (strcasecmp_local(name, filename) == 0) {
                uint32_t cluster = *(uint16_t*)&entry[26] | (*(uint16_t*)&entry[20] << 16);
                *size = *(uint32_t*)&entry[28];
                
                uint32_t file_lba = cluster_to_lba(cluster);
                uint32_t sectors = (*size + 511) / 512;
                
                for (uint32_t s = 0; s < sectors && s < 8; s++) {
                    storage_read_sectors(file_lba + s, 1, (uint8_t*)buffer + s * 512);
                }
                
                return true;
            }
        }
    }
    
    return false;
}

bool fat32_read_file(const char* filename, void* buffer, uint32_t* size) {
    return fat32_read_file_simple(filename, buffer, size);
}


bool fat32_write_file(const char* filename, const uint8_t* data, uint32_t size) {
    // Delete existing file first (allows overwrite)
    fat32_delete_file(filename);
    
    uint32_t cluster = allocate_cluster();
    if (!cluster) return false;
    
    // Write data
    uint32_t lba = cluster_to_lba(cluster);
    uint32_t sectors = (size + 511) / 512;
    
    for (uint32_t i = 0; i < sectors; i++) {
        memset(sector_buf, 0, 512);
        uint32_t copy_size = (size - i * 512 > 512) ? 512 : size - i * 512;
        memcpy(sector_buf, &data[i * 512], copy_size);
        storage_write_sectors(lba + i, 1, sector_buf);
    }
    
    // Generate short name and check if LFN needed
    uint8_t shortname[11];
    generate_short_name(filename, shortname);
    uint8_t checksum = lfn_checksum(shortname);
    
    int namelen = strlen(filename);
    int lfn_entries = (namelen + 12) / 13;  // Number of LFN entries needed
    bool needs_lfn = (namelen > 11);
    for (int i = 0; filename[i] && !needs_lfn; i++) {
        if (filename[i] >= 'a' && filename[i] <= 'z') needs_lfn = true;
        if (filename[i] == ' ') needs_lfn = true;
    }
    if (!needs_lfn) lfn_entries = 0;
    
    int entries_needed = lfn_entries + 1;  // LFN entries + 1 short entry
    
    // Find consecutive free entries in directory
    uint32_t dir_lba = cluster_to_lba(fat32_get_cwd_cluster());
    storage_read_sectors(dir_lba, 1, sector_buf);
    
    int free_start = -1;
    int free_count = 0;
    
    for (int i = 0; i < 16; i++) {
        uint8_t* entry = &sector_buf[i * 32];
        if (entry[0] == 0 || entry[0] == 0xE5) {
            if (free_start < 0) free_start = i;
            free_count++;
            if (free_count >= entries_needed) break;
        } else {
            free_start = -1;
            free_count = 0;
        }
    }
    
    if (free_count < entries_needed) return false;
    
    // Write LFN entries (in reverse order)
    for (int seq = lfn_entries; seq >= 1; seq--) {
        int idx = free_start + (lfn_entries - seq);
        lfn_create_entry(&sector_buf[idx * 32], filename, seq, checksum, seq == lfn_entries);
    }
    
    // Write short entry
    uint8_t* entry = &sector_buf[(free_start + lfn_entries) * 32];
    memset(entry, 0, 32);
    memcpy(entry, shortname, 11);
    entry[11] = 0x20;
    *(uint16_t*)&entry[26] = cluster & 0xFFFF;
    *(uint16_t*)&entry[20] = cluster >> 16;
    *(uint32_t*)&entry[28] = size;
    
    storage_write_sectors(dir_lba, 1, sector_buf);
    return true;
}

bool fat32_delete_file(const char* filename) {
    uint32_t lba = cluster_to_lba(fat32_get_cwd_cluster());
    char lfn[256];
    memset(lfn, 0, 256);
    int lfn_start = -1;
    
    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        if (!storage_read_sectors(lba + sec, 1, sector_buf)) return false;
        
        for (int i = 0; i < 16; i++) {
            uint8_t* entry = &sector_buf[i * 32];
            if (entry[0] == 0) return false;
            if (entry[0] == 0xE5) continue;
            
            // LFN entry
            if (entry[11] == 0x0F) {
                if (entry[0] & 0x40) lfn_start = i;  // First LFN entry
                lfn_extract(entry, lfn, entry[0]);
                continue;
            }
            
            // Get name (LFN or 8.3)
            char name[256];
            if (lfn[0]) {
                strcpy(name, lfn);
            } else {
                int pos = 0;
                for (int j = 0; j < 8 && entry[j] != ' '; j++) {
                    name[pos++] = entry[j];
                }
                if (entry[8] != ' ') {
                    name[pos++] = '.';
                    for (int j = 8; j < 11 && entry[j] != ' '; j++) {
                        name[pos++] = entry[j];
                    }
                }
                name[pos] = 0;
                lfn_start = i;  // No LFN, just this entry
            }
            
            if (strcasecmp_local(name, filename) == 0) {
                // Delete all entries from lfn_start to i
                for (int j = lfn_start; j <= i; j++) {
                    sector_buf[j * 32] = 0xE5;
                }
                storage_write_sectors(lba + sec, 1, sector_buf);
                return true;
            }
            
            memset(lfn, 0, 256);
            lfn_start = -1;
        }
    }
    
    return false;
}

bool fat32_create_directory(const char* dirname) {
    uint32_t cluster = allocate_cluster();
    if (!cluster) return false;
    
    uint32_t parent_cluster = fat32_get_cwd_cluster();
    
    // Create . and .. entries
    uint32_t lba = cluster_to_lba(cluster);
    memset(sector_buf, 0, 512);
    
    uint8_t* dot = &sector_buf[0];
    memset(dot, ' ', 11);
    dot[0] = '.';
    dot[11] = 0x10;
    *(uint16_t*)&dot[26] = cluster & 0xFFFF;
    *(uint16_t*)&dot[20] = cluster >> 16;
    
    uint8_t* dotdot = &sector_buf[32];
    memset(dotdot, ' ', 11);
    dotdot[0] = '.';
    dotdot[1] = '.';
    dotdot[11] = 0x10;
    *(uint16_t*)&dotdot[26] = parent_cluster & 0xFFFF;
    *(uint16_t*)&dotdot[20] = parent_cluster >> 16;
    
    if (!storage_write_sectors(lba, 1, sector_buf)) return false;
    
    // Generate short name and LFN entries
    uint8_t shortname[11];
    generate_short_name(dirname, shortname);
    uint8_t checksum = lfn_checksum(shortname);
    
    int namelen = strlen(dirname);
    int lfn_entries = (namelen + 12) / 13;
    bool needs_lfn = (namelen > 8);  // Dirs don't have extensions typically
    for (int i = 0; dirname[i] && !needs_lfn; i++) {
        if (dirname[i] >= 'a' && dirname[i] <= 'z') needs_lfn = true;
        if (dirname[i] == ' ') needs_lfn = true;
    }
    if (!needs_lfn) lfn_entries = 0;
    
    int entries_needed = lfn_entries + 1;
    
    // Add to current directory
    uint32_t dir_lba = cluster_to_lba(parent_cluster);
    if (!storage_read_sectors(dir_lba, 1, sector_buf)) return false;
    
    int free_start = -1;
    int free_count = 0;
    
    for (int i = 0; i < 16; i++) {
        uint8_t* entry = &sector_buf[i * 32];
        if (entry[0] == 0 || entry[0] == 0xE5) {
            if (free_start < 0) free_start = i;
            free_count++;
            if (free_count >= entries_needed) break;
        } else {
            free_start = -1;
            free_count = 0;
        }
    }
    
    if (free_count < entries_needed) return false;
    
    // Write LFN entries (reverse order)
    for (int seq = lfn_entries; seq >= 1; seq--) {
        int idx = free_start + (lfn_entries - seq);
        lfn_create_entry(&sector_buf[idx * 32], dirname, seq, checksum, seq == lfn_entries);
    }
    
    // Write short entry
    uint8_t* entry = &sector_buf[(free_start + lfn_entries) * 32];
    memset(entry, 0, 32);
    memcpy(entry, shortname, 11);
    entry[11] = 0x10;  // Directory attribute
    *(uint16_t*)&entry[26] = cluster & 0xFFFF;
    *(uint16_t*)&entry[20] = cluster >> 16;
    
    return storage_write_sectors(dir_lba, 1, sector_buf);
}
