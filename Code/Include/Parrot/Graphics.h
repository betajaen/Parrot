/**
    $Id: Graphics.h 1.2 2020/05/29 17:02:00, betajaen Exp $

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

#ifndef PARROT_GRAPHICS_H
#define PARROT_GRAPHICS_H

#include <Parrot/Parrot.h>



typedef struct PtViewLayout_t PtViewLayout;

struct PtViewLayout_t
{
  PtUnsigned16  vl_Width;
  PtUnsigned16  vl_Height;
  PtUnsigned16  vl_BitMapWidth;
  PtUnsigned16  vl_BitmapHeight;
  PtSigned16   vl_Horizontal;
  PtSigned16   vl_Vertical;
  PtUnsigned16  vl_Depth;
};

typedef struct PtViewLayouts_t PtViewLayouts;

struct PtViewLayouts_t
{
  PtSigned32    v_Left;
  PtSigned32    v_Top;
  PtSigned32    v_Width;
  PtSigned32    v_Height;
  PtUnsigned16  v_NumLayouts;
  PtViewLayout  v_Layouts[MAX_VIEW_LAYOUTS];
};



PtPublic void GfxInitialise();

PtPublic void GfxOpen(PtViewLayouts* view);

PtPublic void GfxClose();

PtPublic void GfxShow();

PtPublic void GfxHide();

PtPublic PtBool GfxIsPal();

PtPublic void GfxSetScrollOffset(PtUnsigned16 vp, PtSigned16 x, PtSigned16 y);

PtPublic void GfxSubmit(PtUnsigned16 id);

PtPublic void GfxClear(PtUnsigned16 id);

PtPublic void GfxSetAPen(PtUnsigned16 vp, PtUnsigned16 pen);

PtPublic void GfxSetBPen(PtUnsigned16 vp, PtUnsigned16 pen);

PtPublic void GfxRectFill(PtUnsigned16 vp, PtSigned16 x0, PtSigned16 y0, PtSigned16 x1, PtSigned16 y1);

PtPublic void GfxBlitBitmap(PtUnsigned16 viewportId, struct IMAGE* image, PtSigned16 dx, PtSigned16 dy, PtSigned16 sx, PtSigned16 sy, PtSigned16 sw, PtSigned16 sh);

PtPublic void GfxLoadColours32(PtUnsigned16 vp, PtUnsigned32* table);

PtPublic void GfxMove(PtUnsigned16 vp, PtSigned16 x, PtSigned16 y);

PtPublic PtSigned16 GfxTextLength(PtUnsigned16 vp, STRPTR text, PtSigned16 textLength);

PtPublic void GfxText(PtUnsigned16 vp, STRPTR text, PtSigned16 textLength);

PtPublic void GfxDrawHitBox(PtUnsigned16 id, struct Rect* rect, STRPTR name, PtUnsigned16 nameLength);

PtPublic void GfxAnimateCursor(PtUnsigned16 frame);

PtPublic void Busy();

PtPublic void NotBusy();

PtPublic PtUnsigned16 CursorInitialise();

PtPublic void CursorShutdown();

PtPublic void CursorSetType(PtUnsigned16 cursor);

PtPublic PtUnsigned16 CursorGetType();

PtPublic void CursorSetPos(PtSigned16 x, PtSigned16 y);

PtPublic void CursorGetPos(PtSigned16* x, PtSigned16* y);

#endif
