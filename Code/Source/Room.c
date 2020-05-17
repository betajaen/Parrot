/**
    $Id: Room.c, 0.1, 2020/05/11 15:49:00, betajaen Exp $

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

#include <proto/exec.h>

#define ROOM_MAGIC 0x524F4F4Dul

APTR ObjAlloc(APTR arena, ULONG size, ULONG class);
LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...);
struct ASSET* LoadAsset(APTR arena, ULONG id);
void UnloadAsset(APTR arena, struct ASSET* asset);

EXPORT struct ASSET* LoadRoom(APTR arena, ULONG id)
{
  struct ROOM* room;

  room = ObjAlloc(arena, sizeof(struct ROOM), ROOM_MAGIC);

  return (struct ASSET*) room;
}


EXPORT VOID UnpackRoom(APTR arena, struct ROOM* room, ULONG unpack)
{
  UBYTE ii;
  struct IMAGE_REF* backdrop;

  if ((unpack & UNPACK_ROOM_BACKDROPS) != 0)
  {
    for (ii = 0; ii < 4; ii++)
    {
      backdrop = &room->rm_Backdrops[ii];

      if (0 != backdrop->ar_Id && NULL == backdrop->ar_Ptr)
      {
        backdrop->ar_Ptr = (struct IMAGE*) LoadAsset(arena, backdrop->ar_Id);
      }
    }
  }
}

EXPORT VOID PackRoom(APTR arena, struct ROOM* room, ULONG pack)
{
  UBYTE ii;
  struct IMAGE_REF* backdrop;

  if ((pack & UNPACK_ROOM_BACKDROPS) != 0)
  {
    for (ii = 0; ii < 4; ii++)
    {
      backdrop = &room->rm_Backdrops[ii];

      if (0 != backdrop->ar_Id && NULL != backdrop->ar_Ptr)
      {
        UnloadAsset(arena, (struct ASSET*) backdrop->ar_Ptr);
        backdrop->ar_Ptr = NULL;
      }
    }
  }
}
