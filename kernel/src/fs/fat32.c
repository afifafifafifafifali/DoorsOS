#include "fat32.h"
#include "../storage/storage.h"
#include "../gfx/printf.h"
#include "../libs/string.h"
#include "../mem/new/pmm.h"
#include "../gfx/serial_io.h"
static fat32_info_t fs;
static uint8_t sector_buf[512];
static uint32_t current_dir_cluster = 0;

// Full FAT cache - cache entire FAT in memory for speed
static uint8_t* fat_cache = NULL;
static uint32_t fat_cache_size = 0;
static bool fat_cache_dirty = false;

// Free cluster hint - start searching from here for speed
static uint32_t free_cluster_hint = 2;

static uint32_t cluster_to_lba(uint32_t cluster) {
    return fs.start_lba + fs.reserved_sector_count +
           (fs.num_fats * fs.fat_size_sectors) +
           ((cluster - 2) * fs.sectors_per_cluster);
}

static uint32_t get_fat_entry(uint32_t cluster) {
    if (!fat_cache) return 0;
    uint32_t fat_offset = cluster * 4;
    if (fat_offset >= fat_cache_size) return 0;
    return *(uint32_t*)&fat_cache[fat_offset] & 0x0FFFFFFF;
}

static void set_fat_entry(uint32_t cluster, uint32_t value) {
    if (!fat_cache) return;
    uint32_t fat_offset = cluster * 4;
    if (fat_offset >= fat_cache_size) return;
    *(uint32_t*)&fat_cache[fat_offset] = (*(uint32_t*)&fat_cache[fat_offset] & 0xF0000000) | (value & 0x0FFFFFFF);
    fat_cache_dirty = true;
}

static void flush_fat_cache(void) {
    if (!fat_cache || !fat_cache_dirty) return;
    uint32_t fat_start = fs.start_lba + fs.reserved_sector_count;
    uint32_t sectors = (fat_cache_size + 511) / 512;
    // Write each FAT copy in one batch
    for (uint8_t f = 0; f < fs.num_fats; f++) {
        storage_write_sectors(fat_start + (fs.fat_size_sectors * f), sectors, fat_cache);
    }
    fat_cache_dirty = false;
}

static uint32_t allocate_cluster_from(uint32_t start) {
    // Search FAT for free cluster starting from given position
    for (uint32_t offset = start * 4; offset < fat_cache_size; offset += 4) {
        uint32_t cluster = offset / 4;
        if (cluster < 2) continue;

        uint32_t entry = *(uint32_t*)&fat_cache[offset] & 0x0FFFFFFF;
        if (entry == 0) {
            // Mark as EOF (don't flush yet - batch at end)
            *(uint32_t*)&fat_cache[offset] = (*(uint32_t*)&fat_cache[offset] & 0xF0000000) | 0x0FFFFFFF;
            fat_cache_dirty = true;

            // Clear entire cluster at once
            uint32_t lba = cluster_to_lba(cluster);
            uint8_t* zero_buf = k_malloc(fs.sectors_per_cluster * 512);
            if (zero_buf) {
                memset(zero_buf, 0, fs.sectors_per_cluster * 512);
                storage_write_sectors(lba, fs.sectors_per_cluster, zero_buf);
                k_free(zero_buf);
            } else {
                memset(sector_buf, 0, 512);
                for (uint32_t j = 0; j < fs.sectors_per_cluster; j++) {
                    storage_write_sectors(lba + j, 1, sector_buf);
                }
            }

            free_cluster_hint = cluster + 1;  // Update hint for next allocation
            return cluster;
        }
    }
    return 0;
}

static uint32_t allocate_cluster(void) {
    // Try from hint first
    uint32_t cluster = allocate_cluster_from(free_cluster_hint);
    if (cluster) return cluster;
    
    // Wrap around and search from beginning
    free_cluster_hint = 2;
    return allocate_cluster_from(2);
}

