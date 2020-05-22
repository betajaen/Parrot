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
#include <exec/lists.h>

STATIC struct MinList Assets;
STATIC struct MinList OpenArchives;

STATIC CHAR* BasePath;
STATIC CHAR* JoinPathStr;
STATIC struct OBJECT_TABLE RoomTable;
STATIC struct OBJECT_TABLE ImageTable;

#define ARCHIVE_ID 0x9640c817ul

struct ARCHIVE
{
  struct MinNode    pa_Node;
  BPTR              pa_File;
  struct IFFHandle* pa_Iff;
  ULONG             pa_Usage;
  UWORD             pa_Id;
};

struct ASSET_FACTORY
{
  ULONG af_NodeType;
  ULONG af_Size;
  struct OBJECT_TABLE* af_Table;
};

struct ASSET_FACTORY AssetFactories[] = {
  { CT_GAME_INFO, sizeof(struct GAME_INFO), NULL },
  { CT_PALETTE32, sizeof(struct PALETTE32_TABLE), NULL },
  { CT_PALETTE4, sizeof(struct PALETTE4_TABLE), NULL },
  { CT_ROOM, sizeof(struct ROOM), &RoomTable },
  { CT_IMAGE, sizeof(struct IMAGE), &ImageTable },
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

EXPORT VOID InitialiseArchives(CHAR* path)
{
  BasePath = path;

  if (StrEndsWith(BasePath, '/'))
    JoinPathStr = "%s%ld.Parrot";
  else
    JoinPathStr = "%s/%ld.Parrot";

  NEW_MIN_LIST(OpenArchives);
  NEW_MIN_LIST(Assets);
}

STATIC struct ARCHIVE* ArchiveReadFromFile(UWORD id)
{
  struct ARCHIVE* archive;
  BPTR file;
  CHAR path[128];

  if (0 == StrFormat(&path[0], sizeof(path), JoinPathStr, BasePath, (ULONG) id))
  {
    return NULL;
  }

  file = Open(&path[0], MODE_OLDFILE);

  if (NULL == file)
  {
    ErrorF("Could not open archive %s", path);
  }

  archive = (struct ARCHIVE*) ObjAlloc(ArenaGame, sizeof(struct ARCHIVE), ARCHIVE_ID, TRUE);

  if (NULL == archive)
  {
    ErrorF("Did not allocate arena memory for Archive");
  }

  AddHead((struct List*) & OpenArchives, (struct Node*) archive);

  archive->pa_Id = id;
  archive->pa_Usage = 0;
  archive->pa_File = file;

  archive->pa_Iff = AllocIFF();
  archive->pa_Iff->iff_Stream = archive->pa_File;
  InitIFFasDOS(archive->pa_Iff);

  return archive;
}


EXPORT struct ARCHIVE* OpenArchive(UWORD id)
{
  struct ARCHIVE* archive;

  for (archive = (struct ARCHIVE*) OpenArchives.mlh_Head; archive->pa_Node.mln_Succ; archive = (struct ARCHIVE*) archive->pa_Node.mln_Succ)
  {
    if (archive->pa_Id == id)
    {
      return archive;
    }
  }

  return ArchiveReadFromFile(id);
}

EXPORT VOID CloseArchive(UWORD id)
{
  struct ARCHIVE* archive;

  for (archive = (struct ARCHIVE*) OpenArchives.mlh_Head; archive->pa_Node.mln_Succ; archive = (struct ARCHIVE*) archive->pa_Node.mln_Succ)
  {
    if (archive->pa_Id == id)
    {

      FreeIFF(archive->pa_Iff);
      Close(archive->pa_File);

      archive->pa_File = NULL;
      archive->pa_Iff = NULL;
      archive->pa_Id = 0;
      archive->pa_Usage = 0;

      Remove((struct Node*) archive);

      return;
    }
  }

}


EXPORT VOID CloseArchives()
{
  struct ARCHIVE* archive;

  while ( (archive = (struct ARCHIVE*) RemHead((struct List*) &OpenArchives) ) != NULL )
  {
    FreeIFF(archive->pa_Iff);
    Close(archive->pa_File);

    archive->pa_File = NULL;
    archive->pa_Iff = NULL;
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
  BOOL rc;

  rc = FALSE;

  if (NULL == archive)
  {
    ErrorF("Null Archive");
  }

  if (NULL == archive->pa_File)
  {
    ErrorF("Null pa_File");
  }

  Seek(archive->pa_File, 0, OFFSET_BEGINNING);
  LONG v = OpenIFF(archive->pa_Iff, IFFF_READ | IFFF_RSEEK);
  
  while (TRUE)
  {
    err = ParseIFF(archive->pa_Iff, IFFPARSE_RAWSTEP);

    if (err == IFFERR_EOC)
    {
      continue;
    }
    else if (err)
    {
      ErrorF("Chunk Read Error %ld", err);

      goto CLEAN_EXIT;
    }

    node = CurrentChunk(archive->pa_Iff);

    if (node->cn_ID == nodeType)
    {

      if ((node->cn_Size - sizeof(chunkHeader)) <= dataSize)
      {
        ReadChunkBytes(archive->pa_Iff, &chunkHeader, sizeof(struct CHUNK_HEADER));

        if (chunkHeader.ch_Id != chunkId)
        {
          continue;
        }

        if ((chunkHeader.ch_Flags & chunkArch) == 0)
        {
          continue;
        }

        ReadChunkBytes(archive->pa_Iff, data, dataSize);

        rc = TRUE;

        goto CLEAN_EXIT;
      }

      ErrorF("Error Chunk %s is to large (%ld) to fit into capacity (%ld)",
        IDtoStr(node->cn_ID, idBuf),
        node->cn_Size,
        dataSize
      );

      goto CLEAN_EXIT;
    }
  }

CLEAN_EXIT:

  CloseIFF(archive->pa_Iff);
  return rc;
}


EXPORT APTR LoadAsset(APTR arena, UWORD archiveId, ULONG nodeType, UWORD assetId, UWORD arch)
{
  struct ASSET* asset;
  struct ARCHIVE* archive;
  ULONG  assetSize;
  CHAR   strtype[5];

  for (asset = ((struct ASSET*) Assets.mlh_Head);
       asset != NULL; 
       asset = ((struct ASSET*)asset->as_Node.mln_Succ))
  {
    if (asset->as_ClassType == nodeType && asset->as_Id == assetId && (asset->as_Arch & arch) != 0)
    {
      return (APTR) (asset+1);
    }
  }

  archive = OpenArchive(archiveId);

  if (archive == NULL)
  {
    ErrorF("Could not open archive %ld", (ULONG)archiveId);
    return NULL;
  }

  assetSize = FindFactory(nodeType);

  if (assetSize == 0)
  {
    ErrorF("Could not find registered factory for \"%s\"", IDtoStr(nodeType, strtype) );
    return NULL;
  }

  asset = ObjAlloc(arena, assetSize + sizeof(struct ASSET), nodeType, TRUE);

  if (ReadAssetFromArchive(archive, nodeType, assetId, arch, (APTR) (asset + 1), assetSize) == FALSE)
  {
    ErrorF("Could not load asset %s:%ld from archive %ld", IDtoStr(nodeType, strtype), (ULONG) assetId, (ULONG) archiveId);
    return NULL;
  }

  asset->as_Id = assetId;
  asset->as_Arch = arch;
  asset->as_ClassType = nodeType;
  asset->as_Node.mln_Succ = NULL;
  asset->as_Node.mln_Pred = NULL;

  AddHead((struct List*) &Assets, (struct Node*) asset);

  return (APTR)(asset + 1);
}

EXPORT VOID UnloadAsset(APTR arena, struct ASSET* asset)
{
  
}

EXPORT VOID LoadObjectTable(struct OBJECT_TABLE_REF* ref)
{
  struct ARCHIVE* archive;
  struct OBJECT_TABLE* table;
  struct ASSET_FACTORY* assetFactory;
  struct ContextNode* node;
  struct CHUNK_HEADER chunkHeader;
  LONG err;
  CHAR idtype[5];
  
  table = NULL;
  assetFactory = &AssetFactories[0];

  while (assetFactory->af_NodeType != 0)
  {
    if (assetFactory->af_NodeType == ref->tr_ClassType)
    {
      table = assetFactory->af_Table;
      break;
    }

    assetFactory++;
  }

  if (table == NULL)
  {
    PARROT_ERR(
      "Unable to load Object Table from archive file.\n"
      "Reason: Could not find suitable table to write to. (1)"
      PARROT_ERR_STR("Class Type")
      PARROT_ERR_INT("Chunk ID")
      PARROT_ERR_INT("Archive ID"),
      IDtoStr(ref->tr_ClassType, idtype),
      (ULONG)ref->tr_ChunkHeaderId,
      (ULONG)ref->tr_ArchiveId
    );

    return;
  }
  
  archive = OpenArchive(ref->tr_ArchiveId);

  if (archive == NULL)
  {
    PARROT_ERR(
      "Unable to load Object Table from archive file.\n"
      "Reason: Archive could not be opened. (2)"
      PARROT_ERR_STR("Class Type")
      PARROT_ERR_INT("Chunk ID")
      PARROT_ERR_INT("Archive ID"),
      IDtoStr(ref->tr_ClassType, idtype),
      (ULONG)ref->tr_ChunkHeaderId,
      (ULONG)ref->tr_ArchiveId
    );
    return;
  }
  
  if (archive->pa_File == NULL)
  {

    PARROT_ERR(
      "Unable to load Object Table from archive file.\n"
      "Reason: File(pa_File) was NULL. (3)"
      PARROT_ERR_STR("Class Type")
      PARROT_ERR_INT("Chunk ID")
      PARROT_ERR_INT("Archive ID"),
      IDtoStr(ref->tr_ClassType, idtype),
      (ULONG)ref->tr_ChunkHeaderId,
      (ULONG)ref->tr_ArchiveId
    );

  }

  Seek(archive->pa_File, 0, OFFSET_BEGINNING);
  LONG v = OpenIFF(archive->pa_Iff, IFFF_READ | IFFF_RSEEK);

  while (TRUE)
  {
    err = ParseIFF(archive->pa_Iff, IFFPARSE_RAWSTEP);

    if (err == IFFERR_EOC)
    {
      continue;
    }
    else if (err == IFFERR_EOF)
    {
      break;
    }
    else if (err)
    {

      PARROT_ERR(
        "Unable to load Object Table from archive file.\n"
        "Reason: Chunk Error whilst parsing IFF. (4)"
        PARROT_ERR_STR("Class Type")
        PARROT_ERR_INT("Chunk ID")
        PARROT_ERR_INT("Archive ID")
        PARROT_ERR_INT("err"),
        IDtoStr(ref->tr_ClassType, idtype),
        (ULONG)ref->tr_ChunkHeaderId,
        (ULONG)ref->tr_ArchiveId,
        err
      );

      goto CLEAN_EXIT;
    }

    node = CurrentChunk(archive->pa_Iff);

    if (node->cn_ID == CT_TABLE)
    {
      if ((node->cn_Size - sizeof(chunkHeader)) == sizeof(struct OBJECT_TABLE))
      {
        ReadChunkBytes(archive->pa_Iff, &chunkHeader, sizeof(struct CHUNK_HEADER));

        if (chunkHeader.ch_Id != ref->tr_ChunkHeaderId)
        {
          continue;
        }

        ReadChunkBytes(archive->pa_Iff, table, sizeof(struct OBJECT_TABLE));

        goto CLEAN_EXIT;
      }


      PARROT_ERR(
        "Unable to load Object Table from archive file.\n"
        "Reason: Chunk is to large to fit in OBJECT_TABLE structure. (5)"
        PARROT_ERR_STR("Class Type")
        PARROT_ERR_INT("Chunk ID")
        PARROT_ERR_INT("Archive ID")
        PARROT_ERR_INT("node->cn_Size"),
        IDtoStr(ref->tr_ClassType, idtype),
        (ULONG)ref->tr_ChunkHeaderId,
        (ULONG)ref->tr_ArchiveId,
        node->cn_Size
      );

      goto CLEAN_EXIT;
    }
  }

  PARROT_ERR(
    "Unable to load Object Table from archive file.\n"
    "Reason: End of File"
    PARROT_ERR_STR("Class Type")
    PARROT_ERR_INT("Chunk ID")
    PARROT_ERR_INT("Archive ID"),
    IDtoStr(ref->tr_ClassType, idtype),
    (ULONG) ref->tr_ChunkHeaderId,
    (ULONG) ref->tr_ArchiveId
  );

CLEAN_EXIT:

  CloseIFF(archive->pa_Iff);
}
