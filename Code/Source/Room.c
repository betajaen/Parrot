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
#include <Parrot/Asset.h>
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

PtPublic PtSigned16 CursorX;
PtPublic PtSigned16 CursorY;

STATIC void PlayRoomDebug(struct UNPACKED_ROOM* room);

void Room_Initialise(struct UNPACKED_ROOM* room, PtUnsigned32 unpack, PtUnsigned16 roomId)
{
  PtUnsigned8 ii;
  PtUnsigned16 id;

  VERBOSEF("ROOM Unpack. Unpacking Room Id=%ld", roomId);

  if ((unpack & RIF_ASSET) != 0 && (room->lv_InitialisationFlags & RIF_ASSET) == 0)
  {
    if ( FALSE == Asset_LoadInto(roomId, 1, PT_AT_ROOM, (PtAsset*) room, sizeof(struct Room)) )
    {
      ERRORF("ROOM Unpack. Cannot find room asset Id=%ld, UnpackFlags=%lx", (PtUnsigned32) roomId, unpack);
      return;
    }

    room->lv_InitialisationFlags |= RIF_ASSET;
  }

  if ((unpack & RIF_BACKDROPS) != 0 && (room->lv_InitialisationFlags & RIF_BACKDROPS) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
    {
      id = room->lv_Room.rm_Backdrops[ii];

      if (0 != id)
      {
        TRACEF("ROOM Unpack. Unpacking Backdrop Id=%ld", id);

        struct IMAGE* backdrop = &room->lv_Backdrops[ii];

        Asset_LoadInto_Callback(id, 1, PT_AT_IMAGE, (PtAsset*) backdrop, sizeof(struct IMAGE), Asset_CallbackFor_Image);

        TRACEF("ROOM Unpack. Id=%ld, Loaded Backdrop = %ld, Width=%ld, Height=%ld", roomId, id, backdrop->im_Width, backdrop->im_Height);
        
        TRACEF("ROOM. Unpack. Backdrop Plane[0] = %lx", backdrop->im_Planes[0]);
      }
      else
      {
        room->lv_Backdrops[ii].im_Planes[0] = NULL;
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

    room->lv_InitialisationFlags |= RIF_BACKDROPS;
  }

  if ((unpack & RIF_ENTITIES) != 0 && (room->lv_InitialisationFlags & RIF_ENTITIES) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
    {
      id = room->lv_Room.rm_Exits[ii];

      if (0 == id)
        break;

      if (NULL == room->lv_Exits[ii])
      {
#if 0
        room->ur_Exits[ii] = LoadAsset(ArenaRoom, room->ur_Id, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
#endif
      }

    }

    for (ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
    {
      id = room->lv_Room.rm_Entities[ii];

      if (0 == id)
        break;

      if (NULL == room->lv_Entities[ii])
      {
#if 0
        room->ur_Entities[ii] = LoadAsset(ArenaRoom, room->ur_Id, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
#endif
      }
    }

    room->lv_InitialisationFlags |= RIF_ENTITIES;
  }
}

void Room_Teardown(struct UNPACKED_ROOM* room, PtUnsigned32 pack)
{
  PtUnsigned8 ii;
  PtUnsigned16 backdrop;
  PtUnsigned16 id;

  if ((pack & RIF_ENTITIES) != 0 && (room->lv_InitialisationFlags & RIF_ENTITIES) != 0)
  {
    for (ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
    {
      id = room->lv_Room.rm_Entities[ii];

      if (0 == id)
        break;

      if (NULL != room->lv_Entities[ii])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Entities[ii]);
#endif
        room->lv_Entities[ii] = NULL;
      }
    }

    for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
    {
      id = room->lv_Room.rm_Exits[ii];

      if (0 == id)
        break;

      if (NULL != room->lv_Exits[ii])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Exits[ii]);
#endif
        room->lv_Exits[ii] = NULL;
      }
    }

    room->lv_InitialisationFlags &= ~MAX_ROOM_ENTITIES;
  }

  if ((pack & RIF_BACKDROPS) != 0 && (room->lv_InitialisationFlags & RIF_BACKDROPS) != 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
    {
      backdrop = room->lv_Room.rm_Backdrops[ii];

      if (0 != backdrop && NULL != room->lv_Backdrops[ii].im_Planes[0])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Backdrops[ii]);
#endif
        room->lv_Backdrops[ii].im_Planes[0] = NULL;
      }
    }

    room->lv_InitialisationFlags &= ~RIF_BACKDROPS;
  }

  if ((pack & RIF_ASSET) != 0 && (room->lv_InitialisationFlags & RIF_ASSET) != 0)
  {
#if 0
    UnloadAsset(ArenaChapter, room->ur_Room);
#endif
    room->lv_InitialisationFlags &= ~RIF_ASSET;
  }
}

