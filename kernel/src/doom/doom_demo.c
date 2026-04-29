#include "doom_demo.h"

#include "input.h"
#include "wad.h"

#include "../gfx/printf.h"
#include "../interrupts/timer.h"
#include "../ps2/kbio.h"
#include "../libs/string.h"

static void print_name8(const char name[8]) {
    char tmp[9];
    for (int i = 0; i < 8; i++) tmp[i] = name[i] ? name[i] : ' ';
    tmp[8] = 0;
    printf("%s", tmp);
}

void doomwad_demo_run(const char *wad_path) {
    wad_file_t wad;
    if (!wad_load_file(wad_path, &wad)) {
        printf("doomwad: failed to load %s\n", wad_path);
        return;
    }

    wad_view_t view;
    if (!wad_open_view(&wad, &view)) {
        printf("doomwad: invalid WAD header\n");
        wad_unload_file(&wad);
        return;
    }

    printf("doomwad: %.4s lumps=%d dir=0x%x size=%u\n",
           view.header.identification, view.header.numlumps,
           (unsigned)view.header.infotableofs, (unsigned)wad.size);

    char playpal_name[8] = {0};
    memcpy(playpal_name, "PLAYPAL", 7);
    const wad_lumpinfo_t *playpal = wad_find_lump(&view, playpal_name);
    if (playpal) {
        printf("doomwad: found PLAYPAL @%d size=%d\n", playpal->filepos, playpal->size);
    }

    printf("doomwad: press ESC to exit (arrow keys parsed like snake)\n");
    kbio_set_mode(KBIO_MODE_RAW);

    while (1) {
        doom_key_event_t ke;
        if (doom_kbio_poll_event(&ke) && ke.pressed) {
            if (ke.key == DOOM_KEY_ESC) break;
            if (ke.key == DOOM_KEY_UP || ke.key == DOOM_KEY_DOWN ||
                ke.key == DOOM_KEY_LEFT || ke.key == DOOM_KEY_RIGHT) {
                printf("key: ");
                switch (ke.key) {
                    case DOOM_KEY_UP: printf("UP\n"); break;
                    case DOOM_KEY_DOWN: printf("DOWN\n"); break;
                    case DOOM_KEY_LEFT: printf("LEFT\n"); break;
                    case DOOM_KEY_RIGHT: printf("RIGHT\n"); break;
                    default: printf("?\n"); break;
                }
            }
        }

        doom_mouse_event_t me;
        (void)me;
        doom_mouse_poll_event(&me);

        timer_sleep_ms(16);
    }

    kbio_set_mode(KBIO_MODE_COOKED);
    wad_unload_file(&wad);
    printf("doomwad: done\n");
}