// Allocate multiple consecutive clusters at once
static uint32_t allocate_clusters_batch(uint32_t count, uint32_t* first_cluster_out) {
    uint32_t first_cluster = 0;
    uint32_t prev_cluster = 0;
    
    for (uint32_t i = 0; i < count; i++) {
        uint32_t cluster = allocate_cluster();
        if (!cluster) {
            // Cleanup on failure
            if (first_cluster) {
                uint32_t cleanup = first_cluster;
                while (cleanup && cleanup < 0x0FFFFFF8) {
                    uint32_t next = get_fat_entry(cleanup);
                    set_fat_entry(cleanup, 0);
                    cleanup = next;
                }
            }
            return 0;
        }
        
        if (!first_cluster) first_cluster = cluster;
        if (prev_cluster) set_fat_entry(prev_cluster, cluster);
        prev_cluster = cluster;
    }
    
    // Mark last cluster as EOF
    set_fat_entry(prev_cluster, 0x0FFFFFFF);
    *first_cluster_out = first_cluster;
    return first_cluster;
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

    // Allocate and load entire FAT into memory
    fat_cache_size = fs.fat_size_sectors * 512;
    fat_cache = k_malloc(fat_cache_size);
    if (!fat_cache) {
        serial_io_printf("FAT32: failed to allocate FAT cache (%u bytes)\n", fat_cache_size);
        return false;
    }

    // Read first FAT copy
    uint32_t fat_start = fs.start_lba + fs.reserved_sector_count;
    serial_io_printf("FAT32: loading FAT cache from LBA %u (%u sectors)\n", fat_start, fs.fat_size_sectors);
    for (uint32_t i = 0; i < fs.fat_size_sectors; i++) {
        storage_read_sectors(fat_start + i, 1, &fat_cache[i * 512]);
    }
    serial_io_printf("FAT32: FAT cache loaded\n");

    return fs.bytes_per_sector == 512;
}

// Extract characters from LFN entry
static void lfn_extract(uint8_t* entry, char* lfn, int seq) {
    int base = ((seq & 0x1F) - 1) * 13;
    static const int offsets[] = {1,3,5,7,9, 14,16,18,20,22,24, 28,30};
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

    static const int offsets[] = {1,3,5,7,9, 14,16,18,20,22,24, 28,30};
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

// Resolve path components: separate directory path and filename
// Returns true if successful, sets dir_cluster and filename
bool resolve_path_components(const char* path, uint32_t* dir_cluster, char* filename) {
    if (!path || !dir_cluster || !filename) return false;

    char pathcopy[256];
    strncpy(pathcopy, path, 255);
    pathcopy[255] = 0;

    // Determine starting cluster
    uint32_t cluster = (path[0] == '/') ? fs.root_cluster : fat32_get_cwd_cluster();

    // Find the last '/' to separate directory and filename
    char* last_slash = strrchr(pathcopy, '/');
    char* dir_path;
    char* fname;

    if (last_slash) {
        // Path has directory component
        *last_slash = 0;  // Split the string
        dir_path = pathcopy;
        fname = last_slash + 1;

        // If path started with '/', dir_path might be empty or start with /
        if (path[0] == '/' && strlen(dir_path) == 0) {
            cluster = fs.root_cluster;
        } else {
            // Resolve directory path
            cluster = fat32_path_to_cluster(dir_path);
            if (!cluster) return false;
        }
    } else {
        // No directory component, just filename in current dir
        fname = pathcopy;
    }

    // Check if filename is empty (path ended with /)
    if (strlen(fname) == 0) return false;

    *dir_cluster = cluster;
    strcpy(filename, fname);
    return true;
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






// Read file from specific directory cluster
static bool fat32_read_file_in_dir(uint32_t dir_cluster, const char* filename, void* buffer, uint32_t* size) {
    uint32_t lba = cluster_to_lba(dir_cluster);
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

                // Optimized: read entire clusters at once
                uint32_t bytes_remaining = *size;
                uint8_t* buf_ptr = (uint8_t*)buffer;
                uint32_t cluster_lba = cluster_to_lba(cluster);
                uint32_t read_count = 0;

                serial_io_printf("FAT32 read: %u bytes, %u sectors/cluster\n", *size, fs.sectors_per_cluster);

                while (cluster >= 2 && cluster < 0x0FFFFFF8 && bytes_remaining > 0) {
                    uint32_t cluster_size = fs.sectors_per_cluster * 512;
                    uint32_t to_read = (bytes_remaining < cluster_size) ? bytes_remaining : cluster_size;
                    uint32_t sectors = (to_read + 511) / 512;

                    if (!storage_read_sectors(cluster_lba, sectors, buf_ptr)) {
                        serial_io_printf("FAT32 read: failed at cluster %u, LBA %u\n", cluster, cluster_lba);
                        return false;
                    }
                    read_count++;

                    buf_ptr += to_read;
                    bytes_remaining -= to_read;

                    cluster = get_fat_entry(cluster);
                    if (cluster < 0x0FFFFFF8) cluster_lba = cluster_to_lba(cluster);
                }

                serial_io_printf("FAT32 read: completed %u read operations\n", read_count);
                return true;
            }
        }
    }

    return false;
}

