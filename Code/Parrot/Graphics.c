/**
    $Id: Log.c 1.5 2021/02/21 10:22:00, betajaen Exp $

    Parrot - Point and Click Adventure Game Player
    ==============================================

    Copyright 2020 Robin Southern http://github.com/betajaen/parrot

    Permission is hereby granted, free of charge, to any person obtaining a
    copy of this software and associated documentation files (the "Software"),
    to deal in the Software without restriction, including without limitation
    the rights to use, copy, modify, merge, publish, distribute, sublicense,
    and/or sell copies of the Software, and to permit persons to whom the
    Software is furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included
    in all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
    THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
    FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
    DEALINGS IN THE SOFTWARE.
*/

#include <Parrot/Parrot.h>
#include <Parrot/Graphics.h>
#include <Parrot/Log.h>
#include <Parrot/Events.h>

#include <proto/exec.h>
#include <proto/intuition.h>

#include <hardware/dmabits.h>
#include <hardware/custom.h>
#include <hardware/blit.h>
#include <graphics/gfx.h>
#include <graphics/gfxmacros.h>
#include <graphics/copper.h>
#include <graphics/view.h>
#include <graphics/gels.h>
#include <graphics/regions.h>
#include <graphics/clip.h>
#include <graphics/text.h>
#include <graphics/gfxbase.h>

#include <clib/graphics_protos.h>

#include "CursorsData.inl"
#include "PaletteData.inl"

#define MAX_VIEWPORTS 2

typedef struct _GraphicsView GraphicsView;

struct _GraphicsView
{
  struct ViewPort viewPort;
  struct RasInfo rasInfo;
  struct RastPort rastPort;
  struct BitMap* bitmap;
  struct ColorMap* colorMap;

  uint32 width, height;
  uint32 bitmapWidth, bitmapFrameHeight, bitmapHeight;
  int32  horizontal, vertical;
  int32  scrollX, scrollY;
  uint16 depth;

  uint32 offsetStride;
  uint32 writeOffset, readOffset;
  bool   dirty;
};

extern struct GfxBase* GfxBase;

static struct View* sWbView = NULL;
static struct View sView;

static bool sIsSetup = FALSE;
static bool sViewsAllocated = FALSE;
static bool sViewsAreOpen = FALSE;

static GraphicsView sViews[MAX_VIEWPORTS];

static uint8  sNumViewPorts = 0;
static CursorType sCursorType = CursorType_Crosshair;
static struct SimpleSprite sCursorSprite = { 0 };
static int16 sCursorSpriteNum = -1;

static int32 DepthToColours(uint8 depth)
{
  switch (depth)
  {
    default:
    {
      log_error_fmt("DepthToColours. Depth is an unexpected value %ld", (uint) depth);
    }
    return 0;
    case 1: return 2;
    case 2: return 4;
    case 3: return 8;
    case 4: return 16;
    case 5: return 32;
    case 6: return 64;
    case 7: return 128;
    case 8: return 256;
    case 16: return 65536;
    case 24: return 16777216;
    case 32: return 2147483647; /* Note: GetColours accepts an int, so an UINT_MAX
                                         would role over. It is unlikely for a Parrot
                                         game to use all UINT_MAX colours in one view
                                         anyway.
                                */
  }
}

bool gfx_setup()
{
  if (sIsSetup == TRUE)
  {
    log_warn("gfx_setup Was called when graphics is already setup");
    return FALSE;
  }

  sWbView = GfxBase->ActiView;

  CloseWorkBench();
  WaitTOF();
  WaitTOF();

  sIsSetup = TRUE;

  log_trace("Opened Graphics View");
  
  LoadView(NULL);
  WaitTOF();
  WaitTOF();

  return TRUE;
}

bool gfx_teardown()
{
  if (sViewsAllocated == TRUE)
  {
    gfx_destroy_views();
  }

  if (sIsSetup == FALSE)
  {
    log_warn("gfx_close Was called when View is already closed");
    return FALSE;
  }

  WaitTOF();
  LoadView(sWbView);
  WaitTOF();
  WaitTOF();

  OpenWorkBench();

  log_trace("Closed Graphics View");

  sIsSetup = FALSE;
  return TRUE;
}

