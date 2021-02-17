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

#define MM_MAX_ROOMS   55
#define MM_MAX_OBJECTS 1024

PtUnsigned16 RoomExportOrder[] = {
  44,
  1,
  10
};

#define ROOM_COUNT (sizeof(RoomExportOrder) / sizeof(PtUnsigned16))

struct RM_Table
{
  PtUnsigned16 rt_Mm;
  PtUnsigned16 rt_Parrot;
};

STATIC struct RM_Table MM_RoomId_Table[MM_MAX_ROOMS] = { 0 };

struct MM_EXIT
{
  PtUnsigned16 ex_Mm_Id;
  PtUnsigned16 ex_Mm_Target;
  PtUnsigned16 ex_Parrot_Id;
  PtUnsigned16 ex_Parrot_Target;
};

STATIC struct MM_EXIT Mm_Exit_Table[] = {
  { 258 /* 44 */ , 257 /* 1 */, 0,0 },
  { 61 /* 1 */, 67 /* 10 */, 0,0  },
  { 0,0, 0,0}
};

struct MM_OBJECT
{
  PtUnsigned16 ob_Mm_Id;
  PtUnsigned16 ob_Parrot_Room;
  PtUnsigned16 ob_Parrot_Id;
  PtUnsigned16 ob_Parrot_Type;
};

STATIC struct MM_OBJECT MM_Object_Table[MM_MAX_OBJECTS] = { 0 };

STATIC void AddRoom(PtUnsigned16 mmId, PtUnsigned16 parrotId)
{
  MM_RoomId_Table[mmId].rt_Mm = mmId;
  MM_RoomId_Table[mmId].rt_Parrot = parrotId;
}

STATIC void AddObject(PtUnsigned16 mmId, PtUnsigned16 parrotId, PtUnsigned16 parrotRoom)
{
  MM_Object_Table[mmId].ob_Mm_Id = mmId;
  MM_Object_Table[mmId].ob_Parrot_Id = parrotId;
  MM_Object_Table[mmId].ob_Parrot_Room = parrotRoom;
}

STATIC PtBool AddExit(PtUnsigned16 mmId, PtUnsigned16 parrotId)
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

STATIC PtBool FindRoom(PtUnsigned16 mmId, PtUnsigned16* out_parrotId)
{
  if (mmId >= 55)
  {
    return FALSE;
  }

  *out_parrotId = MM_RoomId_Table[mmId].rt_Parrot;

  return TRUE;
}

STATIC PtBool FindObject(PtUnsigned16 mmId, struct MM_OBJECT** obj)
{
  PtUnsigned16 ii;

  for (ii = 0; ii < MM_MAX_OBJECTS; ii++)
  {
    (*obj) = &MM_Object_Table[ii];

    if ((*obj)->ob_Mm_Id == mmId)
    {
      return TRUE;
    }
  }

  (*obj) = NULL;

  return FALSE;
}

STATIC PtBool FindExit(PtUnsigned16 mmId, PtUnsigned16* out_parrotId, PtUnsigned16* out_targetId)
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
