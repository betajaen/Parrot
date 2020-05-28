/**
    $Id: Main.c, 1.0, 2020/05/13 07:16:00, betajaen Exp $

    Maniac Game Converter for Parrot
    ================================

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

#include <exec/types.h>

UWORD RoomExportOrder[] = {
  44,
  1,
  10
};

#define ROOM_COUNT (sizeof(RoomExportOrder) / sizeof(UWORD))

struct RM_Table
{
  UWORD rt_Mm;
  UWORD rt_Parrot;
};

STATIC struct RM_Table MM_RoomId_Table[55] = { 0 };

struct MM_EXIT
{
  UWORD ex_Mm_Id;
  UWORD ex_Mm_Target;
  UWORD ex_Parrot_Id;
  UWORD ex_Parrot_Target;
};

STATIC struct MM_EXIT Mm_Exit_Table[] = {
  { 258 /* 44 */ , 257 /* 1 */, 0,0 },
  { 61 /* 1 */, 67 /* 10 */, 0,0  },
  { 0,0, 0,0}
};

STATIC VOID AddRoom(UWORD mmId, UWORD parrotId)
{
  MM_RoomId_Table[mmId].rt_Mm = mmId;
  MM_RoomId_Table[mmId].rt_Parrot = parrotId;
}

STATIC BOOL AddExit(UWORD mmId, UWORD parrotId)
{
  struct MM_EXIT* ex;

  ex = &Mm_Exit_Table[0];

  while (ex->ex_Mm_Id != 0)
  {
    if (ex->ex_Mm_Id == mmId)
    {
      ex->ex_Parrot_Id = parrotId;
      return TRUE;
    }

    if (ex->ex_Mm_Target == mmId)
    {
      ex->ex_Parrot_Target = parrotId;
      return TRUE;
    }

    ex++;
  }
  return FALSE;
}

STATIC BOOL FindRoom(UWORD mmId, UWORD* out_parrotId)
{
  if (mmId >= 55)
  {
    return FALSE;
  }

  *out_parrotId = MM_RoomId_Table[mmId].rt_Parrot;

  return TRUE;
}

STATIC BOOL FindExit(UWORD mmId, UWORD* out_parrotId, UWORD* out_targetId)
{
  struct MM_EXIT* ex;

  ex = &Mm_Exit_Table[0];

  while (ex->ex_Mm_Id != 0)
  {
    if (ex->ex_Mm_Id == mmId)
    {
      *out_parrotId = ex->ex_Parrot_Id;
      *out_targetId = ex->ex_Parrot_Target;
      return TRUE;
    }

    if (ex->ex_Mm_Target == mmId)
    {
      *out_parrotId = ex->ex_Parrot_Target;
      *out_targetId = ex->ex_Parrot_Id;
      return TRUE;
    }

    ex++;
  }

  return FALSE;
}