bool gfx_is_visible()
{
  return sIsSetup;
}

bool gfx_create_views(GraphicsViewInfo views_ary[], uint8 count)
{
  GraphicsView *view;
  GraphicsViewInfo* info;
  struct ViewPort* vp, *lastVp;
  uint8 viewIdx;
  int32 numColours;

  if (sIsSetup == FALSE)
  {
    log_warn("gfx_create_views Was called when View is already open");
    return FALSE;
  }

  if (sViewsAllocated == TRUE)
  {
    log_warn("gfx_create_views Was called when View is already created");
    return FALSE;
  }

  if (count > MAX_VIEWPORTS)
  {
    log_warn_fmt("gfx_create_views %ld views are created, but only %ld are allowed", (int)count, (int)MAX_VIEWPORTS);
    return FALSE;
  }

  sNumViewPorts = count;

  InitView(&sView);

  lastVp = NULL;

  for (viewIdx = 0; viewIdx < sNumViewPorts; viewIdx++)
  {
    view = &sViews[viewIdx];
    info = &views_ary[viewIdx];
    
    view->bitmapWidth = info->bitmapWidth;
    view->bitmapFrameHeight = info->bitmapHeight;
    view->bitmapHeight = info->bitmapHeight * 2;

    view->offsetStride = info->bitmapHeight;
    view->readOffset = 0;
    view->writeOffset = view->offsetStride;

    view->width = info->viewWidth;
    view->height = info->viewHeight;
    view->horizontal = info->viewLeft;
    view->vertical = info->viewTop;
    view->depth = info->depth;
    view->scrollX = 0;
    view->scrollY = 0;
    view->dirty = FALSE;

    log_trace_fmt("gfx_create_views %ld Bitmap Width = %ld", viewIdx, view->bitmapWidth);
    log_trace_fmt("gfx_create_views %ld Bitmap Height = %ld", viewIdx, view->bitmapHeight);
    log_trace_fmt("gfx_create_views %ld Bitmap FrameHeight = %ld", viewIdx, view->bitmapFrameHeight);

    view->bitmap = AllocBitMap(
      view->bitmapWidth,
      view->bitmapHeight,
      view->depth,
      BMF_DISPLAYABLE | BMF_INTERLEAVED | BMF_CLEAR, NULL);

    log_trace_fmt("gfx_create_views %ld Bitmap = %lx", viewIdx, view->bitmap);

    InitRastPort(&view->rastPort);
    view->rastPort.BitMap = view->bitmap;

    vp = &view->viewPort;
    InitVPort(vp);

    vp->RasInfo = &view->rasInfo;
    vp->RasInfo->Next = NULL;
    vp->RasInfo->RxOffset = 0;
    vp->RasInfo->RyOffset = 0;
    vp->RasInfo->BitMap = view->bitmap;
    vp->DWidth = view->width;
    vp->DHeight = view->height;
    vp->Modes = SPRITES;
    vp->DxOffset = view->horizontal;
    vp->DyOffset = view->vertical;


    log_trace_fmt("gfx_create_views %ld DWidth = %ld", viewIdx, vp->DWidth);
    log_trace_fmt("gfx_create_views %ld DHeight = %ld", viewIdx, vp->DHeight);
    log_trace_fmt("gfx_create_views %ld DxOffset = %ld", viewIdx, vp->DxOffset);
    log_trace_fmt("gfx_create_views %ld DyOffset = %ld", viewIdx, vp->DyOffset);
    log_trace_fmt("gfx_create_views %ld Modes = %lx", viewIdx, vp->Modes);
    log_trace_fmt("gfx_create_views %ld Vp.Bitmap = %lx", viewIdx, vp->RasInfo->BitMap);

    numColours = DepthToColours(view->depth);
    
    if (numColours < 20)
    {
      // Allocate 4 extra colours for the first sprite (mouse pointer). These
      // are fixed at 16, 17, 18, 19
      numColours = 20;
    }

    view->colorMap = GetColorMap(numColours);
    vp->ColorMap = view->colorMap;

    log_trace_fmt("gfx_create_views %ld ColorMap = %lx", viewIdx, vp->ColorMap);

    if (lastVp != NULL)
    {
      // nth or last
      lastVp->Next = vp;
      log_trace_fmt("gfx_create_views %ld IsNth", viewIdx);
    }
    else
    {
      // first
      sView.ViewPort = vp;
      log_trace_fmt("gfx_create_views %ld IsFirst = %lx", viewIdx);
    }

    MakeVPort(&sView, vp);

    lastVp = vp;
  }

  sView.Modes = SPRITES;
  MrgCop(&sView);

  sViewsAllocated = TRUE;

  return TRUE;
}

