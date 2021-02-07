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
#include <Parrot/Script.h>

#include <Parrot/Asset.h>
#include <Parrot/Squawk.h>

#include <proto/exec.h>
#include <proto/dos.h>

struct ARCHIVE* GameArchive;
struct GAME_INFO* GameInfo;
struct UNPACKED_ROOM  GameRoom;

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
  ArenaGame = NULL;
  ArenaChapter = NULL;
  ArenaRoom = NULL;

  InitStackVar(uroom);

  ArenaGame = ArenaOpen(MAKE_NODE_ID('G', 'A', 'M', 'E'), 16384, MEMF_CLEAR);
  ArenaChapter = ArenaOpen(MAKE_NODE_ID('C', 'H', 'A', 'P'), 131072, MEMF_CLEAR);
  ArenaRoom = ArenaOpen(MAKE_NODE_ID('R', 'O', 'O', 'M'), 131072, MEMF_CLEAR);
  ArenaFrameTemp = ArenaOpen(MAKE_NODE_ID('F', 'R', 'T', 'M'), 16384, MEMF_CLEAR);

  InitArchives();

  /*
    Load the first and only 1 of a GameInfo
    As the ID may not necessarily be 1.
  */
  if (1 != GetAllAssetsFromArchive(CT_GAME_INFO, 0, ArenaGame, (struct ANY_ASSET**) &GameInfo, 1))
  {
    PARROT_ERR0(
      "Could not start Game!\n"
      "Reason: (1) GAME_INFO structure was not found in the first archive"
    );
  }

  LoadAssetTables(0, 0, GameInfo->gi_NumAssetTables);

  VmInitialise();


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

  ArenaRollback(ArenaFrameTemp);
  ArenaRollback(ArenaChapter);

  InputInitialise();

#if 1
  
  RunScriptNow(GameInfo->gi_StartScript, 0, ArenaFrameTemp);
  ArenaRollback(ArenaFrameTemp);
  
#else
  entrance.en_Room = 3; // GameInfo->gi_StartRoom;
  entrance.en_Exit = 0;

  while (entrance.en_Room != 0 && InEvtForceQuit == FALSE)
  {
    ArenaRollback(ArenaRoom);

    /* Start First Room */
    PlayRoom(0, &entrance, GameInfo);
  }
#endif

  InputExit();

  GfxHide();
  GfxClose();

#endif
  
  VmShutdown();

#if 0
  CloseArchives();
#endif

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


EXPORT VOID GameLoadPalette(UWORD palette)
{
  struct PALETTE_TABLE pal;
  
  GetAssetInto(palette, 0, CT_PALETTE, (struct ANY_ASSET*) &pal, sizeof(pal));

  GfxLoadColours32(0, (ULONG*)&pal.pt_Data[0]);
}
