#include <proto/exec.h>
#include <proto/dos.h>

#include "gadgets.h"
#include "palette.h"
#include "gm_pens.h"


static struct Palette* s_palette = NULL;
static struct GadgetDesc s_gadgets[];


int main(int argc, char** argv) {

    s_palette = g_CreatePalette(20);
    gm_WritePensToPalette(s_palette);

    struct Screen* screen = g_OpenScreen(320, 200, 4, "Editor");
    if (screen == NULL)
        goto exit;

    g_CopyToScreenPalette(screen, s_palette, 0, 0, 20);

    struct Window* window = g_OpenWindow(screen, 0,11,320,200, "Window", WK_Background);
    if (window == NULL)
        goto exit;
                  
                           
    g_AttachToWindow(window, s_gadgets);

    Delay(200);


exit:
    if (s_palette)
    	g_DestroyPalette(s_palette);
    if (window)
    	g_CloseWindow(window);
    if (screen)
    	g_CloseScreen(screen);

    return RETURN_OK;
}

static struct GadgetDesc s_gadgets[] = {
    G_BUTTON(1,  10,10,  50, 15, "Hello"),
    G_BUTTON(2, 130,50, 150,100, "Bonjour"),

    G_PALETTE(3, 10,50, 100,100, 0, 16),
    G_STOP
};
