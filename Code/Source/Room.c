/**
    $Id: Room.c, 1.2 2020/05/11 15:49:00, betajaen Exp $

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
#include <Parrot/View.h>

#include <proto/dos.h>

EXPORT VOID UnpackRoom(struct UNPACKED_ROOM* room, ULONG unpack)
{
  UBYTE ii;
  UWORD id;

  if ((unpack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) == 0)
  {
    room->ur_Room = LoadAssetT(struct ROOM, ArenaChapter, ARCHIVE_UNKNOWN, CT_ROOM, room->ur_Id, CHUNK_FLAG_ARCH_ANY);

    room->ur_Unpacked |= UNPACK_ROOM_ASSET;
  }

  if ((unpack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
    {
      id = room->ur_Room->rm_Backdrops[ii];

      if (0 != id && NULL == room->ur_Backdrops[ii])
      {
        room->ur_Backdrops[ii] = LoadAsset(ArenaRoom, ARCHIVE_UNKNOWN, CT_IMAGE, id, CHUNK_FLAG_ARCH_ANY);
      }
    }

    room->ur_Unpacked |= UNPACK_ROOM_BACKDROPS;
  }

  if ((unpack & UNPACK_ROOM_ENTITIES) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ENTITIES) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
    {
      id = room->ur_Room->rm_Exits[ii];

      if (0 == id)
        break;

      if (NULL == room->ur_Exits[ii])
      {
        room->ur_Exits[ii] = LoadAsset(ArenaRoom, ARCHIVE_UNKNOWN, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
      }

    }

    room->ur_Unpacked |= UNPACK_ROOM_ENTITIES;
  }
}

EXPORT VOID PackRoom(struct UNPACKED_ROOM* room, ULONG pack)
{
  UBYTE ii;
  UWORD backdrop;
  UWORD id;

  if ((pack & UNPACK_ROOM_ENTITIES) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ENTITIES) != 0)
  {
    for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
    {
      id = room->ur_Room->rm_Exits[ii];

      if (0 == id)
        break;

      if (NULL != room->ur_Exits[ii])
      {
        UnloadAsset(ArenaRoom, room->ur_Exits[ii]);
        room->ur_Exits[ii] = NULL;
      }
    }

    room->ur_Unpacked &= ~MAX_ROOM_ENTITIES;
  }

  if ((pack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) != 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
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

STATIC UWORD GetRoomFromExit(struct EXIT* exit)
{
  UWORD archive;
  archive = FindAssetArchive(exit->ex_Target, CT_ENTITY, CHUNK_FLAG_ARCH_ANY);

  return archive;
}

STATIC struct EXIT* FindExit(struct UNPACKED_ROOM* room, UWORD id)
{
  UWORD ii;
  struct EXIT* exit;

  for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
  {
    exit = room->ur_Exits[ii];

    if (NULL == exit)
      break;
    
    if (GET_ASSET_ID(exit) == id)
      break;
  }

  return exit;
}

STATIC BOOL PointInside(struct RECT* rect, WORD x, WORD y)
{
  return x >= rect->rt_Left && x <= rect->rt_Right && y >= rect->rt_Top && y <= rect->rt_Bottom;
}

VOID PlayRoom(UWORD screen, struct ENTRANCE* entrance, struct GAME_INFO* gameInfo)
{
  UWORD screenW, screenH;
  BOOL exitRoom;
  WORD scrollDir;
  struct UNPACKED_ROOM room;
  UWORD evt;
  UWORD mostLeftEdge;
  WORD screenUpdate;
  UWORD cursor;
  UWORD ii;
  struct ENTITY* ent;
  struct EXIT* exit;
  WORD rmMouseX, rmMouseY;
  WORD camRight;

  exitRoom = FALSE;
  scrollDir = 0;
  cursor = CURSOR_BUSY;
  screenUpdate = TRUE;

  Busy();

  /* Get Screen Info */
  ScreenGetWidthHeight(screen, &screenW, &screenH);

  /* Load Room Asset and Backdrops */
  InitStackVar(struct UNPACKED_ROOM, room);

  room.ur_Id = entrance->en_Room;

  UnpackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES);

  mostLeftEdge = room.ur_Room->rm_Width - gameInfo->gi_Width;

  if (mostLeftEdge > 1)
  {
    mostLeftEdge -= 1;
  }

  if (entrance->en_Exit != 0)
  {
    exit = FindExit(&room, entrance->en_Exit);
    room.ur_CamX = exit->ex_HitBox.rt_Left;
    room.ur_CamY = 0;

    if (room.ur_CamX >= mostLeftEdge)
      room.ur_CamX = mostLeftEdge;
    else if (room.ur_CamX < 0)
      room.ur_CamX = 0;
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
          entrance->en_Room = 0;
        }
        break;
        case KC_F1:
        {
          if (room.ur_Exits[0] != NULL)
          {
            exitRoom = TRUE;
            entrance->en_Room = GetRoomFromExit(room.ur_Exits[0]);
            entrance->en_Exit = room.ur_Exits[0]->ex_Target;
          }
          else
          {
            TraceF("There are no exits in this room to go through!");
          }
        }
        break;
        case KC_LEFT:
        {
          scrollDir = -10;
        }
        break;
        case KC_RIGHT:
        {
          scrollDir = +10;
        }
        break;
      }
    }
    
    if ((evt & WE_CURSOR) != 0)
    {
      rmMouseX = EvtMouseX + room.ur_CamX;
      rmMouseY = EvtMouseY + room.ur_CamY;
    }

    if ((evt & WE_SELECT) != 0)
    {
      for (UWORD ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
      {

        exit = room.ur_Exits[ii];

        if (NULL == exit)
          break;

        if (PointInside(&exit->ex_HitBox, rmMouseX, rmMouseY))
        {
          exitRoom = TRUE;
          entrance->en_Room = GetRoomFromExit(exit);
          entrance->en_Exit = exit->ex_Target;
          break;
        }
      }
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
      camRight = room.ur_CamX + gameInfo->gi_Width;
      screenUpdate = TRUE;
    }
    else if (scrollDir > 0 && room.ur_CamX <= mostLeftEdge)
    {
      room.ur_CamX += 1;
      camRight = room.ur_CamX + gameInfo->gi_Width;
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
      ViewBlitBitmap(screen, room.ur_Backdrops[0], 0, 0, room.ur_CamX, room.ur_CamY, screenW, room.ur_Backdrops[0]->im_Height);

      //ScreenRpSetAPen(screen, 15);
      //for (UWORD ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
      //{
      //
      //  exit = room.ur_Exits[ii];
      //
      //  if (NULL == exit)
      //    break;
      //
      //  struct RECT localRect;
      //  localRect.rt_Left = exit->ex_HitBox.rt_Left - room.ur_CamX;
      //  localRect.rt_Top = exit->ex_HitBox.rt_Top - room.ur_CamY;
      //  localRect.rt_Right = exit->ex_HitBox.rt_Right - room.ur_CamX;
      //  localRect.rt_Bottom = exit->ex_HitBox.rt_Bottom - room.ur_CamY;
      //
      //  if (localRect.rt_Left >= 0 && localRect.rt_Right < gameInfo->gi_Width)
      //  {
      //    ScreenRpDrawBox(screen, &localRect);
      //  }
      //
      //
      //}

      ViewSwapBuffers(screen);

      //ScreenSwapBuffers(screen);

      screenUpdate = FALSE;
      scrollDir = 0;
    }


    evt = WaitForEvents(0);
  }

  /* Unload */
  PackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES);
}

