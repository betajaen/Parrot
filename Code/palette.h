#ifndef __PARROT_PALETTE_H__
#define __PARROT_PALETTE_H__

#include <exec/types.h>

struct Palette {
    UWORD  num_pens;
	UWORD  pens[];
};

struct Palette* g_CreatePalette(UWORD num_pens);
VOID g_DestroyPalette(struct Palette* palette);
VOID g_SetPaletteColour4(struct Palette* palette, UWORD pen, UBYTE r, UBYTE g, UBYTE b);
VOID g_SetPaletteColourw(struct Palette* palette, UWORD pen, UWORD xrgb);


#endif
