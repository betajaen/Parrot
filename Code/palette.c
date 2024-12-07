#include <proto/exec.h>
#include <proto/dos.h>
#include <intuition/intuition.h>

#include "palette.h"

struct Palette* g_CreatePalette(UWORD num_pens) {
    ULONG size;
    struct Palette *palette;

    if (num_pens < 2) {
        num_pens = 2;
    }

    size = sizeof(UWORD) + sizeof(UWORD) * (1 + num_pens);

    palette = (struct Palette*) AllocVec(size, MEMF_CLEAR);
    if (palette) {
        palette->num_pens = num_pens;
    }

    palette->pens[num_pens] = 0xFFFF; // Terminator.

    return palette;
}

VOID g_DestroyPalette(struct Palette* palette) {
    if (palette) {
        FreeVec(palette);
    }
}

VOID g_SetPaletteColour4(struct Palette* palette, UWORD pen, UBYTE r, UBYTE g, UBYTE b) {
    if (palette == NULL || pen >= palette->num_pens)
        return;

    palette->pens[pen] = 0x0000 | (r&0xF) << 12 | (g&0xF) << 4 | (b&0xF);
}

VOID g_SetPaletteColourw(struct Palette* palette, UWORD pen, UWORD xrgb) {
    if (palette == NULL || pen >= palette->num_pens)
        return;

    palette->pens[pen] = xrgb;
}
