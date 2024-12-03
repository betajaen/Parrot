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
#include "list.h"
#include "object.h"
#include "debug.h"

#if defined(PARROT_PAL) && (PARROT_PAL == 0)
#define PARROT_MONITOR_ID NTSC_MONITOR_ID
#else
#define PARROT_MONITOR_ID PAL_MONITOR_ID
#endif


struct PScreenInfo {
    struct Node    node;
    ULONG          tag;
    struct Screen* screen;
    struct ColorSpec*  colorspec;
};

#define PSCREENINFO_TAG TAG('S','C','I','F')

struct PWindowInfo {
    struct Node    node;
    ULONG          tag;
    struct Window* window;
};

#define PWINDOWINFO_TAG TAG('W','N','I','F')

struct PGadgetInfo {
    struct Node       node;
    ULONG             tag;
    struct Gadget*    gadget;
    struct NewGadget* new_gadget;
};

#define PGADGETINFO_TAG TAG('G','A','I','F')


static ULONG s_WindowSignals = 0UL;
static struct TextAttr s_ScreenFont = { (STRPTR) "topaz.font", 8, 0, 1 };
static UWORD s_WindowEventLoop = FALSE;
static struct List s_WindowInfoList = LL_NULL_LIST;

struct Screen* g_OpenScreen(UWORD w, UWORD h, UWORD d, CONST_STRPTR title, struct Palette* palette) {

    struct Screen* screen = NULL;
    struct TagItem* tags = NULL;
    struct PScreenInfo* info = NULL;
	ULONG displayMode = 0UL, numColours = 0;

    if (d == 0 || d > 8) {
        goto err;
    }

    numColours = 1 << d;

    if (palette && palette->num_colours > numColours) {
        goto err;
    }

    info = U_ALLOC_OBJECT(sizeof(struct PScreenInfo), PSCREENINFO_TAG);

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
    	U_FREE_OBJECT(info, PSCREENINFO_TAG);
    }

    if (tags) {
        u_clear_tags();
    }

    return NULL;
}

VOID g_CloseScreen(struct Screen* screen) {
    if (screen) {
        if (screen->UserData) {
        	U_FREE_OBJECT(screen->UserData, PSCREENINFO_TAG);
        	screen->UserData = NULL;
        }
        CloseScreen(screen);
    }
}

struct Window* g_OpenWindow(struct Screen* screen, WORD x, WORD y, UWORD w, UWORD h, CONST_STRPTR title, UWORD kind) {
    struct Window* window = NULL;
    struct TagItem* tags = NULL;
    struct PWindowInfo* info = NULL;
    ULONG idcmp = 0UL, winFlags = 0UL;

    if (LL_IS_NULL(&s_WindowInfoList)) {
        LL_INIT_LIST(&s_WindowInfoList);
    }

    u_clear_tags();
    tags = u_start_tags();

    u_push_tagl(WA_Left, x);
    u_push_tagl(WA_Top, y);
    u_push_tagu(WA_Width, w);
    u_push_tagu(WA_Height, h);

    if (screen != NULL) {
        u_push_tagp(WA_CustomScreen, screen);
    }

    if (kind == WK_Normal) {
        idcmp |= IDCMP_CLOSEWINDOW | IDCMP_GADGETUP;
        winFlags |= WFLG_ACTIVATE | WFLG_DRAGBAR | WFLG_CLOSEGADGET;
    }
    else if (kind == WK_Background) {
        idcmp |= IDCMP_GADGETUP;
        winFlags |= WFLG_ACTIVATE | WFLG_BORDERLESS;
    }
    else {
        U_ERROR("Unknown Window kind!");
        goto err;
    }

    idcmp |= BUTTONIDCMP;

    u_push_tagu(WA_IDCMP, idcmp);
    u_push_tagu(WA_Flags, winFlags);
    u_push_tags(WA_Title, title);
    u_end_tags();

    window = OpenWindowTagList(NULL, tags);

    if (window == NULL) {
        U_ERROR("Could not open window!");
        goto err;
    }

    info = U_ALLOC_OBJECT(sizeof(struct PWindowInfo), PWINDOWINFO_TAG);
    window->UserData = (BYTE*) info;

    return window;
err:

    if (info) {
        U_FREE_OBJECT(info, PWINDOWINFO_TAG);
        info = NULL;

        if (window) {
            window->UserData = NULL;
        }
    }

    if (window) {
        CloseWindow(window);
    }


    if (tags) {
        u_clear_tags();
    }

}

VOID g_CloseWindow(struct Window* window) {
    if (window) {
    	if (window->UserData) {
            U_FREE_OBJECT(window->UserData, PWINDOWINFO_TAG);
            window->UserData = NULL;
        }
        CloseWindow(window);
    }
}

