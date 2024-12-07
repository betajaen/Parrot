#ifndef __PARROT_GADGETS_H__
#define __PARROT_GADGETS_H__

#include <exec/types.h>

enum WindowKind {
    WK_Normal,
    WK_Background,
    WK_Scene,
    WK_Toolbar
};

struct Screen;
struct Window;
struct Palette;

struct GadgetDesc {
    UWORD kind, num;
    WORD  left, top;
    UWORD width, height;
    ULONG data, data1, data2;
};

typedef void(*g_WindowEventCb)(struct Window*, UWORD, UWORD, LONG);

struct Screen* g_OpenScreen(UWORD w, UWORD h, UWORD d, CONST_STRPTR title);
VOID g_CloseScreen(struct Screen* screen);
VOID g_CopyToScreenPalette(struct Screen* screen, struct Palette* palette, UWORD from, UWORD to, UWORD length);

struct Window* g_OpenWindow(struct Screen*, WORD x, WORD y, UWORD w, UWORD h, CONST_STRPTR title, UWORD kind);
VOID g_CloseWindow();

VOID g_AttachToWindow(struct Window* window, struct GadgetDesc* ary);
VOID g_BindToWindow(struct Window* window, g_WindowEventCb cb);

VOID g_Listen();
VOID g_StopListening();


#define G_BUTTON(NUM,L,T,W,H,CAPTION) {1, NUM, L,T,W,H, (ULONG)CAPTION,0,0 }
#define G_PALETTE(NUM,L,T,W,H,START,END) { 2, NUM, L,T,W,H,  0,START,END }
#define G_STOP { 0, ~1, 0,0,0,0, 0,0,0}

#endif

