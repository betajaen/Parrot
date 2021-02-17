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
  PtSigned16  OffsetX, OffsetY;
  PtUnsigned16 Height;
  PtUnsigned16 Data[2 * 24];
};

struct VIEWPORT
{
  struct ViewPort     v_ViewPort;
  struct RasInfo      v_RasInfo;
  struct RastPort     v_RastPort;
  struct BitMap*      v_Bitmap;
  struct ColorMap*    v_ColorMap;
  PtUnsigned16               v_Offset;
  PtUnsigned16               v_ReadOffset;
  PtUnsigned16               v_WriteOffset;
  PtUnsigned16               v_Width;
  PtUnsigned16               v_Height;
  PtUnsigned16               v_BitMapWidth;
  PtUnsigned16               v_BitmapHeight;
  PtSigned16                v_Horizontal;
  PtSigned16                v_Vertical;
  PtUnsigned16               v_Depth;
  PtSigned16                v_ScrollX;
  PtSigned16                v_ScrollY;
};

struct View*         IntuitionView;
struct View          View;
struct VIEWPORT      ViewPorts[MAX_VIEW_LAYOUTS];
PtUnsigned16                ScreenWidth;
PtUnsigned16                ScreenHeight;
PtUnsigned16                NumViewPorts;
PtUnsigned16                IsShown;

STATIC PtUnsigned32 DefaultPalette[] =
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

STATIC PtUnsigned32 MouseFramePalette0[8] =
{
    2 << 16 | 17,
    0,0,0,
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
    0,
};

STATIC PtUnsigned32 MouseFramePalette1[8] =
{
    2 << 16 | 17,
    0,0,0,
    0XAAAAAAAA, 0XAAAAAAAA, 0XAAAAAAAA,
    0,
};

STATIC PtUnsigned32 MouseFramePalette2[8] =
{
    2 << 16 | 17,
    0,0,0,
    0X55555555, 0X55555555, 0X55555555,
    0,
};

void GfxInitialise()
{
  NumViewPorts = 0;
  IsShown = FALSE;
  ScreenWidth = 0;
  ScreenHeight = 0;
  IntuitionView = NULL;

  InitView(&View);
}

void ViewExitNow()
{
  GfxHide();
  GfxClose();
}

void GfxOpen(PtViewLayouts* layouts)
{
  struct VIEWPORT* vp, *lastVp;
  PtViewLayout* vl;
  struct ViewPort* avp;
  struct RastPort* rp;
  PtUnsigned16 ii, jj, numColours;

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

void GfxClose()
{
  struct VIEWPORT* vp;
  PtUnsigned16 ii;

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

void GfxShow()
{
  PtUnsigned16 ii, spriteNum, colourRegister;
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
    LoadRGB32(vp, (CONST PtUnsigned32*) &DefaultPalette[0]);
  }

  TRACE("GFX Shown.");
}

void GfxHide()
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

PtBool GfxIsPal()
{
  return (GfxBase->DisplayFlags & PAL) == PAL;
}

void GfxAnimateCursor(PtUnsigned16 frame)
{
  PtUnsigned32* animation;

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

void GfxLoadColours32(PtUnsigned16 vp, PtUnsigned32* table)
{
  TRACEF("Gfx LoadPal. Loading Palette into vp = %ld", vp);

  LoadRGB32(&ViewPorts[vp].v_ViewPort, (CONST PtUnsigned32*) table);
}

void GfxMove(PtUnsigned16 vp, PtSigned16 x, PtSigned16 y)
{
  struct VIEWPORT* vpp;
  PtSigned16 offset;

  vpp = &ViewPorts[vp];

  offset = vpp->v_WriteOffset;

  Move(&vpp->v_RastPort, x, offset + y);
}

PtSigned16 GfxTextLength(PtUnsigned16 vp, STRPTR text, PtSigned16 textLength)
{
  return TextLength(&ViewPorts[vp].v_RastPort, text, textLength);
}

void GfxText(PtUnsigned16 vp, STRPTR text, PtSigned16 textLength)
{
  Text(&ViewPorts[vp].v_RastPort, text, textLength);
}

void GfxSubmit(PtUnsigned16 id)
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

void GfxSetScrollOffset(PtUnsigned16 id, PtSigned16 x, PtSigned16 y)
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

void GfxClear(PtUnsigned16 id)
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

void GfxSetAPen(PtUnsigned16 vp, PtUnsigned16 pen)
{
  SetAPen(
    &ViewPorts[vp].v_RastPort,
    pen
  );
}

void GfxSetBPen(PtUnsigned16 vp, PtUnsigned16 pen)
{
  SetBPen(
    &ViewPorts[vp].v_RastPort,
    pen
  );
}

void GfxRectFill(PtUnsigned16 id, PtSigned16 x0, PtSigned16 y0, PtSigned16 x1, PtSigned16 y1)
{
  struct VIEWPORT* vp; 
  vp = &ViewPorts[id];

  PtSigned16 offset;

  offset = vp->v_WriteOffset;

  RectFill(
    &vp->v_RastPort,
    x0, offset + y0,
    x1, offset + y1
  );
}

void GfxBlitBitmap(PtUnsigned16 viewportId, struct IMAGE* image, PtSigned16 dx, PtSigned16 dy, PtSigned16 sx, PtSigned16 sy, PtSigned16 sw, PtSigned16 sh)
{
  if (NULL == image)
  {
    WARNINGF("GFX Blit. Cannot blit null image to View %ld!", viewportId);
    return;
  }

  struct RastPort* rp;
  PtSigned16 offset;

  offset = ViewPorts[viewportId].v_WriteOffset;

  rp = &ViewPorts[viewportId].v_RastPort;
  
  BltBitMapRastPort(CAST_IMAGE_TO_BITMAP(image), sx, sy, rp, dx, dy + offset, sw, sh, 0xC0);

  TRACEF("GFX Blit. Id=%ld Image=%lx", viewportId, image);
}


void GfxDrawHitBox(PtUnsigned16 id, struct Rect* rect, STRPTR name, PtUnsigned16 nameLength)
{
  struct VIEWPORT* vp;
  struct RastPort* rp;
  PtSigned16 offset;
  PtSigned32 x0, y0, x1, y1, t;

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

void Api_LoadPalette(PtUnsigned16 palette)
{
  struct PaletteTable pal;
  
  TRACEF("API LoadPalette. Palette = %ld", palette);
  
  Asset_LoadInto_KnownArchive(palette, 0, PT_AT_PALETTE, &pal, sizeof(pal));
  
  GfxLoadColours32(0, (PtUnsigned32*)&pal.pt_Data[0]);
}
