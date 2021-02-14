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
#include <Parrot/Squawk.h>
#include <Parrot/String.h>
#include <Parrot/Requester.h>
#include <Parrot/Graphics.h>
#include <Parrot/Input.h>
#include <Parrot/Game.h>
#include <Parrot/Log.h>

#include <proto/dos.h>
#include <proto/graphics.h>

EXTERN WORD CursorX;
EXTERN WORD CursorY;

STATIC VOID PlayRoomDebug(struct UNPACKED_ROOM* room);

EXPORT VOID UnpackRoom(struct UNPACKED_ROOM* room, ULONG unpack, UWORD roomId)
{
  UBYTE ii;
  UWORD id;

  VERBOSEF("ROOM Unpack. Unpacking Room Id=%ld", roomId);


  if ((unpack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) == 0)
  {
    if ( FALSE == Asset_LoadInto(roomId, 1, CT_ROOM, (struct ANY_ASSET*) room, sizeof(struct ROOM)) )
    {
      ERRORF("ROOM Unpack. Cannot find room asset Id=%ld, UnpackFlags=%lx", (ULONG) roomId, unpack);
      return;
    }

    room->ur_Unpacked |= UNPACK_ROOM_ASSET;
  }

  if ((unpack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
    {
      id = room->ur_Room.rm_Backdrops[ii];

      if (0 != id)
      {
        TRACEF("ROOM Unpack. Unpacking Backdrop Id=%ld", id);

        struct IMAGE* backdrop = &room->ur_Backdrops[ii];

        Asset_LoadInto_Callback(id, 1, CT_IMAGE, (struct ANY_ASSET*) backdrop, sizeof(struct IMAGE), Asset_CallbackFor_Image);

        TRACEF("ROOM Unpack. Id=%ld, Loaded Backdrop = %ld, Width=%ld, Height=%ld", roomId, id, backdrop->im_Width, backdrop->im_Height);
        
        TRACEF("ROOM. Unpack. Backdrop Plane[0] = %lx", backdrop->im_Planes[0]);
      }
      else
      {
        room->ur_Backdrops[ii].im_Planes[0] = NULL;
        if (ii == 0)
        {
          WARNINGF("ROOM Unpack. Room Id=%ld, Has no backdrop assigned for slot %ld!", roomId, ii);
        }
        else
        {
          TRACEF("ROOM Unpack. No backdrop for slot %ld!", ii);
        }
      }
    }

    room->ur_Unpacked |= UNPACK_ROOM_BACKDROPS;
  }

  if ((unpack & UNPACK_ROOM_ENTITIES) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ENTITIES) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
    {
      id = room->ur_Room.rm_Exits[ii];

      if (0 == id)
        break;

      if (NULL == room->ur_Exits[ii])
      {
#if 0
        room->ur_Exits[ii] = LoadAsset(ArenaRoom, room->ur_Id, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
#endif
      }

    }

    for (ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
    {
      id = room->ur_Room.rm_Entities[ii];

      if (0 == id)
        break;

      if (NULL == room->ur_Entities[ii])
      {
#if 0
        room->ur_Entities[ii] = LoadAsset(ArenaRoom, room->ur_Id, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
#endif
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
    for (ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
    {
      id = room->ur_Room.rm_Entities[ii];

      if (0 == id)
        break;

      if (NULL != room->ur_Entities[ii])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Entities[ii]);
#endif
        room->ur_Entities[ii] = NULL;
      }
    }

    for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
    {
      id = room->ur_Room.rm_Exits[ii];

      if (0 == id)
        break;

      if (NULL != room->ur_Exits[ii])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Exits[ii]);
#endif
        room->ur_Exits[ii] = NULL;
      }
    }

    room->ur_Unpacked &= ~MAX_ROOM_ENTITIES;
  }

  if ((pack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) != 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
    {
      backdrop = room->ur_Room.rm_Backdrops[ii];

      if (0 != backdrop && NULL != room->ur_Backdrops[ii].im_Planes[0])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Backdrops[ii]);
#endif
        room->ur_Backdrops[ii].im_Planes[0] = NULL;
      }
    }

    room->ur_Unpacked &= ~UNPACK_ROOM_BACKDROPS;
  }

  if ((pack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) != 0)
  {
#if 0
    UnloadAsset(ArenaChapter, room->ur_Room);
#endif
    room->ur_Unpacked &= ~UNPACK_ROOM_ASSET;
  }
}

