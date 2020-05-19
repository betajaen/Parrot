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
#include <Parrot/Requester.h>
#include <Parrot/String.h>
#include <Parrot/Archive.h>

#include "Asset.h"

#include <proto/exec.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>

struct MinList Assets = {
    NULL,
    NULL,
    NULL
};

STATIC struct MinList OpenArchives = {
    NULL,
    NULL,
    NULL
};

STATIC CHAR* BasePath;
STATIC CHAR* JoinPathStr;

#define ARCHIVE_ID 0x9640c817ul

struct ARCHIVE
{
  struct MinNode    pa_Node;
  BPTR              pa_File;
  struct IFFHandle* pa_Handle;
  ULONG             pa_Usage;
  UWORD             pa_Id;
};

struct ASSET_FACTORY
{
  ULONG af_NodeType;
  ULONG af_Size;
};

struct ASSET_FACTORY AssetFactories[] = {
  { CT_GAME_INFO, sizeof(struct GAME_INFO) },
  { CT_PALETTE32, sizeof(struct PALETTE32_TABLE) },
  { CT_PALETTE4, sizeof(struct PALETTE4_TABLE) },
  { CT_ROOM, sizeof(struct ROOM) },
  { 0, 0 }
};

STATIC ULONG FindFactory(ULONG nodeType)
{
  struct ASSET_FACTORY* factory;

  factory = &AssetFactories[0];

  while (factory->af_NodeType != 0)
  {
    if (factory->af_NodeType == nodeType)
      return factory->af_Size;

    factory++;
  }
}

EXPORT VOID SetArchivesPath(CHAR* path)
{
  BasePath = path;

  if (StrEndsWith(BasePath, '/'))
    JoinPathStr = "%s%d.Parrot";
  else
    JoinPathStr = "%s/%d.Parrot";
}

STATIC struct ARCHIVE* LoadArchive(UBYTE id)
{
  struct ARCHIVE* archive;
  BPTR file;
  CHAR path[128];

  if (0 == StrFormat(&path[0], sizeof(path), JoinPathStr, BasePath, id))
  {
    return NULL;
  }

  file = Open(&path[0], MODE_OLDFILE);

  if (NULL == file)
  {
    return NULL;
  }

  archive = (struct ARCHIVE*) ObjAlloc(ArenaGame, sizeof(struct ARCHIVE), ARCHIVE_ID);
  AddTail((struct List*) & OpenArchives, (struct Node*) archive);

  archive->pa_Handle = AllocIFF();
  archive->pa_Handle->iff_Stream = file;

  InitIFFasDOS(archive->pa_Handle);
  OpenIFF(archive->pa_Handle, IFFF_READ);

  return archive;
}


EXPORT struct ARCHIVE* OpenArchive(UBYTE id)
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

EXPORT VOID CloseArchive(struct ARCHIVE* archive)
{
  if (NULL != archive)
  {
    CloseIFF(archive->pa_Handle);
    Close(archive->pa_Handle->iff_Stream);
    FreeIFF(archive->pa_Handle);

    Remove((struct Node*) archive);

    archive->pa_File = NULL;
    archive->pa_Handle = NULL;
    archive->pa_Id = 0;
    archive->pa_Usage = 0;
  }
}


EXPORT BOOL ReadAssetFromArchive(struct ARCHIVE* archive, ULONG nodeType, UWORD chunkId, UWORD chunkArch, APTR data, ULONG dataSize)
{
  struct ContextNode* node;
  struct CHUNK_HEADER chunkHeader;
  LONG err;
  CHAR idBuf[5];

  Seek(archive->pa_File, 0, OFFSET_BEGINNING);

  while (TRUE)
  {
    err = ParseIFF(archive->pa_Handle, IFFPARSE_RAWSTEP);

    if (err == IFFERR_EOC)
      continue;
    else if (err)
      break;

    node = CurrentChunk(archive->pa_Handle);

    if (node->cn_ID == nodeType)
    {

      if ((node->cn_Size - sizeof(chunkHeader)) <= dataSize)
      {
        ReadChunkBytes(archive->pa_Handle, &chunkHeader, sizeof(struct CHUNK_HEADER));

        if (chunkHeader.Id != chunkId)
          continue;

        if ((chunkHeader.Flags & chunkArch) == 0)
          continue;

        ReadChunkBytes(archive->pa_Handle, data, dataSize);

        return TRUE;
      }

      RequesterF("OK", "Error Chunk %s is to large (%ld) to fit into capacity (%ld)",
        IDtoStr(node->cn_ID, idBuf),
        node->cn_Size,
        dataSize
      );

      return FALSE;
    }
  }

  RequesterF("OK", "Chunk End");

  return FALSE;
}


EXPORT APTR LoadAsset(APTR arena, UWORD archiveId, ULONG nodeType, UWORD assetId, UWORD arch)
{
  struct ASSET* asset;
  struct ARCHIVE* archive;
  ULONG  assetSize;

  for (asset = ((struct ASSET*) Assets.mlh_Head);
       asset != NULL; 
       asset = ((struct ASSET*)asset->as_Node.mln_Succ))
  {
    if (asset->as_Id == assetId && (asset->as_Arch & arch) != 0)
    {
      return (APTR) (asset+1);
    }
  }

  archive = OpenArchive(archiveId);

  if (archive == NULL)
  {
    RequesterF("OK", "Could not open archive %ld", (ULONG)archiveId);
    return NULL;
  }

  assetSize = FindFactory(nodeType);

  if (assetSize == 0)
  {
    RequesterF("OK", "Could not find registered factory for %lx", nodeType);
    return NULL;
  }

  asset = ArenaNew(assetSize + sizeof(struct ASSET), MEMF_CLEAR);

  if (ReadAssetFromArchive(archive, nodeType, assetId, arch, (APTR) (asset + 1), assetSize) == FALSE)
  {
    RequesterF("OK", "Could not find registered factory for %ld, %lx", nodeType, (ULONG) assetId);
    return NULL;
  }
  
  asset->as_Id = assetId;
  asset->as_Arch = arch;
  asset->as_Node.mln_Succ = NULL;
  asset->as_Node.mln_Pred = NULL;

  AddTail((struct List*) &Assets, (struct Node*) asset);

  return (APTR)(asset + 1);

}

EXPORT void UnloadAsset(APTR arena, struct ASSET* asset)
{
  
}
