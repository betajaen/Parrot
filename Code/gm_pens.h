#ifndef __PARROT_PENS_H__
#define __PARROT_PENS_H__

#include <exec/types.h>

#define PEN_BLACK         0
#define PEN_WHITE         1
#define PEN_BLUE          2
#define PEN_GREEN         3
#define PEN_CYAN          4
#define PEN_RED           5
#define PEN_MAGENTA       6
#define PEN_BROWN         7
#define PEN_GREY          8
#define PEN_DARK_GREY     9
#define PEN_LIGHT_BLUE    10
#define PEN_LIGHT_GREEN   11
#define PEN_LIGHT_CYAN    12
#define PEN_LIGHT_RED     13
#define PEN_LIGHT_MAGENTA 14
#define PEN_YELLOW        15
#define PEN_COUNT         16

#define PEN_CURSOR0       17
#define PEN_CURSOR1       18
#define PEN_CURSOR2       19

struct Palette;

VOID gm_WritePensToPalette(struct Palette*);

#endif
