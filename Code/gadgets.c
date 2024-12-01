#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/utility.h>
#include <proto/graphics.h>
#include <libraries/gadtools.h>
#include <intuition/screens.h>
#include <graphics/modeid.h>

#include "gadgets.h"
#include "palette.h"
#include "tags.h"

#if defined(PARROT_PAL) && (PARROT_PAL == 0)
#define PARROT_MONITOR_ID NTSC_MONITOR_ID
#else
#define PARROT_MONITOR_ID PAL_MONITOR_ID
#endif


struct ScreenInfo {
    struct Node    node;
    ULONG          class_tag;
    struct Screen* screen;
    struct ColorSpec*  colorspec;
};

struct Screen* g_OpenScreen(UWORD w, UWORD h, UWORD d, CONST_STRPTR title, struct Palette* palette) {

    struct Screen* screen = NULL;
    struct TagItem* tags = NULL;
    struct ScreenInfo* info = NULL;
	ULONG displayMode = 0UL;

    if (d == 0 || d > 8) {
        goto err;
    }

    info = AllocVec(sizeof(struct ScreenInfo), MEMF_CLEAR | MEMF_FAST);

    displayMode = BestModeID(
      BIDTAG_NominalWidth,	w,
      BIDTAG_NominalHeight,	h,
      BIDTAG_DesiredWidth,	w,
      BIDTAG_DesiredHeight, h,
      BIDTAG_Depth, 		d,
	  BIDTAG_MonitorID,     PARROT_MONITOR_ID,
      TAG_END
    );

    if (displayMode == INVALID_ID) {
        goto err;
    }

    tags = u_start_tags();
    u_push_tagu(SA_Left,		0UL);
    u_push_tagu(SA_Top,			0UL);
    u_push_tagu(SA_Width,		(ULONG) w);
    u_push_tagu(SA_Height,		(ULONG) h);
    u_push_tagu(SA_Depth,		(ULONG) d);
    u_push_tagu(SA_DisplayID,   displayMode);
    u_push_tagu(SA_Type,        CUSTOMSCREEN);
    u_push_tags(SA_Title,       title);

    if (palette) {
        info->colorspec = g_create_colorspec(palette);
        u_push_tagp(SA_Colors, info->colorspec);
    }

    u_end_tags();

    screen = OpenScreenTagList(NULL, tags);

    if (screen == NULL) {
        goto err;
    }

    screen->UserData = (BYTE*) info;

    u_clear_tags();
    return screen;

err:
    if (info) {
        if (info->colorspec) {
            g_destroy_colorspec(info->colorspec);
            info->colorspec = NULL;
    	}
        FreeVec(info);
    }

    if (tags) {
        u_clear_tags();
    }

    return NULL;
}

VOID g_CloseScreen(struct Screen* screen) {
    if (screen) {
        struct ScreenInfo* info = (struct ScreenInfo*) screen->UserData;
        FreeVec(info);
        screen->UserData = NULL;
        CloseScreen(screen);
    }
}


