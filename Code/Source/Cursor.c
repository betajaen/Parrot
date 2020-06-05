/**
    $Id: Cursor.c, 1.2 2020/06/05 20:47:00, betajaen Exp $

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

struct CURSOR_IMAGE
{
  WORD  OffsetX, OffsetY;
  UWORD Height;
  UWORD Data[2 * 24];
};


struct SimpleSprite CursorSprite;
WORD CursorOffsetX;
WORD CursorOffsetY;
WORD CursorX;
WORD CursorY;
WORD CursorXLimit;
WORD CursorYLimit;

STATIC ULONG DefaultPalette[] =
{
    2 << 16 | 17,
    0,0,0,
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
    0
};

CHIP struct CURSOR_IMAGE Cursors[] = {
  {
    -7,-7,15,
    {
        0x0000,0x0000,
        0x380 ,0x0   ,
        0x280 ,0x100 ,
        0x280 ,0x100 ,
        0x280 ,0x100 ,
        0x380 ,0x0   ,
        0x0   ,0x0   ,
        0xf83e,0x0   ,
        0x8822,0x701c,
        0xf83e,0x0   ,
        0x0   ,0x0   ,
        0x380 ,0x0   ,
        0x280 ,0x100 ,
        0x280 ,0x100 ,
        0x280 ,0x100 ,
        0x380 ,0x0   ,
        0x0000,0x0000
    }
  },
  {
    -7, -7, 15,
    {
      0x0000,0x0000,
      0xfff0,0x0   ,
      0x9058,0x6fa0,
      0x9148,0x6eb0,
      0x9148,0x6eb0,
      0x9048,0x6fb0,
      0x9fc8,0x6030,
      0x8008,0x7ff0,
      0x9fc8,0x6030,
      0xa028,0x5fd0,
      0xafa8,0x5050,
      0xa028,0x5fd0,
      0xa028,0x5fd0,
      0xfff8,0x0   ,
      0x0000,0x0000
    }
  }
};


EXPORT UWORD CursorInitialise()
{
  UWORD spriteNum;

  FreeSprite(0);
  spriteNum = GetSprite(&CursorSprite, 0);

  CursorSprite.x = 0;
  CursorSprite.y = 0;
  CursorSprite.height = Cursors[0].Height;

  CursorX = 0;
  CursorY = 0;
  CursorOffsetX = Cursors[0].OffsetX;
  CursorOffsetY = Cursors[0].OffsetY;

  CursorXLimit = 319;
  CursorYLimit = 199;

  ChangeSprite(NULL, &CursorSprite, (APTR)&Cursors[0].Data);
  MoveSprite(NULL, &CursorSprite, CursorX + CursorOffsetX, CursorY + CursorOffsetY);

  return spriteNum;
}

EXPORT VOID CursorShutdown()
{
  FreeSprite(CursorSprite.num);
}

EXPORT VOID Busy()
{

}

EXPORT VOID NotBusy()
{

}

EXPORT VOID CursorSetType(UWORD cursor)
{

}

EXPORT UWORD CursorGetType()
{

}

EXPORT VOID CursorSetPos(WORD x, WORD y)
{

}

EXPORT VOID CursorGetPos(WORD* x, WORD* y)
{

}
