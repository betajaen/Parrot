#ifndef __PARROT_GADGETS_H__
#define __PARROT_GADGETS_H__

#include <exec/types.h>

struct Screen;
struct Window;
struct Palette;

struct Screen* g_OpenScreen(UWORD w, UWORD h, UWORD d, CONST_STRPTR title, struct Palette* palette);
VOID g_CloseScreen(struct Screen* screen);


#endif