STATIC UWORD GetRoomFromExit(struct EXIT* exit)
{
  UWORD archive;
#if 0
  archive = FindAssetArchive(exit->ex_Target, CT_ENTITY, CHUNK_FLAG_ARCH_ANY);
#endif

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

VOID Room_Run(UWORD screen, struct ENTRANCE* entrance)
{
  struct UNPACKED_ROOM room;
  struct INPUTEVENT evt;
  UWORD screenW, screenH;
  BOOL exitRoom;
  UWORD mostLeftEdge;
  UWORD ii;
  WORD rmMouseX, rmMouseY;
  BOOL updateCaption;
  BOOL hasCaption;
  BOOL eventThisFrame;
  UWORD mouseFrameTimer;
  UWORD mouseFrame;

  exitRoom = FALSE;

  Busy();

  /* Get Screen Info */
  screenW = 320;
  screenH = 128;

  /* Load Room Asset and Backdrops */
  InitStackVar(room);

  room.ur_UpdateFlags = UFLG_ALL;
  room.ur_Verbs.vb_Allowed = VERB_NONE | VERB_WALK;
  room.ur_Verbs.vb_Selected = VERB_NONE;
  

  TRACEF("ROOM Run. Unpacking. Id = %ld. UpdateFlags = %lx, Verbs = %lx", entrance->en_Room, room.ur_UpdateFlags, room.ur_Verbs.vb_Allowed);

  UnpackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES, entrance->en_Room);

  TRACE("ROOM Run. Unpacked");

  mostLeftEdge = room.ur_Room.rm_Width - GameInfo.gi_Width;

  if (mostLeftEdge > 1)
  {
    mostLeftEdge -= 1;
  }

  if (entrance->en_Exit != 0 && room.ur_Room.rm_Width > GameInfo.gi_Width)
  {
    struct EXIT* exit;

    exit = FindExit(&room, entrance->en_Exit);
    room.ur_CamX = exit->ex_HitBox.rt_Left;
    room.ur_CamY = 0;

    if (room.ur_CamX > mostLeftEdge)
      room.ur_CamX = mostLeftEdge;
    else if (room.ur_CamX < 0)
      room.ur_CamX = 0;

    room.ur_UpdateFlags |= UFLG_SCROLL;
  }

  TRACE("ROOM Run. Cls.");
  
  NotBusy();
  GfxClear(0);
  GfxClear(1);

  GfxSubmit(0);
  GfxSubmit(1);
  WaitTOF();

  TRACEF("ROOM Run. Entering Loop. ExitRoom=%ld, ForceQuit=%ld", exitRoom, InEvtForceQuit);

  mouseFrameTimer = 0;
  mouseFrame = 0;

  while (exitRoom == FALSE && InEvtForceQuit == FALSE)
  {
    eventThisFrame = FALSE;

    while (PopEvent(&evt))
    {
      eventThisFrame = TRUE;
      switch (evt.ie_Type)
      {
        case IET_KEYUP:
        {
          if (evt.ie_Code == KC_ESC)
          {
            exitRoom = TRUE;
            entrance->en_Room = 0;
            TRACE("ROOM Run. Escaping room via ESC key");
          }
          else if (evt.ie_Code == KC_F1)
          {
            GfxClear(0);
            GfxClear(1);

            GfxSubmit(0);
            GfxSubmit(1);
            WaitTOF();

            if ((room.ur_UpdateFlags & UFLG_DEBUG) != 0)
            {
              room.ur_UpdateFlags = UFLG_ALL;
              TRACE("ROOM Run. Normal Mode via F1 key");
            }
            else
            {
              room.ur_UpdateFlags = UFLG_ALL | UFLG_DEBUG;
              TRACE("ROOM Run. Debug Mode via F1 key");
            }
          }
          else if (evt.ie_Code == KC_F2)
          {
            room.ur_UpdateFlags = UFLG_ALL;
            TRACE("ROOM Run. Forced Redraw (Not debug)");
          }
        }
        break;
        case IET_CURSOR:
        {
          struct ENTITY* entity;
          BOOL didFind;

          didFind = FALSE;
          rmMouseX = room.ur_CamX + CursorX;
          rmMouseY = room.ur_CamY + CursorY;

          for (UWORD ii = 0; ii < MAX_ROOM_EXITS; ii++)
          {
            entity = (struct ENTITY*) room.ur_Exits[ii];
          
            if (NULL != entity && PointInside(&entity->en_HitBox, rmMouseX, rmMouseY))
            {
              didFind = TRUE;

              if (room.ur_HoverEntity != entity)
              {
                room.ur_HoverEntity = entity;
                room.ur_UpdateFlags |= UFLG_CAPTION;
                break;
              }
            }
          }

          if (didFind == FALSE)
          {
            for (UWORD ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
            {
              entity = (struct ENTITY*) room.ur_Entities[ii];

              if (NULL != entity && PointInside(&entity->en_HitBox, rmMouseX, rmMouseY))
              {
                didFind = TRUE;

                if (room.ur_HoverEntity != entity)
                {
                  room.ur_HoverEntity = entity;
                  room.ur_UpdateFlags |= UFLG_CAPTION;
                  break;
                }
              }
            }
          }

          if (didFind == FALSE && room.ur_HoverEntity != NULL)
          {
            room.ur_HoverEntity = NULL;
            room.ur_UpdateFlags |= UFLG_CAPTION;
          }

        }
        break;
        case IET_SELECT:
        {
          if (room.ur_HoverEntity != NULL)
          {
            if (room.ur_HoverEntity->en_Type == ET_EXIT)
            {
              struct EXIT* exit;

              exit = (struct EXIT*) room.ur_HoverEntity;

              exitRoom = TRUE;
              entrance->en_Room = GetRoomFromExit(exit);
              entrance->en_Exit = exit->ex_Target;
            }

          }
        }
        break;
      }
    }

    if (mouseFrameTimer++ >= 50000)
    {
      mouseFrameTimer = 0;

      mouseFrame++;
      if (mouseFrame == 4)
        mouseFrame = 0;

      GfxAnimateCursor(mouseFrame);
    }

    if (eventThisFrame == FALSE)
    {
      continue;
    }

    if (IsMenuDown())
    {
      room.ur_CamX = CursorX << 2;

      if (room.ur_CamX < 0)
      {
        room.ur_CamX = 0;
      }
      else if (room.ur_CamX > mostLeftEdge)
      {
        room.ur_CamX = mostLeftEdge;
      }

      room.ur_UpdateFlags |= UFLG_SCROLL;
    }

    if ((room.ur_UpdateFlags & (UFLG_SCENE | UFLG_DEBUG)) != 0)
    {
      PlayRoomDebug(&room);
    }

    if ((room.ur_UpdateFlags & UFLG_CAPTION) != 0)
    {
      PlayCaption(&room);
    }

    if ((room.ur_UpdateFlags & UFLG_SCROLL) != 0)
    {
      GfxSetScrollOffset(0, room.ur_CamX, 0);
      room.ur_UpdateFlags &= ~UFLG_SCROLL;
    }


    if ((room.ur_UpdateFlags & UFLG_SCENE) != 0)
    {
      struct EXIT* exit;
      struct ENTITY* entity;

      room.ur_UpdateFlags &= ~UFLG_SCENE;

      GfxBlitBitmap(0, &room.ur_Backdrops[0], 0, 0, 0, 0, room.ur_Backdrops[0].im_Width, room.ur_Backdrops[0].im_Height);
      {
        if ((room.ur_UpdateFlags & UFLG_DEBUG) != 0)
        {
          for (UWORD ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
          {

            exit = room.ur_Exits[ii];

            if (NULL == exit)
              break;

            GfxDrawHitBox(0, &exit->ex_HitBox, &exit->ex_Name[0], StrLen(exit->ex_Name));

          }

          for (UWORD ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
          {

            entity = room.ur_Entities[ii];

            if (NULL == entity)
              break;

            GfxDrawHitBox(0, &entity->en_HitBox, &entity->en_Name[0], StrLen(entity->en_Name));

          }
        }
      }

      GfxSubmit(0);
    }

  }

  if (InEvtForceQuit == TRUE)
  {
    entrance->en_Room = 0;
  }

  TRACEF("ROOM Run. Left Loop. Next Room is = %ld", entrance->en_Room);

  /* Unload */
  PackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES);

  TRACEF("ROOM Run. Packing Room. Id= %ld", room.ur_Room.as_Id);
}


STATIC VOID PlayRoomDebug(struct UNPACKED_ROOM* room)
{
  UWORD numExits, numEntities;
  UWORD ii;
  UWORD strLen;
  CHAR  debugText[40];
  
  for (ii = 0, numExits = 0; ii < MAX_ROOM_EXITS; ii++)
  {
    if (room->ur_Exits[ii] != NULL)
      numExits++;
  }

  for (ii = 0, numEntities = 0; ii < MAX_ROOM_ENTITIES; ii++)
  {
    if (room->ur_Entities[ii] != NULL)
      numEntities++;
  }


  strLen = StrFormat(debugText, sizeof(debugText), "Rm %ld Ex %ld En %ld", (ULONG)room->ur_Room.as_Id, (ULONG)numExits, (ULONG) numEntities )-1;
  GfxMove(1, 0, 50);
  GfxText(1, debugText, strLen);
}

VOID Api_LoadRoom(LONG roomNum)
{

  struct ENTRANCE entrance;

  entrance.en_Room = roomNum;
  entrance.en_Exit = 0;

  TRACEF("ROOM. LoadRoom. Room Num = %ld, Exit = %ld", entrance.en_Room, entrance.en_Exit);

  while (entrance.en_Room != 0 && InEvtForceQuit == FALSE)
  {
    ArenaRollback(ArenaRoom);

    /* Start First Room */
    Room_Run(0, &entrance);
  }

  TRACEF("ROOM. LoadRoom. NextRoom = %ld, Exit = %ld", entrance.en_Room, entrance.en_Exit);
}