bool fat32_read_file(const char* filename, void* buffer, uint32_t* size) {
    uint32_t dir_cluster;
    char fname[256];

    if (!resolve_path_components(filename, &dir_cluster, fname)) {
        return false;
    }

    return fat32_read_file_in_dir(dir_cluster, fname, buffer, size);
}


// Write file to specific directory cluster
static bool fat32_write_file_in_dir(uint32_t dir_cluster, const char* filename, const uint8_t* data, uint32_t size) {
    if (size == 0) return false;

    // Calculate clusters needed
    uint32_t bytes_per_cluster = fs.sectors_per_cluster * 512;
    uint32_t clusters_needed = (size + bytes_per_cluster - 1) / bytes_per_cluster;

    serial_io_printf("FAT32 write: %u bytes, %u sectors/cluster, %u clusters\n", 
                     size, fs.sectors_per_cluster, clusters_needed);

    // Allocate all clusters at once and build FAT chain
    uint32_t first_cluster = 0;
    if (!allocate_clusters_batch(clusters_needed, &first_cluster)) {
        serial_io_printf("FAT32 write: cluster allocation failed\n");
        return false;
    }

    // Write all data clusters - batch consecutive sectors
    uint32_t cluster = first_cluster;
    uint32_t bytes_remaining = size;
    const uint8_t* data_ptr = data;
    uint32_t write_count = 0;

    while (cluster && cluster < 0x0FFFFFF8 && bytes_remaining > 0) {
        uint32_t cluster_lba = cluster_to_lba(cluster);
        uint32_t to_write = (bytes_remaining < bytes_per_cluster) ? bytes_remaining : bytes_per_cluster;
        uint32_t sectors = (to_write + 511) / 512;

        // Write entire cluster in one call
        if (!storage_write_sectors(cluster_lba, sectors, data_ptr)) {
            serial_io_printf("FAT32 write: storage_write failed at cluster %u, LBA %u\n", cluster, cluster_lba);
            return false;
        }
        write_count++;

        data_ptr += to_write;
        bytes_remaining -= to_write;

        cluster = get_fat_entry(cluster);
        if (cluster >= 0x0FFFFFF8) break;
        cluster_lba = cluster_to_lba(cluster);
    }

    serial_io_printf("FAT32 write: completed %u write operations\n", write_count);

    // Generate short name and check if LFN needed
    uint8_t shortname[11];
    generate_short_name(filename, shortname);
    uint8_t checksum = lfn_checksum(shortname);

    int namelen = strlen(filename);
    int lfn_entries = (namelen + 12) / 13;
    bool needs_lfn = (namelen > 11);
    for (int i = 0; filename[i] && !needs_lfn; i++) {
        if (filename[i] >= 'a' && filename[i] <= 'z') needs_lfn = true;
        if (filename[i] == ' ') needs_lfn = true;
    }
    if (!needs_lfn) lfn_entries = 0;

    int entries_needed = lfn_entries + 1;

    // Find consecutive free entries in directory (scan all sectors in cluster chain)
    int free_start = -1;
    int free_count = 0;
    uint32_t write_cluster = 0;  // Cluster where we found space
    uint32_t search_cluster = dir_cluster;

    // Scan all clusters in directory chain
    while (search_cluster && search_cluster < 0x0FFFFFF8) {
        uint32_t cluster_lba = cluster_to_lba(search_cluster);
        for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
            if (!storage_read_sectors(cluster_lba + sec, 1, sector_buf)) break;

            for (int i = 0; i < 16; i++) {
                uint8_t* entry = &sector_buf[i * 32];
                if (entry[0] == 0 || entry[0] == 0xE5) {
                    if (free_start < 0) {
                        free_start = i;
                        write_cluster = search_cluster;
                    }
                    free_count++;
                    if (free_count >= entries_needed) goto found_space;
                } else {
                    free_start = -1;
                    free_count = 0;
                }
            }
            // Reset at end of sector - need consecutive entries within same sector
            free_start = -1;
            free_count = 0;
        }
        // Move to next cluster in chain
        search_cluster = get_fat_entry(search_cluster);
    }

    // No space found - expand directory by adding a new cluster
    uint32_t new_cluster = allocate_cluster();
    if (!new_cluster) {
        serial_io_printf("FAT32 write: failed to allocate directory cluster\n");
        return false;
    }

    // Find last cluster in directory chain and link new cluster
    uint32_t last_cluster = dir_cluster;
    while (last_cluster && last_cluster < 0x0FFFFFF8) {
        uint32_t next = get_fat_entry(last_cluster);
        if (next >= 0x0FFFFFF8) break;
        last_cluster = next;
    }
    set_fat_entry(last_cluster, new_cluster);
    set_fat_entry(new_cluster, 0x0FFFFFFF);

    // Zero out new cluster
    uint32_t new_cluster_lba = cluster_to_lba(new_cluster);
    memset(sector_buf, 0, 512);
    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        storage_write_sectors(new_cluster_lba + sec, 1, sector_buf);
    }

    // Use first entries in new cluster
    free_start = 0;
    write_cluster = new_cluster;

