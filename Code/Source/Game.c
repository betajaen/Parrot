/**
    $Id: Game.c, 0.1, 2020/05/07 08:54:00, betajaen Exp $

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
#include <Parrot/Archive.h>
#include <Parrot/Arena.h>
#include <Parrot/Requester.h>
#include <Parrot/Screen.h>

#include "Asset.h"

#include <proto/exec.h>
#include <proto/dos.h>

struct ARCHIVE* GameArchive;
struct CHUNK_GAME_INFO GameInfo;
struct CHUNK_PALETTE_32 GamePalette;
struct CHUNK_SPRITE_PALETTE_32 GameCursorPalette;

APTR GameScreen;

EXPORT VOID GameStart(STRPTR path)
{
  struct SCREEN_INFO screenInfo;

  ArenaGame = ArenaNew(16384, 0ul);
  ArenaChapter = NULL;
  ArenaRoom = NULL;

  SetArchivesPath(path);
  GameArchive = OpenArchive(0);

  ReadAssetFromArchive(GameArchive, CHUNK_GAME_INFO_ID, (APTR) &GameInfo, sizeof(struct CHUNK_GAME_INFO));
  ReadAssetFromArchive(GameArchive, CHUNK_PALETTE_32_ID, (APTR)&GamePalette, sizeof(struct CHUNK_PALETTE_32));
  ReadAssetFromArchive(GameArchive, CHUNK_SPRITE_PALETTE_32_ID, (APTR)&GameCursorPalette, sizeof(struct CHUNK_SPRITE_PALETTE_32));

  screenInfo.si_Width = GameInfo.Width;
  screenInfo.si_Height = GameInfo.Height;
  screenInfo.si_Depth = GameInfo.Depth;
  screenInfo.si_Flags = 0;
  screenInfo.si_Left = 0;
  screenInfo.si_Top = 0;
  screenInfo.si_Title = &GameInfo.Title[0];

  GameScreen = ScreenNew(ArenaGame, &screenInfo);
  ScreenLoadPaletteTable32(GameScreen, (ULONG*) &GamePalette.Palette32);
  ScreenLoadPaletteTable32(GameScreen, (ULONG*) &GameCursorPalette.Palette);
  
  Delay(50 * 3);
  ScreenDelete(GameScreen);

  CloseArchive(GameArchive);
}

EXPORT VOID GameDelayTicks(UWORD ticks)
{
  Delay(ticks);
}

EXPORT VOID GameDelaySeconds(UWORD seconds)
{
  Delay(seconds * 50);
}
