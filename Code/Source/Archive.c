/**
    $Id: Arena.c, 0.1, 2020/05/11 10:48:00, betajaen Exp $

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

#include "Asset.h"

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>

ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);

STATIC struct MinList OpenArchives = {
    NULL,
    NULL,
    NULL
};

STATIC CHAR* BasePath;

#define ARCHIVE_ID 0x9640c817ul

struct ARCHIVE
{
  struct MinNode    pa_Node;
  BPTR              pa_File;
  struct IFFHandle* pa_Handle;
  ULONG             pa_Usage;
  UWORD             pa_Id;
};

EXPORT VOID SetArchivesPath(CHAR* path)
{
  BasePath = path;
}

STATIC struct ARCHIVE* LoadArchive(UBYTE id)
{
  struct ARCHIVE* archive;
  CHAR path[128];

  if (0 == StrFormat(&path[0], sizeof(path), "%s/%d.Parrot", BasePath, id))
  {
    return NULL;
  }
  
  archive = (struct ARCHIVE*) ObjAlloc(ArenaGame, sizeof(struct ARCHIVE), ARCHIVE_ID);
  AddTail((struct List*) & OpenArchives, (struct Node*) archive);
  
  archive->pa_Handle = AllocIFF();
  archive->pa_Handle->iff_Stream = Open(&path[0], MODE_OLDFILE);

  InitIFFasDOS(archive->pa_Handle);
  OpenIFF(archive->pa_Handle, IFFF_READ);

  return archive;
}


EXPORT APTR OpenArchive(UBYTE id)
{
  struct ARCHIVE* archive;

  for (archive = ((struct ARCHIVE*) OpenArchives.mlh_Head);
       archive != NULL;
       archive = ((struct ARCHIVE*) archive->pa_Node.mln_Succ))
  {
    if (archive->pa_Id == id)
    {
      return archive;
    }
  }

  return LoadArchive(id);
}

EXPORT VOID CloseArchive(APTR archive)
{
}

EXPORT ULONG GetChunkSize(APTR archive, ULONG id)
{
  return FALSE;
}

EXPORT BOOL ReadChunk(APTR archive, ULONG id, UBYTE* data, ULONG dataCapacity)
{
  return FALSE;
}

