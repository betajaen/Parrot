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
#include <Parrot/View.h>

#include <proto/exec.h>

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


struct VIEWPORT
{
  struct ViewPort  v_ViewPort;
  struct RasInfo   v_RasInfo;
  struct RastPort  v_RastPort;
  struct BitMap*   v_BitMaps[2];
  struct ColorMap* v_ColorMap;
  UWORD            v_ReadBitMap;
  UWORD            v_WriteBitMap;
  UWORD            v_Width;
  UWORD            v_Height;
  UWORD            v_BitMapWidth;
  UWORD            v_BitmapHeight;
  WORD             v_Horizontal;
  WORD             v_Vertical;
  UWORD            v_Depth;
};

struct View*         IntuitionView;
struct View          View;
struct VIEWPORT      ViewPorts[MAX_VIEW_LAYOUTS];
UWORD                ScreenWidth;
UWORD                ScreenHeight;
UWORD                NumViewPorts;
UWORD                IsShown;

EXPORT VOID ViewInitialise()
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
  ViewHide();
  ViewClose();
}

EXPORT VOID ViewOpen(struct VIEW_LAYOUTS* layouts)
{
  struct VIEWPORT* vp, *lastVp;
  struct VIEW_LAYOUT* vl;
  struct ViewPort* avp;
  struct RastPort* rp;
  UWORD ii, jj;

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

    vp->v_ReadBitMap = 1;
    vp->v_WriteBitMap = 0;

    vp->v_Width = vl->vl_Width;
    vp->v_Height = vl->vl_Height;
    vp->v_BitMapWidth = vl->vl_BitMapWidth;
    vp->v_BitmapHeight = vl->vl_BitmapHeight;
    vp->v_Horizontal = vl->vl_Horizontal;
    vp->v_Vertical = vl->vl_Vertical;
    vp->v_Depth = vl->vl_Depth;
    
    for (jj = 0; jj < 2; jj++)
    {
      vp->v_BitMaps[jj] = AllocBitMap(vp->v_BitMapWidth, vp->v_BitmapHeight, vp->v_Depth,
        BMF_DISPLAYABLE | BMF_INTERLEAVED | BMF_CLEAR, NULL);
    }

    avp = &vp->v_ViewPort;
    InitVPort(avp);

    avp->RasInfo = &vp->v_RasInfo;
    avp->RasInfo->BitMap = vp->v_BitMaps[0];
    avp->RasInfo->Next = NULL;
    avp->RasInfo->RxOffset = 0;
    avp->RasInfo->RyOffset = 0;
    avp->DWidth = vl->vl_Width;
    avp->DHeight = vl->vl_Height;
    
    vp->v_ColorMap = GetColorMap(32);
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

EXPORT VOID ViewClose()
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

    if (vp->v_BitMaps[0] != NULL)
    {
      FreeBitMap(vp->v_BitMaps[0]);
      vp->v_BitMaps[0] = NULL;
    }
    
    if (vp->v_BitMaps[1] != NULL)
    {
      FreeBitMap(vp->v_BitMaps[1]);
      vp->v_BitMaps[1] = NULL;
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

EXPORT VOID ViewShow()
{
  if (IsShown)
  {
    return;
  }
  
  IsShown = TRUE;
  IntuitionView = GfxBase->ActiView;
  WaitTOF();
  LoadView(&View);
  WaitTOF();
  WaitTOF();
}

EXPORT VOID ViewHide()
{
  if (IsShown == FALSE)
  {
    return;
  }

  IsShown = FALSE;
  WaitTOF();
  LoadView(IntuitionView);
  WaitTOF();
  WaitTOF();
}

EXPORT BOOL ViewIsPal()
{
  return (GfxBase->DisplayFlags & PAL) == PAL;
}


EXPORT VOID ViewLoadColours32(UWORD vp, ULONG* table)
{
  LoadRGB32(&ViewPorts[vp].v_ViewPort, table);
}
