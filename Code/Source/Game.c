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
#include <Parrot/Arena.h>
#include <Parrot/Requester.h>
#include <Parrot/Room.h>
#include <Parrot/String.h>
#include <Parrot/Graphics.h>
#include <Parrot/Input.h>
#include <Parrot/Vm.h>
#include <Parrot/Log.h>

#include <Parrot/Squawk.h>

#include <proto/exec.h>
#include <proto/dos.h>

PtGameInfo GameInfo;

struct UNPACKED_ROOM  GameRoom;

void GameStart(STRPTR path)
{
  struct SCREEN_INFO screenInfo;
  struct Room* room;
  struct UNPACKED_ROOM uroom;
  struct RoomEntrance entrance;
  PtViewLayouts viewLayouts;
  PtViewLayout* roomLayout;
  PtViewLayout* verbLayout;

  PtUnsigned8 ii;

  ArenaGame = NULL;
  ArenaChapter = NULL;
  ArenaRoom = NULL;

  ClearMem(uroom);
  ClearMem(GameInfo);

  ArenaGame = ArenaOpen(MAKE_NODE_ID('G', 'A', 'M', 'E'), 16384, MEMF_CLEAR);
  ArenaChapter = ArenaOpen(MAKE_NODE_ID('C', 'H', 'A', 'P'), 131072, MEMF_CLEAR);
  ArenaRoom = ArenaOpen(MAKE_NODE_ID('R', 'O', 'O', 'M'), 131072, MEMF_CLEAR);
  ArenaFrameTemp = ArenaOpen(MAKE_NODE_ID('F', 'R', 'T', 'M'), 16384, MEMF_CLEAR);

  Archives_Initialise();

  TRACE("GAME Start. Attempting to load Game Info.");

  if (FALSE == Asset_LoadInto_KnownArchive(1, 0, PT_AT_GAME_INFO, (PtAsset*) &GameInfo, sizeof(GameInfo)))
  {
    ERROR("GAME_INFO structure was not found in the first archive");
    
    PARROT_ERR0(
      "Could not start Game!\n"
      "Reason: (1) GAME_INFO structure was not found in the first archive"
    );
  }

  TRACEF("GAME Start. Id=%ld, Version=%ld, Width=%ld, Height=%ld, Depth=%ld", GameInfo.gi_GameId, GameInfo.gi_GameVersion, GameInfo.gi_Width, GameInfo.gi_Height, GameInfo.gi_Depth);

  AssetTables_Load(0, 0, GameInfo.gi_NumAssetTables);

  PtVm_Initialise();

  viewLayouts.v_NumLayouts = 2;
  viewLayouts.v_Width = GameInfo.gi_Width;
  viewLayouts.v_Height = GameInfo.gi_Height;
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

  ArenaRollback(ArenaFrameTemp);
  ArenaRollback(ArenaChapter);

  InputInitialise();
  
  PtVm_RunScriptNow(GameInfo.gi_StartScript, 0, ArenaFrameTemp);
  
  InputExit();

  GfxHide();
  GfxClose();

  PtVm_Shutdown();

  /* CloseArchives(); */

  ArenaClose(ArenaRoom);
  ArenaClose(ArenaChapter);
  ArenaClose(ArenaGame);

}

void Api_DelayTicks(PtUnsigned16 ticks)
{
  Delay(ticks);
}

void Api_DelaySeconds(PtUnsigned16 seconds)
{
  Delay(seconds);
}
