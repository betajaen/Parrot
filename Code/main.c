#include <proto/exec.h>
#include <proto/dos.h>

#include "gadgets.h"
#include "palette.h"
#include "gm_pens.h"


static struct Palette* s_palette = NULL;
static struct GadgetDesc s_gadgets[];
static struct Screen* s_screen;
static struct Window* s_scene_window;
static struct Window* s_toolbar_window;

int main(int argc, char** argv) {

    s_palette = g_CreatePalette(20);
    gm_WritePensToPalette(s_palette);

    s_screen = g_OpenScreen(320, 200, 4, "Editor");
    if (s_screen == NULL)
        goto exit;

    g_CopyToScreenPalette(s_screen, s_palette, 0, 0, 20);

    s_scene_window = g_OpenWindow(s_screen, 0,200-160,320,160, "Scene", WK_Scene);
    if (s_scene_window == NULL)
        goto exit;

    g_AttachToWindow(s_scene_window, s_gadgets);

    Delay(500);


exit:
    if (s_palette)
    	g_DestroyPalette(s_palette);
    if (s_scene_window)
    	g_CloseWindow(s_scene_window);
    if (s_screen)
    	g_CloseScreen(s_screen);

    return RETURN_OK;
}

static struct GadgetDesc s_gadgets[] = {
    G_BUTTON(1,  1,1,  50, 15, "Hello"),
    G_BUTTON(2, 130,50, 150,100, "Bonjour"),

    G_PALETTE(3, 10,50, 48,48, 0, 16),
    G_STOP
};
