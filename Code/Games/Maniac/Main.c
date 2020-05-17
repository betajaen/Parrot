/**
    $Id: Main.c, 1.0, 2020/05/10 07:17:00, betajaen Exp $

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

#include "Game.h"

VOID GameInitialise()
{
  struct SCREEN_INFO info;
  struct ROOM* testRoom;


  THIS_GAME->gb_ArenaGame = ArenaNew(ARENA_GENERIC_SIZE, 0);
  THIS_GAME->gb_ArenaChapter = ArenaNew(ARENA_CHAPTER_SIZE, 0);

  info.si_Depth = 3;
  info.si_Flags = SIF_IS_PUBLIC;
  info.si_Left = 0;
  info.si_Top = 0;
  info.si_Width = 320;
  info.si_Height = 200;
  info.si_Title = "Maniac Mansion";

  THIS_GAME->gb_Screen = ScreenNew(THIS_GAME->gb_ArenaGame, &info);
  NewList(THIS_ROOMS);

  ScreenClear(THIS_SCREEN);
  ScreenSwapBuffers(THIS_SCREEN);
}

VOID GameShutdown()
{
  ArenaDelete(THIS_GAME->gb_ArenaChapter);
  THIS_GAME->gb_ArenaChapter = 0;

  ScreenDelete(THIS_GAME->gb_Screen);
  THIS_GAME->gb_Screen = 0;

  ArenaDelete(THIS_GAME->gb_ArenaGame);
  THIS_GAME->gb_ArenaGame = 0;
}

BOOL OnGameEvent(ULONG event, ULONG data)
{
  return TRUE;
}
