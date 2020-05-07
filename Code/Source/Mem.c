/**
    $Id: Mem.c, 0.1, 2020/05/07 07:29:00, betajaen Exp $

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

#include "Common.h"

#include <proto/exec.h>

#define MEM_MAGIC 0X944825DFUL

struct MEM_HEADER
{
  ULONG   mh_Size;
  ULONG   mh_Magic;
};

APTR MemNew(ULONG size, ULONG requirements)
{
  struct MEM_HEADER* hdr;

  /* Round up to nearest 4th byte */
  size = (size + 3) & ~0x03;

  hdr = (struct MEM_HEADER*) AllocVec(size + sizeof(struct MEM_HEADER),
                                      requirements);

  if (NULL == hdr)
    return NULL;

  hdr->mh_Size  = size;
  hdr->mh_Magic = MEM_MAGIC;

  return (APTR)(hdr + 1);
}

BOOL MemDelete(APTR pMem)
{
  struct MEM_HEADER* hdr;

  if (NULL == pMem)
    return FALSE;

  hdr = ((struct MEM_HEADER*) pMem) - 1;

  if (MEM_MAGIC != hdr->mh_Magic)
    return FALSE;

  hdr->mh_Magic = 0;
  hdr->mh_Size = 0;
  
  FreeVec(hdr);

  return TRUE;
}

ULONG MemSize(APTR pMem)
{
  struct MEM_HEADER* hdr;

  if (NULL == pMem)
    return 0UL;

  hdr = ((struct MEM_HEADER*) pMem) - 1;

  if (MEM_MAGIC != hdr->mh_Magic)
    return 0UL;

  return hdr->mh_Size;
}