bool gfx_destroy_views()
{
  GraphicsView* view;
  uint8 viewIdx;


  if (sViewsAllocated == FALSE)
  {
    log_warn("gfx_destroy_views was called when no views are available");
    return FALSE;
  }

  if (sViewsAreOpen == TRUE)
  {
    if (gfx_close_views() == FALSE)
    {
      log_warn("gfx_destroy_views cannot destroy views due to previous error");
      return FALSE;
    }
  }

  for (viewIdx = 0; viewIdx < sNumViewPorts; viewIdx++)
  {
    view = &sViews[viewIdx];

    if (view->colorMap != NULL)
    {
      FreeColorMap(view->colorMap);
      view->colorMap = NULL;
    }

    if (view->bitmap != NULL)
    {
      FreeBitMap(view->bitmap);
      view->bitmap = NULL;
    }

    FreeVPortCopLists(&view->viewPort);

    /* Reset */
    InitVPort(&view->viewPort);
  }

  if (sView.LOFCprList != NULL)
  {
    FreeCprList(sView.LOFCprList);
  }

  if (sView.SHFCprList != NULL)
  {
    FreeCprList(sView.SHFCprList);
  }

  sNumViewPorts = 0;
  
  sViewsAllocated = FALSE;

  return TRUE;
}


bool gfx_open_views()
{
  uint8 viewIndex;
  struct ViewPort* vp;

  if (sViewsAllocated == FALSE)
  {
    log_warn("gfx_open_views was called when no views are available");
    return FALSE;
  }

  if (sViewsAreOpen == TRUE)
  {
    log_warn("gfx_open_views was called when view is already open");
    return FALSE;
  }

  LoadView(&sView);
  WaitTOF();
  WaitTOF();

  sViewsAreOpen = TRUE;

  // Apply default palette to all of the viewports
  for (viewIndex = 0; viewIndex < sNumViewPorts; viewIndex++)
  {
    vp = &sViews[viewIndex].viewPort;
    LoadRGB32(vp, kDefaultPalette);
  }

  // Reserve the first sprite which is used for the mouse.
  FreeSprite(0);

  sCursorSpriteNum = GetSprite(&sCursorSprite, 0);

  log_trace_fmt("GetSprite result = %ld", sCursorSpriteNum);

  sCursorSprite.x = 0;
  sCursorSprite.y = 0;
  sCursorSprite.height = CursorImageData[CursorType_Crosshair].height;
  ChangeSprite(NULL, &sCursorSprite, (APTR)&CursorImageData[CursorType_Crosshair].data);
  
  // Wait two frames for it all to apply.
  WaitTOF();
  WaitTOF();
  
  
  return TRUE;
}

bool gfx_close_views()
{
  if (sViewsAllocated == FALSE)
  {
    log_warn("gfx_open_views was called when no views are available");
    return FALSE;
  }

  if (sViewsAreOpen == FALSE)
  {
    log_warn("gfx_open_views was called when view is already closed");
    return FALSE;
  }

  if (sCursorSpriteNum != -1)
  {
    FreeSprite(0);
  }

  LoadView(NULL);
  WaitTOF();
  WaitTOF();

  sViewsAreOpen = FALSE;

  return TRUE;
}

void gfx_set_cursor(CursorType type)
{
  sCursorType = type;

  if (sCursorType >= CursorType_Count)
    sCursorType = 0;

  ChangeSprite(NULL, &sCursorSprite, (APTR)&CursorImageData[sCursorType].data);

}

