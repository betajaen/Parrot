#ifndef __PARROT_GADGETS_H__
#define __PARROT_GADGETS_H__

#include <exec/types.h>

enum WindowKind {
    WK_Normal,
    WK_Background
};

struct Screen;
struct Window;
struct Palette;

struct Screen* g_OpenScreen(UWORD w, UWORD h, UWORD d, CONST_STRPTR title, struct Palette* palette);
VOID g_CloseScreen(struct Screen* screen);

struct Window* g_OpenWindow(struct Screen*, WORD x, WORD y, UWORD w, UWORD h, CONST_STRPTR title, UWORD kind);
VOID g_CloseWindow();

#endif

