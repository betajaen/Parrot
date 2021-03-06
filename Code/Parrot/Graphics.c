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
  uint32 bitmapWidth, bitmapFrameHeight, bitmapTotalHeight;
  int32  horizontal, vertical;
  int32  scrollX, scrollY;
  uint16 depth;

  uint32 offsetStride;
  uint32 writeOffset, readOffset;
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
static struct SimpleSprite sCursorSprite;
static uint sCursorSpriteNum;

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

  FreeSprite(0);
  sCursorSpriteNum = GetSprite(&sCursorSprite, 0);
  sCursorSprite.x = 0;
  sCursorSprite.y = 0;
  sCursorSprite.height = CursorImageData[0].height;

  ChangeSprite(NULL, &sCursorSprite, (APTR)&CursorImageData[CursorType_Busy].data);
  MoveSprite(NULL, &sCursorSprite, 50, 50);

  sIsSetup = TRUE;

  log_trace("Opened Graphics View");

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
    view->bitmapTotalHeight = info->bitmapHeight * 2;

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

    view->bitmap = AllocBitMap(
      view->bitmapWidth,
      view->bitmapTotalHeight,
      view->depth,
      BMF_DISPLAYABLE | BMF_INTERLEAVED | BMF_CLEAR, NULL);

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
    vp->Modes = 0 | SPRITES;
    vp->DxOffset = view->horizontal;
    vp->DyOffset = view->vertical;

    numColours = DepthToColours(view->depth);

    view->colorMap = GetColorMap(numColours);
    vp->ColorMap = view->colorMap;

    if (lastVp != NULL)
    {
      // nth or last
      lastVp->Next = vp;
    }
    else
    {
      // first
      sView.ViewPort = vp;
    }

    MakeVPort(&sView, vp);

    lastVp = vp;
  }

  sView.Modes = 0 | SPRITES;
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

  gfx_set_cursor(CursorType_Crosshair);

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

CursorType gfx_get_cursor()
{
  return sCursorType;
}
