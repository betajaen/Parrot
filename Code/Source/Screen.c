/**
    $Id: Screen.c, 0.1, 2020/05/07 15:55:00, betajaen Exp $

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

#include "Common.h"

#include <proto/exec.h>
#include <proto/intuition.h>

EXPORT ULONG ScreenNew(struct SCREEN_INFO* info)
{
  struct NewScreen newScreen;
  struct Screen* screen;

  if (info->si_Width < 320)
  {
    info->si_Width = 320;
  }
  
  if (info->si_Height < 240)
  {
    info->si_Height = 240;
  }

  if (info->si_Depth < 2)
  {
    info->si_Depth = 2;
  }

  if (NULL == info->si_Title || '\0' == info->si_Title[0])
  {
    info->si_Title = "Parrot";
  }

  newScreen.LeftEdge = info->si_Left;
  newScreen.TopEdge = info->si_Top;
  newScreen.Width = info->si_Width;
  newScreen.Height = info->si_Height;
  newScreen.Depth = info->si_Depth;
  newScreen.ViewModes = 0;

  if ((info->si_Flags & SIF_IS_HIRES) != 0)
  {
    newScreen.ViewModes |= HIRES;
  }

  if ((info->si_Flags & SIF_IS_LACE) != 0)
  {
    newScreen.ViewModes |= LACE;
  }

  newScreen.Type = 0;

  if ((info->si_Flags & SIF_IS_PUBLIC) != 0)
  {
    newScreen.Type |= PUBLICSCREEN;
  }

  newScreen.BlockPen = BLOCKPEN;
  newScreen.DetailPen = DETAILPEN;

  newScreen.Font = NULL;
  newScreen.DefaultTitle = (UBYTE*)info->si_Title;
  newScreen.Gadgets = NULL;
  newScreen.CustomBitMap = NULL;

  screen = OpenScreen(&newScreen);

  return (ULONG) screen;
}


EXPORT VOID ScreenDelete(ULONG screen)
{
  if (0 != screen)
  {
    CloseScreen((struct Screen*) screen);
  }
}

