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
#include <Parrot/Asset.h>
#include <Parrot/Room.h>

#include "Asset.h"

#include <proto/exec.h>
#include <proto/dos.h>

struct ARCHIVE* GameArchive;
struct GAME_INFO* GameInfo;
struct PALETTE32_TABLE* GamePalette;
struct PALETTE4_TABLE*  GameCursorPalette;

APTR GameScreen;

STATIC VOID Busy()
{
  ScreenSetCursor(GameScreen, CURSOR_BUSY);
}

STATIC VOID NotBusy()
{
  ScreenSetCursor(GameScreen, CURSOR_POINT);
}

EXPORT VOID GameStart(STRPTR path)
{
  struct SCREEN_INFO screenInfo;
  struct ROOM* room;

  UBYTE ii;

  ArenaGame = ArenaNew(16384, 0ul);
  ArenaChapter = ArenaNew(131072, 0ul);
  ArenaRoom = ArenaNew(131072, 0ul);

  SetArchivesPath(path);
  GameArchive = OpenArchive(0);

  GameInfo = LoadAssetT(struct GAME_INFO, ArenaGame, ARCHIVE_GLOBAL, CT_GAME_INFO, 1, CHUNK_FLAG_ARCH_ANY);
  GamePalette = LoadAssetT(struct PALETTE32_TABLE, ArenaGame, ARCHIVE_GLOBAL, CT_PALETTE32, 1, CHUNK_FLAG_ARCH_AGA);
  GameCursorPalette = LoadAssetT(struct PALETTE4_TABLE, ArenaGame, ARCHIVE_GLOBAL, CT_PALETTE4, 1, CHUNK_FLAG_ARCH_AGA);

  screenInfo.si_Width = GameInfo->Width;
  screenInfo.si_Height = GameInfo->Height;
  screenInfo.si_Depth = GameInfo->Depth;
  screenInfo.si_Flags = 0;
  screenInfo.si_Left = 0;
  screenInfo.si_Top = 0;
  screenInfo.si_Title = &GameInfo->Title[0];

  GameScreen = ScreenNew(ArenaGame, &screenInfo);
  ScreenLoadPaletteTable32(GameScreen, GamePalette);
  ScreenLoadPaletteTable4(GameScreen, GameCursorPalette);

  Busy();
  
  /* ArenaRollback(ArenaRoom);
  room = (struct ROOM*) LoadAsset(ArenaChapter, MAKE_ASSET_ID(1, ASSET_TYPE_ROOM, 1));

  if (room == NULL)
  {
    RequesterF("OK", "Could not load room!");
  }
  else
  {
    UnpackRoom(ArenaRoom, room, UNPACK_ROOM_BACKDROPS);

    RequesterF("OK", "Room W=%ld H=%ld", (ULONG)room->rm_Width, (ULONG)room->rm_Height);

    RequesterF("OK", "Backdrop pointer Id=[%lx] == %lx", room->rm_Backdrops[0].ar_Id, room->rm_Backdrops[0].ar_Ptr);
  }
  */

  Delay(50 * 2);

  NotBusy();

  ScreenDelete(GameScreen);

  CloseArchive(GameArchive);

  ArenaDelete(ArenaRoom);
  ArenaDelete(ArenaChapter);
  ArenaDelete(ArenaGame);

}

EXPORT VOID GameDelayTicks(UWORD ticks)
{
  Delay(ticks);
}

EXPORT VOID GameDelaySeconds(UWORD seconds)
{
  Delay(seconds * 50);
}
