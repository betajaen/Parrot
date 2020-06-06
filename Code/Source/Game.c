/**
    $Id: Game.c, 1.2 2020/05/07 08:54:00, betajaen Exp $

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
#include <Parrot/Asset.h>
#include <Parrot/Room.h>
#include <Parrot/String.h>
#include <Parrot/Graphics.h>
#include <Parrot/Input.h>

#include "Asset.h"

#include <proto/exec.h>
#include <proto/dos.h>

struct ARCHIVE* GameArchive;
struct GAME_INFO* GameInfo;
struct PALETTE_TABLE* GamePalette;
struct PALETTE_TABLE* GameCursorPalette;
struct UNPACKED_ROOM  GameRoom;

STATIC VOID Load(STRPTR path)
{
  UWORD ii, roomNo;

  WORD w, lw;

  Busy();

  /* Load all Start Object Tables */

  for (ii = 0; ii < 16; ii++)
  {
    if (GameInfo->gi_StartTables[ii].tr_ClassType == 0)
      break;

    LoadObjectTable(&GameInfo->gi_StartTables[ii]);
  }

  /* Load and Use Start Palette */

  if (0 == GameInfo->gi_StartPalette)
  {
    PARROT_ERR(
      "Unable to start Game!\n"
      "Reason: Starting Palette was None"
      PARROT_ERR_INT("GAME_INFO::gi_StartPalette"),
      GameInfo->gi_StartPalette
    );
  }

  GamePalette = LoadAssetT(struct PALETTE_TABLE, ArenaGame, ARCHIVE_UNKNOWN, CT_PALETTE, GameInfo->gi_StartPalette, CHUNK_FLAG_ARCH_AGA);

  if (NULL == GamePalette)
  {
    PARROT_ERR(
      "Unable to start Game!\n"
      "Reason: Starting Palette Table was not found"
      PARROT_ERR_INT("GAME_INFO::gi_StartPalette"),
      (ULONG)GameInfo->gi_StartCursorPalette
    );
  }

  GfxLoadColours32(0, (ULONG*) &GamePalette->pt_Data[0]);

  ArenaRollback(ArenaChapter);
  ArenaRollback(ArenaRoom);

  NotBusy();

}

EXPORT VOID GameStart(STRPTR path)
{
  struct SCREEN_INFO screenInfo;
  struct ROOM* room;
  struct UNPACKED_ROOM uroom;
  struct ENTRANCE entrance;
  struct VIEW_LAYOUTS viewLayouts;
  struct VIEW_LAYOUT* roomLayout;
  struct VIEW_LAYOUT* verbLayout;

  UBYTE ii;

  GameArchive = NULL;
  GameInfo = NULL;
  GamePalette = NULL;
  GameCursorPalette = NULL;
  ArenaGame = NULL;
  ArenaChapter = NULL;
  ArenaRoom = NULL;

  InitStackVar(struct UNPACKED_ROOM, uroom);

  ArenaGame = ArenaOpen(16384, MEMF_CLEAR);
  ArenaChapter = ArenaOpen(131072, MEMF_CLEAR);
  ArenaRoom = ArenaOpen(131072, MEMF_CLEAR);

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

#if 0

  screenInfo.si_Width = GameInfo->gi_Width;
  screenInfo.si_Height = 128; // GameInfo->gi_Height;
  screenInfo.si_Depth = GameInfo->gi_Depth;
  screenInfo.si_Flags = 0;
  screenInfo.si_Left = 0;
  screenInfo.si_Top = 0;
  screenInfo.si_Title = &GameInfo->gi_Title[0];
  ScreenOpen(0, &screenInfo);
  ScreenLoadPaletteTable(0, &DefaultPalette);

  

  ScreenClose(0);
#else

  viewLayouts.v_NumLayouts = 2;
  viewLayouts.v_Width = GameInfo->gi_Width;
  viewLayouts.v_Height = GameInfo->gi_Height;
  viewLayouts.v_Left = 0;
  viewLayouts.v_Top = 0;
  roomLayout = &viewLayouts.v_Layouts[0];
  verbLayout = &viewLayouts.v_Layouts[1];

  roomLayout->vl_Width = 320;
  roomLayout->vl_Height = 128;
  roomLayout->vl_BitMapWidth = 960;
  roomLayout->vl_BitmapHeight = 128;
  roomLayout->vl_Horizontal = 0;
  roomLayout->vl_Vertical = 0;
  roomLayout->vl_Depth = 4;

  verbLayout->vl_Width = 320;
  verbLayout->vl_Height = 70;
  verbLayout->vl_BitMapWidth = 320;
  verbLayout->vl_BitmapHeight = 70;
  verbLayout->vl_Horizontal = 0;
  verbLayout->vl_Vertical = 130;
  verbLayout->vl_Depth = 2;

  GfxInitialise();

  GfxOpen(&viewLayouts);
  GfxShow();

  Load(path);

  ArenaRollback(ArenaChapter);

  InputInitialise();

  entrance.en_Room = 3; // GameInfo->gi_StartRoom;
  entrance.en_Exit = 0;

  while (entrance.en_Room != 0 && InEvtForceQuit == FALSE)
  {
    ArenaRollback(ArenaRoom);

    /* Start First Room */
    PlayRoom(0, &entrance, GameInfo);
  }

  InputExit();

  GfxHide();
  GfxClose();

#endif
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
