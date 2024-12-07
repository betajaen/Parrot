#include "gm_pens.h"
#include "palette.h"
#include "math.h"

#define GM_NUM_PENS 20

UWORD s_Palette[GM_NUM_PENS] = {
/*     RGB */
    0x0000, // Black
    0x0FFF, // White
    0x000A, // Blue
    0x00A0, // Green
    0x00AA, // Cyan
    0x0A00, // Red
    0x0A0A, // Magenta
    0x0A5A, // Brown
    0x0AAA, // Grey
    0x0555, // Dark Grey
    0x055F, // Light Blue
    0x05F5, // Light Green
    0x05FF, // Light Cyan
    0x0F55, // Light Red
    0x0F5F, // Light Magenta
    0x0FF5, // Yellow
    0x0000, // Unused
    0x0FFF, // Cursor 0
    0x0555, // Cursor 1
    0x0000, // Cursor 2
};



VOID gm_WritePensToPalette(struct Palette* palette) {
    UWORD lm,i;

    lm = u_Min(GM_NUM_PENS, palette->num_pens);

    for(i=0;i < lm;i++) {
        palette->pens[i] = s_Palette[i];
    }
}
