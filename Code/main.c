#include <proto/exec.h>
#include <proto/dos.h>

#include "gadgets.h"
#include "palette.h"

static struct Palette* s_palette = NULL;

static struct Palette* s_create_ega_palette();

int main(int argc, char** argv) {
                   
    s_palette = s_create_ega_palette();
    struct Screen* screen = g_OpenScreen(320, 200, 5, "Editor", s_palette);

    Delay(200);

    g_CloseScreen(screen);

    g_destroy_palette(s_palette);

    return RETURN_OK;
}

static struct Palette* s_create_ega_palette() {
    struct Palette* pal;

    pal = g_create_palette();
    g_init_palette(pal, 32);

    g_set_palette_colour(pal, 1, 0x00, 0x00, 0x00);
    g_set_palette_colour(pal, 1, 0xFF, 0xFF, 0xFF);
    g_set_palette_colour(pal,17, 0xFF, 0xFF, 0xFF);


    return pal;
}
