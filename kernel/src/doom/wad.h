#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t *data;
    uint32_t size;
} wad_file_t;

typedef struct {
    char identification[4]; /* "IWAD" or "PWAD" (not NUL-terminated) */
    int32_t numlumps;
    int32_t infotableofs;
} wad_header_t;

typedef struct {
    int32_t filepos;
    int32_t size;
    char name[8]; /* not NUL-terminated */
} wad_lumpinfo_t;

typedef struct {
    wad_header_t header;
    const uint8_t *base;          /* wad file base (wad_file_t->data) */
    uint32_t base_size;           /* wad file size (wad_file_t->size) */
    const wad_lumpinfo_t *lumps; /* points into wad_file_t->data */
} wad_view_t;

bool wad_load_file(const char *path, wad_file_t *out);
void wad_unload_file(wad_file_t *wad);

bool wad_open_view(const wad_file_t *wad, wad_view_t *out);
const wad_lumpinfo_t *wad_find_lump(const wad_view_t *view, const char name8[8]);
bool wad_get_lump_data(const wad_view_t *view, const wad_lumpinfo_t *lump,
                       const void **data, uint32_t *size);