found_space:
    // Read the sector (sector 0) of the cluster where we found space
    storage_read_sectors(cluster_to_lba(write_cluster), 1, sector_buf);

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
    *(uint16_t*)&entry[26] = first_cluster & 0xFFFF;
    *(uint16_t*)&entry[20] = first_cluster >> 16;
    *(uint32_t*)&entry[28] = size;

    storage_write_sectors(cluster_to_lba(write_cluster), 1, sector_buf);

    // Flush FAT cache once after all allocations
    flush_fat_cache();

    return true;
}

bool fat32_write_file(const char* filename, const uint8_t* data, uint32_t size) {
    uint32_t dir_cluster;
    char fname[256];

    if (!resolve_path_components(filename, &dir_cluster, fname)) {
        return false;
    }

    // Delete existing file first (allows overwrite)
    // TODO: Implement delete in specific directory
    fat32_delete_file(filename);

    return fat32_write_file_in_dir(dir_cluster, fname, data, size);
}

// Delete file from specific directory cluster
static bool fat32_delete_file_in_dir(uint32_t dir_cluster, const char* filename) {
    uint32_t lba = cluster_to_lba(dir_cluster);
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

bool fat32_delete_file(const char* filename) {
    uint32_t dir_cluster;
    char fname[256];

    if (!resolve_path_components(filename, &dir_cluster, fname)) {
        return false;
    }

    return fat32_delete_file_in_dir(dir_cluster, fname);
}

// Create directory in specific parent cluster
static bool fat32_create_directory_in_parent(uint32_t parent_cluster, const char* dirname) {
    uint32_t cluster = allocate_cluster();
    if (!cluster) return false;

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

    // Add to parent directory
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

    flush_fat_cache();
    return storage_write_sectors(dir_lba, 1, sector_buf);
}

bool fat32_create_directory(const char* dirname) {
    // Handle full paths with intermediate directory creation
    char pathcopy[256];
    strncpy(pathcopy, dirname, 255);
    pathcopy[255] = 0;

    uint32_t current_cluster = (pathcopy[0] == '/') ? fs.root_cluster : fat32_get_cwd_cluster();

    char* token = pathcopy;
    if (*token == '/') token++;

    while (*token) {
        char* next = strchr(token, '/');
        if (next) *next = 0;

        if (strlen(token) > 0) {
            // Check if directory exists
            uint32_t existing = find_dir_cluster(current_cluster, token);
            if (existing) {
                current_cluster = existing;
            } else {
                // Create it
                if (!fat32_create_directory_in_parent(current_cluster, token)) {
                    return false;
                }
                // Find the newly created cluster
                existing = find_dir_cluster(current_cluster, token);
                if (!existing) return false;
                current_cluster = existing;
            }
        }

        if (next) token = next + 1;
        else break;
    }

    return true;
}

bool fat32_delete_directory(const char* path) {
    uint32_t cluster = fat32_path_to_cluster(path);
    if (!cluster) return false;

    // Check if directory is empty (only . and .. entries)
    uint32_t lba = cluster_to_lba(cluster);
    if (!storage_read_sectors(lba, 1, sector_buf)) return false;

    bool empty = true;
    for (int i = 2; i < 16; i++) {  // Skip . and ..
        uint8_t* entry = &sector_buf[i * 32];
        if (entry[0] != 0 && entry[0] != 0xE5) {
            empty = false;
            break;
        }
    }

    if (!empty) return false;  // Directory not empty

    // Find and delete directory entry from parent
    char pathcopy[256];
    strncpy(pathcopy, path, 255);
    pathcopy[255] = 0;

    char* last_slash = strrchr(pathcopy, '/');
    uint32_t parent_cluster;
    char dirname[256];

    if (last_slash) {
        *last_slash = 0;
        parent_cluster = fat32_path_to_cluster(pathcopy[0] ? pathcopy : "/");
        strcpy(dirname, last_slash + 1);
    } else {
        parent_cluster = fat32_get_cwd_cluster();
        strcpy(dirname, pathcopy);
    }

    if (!parent_cluster) return false;

    // Delete entry from parent directory
    uint32_t parent_lba = cluster_to_lba(parent_cluster);
    char lfn[256];
    memset(lfn, 0, 256);
    int lfn_start = -1;

    for (uint32_t sec = 0; sec < fs.sectors_per_cluster; sec++) {
        if (!storage_read_sectors(parent_lba + sec, 1, sector_buf)) return false;

        for (int i = 0; i < 16; i++) {
            uint8_t* entry = &sector_buf[i * 32];
            if (entry[0] == 0) return false;
            if (entry[0] == 0xE5) continue;

            // LFN entry
            if (entry[11] == 0x0F) {
                if (entry[0] & 0x40) lfn_start = i;
                lfn_extract(entry, lfn, entry[0]);
                continue;
            }

            // Get name
            char name[256];
            if (lfn[0]) {
                strcpy(name, lfn);
            } else {
                int pos = 0;
                for (int j = 0; j < 8 && entry[j] != ' '; j++) {
                    name[pos++] = entry[j];
                }
                name[pos] = 0;
                lfn_start = i;
            }

            if (strcasecmp_local(name, dirname) == 0 && (entry[11] & 0x10)) {
                // Mark entries as deleted
                for (int j = lfn_start; j <= i; j++) {
                    sector_buf[j * 32] = 0xE5;
                }
                storage_write_sectors(parent_lba + sec, 1, sector_buf);
                return true;
            }

            memset(lfn, 0, 256);
            lfn_start = -1;
        }
    }

    return false;
}