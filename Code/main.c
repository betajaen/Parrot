#include <proto/exec.h>
#include <proto/dos.h>

#include "gadgets.h"
#include "palette.h"

static struct Palette* s_palette = NULL;
static struct GadgetDesc s_gadgets[];

static struct Palette* s_create_ega_palette();

int main(int argc, char** argv) {

    s_palette = s_create_ega_palette();
    struct Screen* screen = g_OpenScreen(320, 200, 5, "Editor", s_palette);
    if (screen == NULL)
        goto exit;

    struct Window* window = g_OpenWindow(screen, 0,11,320,200, "Window", WK_Background);
    if (window == NULL)
        goto exit;
                  
                           
    g_AttachToWindow(window, s_gadgets);

    Delay(200);


exit:
    if (s_palette)
    	g_destroy_palette(s_palette);
    if (window)
    	g_CloseWindow(window);
    if (screen)
    	g_CloseScreen(screen);

    return RETURN_OK;
}

static struct Palette* s_create_ega_palette() {
    struct Palette* pal;

    pal = g_create_palette();
    g_init_palette(pal, 32);

    g_set_palette_colour(pal, 0, 0x00, 0x00, 0x00);
    g_set_palette_colour(pal, 1, 0xFF, 0xFF, 0xFF);
    g_set_palette_colour(pal,17, 0xFF, 0xFF, 0xFF);


    return pal;
}

static struct GadgetDesc s_gadgets[] = {
    G_BUTTON(1, 10,10, 50,15, "Hello"),
    G_BUTTON(2, 130,50, 150,100, "Bonjour"),

    G_PALETTE(3, 10,50, 100,100, 0, 32),
    G_STOP
};
