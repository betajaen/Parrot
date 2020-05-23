/**
    $Id: Arena.c, 1.1 2020/05/11 10:48:00, betajaen Exp $

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
#include <Parrot/Requester.h>
#include <Parrot/String.h>

#include <proto/exec.h>

#define ARENA_MAGIC 0xda9a5983UL

struct ARENA_HEADER
{
  ULONG   ah_Size;
  ULONG   ah_Magic;
  ULONG   ah_Used;
  ULONG*  ah_Base;
};

APTR ArenaGame, ArenaChapter, ArenaRoom;

EXPORT APTR ArenaOpen(ULONG size, ULONG requirements)
{
  struct ARENA_HEADER* hdr;

  /* Round up to nearest 4th byte */
  size = (size + 3) & ~0x03;

  hdr = (struct ARENA_HEADER*) AllocVec(size + sizeof(struct ARENA_HEADER),
    requirements | MEMF_CLEAR);

  if (NULL == hdr)
  {
    ErrorF("Could not allocate arena size of %ld", size);
  }

  hdr->ah_Size = size;
  hdr->ah_Magic = ARENA_MAGIC;
  hdr->ah_Used = 0;
  hdr->ah_Base = (APTR)(hdr + 1);
  
  return (APTR)(hdr);
}

EXPORT BOOL ArenaClose(APTR arena)
{
  struct ARENA_HEADER* hdr;

  if (NULL == arena)
    return FALSE;

  hdr = ((struct ARENA_HEADER*) arena);

  if (ARENA_MAGIC != hdr->ah_Magic)
    return FALSE;

  hdr->ah_Magic = 0;
  hdr->ah_Size = 0;

  FreeVec(hdr);

  return TRUE;
}

EXPORT BOOL ArenaRollback(APTR arena)
{
  struct ARENA_HEADER* hdr;

  if (NULL == arena)
    return FALSE;

  hdr = ((struct ARENA_HEADER*) arena);

  if (ARENA_MAGIC != hdr->ah_Magic)
    return FALSE;

  hdr->ah_Used = 0;

  return TRUE;
}

EXPORT ULONG ArenaSpace(APTR arena)
{
  struct ARENA_HEADER* hdr;

  if (NULL == arena)
    return 0UL;

  hdr = ((struct ARENA_HEADER*) arena);

  if (ARENA_MAGIC != hdr->ah_Magic)
    return 0UL;

  return hdr->ah_Size - hdr->ah_Used;
}

EXPORT ULONG ArenaSize(APTR arena)
{
  struct ARENA_HEADER* hdr;

  if (NULL == arena)
    return 0UL;

  hdr = ((struct ARENA_HEADER*) arena);

  if (ARENA_MAGIC != hdr->ah_Magic)
    return 0UL;

  return hdr->ah_Size;
}


EXPORT APTR ObjAlloc(APTR arena, ULONG size, ULONG class, BOOL zeroFill)
{
  struct ARENA_HEADER* hdr;
  struct ARENA_ALLOC* alloc;
  APTR result;

  result = NULL;


  if (NULL == arena)
  {
    ErrorF("Arena is null");
  }

  hdr = ((struct ARENA_HEADER*) arena);

  if (ARENA_MAGIC != hdr->ah_Magic)
  {
    ErrorF("Arena magic is not correct. It is likely memory is corrupted");
  }

  size = (size + 3) & ~0x03;
  
  if ((hdr->ah_Used + size) >= hdr->ah_Size)
  {
    ErrorF("Out of Area memory");
  }

  alloc = (struct ARENA_ALLOC*) (hdr->ah_Base + hdr->ah_Used);

  if (zeroFill == TRUE)
  {
    FillMem((UBYTE*)alloc, size, 0);
  }

  hdr->ah_Used += size;

  result = (APTR) alloc;

  return result;
}
