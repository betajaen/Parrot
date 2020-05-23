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
#include <Parrot/String.h>

#include "Asset.h"

#include <proto/exec.h>
#include <proto/dos.h>

struct ARCHIVE* GameArchive;
struct GAME_INFO* GameInfo;
struct PALETTE_TABLE* GamePalette;
struct PALETTE_TABLE* GameCursorPalette;

struct PALETTE_TABLE DefaultPalette =
{
  0,0,
  {
    3 << 16 | 0,
    0X33333333, 0X33333333,  0X33333333,
    0X88888888, 0X88888888,  0X88888888,
    0XFFFFFFFF, 0XFFFFFFFF,  0XFFFFFFFF,
    3 << 16 | 17,
    0,0,0,
    0XFFFFFFFF, 0XFFFFFFFF, 0XFFFFFFFF,
    0XAAAAAAAA, 0XAAAAAAAA, 0XAAAAAAAA,
    0
  }
};

STATIC VOID Busy()
{
  ScreenSetCursor(0, CURSOR_BUSY);
}

STATIC VOID NotBusy()
{
  ScreenSetCursor(0, CURSOR_POINT);
}

STATIC VOID Load(STRPTR path)
{
  UWORD ii;

  Busy();

  for (ii = 0; ii < 16; ii++)
  {
    if (GameInfo->gi_StartTables[ii].tr_ClassType == 0)
      break;

    LoadObjectTable(&GameInfo->gi_StartTables[ii]);
  }

  GamePalette = LoadAssetT(struct PALETTE_TABLE, ArenaGame, ARCHIVE_UNKNOWN, CT_PALETTE, GameInfo->gi_StartPalette, CHUNK_FLAG_ARCH_AGA);

  if (GamePalette == NULL)
  {
    ErrorF("Did not load Game Palette");
  }

  GameCursorPalette = LoadAssetT(struct PALETTE_TABLE, ArenaGame, ARCHIVE_GLOBAL, CT_PALETTE, GameInfo->gi_StartCursorPalette, CHUNK_FLAG_ARCH_AGA);

  if (GamePalette == NULL)
  {
    ErrorF("Did not load Cursor Palette");
  }

  ScreenLoadPaletteTable(0, GamePalette);
  ScreenLoadPaletteTable(0, GameCursorPalette);


  // room = LoadAssetT(struct ROOM, ArenaChapter, 1, CT_ROOM, 1, CHUNK_FLAG_ARCH_ANY);
  // 
  // if (room == NULL)
  // {
  //   RequesterF("OK", "Could not load room!");
  // }
  // else
  // {
  //   UnpackRoom(ArenaRoom, room, &uroom, UNPACK_ROOM_BACKDROPS);
  // 
  //   RequesterF("OK", "Room W=%ld H=%ld", (ULONG)room->rm_Width, (ULONG)room->rm_Height);
  // 
  //   RequesterF("OK", "Backdrop pointer Id=[%ld] == %lx", room->rm_Backdrops[0], uroom.ur_Backdrops[0]);
  // 
  // }


  Delay(50 * 2);


  NotBusy();
}

EXPORT VOID GameStart(STRPTR path)
{
  struct SCREEN_INFO screenInfo;
  struct ROOM* room;
  struct UNPACKED_ROOM uroom;
  UBYTE ii;

  GameArchive = NULL;
  GameInfo = NULL;
  GamePalette = NULL;
  GameCursorPalette = NULL;
  ArenaGame = NULL;
  ArenaChapter = NULL;
  ArenaRoom = NULL;

  InitStackVar(struct UNPACKED_ROOM, uroom);

  ArenaGame = ArenaOpen(16384, 0ul);
  ArenaChapter = ArenaOpen(131072, 0ul);
  ArenaRoom = ArenaOpen(131072, 0ul);

  InitialiseArchives(path);

  GameArchive = OpenArchive(0);

  if (GameArchive == NULL)
  {
    ErrorF("Did not open Game Archive");
  }

  GameInfo = LoadAssetT(struct GAME_INFO, ArenaGame, ARCHIVE_GLOBAL, CT_GAME_INFO, 1, CHUNK_FLAG_ARCH_ANY);

  if (GameInfo == NULL)
  {
    ErrorF("Did not load Game Info");
  }

  CloseArchive(0);

  screenInfo.si_Width = GameInfo->gi_Width;
  screenInfo.si_Height = GameInfo->gi_Height;
  screenInfo.si_Depth = GameInfo->gi_Depth;
  screenInfo.si_Flags = 0;
  screenInfo.si_Left = 0;
  screenInfo.si_Top = 0;
  screenInfo.si_Title = &GameInfo->gi_Title[0];

  ScreenOpen(0, &screenInfo);
  ScreenLoadPaletteTable(0, &DefaultPalette);

  Load(path);


  ScreenClose(0);

  CloseArchives();

  ArenaClose(ArenaRoom);
  ArenaClose(ArenaChapter);
  ArenaClose(ArenaGame);

}

EXPORT VOID GameDelayTicks(UWORD ticks)
{
  Delay(ticks);
}

EXPORT VOID GameDelaySeconds(UWORD seconds)
{
  Delay(seconds * 50);
}
