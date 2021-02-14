/**
    $Id: View.c, 1.2 2020/05/29 17:01:00, betajaen Exp $

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
#include <Parrot/Requester.h>
#include <Parrot/String.h>
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

extern struct GfxBase* GfxBase;

#define ARCH_UNKNOWN 0
#define ARCH_ECS     1
#define ARCH_AGA     2
#define ARCH_RTG     3

struct CURSOR_IMAGE
{
  WORD  OffsetX, OffsetY;
  UWORD Height;
  UWORD Data[2 * 24];
};

struct VIEWPORT
{
  struct ViewPort     v_ViewPort;
  struct RasInfo      v_RasInfo;
  struct RastPort     v_RastPort;
  struct BitMap*      v_Bitmap;
  struct ColorMap*    v_ColorMap;
  UWORD               v_Offset;
  UWORD               v_ReadOffset;
  UWORD               v_WriteOffset;
  UWORD               v_Width;
  UWORD               v_Height;
  UWORD               v_BitMapWidth;
  UWORD               v_BitmapHeight;
  WORD                v_Horizontal;
  WORD                v_Vertical;
  UWORD               v_Depth;
  WORD                v_ScrollX;
  WORD                v_ScrollY;
};

struct View*         IntuitionView;
struct View          View;
struct VIEWPORT      ViewPorts[MAX_VIEW_LAYOUTS];
UWORD                ScreenWidth;
UWORD                ScreenHeight;
UWORD                NumViewPorts;
UWORD                IsShown;

STATIC ULONG DefaultPalette[] =
{
    4 << 16 | 0,
    0X00000000, 0X00000000, 0X00000000,
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
    0XAAAAAAAA, 0XAAAAAAAA, 0XAAAAAAAA,
    0X55555555, 0X55555555, 0X55555555,
    2 << 16 | 17,
    0,0,0,
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
    0
};

STATIC ULONG MouseFramePalette0[8] =
{
    2 << 16 | 17,
    0,0,0,
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
    0,
};

STATIC ULONG MouseFramePalette1[8] =
{
    2 << 16 | 17,
    0,0,0,
    0XAAAAAAAA, 0XAAAAAAAA, 0XAAAAAAAA,
    0,
};

STATIC ULONG MouseFramePalette2[8] =
{
    2 << 16 | 17,
    0,0,0,
    0X55555555, 0X55555555, 0X55555555,
    0,
};

EXPORT VOID GfxInitialise()
{
  NumViewPorts = 0;
  IsShown = FALSE;
  ScreenWidth = 0;
  ScreenHeight = 0;
  IntuitionView = NULL;

  InitView(&View);
}

EXPORT VOID ViewExitNow()
{
  GfxHide();
  GfxClose();
}

EXPORT VOID GfxOpen(struct VIEW_LAYOUTS* layouts)
{
  struct VIEWPORT* vp, *lastVp;
  struct VIEW_LAYOUT* vl;
  struct ViewPort* avp;
  struct RastPort* rp;
  UWORD ii, jj, numColours;

  if (layouts->v_NumLayouts > MAX_VIEW_LAYOUTS)
  {
    ErrorF("ViewPort count exceeded %ld vs %ld", layouts->v_NumLayouts, MAX_VIEW_LAYOUTS);
  }

  if (NumViewPorts != 0)
  {
    ErrorF("There is already a view!");
  }

  NumViewPorts = layouts->v_NumLayouts;
  lastVp = NULL;

  InitView(&View);

  for (ii = 0; ii < NumViewPorts; ii++)
  {
    vp = &ViewPorts[ii];
    vl = &layouts->v_Layouts[ii];

    vp->v_Offset = vl->vl_Height;

    vp->v_ReadOffset = 0;
    vp->v_WriteOffset = vp->v_Offset;

    vp->v_Width = vl->vl_Width;
    vp->v_Height = vl->vl_Height;
    vp->v_BitMapWidth = vl->vl_BitMapWidth;
    vp->v_BitmapHeight = vl->vl_BitmapHeight;
    vp->v_Horizontal = vl->vl_Horizontal;
    vp->v_Vertical = vl->vl_Vertical;
    vp->v_Depth = vl->vl_Depth;
    vp->v_ScrollX = 0;
    vp->v_ScrollY = 0;
    
    vp->v_Bitmap = AllocBitMap(
      vp->v_BitMapWidth, 
      vp->v_BitmapHeight << 1, 
      vp->v_Depth,
      BMF_DISPLAYABLE | BMF_INTERLEAVED | BMF_CLEAR, NULL);

    InitRastPort(&vp->v_RastPort);
    vp->v_RastPort.BitMap = vp->v_Bitmap;

    avp = &vp->v_ViewPort;
    InitVPort(avp);

    avp->RasInfo = &vp->v_RasInfo;
    avp->RasInfo->Next = NULL;
    avp->RasInfo->RxOffset = 0;
    avp->RasInfo->RyOffset = 0;
    avp->RasInfo->BitMap = vp->v_Bitmap;
    avp->DWidth = vl->vl_Width;
    avp->DHeight = vl->vl_Height;
    avp->Modes = 0 | SPRITES;
    avp->DyOffset = vp->v_Vertical;

    numColours = 1 << vl->vl_Depth;
    if (numColours < 32)
    {
      numColours = 32;
    }

    vp->v_ColorMap = GetColorMap(numColours);
    avp->ColorMap = vp->v_ColorMap;

    if (lastVp != NULL)
    {
      lastVp->v_ViewPort.Next = avp;
    }
    else
    {
      View.ViewPort = avp;
    }

    MakeVPort(&View, avp);

    lastVp = vp;
  }

  View.Modes = 0 | SPRITES;
  MrgCop(&View);

}

EXPORT VOID GfxClose()
{
  struct VIEWPORT* vp;
  UWORD ii;

  for (ii = 0; ii < NumViewPorts; ii++)
  {
    vp = &ViewPorts[ii];

    if (vp->v_ColorMap != NULL)
    {
      FreeColorMap(vp->v_ColorMap);
      vp->v_ColorMap = NULL;
    }

    if (vp->v_Bitmap != NULL)
    {
      FreeBitMap(vp->v_Bitmap);
      vp->v_Bitmap = NULL;
    }
    

    FreeVPortCopLists(&vp->v_ViewPort);
    InitVPort(&vp->v_ViewPort);
  }

  if (View.LOFCprList)
  {
    FreeCprList(View.LOFCprList);
  }

  if (View.SHFCprList != NULL)
  {
    FreeCprList(View.SHFCprList);
  }
  NumViewPorts = 0;

}

EXPORT VOID GfxShow()
{
  UWORD ii, spriteNum, colourRegister;
  struct ViewPort* vp;

  if (IsShown)
  {
    return;
  }
  
  IsShown = TRUE;
  CloseWorkBench();

  IntuitionView = GfxBase->ActiView;
  WaitTOF();
  LoadView(&View);
  WaitTOF();
  WaitTOF();


  FreeSprite(0);
  spriteNum = CursorInitialise();
  colourRegister = 16 + ((spriteNum & 0x06) << 1);

  for (ii = 0; ii < NumViewPorts; ii++)
  {
    vp = &ViewPorts[ii].v_ViewPort;
    LoadRGB32(vp, (CONST ULONG*) &DefaultPalette[0]);
  }

  TRACE("GFX Shown.");
}

EXPORT VOID GfxHide()
{
  if (IsShown == FALSE)
  {
    return;
  }

  CursorShutdown();

  IsShown = FALSE;
  WaitTOF();
  LoadView(IntuitionView);
  WaitTOF();
  WaitTOF();

  OpenWorkBench();

  TRACE("GFX Hidden.");
}

EXPORT BOOL GfxIsPal()
{
  return (GfxBase->DisplayFlags & PAL) == PAL;
}

EXPORT VOID GfxAnimateCursor(UWORD frame)
{
  ULONG* animation;

  switch (frame)
  {
    default:
    case 0:
      animation = &MouseFramePalette0;
      break;
    case 1:
      animation = &MouseFramePalette1;
      break;
    case 2:
      animation = &MouseFramePalette2;
      break;
  }

  LoadRGB32(&ViewPorts[0].v_ViewPort, animation);
  LoadRGB32(&ViewPorts[1].v_ViewPort, animation);
}

EXPORT VOID GfxLoadColours32(UWORD vp, ULONG* table)
{
  TRACEF("Gfx LoadPal. Loading Palette into vp = %ld", vp);

  LoadRGB32(&ViewPorts[vp].v_ViewPort, (CONST ULONG*) table);
}

EXPORT VOID GfxMove(UWORD vp, WORD x, WORD y)
{
  struct VIEWPORT* vpp;
  WORD offset;

  vpp = &ViewPorts[vp];

  offset = vpp->v_WriteOffset;

  Move(&vpp->v_RastPort, x, offset + y);
}

EXPORT WORD GfxTextLength(UWORD vp, STRPTR text, WORD textLength)
{
  return TextLength(&ViewPorts[vp].v_RastPort, text, textLength);
}

EXPORT VOID GfxText(UWORD vp, STRPTR text, WORD textLength)
{
  Text(&ViewPorts[vp].v_RastPort, text, textLength);
}

EXPORT VOID GfxSubmit(UWORD id)
{
  struct VIEWPORT* vp;
  
  vp = &ViewPorts[id];

  if (vp->v_ReadOffset == 0)
  {
    vp->v_ReadOffset = vp->v_Offset;
    vp->v_WriteOffset = 0;
  }
  else
  {
    vp->v_ReadOffset = 0;
    vp->v_WriteOffset = vp->v_Offset;
  }

  vp->v_RasInfo.RxOffset = vp->v_ScrollX;
  vp->v_RasInfo.RyOffset = vp->v_ReadOffset + vp->v_ScrollY;

  ScrollVPort(&vp->v_ViewPort);

  TRACEF("GFX Submit. Id=%ld", id);
}

EXPORT VOID GfxSetScrollOffset(UWORD id, WORD x, WORD y)
{
  struct VIEWPORT* vp;
  vp = &ViewPorts[id];

  vp->v_ScrollX = x;
  vp->v_ScrollY = y;

  vp->v_RasInfo.RxOffset = vp->v_ScrollX;
  vp->v_RasInfo.RyOffset = vp->v_ReadOffset + vp->v_ScrollY;

  ScrollVPort(&vp->v_ViewPort);

  TRACEF("GFX Scroll Offset. Id=%ld, X=%ld, Y=%ld", id, x, y);
}

EXPORT VOID GfxClear(UWORD id)
{
  struct VIEWPORT* vp;
  vp = &ViewPorts[id];

  SetAPen(&vp->v_RastPort, 0);
  SetBPen(&vp->v_RastPort, 0);

  RectFill(
    &vp->v_RastPort,
    0,0,
    vp->v_BitMapWidth-1,
    (vp->v_BitmapHeight*2)-1
  );

  TRACEF("GFX Clear. Id=%ld", id);
}

EXPORT VOID GfxSetAPen(UWORD vp, UWORD pen)
{
  SetAPen(
    &ViewPorts[vp].v_RastPort,
    pen
  );
}

EXPORT VOID GfxSetBPen(UWORD vp, UWORD pen)
{
  SetBPen(
    &ViewPorts[vp].v_RastPort,
    pen
  );
}

EXPORT VOID GfxRectFill(UWORD id, WORD x0, WORD y0, WORD x1, WORD y1)
{
  struct VIEWPORT* vp; 
  vp = &ViewPorts[id];

  WORD offset;

  offset = vp->v_WriteOffset;

  RectFill(
    &vp->v_RastPort,
    x0, offset + y0,
    x1, offset + y1
  );
}

EXPORT VOID GfxBlitBitmap(UWORD viewportId, struct IMAGE* image, WORD dx, WORD dy, WORD sx, WORD sy, WORD sw, WORD sh)
{
  if (NULL == image)
  {
    WARNINGF("GFX Blit. Cannot blit null image to View %ld!", viewportId);
    return;
  }

  struct RastPort* rp;
  WORD offset;

  offset = ViewPorts[viewportId].v_WriteOffset;

  rp = &ViewPorts[viewportId].v_RastPort;
  
  BltBitMapRastPort(CAST_IMAGE_TO_BITMAP(image), sx, sy, rp, dx, dy + offset, sw, sh, 0xC0);

  TRACEF("GFX Blit. Id=%ld Image=%lx", viewportId, image);
}


EXPORT VOID GfxDrawHitBox(UWORD id, struct RECT* rect, STRPTR name, UWORD nameLength)
{
  struct VIEWPORT* vp;
  struct RastPort* rp;
  WORD offset;
  LONG x0, y0, x1, y1, t;

  vp = &ViewPorts[id];
  offset = vp->v_WriteOffset;
  rp = &vp->v_RastPort;

  x0 = rect->rt_Left;
  y0 = rect->rt_Top;
  x1 = rect->rt_Right;
  y1 = rect->rt_Bottom;

  if (x0 < 0)
  {
    x0 = 0;
  }
  else if (x0 >= vp->v_BitMapWidth)
  {
    x0 = vp->v_BitMapWidth - 1;
  }

  if (x1 < 0)
  {
    x1 = 0;
  }
  else if (x1 >= vp->v_BitMapWidth)
  {
    x1 = vp->v_BitMapWidth - 1;
  }

  if (y0 < 0)
  {
    y0 = 0;
  }
  else if (y0 >= vp->v_BitmapHeight)
  {
    y0 = vp->v_BitmapHeight - 1;
  }

  if (y1 < 0)
  {
    y1 = 0;
  }
  else if (y1 >= vp->v_BitmapHeight)
  {
    y1 = vp->v_BitmapHeight - 1;
  }

  if (x0 > x1)
  {
    t = x0;
    x0 = x1;
    x1 = t;
  }

  if (y0 > y1)
  {
    t = y0;
    y0 = y1;
    y1 = t;
  }

  y0 += offset;
  y1 += offset;

  SetAPen(rp, 1);
  SetBPen(rp, 2);

  Move(rp, x0, y0);
  Draw(rp, x1, y0);
  Draw(rp, x1, y1);
  Draw(rp, x0, y1);
  Draw(rp, x0, y0);

  x0 = x0 + x1;
  x0 >>= 1;

  y0 = y0 + y1;
  y0 >>= 1;

  Move(rp, x0, y0);
  Text(rp, name, nameLength);

}

EXPORT VOID Api_LoadPalette(UWORD palette)
{
  struct PALETTE_TABLE pal;
  
  TRACEF("API LoadPalette. Palette = %ld", palette);
  
  Asset_LoadInto_KnownArchive(palette, 0, CT_PALETTE, &pal, sizeof(pal));
  
  GfxLoadColours32(0, (ULONG*)&pal.pt_Data[0]);
}
