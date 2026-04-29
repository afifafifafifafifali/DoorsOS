#include "wad.h"

#include "../interrupts/fd.h"
#include "../libs/string.h"
#include "../mem/heap.h"

static bool wad_is_reasonable_size(uint32_t size) {
    /* Keep this conservative: WADs can be large, but a kernel demo shouldn't
       try to malloc hundreds of MB by accident. */
    return size > 16 && size <= (64u * 1024u * 1024u);
}

bool wad_load_file(const char *path, wad_file_t *out) {
    if (!path || !out) return false;
    out->data = NULL;
    out->size = 0;

    int fd = fd_open(path, O_RDONLY);
    if (fd < 0) return false;

    int32_t sz = fd_size(fd);
    if (sz <= 0 || !wad_is_reasonable_size((uint32_t)sz)) {
        fd_close(fd);
        return false;
    }

    uint8_t *buf = (uint8_t *)malloc((size_t)sz);
    if (!buf) {
        fd_close(fd);
        return false;
    }

    int rd = fd_read(fd, buf, (uint32_t)sz);
    fd_close(fd);
    if (rd != sz) {
        free(buf);
        return false;
    }

    out->data = buf;
    out->size = (uint32_t)sz;
    return true;
}

void wad_unload_file(wad_file_t *wad) {
    if (!wad) return;
    if (wad->data) free(wad->data);
    wad->data = NULL;
    wad->size = 0;
}

static bool wad_memrange_ok(const wad_file_t *wad, uint32_t off, uint32_t len) {
    if (!wad || !wad->data) return false;
    if (off > wad->size) return false;
    if (len > wad->size) return false;
    if (off + len < off) return false;
    return (off + len) <= wad->size;
}

bool wad_open_view(const wad_file_t *wad, wad_view_t *out) {
    if (!wad || !wad->data || !out) return false;
    memset(out, 0, sizeof(*out));

    if (!wad_memrange_ok(wad, 0, sizeof(wad_header_t))) return false;

    wad_header_t hdr;
    memcpy(&hdr, wad->data, sizeof(hdr));

    if (memcmp(hdr.identification, "IWAD", 4) != 0 &&
        memcmp(hdr.identification, "PWAD", 4) != 0) {
        return false;
    }

    if (hdr.numlumps <= 0 || hdr.numlumps > 65535) return false;
    if (hdr.infotableofs <= 0) return false;

    uint32_t dir_off = (uint32_t)hdr.infotableofs;
    uint32_t dir_len = (uint32_t)hdr.numlumps * (uint32_t)sizeof(wad_lumpinfo_t);
    if (!wad_memrange_ok(wad, dir_off, dir_len)) return false;

    memcpy(&out->header, &hdr, sizeof(hdr));
    out->base = wad->data;
    out->base_size = wad->size;
    out->lumps = (const wad_lumpinfo_t *)(wad->data + dir_off);
    return true;
}

static bool wad_name8_eq(const char a[8], const char b[8]) {
    return memcmp(a, b, 8) == 0;
}

const wad_lumpinfo_t *wad_find_lump(const wad_view_t *view, const char name8[8]) {
    if (!view || !view->lumps || !name8) return NULL;
    for (int32_t i = 0; i < view->header.numlumps; i++) {
        const wad_lumpinfo_t *li = &view->lumps[i];
        if (wad_name8_eq(li->name, name8)) return li;
    }
    return NULL;
}

bool wad_get_lump_data(const wad_view_t *view, const wad_lumpinfo_t *lump,
                       const void **data, uint32_t *size) {
    if (!view || !lump || !data || !size) return false;

    if (lump->filepos < 0 || lump->size < 0) return false;
    uint32_t off = (uint32_t)lump->filepos;
    uint32_t len = (uint32_t)lump->size;
    if (!view->base || view->base_size == 0) return false;
    if (off > view->base_size) return false;
    if (off + len < off) return false;
    if (off + len > view->base_size) return false;

    *data = (const void *)(view->base + off);
    *size = len;
    return true;
}
