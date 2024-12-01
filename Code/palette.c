#include <proto/exec.h>
#include <proto/dos.h>
#include <intuition/intuition.h>

#include "palette.h"

struct Palette* g_create_palette() {
    return AllocVec(sizeof(struct Palette), MEMF_CLEAR);
}

VOID g_destroy_palette(struct Palette* palette) {
    if (palette) {
        FreeVec(palette);
    }
}

VOID   g_init_palette(struct Palette* palette, UWORD num_colours) {
    UWORD i;
    struct Colour* col;

    if (palette == NULL) {

        return;
    }

    if (num_colours > 64) {
        num_colours = 64;
    }

    palette->num_colours = num_colours;
    col = palette->colours;

    for (i=0;i < num_colours;i++) {
        col->pen = i;
        col->r = 0;
        col->g = 0;
        col->b = 0;
        col++;
    }

    for (;i < 64;i++) {
        col->pen = -1;
        col->r = 0;
        col->g = 0;
        col->b = 0;
        col++;
    }

}

struct ColorSpec* g_create_colorspec(struct Palette* palette) {
    ULONG size, i, c;
    struct ColorSpec *colorspec, *cs;
    struct Colour* col;

    if (palette == NULL) {
        Printf("Pal null!\n");
        return NULL;
    }

    size = (palette->num_colours + 1) * sizeof(struct ColorSpec);

    colorspec = AllocVec(size, MEMF_CLEAR);
    cs = colorspec;
    col = palette->colours;

    for(i = 0; i < palette->num_colours;i++) {
        cs->ColorIndex = col->pen;
        cs->Red = col->r;
        cs->Green = col->g;
        cs->Blue = col->b;

        cs++;
        col++;
    }

    cs->ColorIndex = -1;

    return colorspec;
}

VOID g_destroy_colorspec(struct ColorSpec* colorspec) {
    if (colorspec) {
        FreeVec(colorspec);
    }
}
