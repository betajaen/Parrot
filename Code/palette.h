#ifndef __PARROT_PALETTE_H__
#define __PARROT_PALETTE_H__

#include <exec/types.h>

struct ColorSpec;

struct Colour {
    WORD pen;
    UWORD r, g, b;
};

struct Palette {
    struct Colour colours[64];
    UWORD  num_colours;
};

struct Palette* g_create_palette();
VOID g_destroy_palette(struct Palette* palette);
VOID   g_init_palette(struct Palette* palette, UWORD num_colours);

struct ColorSpec* g_create_colorspec(struct Palette* palette);
VOID g_destroy_colorspec(struct ColorSpec* colorspec);

#define g_set_palette_colour(PAL_PTR, I, R, G, B) \
	do { \
        struct Colour* _col = PAL_PTR->colours + I; \
        _col->pen = I; _col->r = R; _col->g = B; _col->b = B; \
    } while(0)

#endif
