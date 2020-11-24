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
#include <Parrot/String.h>
#include <Parrot/Requester.h>
#include <Parrot/Graphics.h>
#include <Parrot/Input.h>
#include <Parrot/Game.h>

#include <proto/dos.h>
#include <proto/graphics.h>

EXTERN WORD CursorX;
EXTERN WORD CursorY;

STATIC VOID PlayRoomDebug(struct UNPACKED_ROOM* room);

EXPORT VOID UnpackRoom(struct UNPACKED_ROOM* room, ULONG unpack)
{
  UBYTE ii;
  UWORD id;

  if ((unpack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) == 0)
  {
#if 0
    room->ur_Room = LoadAssetT(struct ROOM, ArenaChapter, room->ur_Id, CT_ROOM, room->ur_Id, CHUNK_FLAG_ARCH_ANY);
#endif

    room->ur_Unpacked |= UNPACK_ROOM_ASSET;
  }

  if ((unpack & UNPACK_ROOM_BACKDROPS) != 0 && (room->ur_Unpacked & UNPACK_ROOM_BACKDROPS) == 0)
  {
    for (ii = 0; ii < MAX_ROOM_BACKDROPS; ii++)
    {
      id = room->ur_Room->rm_Backdrops[ii];

      if (0 != id && NULL == room->ur_Backdrops[ii])
      {
#if 0
        room->ur_Backdrops[ii] = LoadAsset(ArenaRoom, room->ur_Id, CT_IMAGE, id, CHUNK_FLAG_ARCH_ANY);
#endif
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
#if 0
        room->ur_Exits[ii] = LoadAsset(ArenaRoom, room->ur_Id, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
#endif
      }

    }

    for (ii = 0; ii < MAX_ROOM_ENTITIES; ii++)
    {
      id = room->ur_Room->rm_Entities[ii];

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
      id = room->ur_Room->rm_Entities[ii];

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
      id = room->ur_Room->rm_Exits[ii];

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
      backdrop = room->ur_Room->rm_Backdrops[ii];

      if (0 != backdrop && NULL != room->ur_Backdrops[ii])
      {
#if 0
        UnloadAsset(ArenaRoom, room->ur_Backdrops[ii]);
#endif
        room->ur_Backdrops[ii] = NULL;
      }
    }

    room->ur_Unpacked &= ~UNPACK_ROOM_BACKDROPS;
  }

  if ((pack & UNPACK_ROOM_ASSET) != 0 && (room->ur_Unpacked & UNPACK_ROOM_ASSET) != 0)
  {
#if 0
    UnloadAsset(ArenaChapter, room->ur_Room);
#endif
    room->ur_Room = NULL;
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

VOID PlayRoom(UWORD screen, struct ENTRANCE* entrance, struct GAME_INFO* gameInfo)
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
  room.ur_Id = entrance->en_Room;

  UnpackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES);

  mostLeftEdge = room.ur_Room->rm_Width - gameInfo->gi_Width;

  if (mostLeftEdge > 1)
  {
    mostLeftEdge -= 1;
  }

  if (entrance->en_Exit != 0 && room.ur_Room->rm_Width > gameInfo->gi_Width)
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
  
  NotBusy();
  GfxClear(0);
  GfxClear(1);

  GfxSubmit(0);
  GfxSubmit(1);
  WaitTOF();

  while (exitRoom == FALSE && InEvtForceQuit == FALSE)
  {
    while (PopEvent(&evt))
    {
      switch (evt.ie_Type)
      {
        case IET_KEYUP:
        {
          if (evt.ie_Code == KC_ESC)
          {
            exitRoom = TRUE;
            entrance->en_Room = 0;
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
            }
            else
            {
              room.ur_UpdateFlags = UFLG_ALL | UFLG_DEBUG;
            }
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

      GfxBlitBitmap(0, room.ur_Backdrops[0], 0, 0, 0, 0, room.ur_Backdrops[0]->im_Width, room.ur_Backdrops[0]->im_Height);
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

  /* Unload */
  PackRoom(&room, UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES);
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


  strLen = StrFormat(debugText, sizeof(debugText), "Rm %ld Ex %ld En %ld", (ULONG)room->ur_Id, (ULONG)numExits, (ULONG) numEntities )-1;
  GfxMove(1, 0, 50);
  GfxText(1, debugText, strLen);
}
