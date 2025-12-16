#ifndef FAT32_H
#define FAT32_H

#include <stdint.h>
#include <stdbool.h>
#include "../libs/string.h"

typedef struct {
    uint32_t start_lba;
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sector_count;
    uint8_t num_fats;
    uint32_t fat_size_sectors;
    uint32_t root_cluster;
} fat32_info_t;

bool fat32_mount(uint32_t lba, bool ahci);
void fat32_list_root(void);
void fat32_list_dir(const char* path);
bool fat32_dir_exists(const char* path);
uint32_t fat32_path_to_cluster(const char* path);
void fat32_set_cwd(const char* path);
uint32_t fat32_get_cwd_cluster(void);
bool fat32_read_file(const char* filename, void* buffer, uint32_t* size);
bool fat32_write_file(const char* filename, const uint8_t* data, uint32_t size);
bool fat32_delete_file(const char* filename);
bool fat32_create_directory(const char* dirname);
bool fat32_read_file_simple(const char* filename, void* buffer, uint32_t* size);
void fat32_test_complete(void);
static bool fat32_read_file_in_dir(uint32_t dir_cluster, const char* filename, void* buffer, uint32_t* size);

#endif