STATIC PtUnsigned16 GetRoomFromExit(struct RoomExit* exit)
{
  PtUnsigned16 archive;
#if 0
  archive = FindAssetArchive(exit->ex_Target, CT_ENTITY, CHUNK_FLAG_ARCH_ANY);
#endif

  return archive;
}

STATIC struct RoomExit* FindExit(struct UNPACKED_ROOM* room, PtUnsigned16 id)
{
  PtUnsigned16 ii;
  struct RoomExit* exit;

  for (ii = 0; ii < MAX_ROOM_EXITS; ii++)
  {
    exit = room->lv_Exits[ii];

    if (NULL == exit)
      break;
    
    if (PtAsset_Id(exit) == id)
      break;
  }

  return exit;
}

STATIC PtBool PointInside(struct Rect* rect, PtSigned16 x, PtSigned16 y)
{
  return x >= rect->rt_Left && x <= rect->rt_Right && y >= rect->rt_Top && y <= rect->rt_Bottom;
}

void Room_Run(PtUnsigned16 screen, struct RoomEntrance* entrance)
{
  struct UNPACKED_ROOM room;
  PtInputEvent evt;
  PtUnsigned16 screenW, screenH;
  PtBool exitRoom;
  PtUnsigned16 mostLeftEdge;
  PtUnsigned16 ii;
  PtSigned16 rmMouseX, rmMouseY;
  PtBool updateCaption;
  PtBool hasCaption;
  PtBool eventThisFrame;
  PtUnsigned16 mouseFrameTimer;
  PtUnsigned16 mouseFrame;

  exitRoom = FALSE;

  Busy();

  /* Get Screen Info */
  screenW = 320;
  screenH = 128;

  /* Load Room Asset and Backdrops */
  ClearMem(room);

  room.lv_UpdateFlags = UFLG_ALL;
  room.lv_Verbs.vb_Allowed = VERB_NONE | VERB_WALK;
  room.lv_Verbs.vb_Selected = VERB_NONE;
  

  TRACEF("ROOM Run. Unpacking. Id = %ld. UpdateFlags = %lx, Verbs = %lx", entrance->en_Room, room.lv_UpdateFlags, room.lv_Verbs.vb_Allowed);

  Room_Initialise(&room, RIF_ASSET | RIF_BACKDROPS | RIF_ENTITIES, entrance->en_Room);

  TRACE("ROOM Run. Unpacked");

  mostLeftEdge = room.lv_Room.rm_Width - GameInfo.gi_Width;

  if (mostLeftEdge > 1)
  {
    mostLeftEdge -= 1;
  }

  if (entrance->en_Exit != 0 && room.lv_Room.rm_Width > GameInfo.gi_Width)
  {
    struct RoomExit* exit;

    exit = FindExit(&room, entrance->en_Exit);
    room.lv_CamX = exit->ex_HitBox.rt_Left;
    room.lv_CamY = 0;

    if (room.lv_CamX > mostLeftEdge)
      room.lv_CamX = mostLeftEdge;
    else if (room.lv_CamX < 0)
      room.lv_CamX = 0;

    room.lv_UpdateFlags |= UFLG_SCROLL;
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
        case PT_IET_KEYUP:
        {
          if (evt.ie_Code == PT_KC_ESC)
          {
            exitRoom = TRUE;
            entrance->en_Room = 0;
            TRACE("ROOM Run. Escaping room via ESC key");
          }
          else if (evt.ie_Code == PT_KC_F1)
          {
            GfxClear(0);
            GfxClear(1);

            GfxSubmit(0);
            GfxSubmit(1);
            WaitTOF();

            if ((room.lv_UpdateFlags & UFLG_DEBUG) != 0)
            {
              room.lv_UpdateFlags = UFLG_ALL;
              TRACE("ROOM Run. Normal Mode via F1 key");
            }
            else
            {
              room.lv_UpdateFlags = UFLG_ALL | UFLG_DEBUG;
              TRACE("ROOM Run. Debug Mode via F1 key");
            }
          }
          else if (evt.ie_Code == PT_KC_F2)
          {
            room.lv_UpdateFlags = UFLG_ALL;
            TRACE("ROOM Run. Forced Redraw (Not debug)");
          }
        }
        break;
        case PT_IET_CURSOR:
        {
          struct Entity* entity;
          PtBool didFind;

          didFind = FALSE;
          rmMouseX = room.lv_CamX + CursorX;
          rmMouseY = room.lv_CamY + CursorY;

          for (PtUnsigned16 ii = 0; ii < MAX_ROOM_EXITS; ii++)
          {
            entity = (struct Entity*) room.lv_Exits[ii];
          
            if (NULL != entity && PointInside(&entity->en_HitBox, rmMouseX, rmMouseY))
            {
              didFind = TRUE;

              if (room.lv_HoverEntity != entity)
              {
                room.lv_HoverEntity = entity;
                room.lv_UpdateFlags |= UFLG_CAPTION;
                break;
              }
            }
          }

          if (didFind == FALSE)
          {
            for (PtUnsigned16 ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
            {
              entity = (struct Entity*) room.lv_Entities[ii];

              if (NULL != entity && PointInside(&entity->en_HitBox, rmMouseX, rmMouseY))
              {
                didFind = TRUE;

                if (room.lv_HoverEntity != entity)
                {
                  room.lv_HoverEntity = entity;
                  room.lv_UpdateFlags |= UFLG_CAPTION;
                  break;
                }
              }
            }
          }

          if (didFind == FALSE && room.lv_HoverEntity != NULL)
          {
            room.lv_HoverEntity = NULL;
            room.lv_UpdateFlags |= UFLG_CAPTION;
          }

        }
        break;
        case PT_IET_SELECT:
        {
          if (room.lv_HoverEntity != NULL)
          {
            if (room.lv_HoverEntity->en_Type == ET_EXIT)
            {
              struct RoomExit* exit;

              exit = (struct RoomExit*) room.lv_HoverEntity;

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
      room.lv_CamX = CursorX << 2;

      if (room.lv_CamX < 0)
      {
        room.lv_CamX = 0;
      }
      else if (room.lv_CamX > mostLeftEdge)
      {
        room.lv_CamX = mostLeftEdge;
      }

      room.lv_UpdateFlags |= UFLG_SCROLL;
    }

    if ((room.lv_UpdateFlags & (UFLG_SCENE | UFLG_DEBUG)) != 0)
    {
      PlayRoomDebug(&room);
    }

    if ((room.lv_UpdateFlags & UFLG_CAPTION) != 0)
    {
      PlayCaption(&room);
    }

    if ((room.lv_UpdateFlags & UFLG_SCROLL) != 0)
    {
      GfxSetScrollOffset(0, room.lv_CamX, 0);
      room.lv_UpdateFlags &= ~UFLG_SCROLL;
    }


    if ((room.lv_UpdateFlags & UFLG_SCENE) != 0)
    {
      struct RoomExit* exit;
      struct Entity* entity;

      room.lv_UpdateFlags &= ~UFLG_SCENE;

      GfxBlitBitmap(0, &room.lv_Backdrops[0], 0, 0, 0, 0, room.lv_Backdrops[0].im_Width, room.lv_Backdrops[0].im_Height);
      {
        if ((room.lv_UpdateFlags & UFLG_DEBUG) != 0)
        {
          for (PtUnsigned16 ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
          {

            exit = room.lv_Exits[ii];

            if (NULL == exit)
              break;

            GfxDrawHitBox(0, &exit->ex_HitBox, &exit->ex_Name[0], StrLen(exit->ex_Name));

          }

          for (PtUnsigned16 ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
          {

            entity = room.lv_Entities[ii];

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
  Room_Teardown(&room, RIF_ASSET | RIF_BACKDROPS | RIF_ENTITIES);

  TRACEF("ROOM Run. Packing Room. Id= %ld", room.lv_Room.as_Id);
}


STATIC void PlayRoomDebug(struct UNPACKED_ROOM* room)
{
  PtUnsigned16 numExits, numEntities;
  PtUnsigned16 ii;
  PtUnsigned16 strLen;
  PtChar  debugText[40];
  
  for (ii = 0, numExits = 0; ii < MAX_ROOM_EXITS; ii++)
  {
    if (room->lv_Exits[ii] != NULL)
      numExits++;
  }

  for (ii = 0, numEntities = 0; ii < MAX_ROOM_ENTITIES; ii++)
  {
    if (room->lv_Entities[ii] != NULL)
      numEntities++;
  }


  strLen = StrFormat(debugText, sizeof(debugText), "Rm %ld Ex %ld En %ld", (PtUnsigned32)room->lv_Room.as_Id, (PtUnsigned32)numExits, (PtUnsigned32) numEntities )-1;
  GfxMove(1, 0, 50);
  GfxText(1, debugText, strLen);
}

void Api_LoadRoom(PtSigned32 roomNum)
{

  struct RoomEntrance entrance;

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