void gfx_warp_cursor(int32 x, int32 y)
{
  MoveSprite(NULL, &sCursorSprite, 
    x + CursorImageData[sCursorType].offsetX,
    y + CursorImageData[sCursorType].offsetY
  );
}

bool gfx_handle_event(union _Event* evt)
{
  if (evt->mouse.type == EventType_MouseMove)
  {
    log_trace_fmt("Mouse Move %ld, %ld", evt->mouse.x, evt->mouse.y);

    MoveSprite(NULL, &sCursorSprite,
      evt->mouse.x + CursorImageData[sCursorType].offsetX,
      evt->mouse.y + CursorImageData[sCursorType].offsetY
      );

    return TRUE;
  }
  return FALSE;
}

void gfx_wait_tof()
{
  WaitTOF();
}

void gfx_try_flipbuffers(uint8 view)
{
  GraphicsView* gv;

#if defined(PARROT_DEBUG)
  if (view >= MAX_VIEWPORTS)
  {
    log_warn_fmt("In %s the view argument is out of bounds %lu", PARROT_STRINGIFY(__FUNCTION__), (uint32)view);
    return;
  }
#endif

  gv = &sViews[view];

  if (gv->dirty)
  {

    uint32 t;
    t = gv->writeOffset;
    gv->writeOffset = gv->readOffset;
    gv->readOffset = t;


    gv->rasInfo.RxOffset = gv->scrollX;
    gv->rasInfo.RyOffset = gv->readOffset + gv->scrollY;

    gv->dirty = FALSE;

    ScrollVPort(&gv->viewPort);

    log_trace_fmt("%s is swappped buffers for %lu", PARROT_STRINGIFY(__FUNCTION__), (uint32)view);
  }

}

void gfx_apen(uint8 view, uint8 pen)
{
  GraphicsView* gv;


#if defined(PARROT_DEBUG)
  if (view >= MAX_VIEWPORTS)
  {
    log_warn_fmt("In %s the view argument is out of bounds %lu", PARROT_STRINGIFY(__FUNCTION__), (uint32)view);
    return;
  }
#endif

  gv = &sViews[view];

  SetAPen(&gv->rastPort, pen);
}

void gfx_bpen(uint8 view, uint8 pen)
{
  GraphicsView* gv;


#if defined(PARROT_DEBUG)
  if (view >= MAX_VIEWPORTS)
  {
    log_warn_fmt("In %s the view argument is out of bounds %lu", PARROT_STRINGIFY(__FUNCTION__), (uint32)view);
    return;
  }
#endif

  gv = &sViews[view];

  SetBPen(&gv->rastPort, pen);
}

void gfx_abpen(uint8 view, uint8 apen, uint8 bpen)
{
  GraphicsView* gv;


#if defined(PARROT_DEBUG)
  if (view >= MAX_VIEWPORTS)
  {
    log_warn_fmt("In %s the view argument is out of bounds %lu", PARROT_STRINGIFY(__FUNCTION__), (uint32)view);
    return;
  }
#endif

  gv = &sViews[view];

  SetAPen(&gv->rastPort, apen);
  SetBPen(&gv->rastPort, bpen);
}

void gfx_box(uint8 view, uint16 left, uint16 top, uint16 right, uint16 bottom)
{
  GraphicsView* gv;
  uint32 otop, obottom;


#if defined(PARROT_DEBUG)
  if (view >= MAX_VIEWPORTS)
  {
    log_warn_fmt("In %s the view argument is out of bounds %lu", PARROT_STRINGIFY(__FUNCTION__), (uint32)view);
    return;
  }
#endif

  gv = &sViews[view];
  
  gv->dirty = TRUE;
  otop = gv->writeOffset + top;
  obottom = gv->writeOffset + bottom;

  Move(&gv->rastPort, left, otop);
  Draw(&gv->rastPort, right, otop);
  Draw(&gv->rastPort, right, obottom);
  Draw(&gv->rastPort, left, obottom);
  Draw(&gv->rastPort, left, otop);
}
