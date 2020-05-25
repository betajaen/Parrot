/**
    $Id: Room.c, 1.1 2020/05/11 15:49:00, betajaen Exp $

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
#include <Parrot/Asset.h>
#include <Parrot/Screen.h>
#include <Parrot/String.h>
#include <Parrot/Events.h>
#include <Parrot/Requester.h>

#include <proto/dos.h>

EXPORT VOID UnpackRoom(struct UNPACKED_ROOM* room, ULONG unpack)
{
  UBYTE ii;
  UWORD backdrop;

  if ((unpack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) == 0)
  {
    room->ur_Room = LoadAssetT(struct ROOM, ArenaChapter, ARCHIVE_UNKNOWN, CT_ROOM, room->ur_Id, CHUNK_FLAG_ARCH_ANY);

    room->ur_Unpacked |= UNPACK_ROOM_ASSET;
  }

  if ((unpack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) == 0)
  {
    for (ii = 0; ii < 4; ii++)
    {
      backdrop = room->ur_Room->rm_Backdrops[ii];

      if (0 != backdrop && NULL == room->ur_Backdrops[ii])
      {
        room->ur_Backdrops[ii] = LoadAsset(ArenaRoom, ARCHIVE_UNKNOWN, CT_IMAGE, backdrop, CHUNK_FLAG_ARCH_ANY);
      }
    }

    room->ur_Unpacked |= UNPACK_ROOM_BACKDROPS;
  }
}

EXPORT VOID PackRoom(struct UNPACKED_ROOM* room, ULONG pack)
{
  UBYTE ii;
  UWORD backdrop;

  if ((pack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) != 0)
  {
    for (ii = 0; ii < 4; ii++)
    {
      backdrop = room->ur_Room->rm_Backdrops[ii];

      if (0 != backdrop && NULL != room->ur_Backdrops[ii])
      {
        UnloadAsset(ArenaRoom, room->ur_Backdrops[ii]);
        room->ur_Backdrops[ii] = NULL;
      }
    }

    room->ur_Unpacked &= ~UNPACK_ROOM_BACKDROPS;
  }

  if ((pack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) != 0)
  {
    UnloadAsset(ArenaChapter, room->ur_Room);
    room->ur_Room = NULL;
    room->ur_Unpacked &= ~UNPACK_ROOM_ASSET;
  }
}

UWORD PlayRoom(UWORD screen, UWORD roomId, struct GAME_INFO* gameInfo)
{
  UWORD screenW, screenH;
  UWORD exitRoom, nextRoom;
  WORD scrollDir;
  struct UNPACKED_ROOM room;
  UWORD evt;
  UWORD mostLeftEdge;
  WORD screenUpdate;
  UWORD cursor;

  exitRoom = FALSE;
  nextRoom = 0;
  scrollDir = 0;
  cursor = CURSOR_BUSY;
  screenUpdate = TRUE;

  Busy();

  /* Get Screen Info */
  ScreenGetWidthHeight(screen, &screenW, &screenH);

  /* Load Room Asset and Backdrops */
  InitStackVar(struct UNPACKED_ROOM, room);

  room.ur_Id = roomId;

  UnpackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS);

  mostLeftEdge = room.ur_Room->rm_Width - gameInfo->gi_Width;

  if (mostLeftEdge > 1)
  {
    mostLeftEdge -= 1;
  }

  NotBusy();

  evt = 0;

  while (exitRoom == FALSE)
  {
    
    if ((evt & WE_KEY) != 0)
    {
      switch (EvtKey)
      {
        case KC_ESC:
        {
          exitRoom = TRUE;
          nextRoom = 0;
        }
        break;
        case KC_F1:
        {
          if (roomId > 1)
          {
            exitRoom = TRUE;
            nextRoom = roomId - 1;
          }
        }
        break;
        case KC_F2:
        {
          if (roomId < gameInfo->gi_RoomCount)
          {
            exitRoom = TRUE;
            nextRoom = roomId + 1;
          }
        }
        break;
        case KC_LEFT:
        {
          scrollDir = -1;
        }
        break;
        case KC_RIGHT:
        {
          scrollDir = +1;
        }
        break;
      }
    }
    
    if ((evt & WE_CURSOR) != 0)
    {
    }


    if (EvtMouseX < 10)
    {
      scrollDir = -1;
    }
    else if (EvtMouseX > gameInfo->gi_Width - 10)
    {
      scrollDir = +1;
    }
    else
    {
      scrollDir = 0;
    }


    if (scrollDir < 0 && room.ur_CamX >= 0)
    {
      room.ur_CamX -= 1;
      screenUpdate = TRUE;
    }
    else if (scrollDir > 0 && room.ur_CamX <= mostLeftEdge)
    {
      room.ur_CamX += 1;
      screenUpdate = TRUE;
    }


    if (scrollDir < 0 && cursor != CURSOR_W)
    {
      cursor = CURSOR_W;
      ScreenSetCursor(screen, cursor);
    }
    else if (scrollDir > 0 && cursor != CURSOR_E)
    {
      cursor = CURSOR_E;
      ScreenSetCursor(screen, cursor);
    }

    if (scrollDir == 0 && cursor != CURSOR_SELECT)
    {
      cursor = CURSOR_SELECT;
      ScreenSetCursor(screen, cursor);
    }

    if (TRUE == screenUpdate)
    {
      
      /* Show first backdrop on screen */
      ScreenRpBlitBitmap(screen, room.ur_Backdrops[0], 0, 0, room.ur_CamX, room.ur_CamY, screenW, room.ur_Backdrops[0]->im_Height);
      ScreenSwapBuffers(screen);

      screenUpdate = FALSE;
      scrollDir = 0;
    }


    evt = WaitForEvents(0);
  }

  /* Unload */
  PackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS);

  return nextRoom;
}

