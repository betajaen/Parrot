/**
    $Id: Game.h, 1.0, 2020/05/11 12:11:00, betajaen Exp $

    Maniac Game Module for Parrot
    =============================

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
#ifndef PARROT_MANIAC_GAME_H
#define PARROT_MANIAC_GAME_H

#include "Parrot.h"
#include "Config.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/parrot.h>

struct GameBase
{
  struct Library     gb_LibBase;
  struct Library*    gb_SysBase;
  ULONG              gb_SegList;
  struct DosLibrary* gb_DOSBase;
  struct ParrotBase* gb_ParrotBase;
  /* Generic Arena used for full life-time objects. Such as the Screen */
  APTR               gb_ArenaGame;
  /* Arena used for chapter objects. */
  APTR               gb_ArenaChapter;
  /* Arena used for current cell objects. */
  APTR               gb_ArenaCell;
  /* Current Screen */
  ULONG              gb_Screen;
};

extern struct GameBase* GameBase;

#define THIS_GAME GameBase

#endif
