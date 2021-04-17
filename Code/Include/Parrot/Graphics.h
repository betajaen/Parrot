/**
    $Id: Log.h 1.5 2021/02/21 10:19:00, betajaen Exp $

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

#ifndef _PARROT_GRAPHICS_H_
#define _PARROT_GRAPHICS_H_

#include <Parrot/Parrot.h>

union _Event;

typedef struct _GraphicsViewInfo GraphicsViewInfo;

struct _GraphicsViewInfo
{
  uint16 viewLeft;
  uint16 viewTop;
  uint16 viewWidth;
  uint16 viewHeight;
  uint8  depth;
  uint8  pad;
  uint16 bitmapWidth;
  uint16 bitmapHeight;
};

typedef enum _CursorType CursorType;

enum _CursorType
{
  CursorType_Crosshair,
  CursorType_Busy,
  CursorType_Count
};

bool gfx_setup();

bool gfx_teardown();

bool gfx_is_visible();

bool gfx_create_views(GraphicsViewInfo views_ary[], uint8 count);

bool gfx_destroy_views();

bool gfx_open_views();

bool gfx_close_views();

bool gfx_handle_event(union _Event* evt);

void gfx_set_cursor(CursorType type);

void gfx_warp_cursor(int32 x, int32 y);

void gfx_wait_tof();

void gfx_try_flipbuffers(uint8 view);

void gfx_apen(uint8 view, uint8 pen);

void gfx_bpen(uint8 view, uint8 pen);

void gfx_abpen(uint8 view, uint8 apen, uint8 bpen);

void gfx_box(uint8 view, uint16 left, uint16 top, uint16 right, uint16 bottom);

#endif
