/**
    $Id: String.c, 1.2 2020/05/07 08:43:00, betajaen Exp $

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
#include <Parrot/Squawk.h>
#include <Parrot/Requester.h>
#include <Parrot/String.h>
#include <Parrot/Arena.h>
#include <Parrot/Log.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <exec/lists.h>

#define NO_ARCHIVE 65535

extern PtUnsigned16 GcCounter;

struct Archive
{
  BPTR              ar_File;
  PtUnsigned16             ar_Id;
  PtUnsigned16             ar_Gc;
  struct SQUAWK_ASSET_LIST_HEADER ar_CurrentHeader;
};

STATIC char strType[5];
STATIC char strType2[5];
STATIC struct Archive Archives[MAX_OPEN_ARCHIVES];

/* Private */

STATIC void UnloadArchive(struct Archive* archive)
{
  /* Check to see if the file pointer exists.
     If not then the archive isn't opened.
  */
  if (archive->ar_File == NULL)
  {
    return;
  }

  Close(archive->ar_File);

  archive->ar_File = NULL;
  archive->ar_Id = 0;
  archive->ar_Gc = 0;
}

STATIC void LoadArchive(struct Archive* archive, PtUnsigned16 id)
{
  PtChar path[128];
  PtUnsigned32 header;

  archive->ar_Id = id;
  archive->ar_Gc = GcCounter;

  if (0 == StrFormat(&path[0], sizeof(path), DEFAULT_ARCHIVE_PATH_FSTR, (PtUnsigned32)id))
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (1) Base path is to long to be assembled into an archive path"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Base Path"),
      (PtUnsigned32)id,
      DEFAULT_ARCHIVE_PATH_FSTR
    );
  }

  archive->ar_File = Open(&path[0], MODE_OLDFILE);

  if (NULL == archive->ar_File)
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (2) Path does not exist or file cannot be opened"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      (PtUnsigned32)id,
      &path[0]
    );
  }

  Read(archive->ar_File, &header, sizeof(PtUnsigned32));

  if (header != ID_SQWK)
  {
    Close(archive->ar_File);

    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (3) File is not a Squawk file"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      (PtUnsigned32)id,
      &path[0]
    );
  }

}

STATIC struct Archive* GetOrOpenArchive(PtUnsigned16 id)
{
  struct Archive* archive;
  PtUnsigned16 ii;

  for (ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];

    if (id != archive->ar_Id)
      continue;

    if (NULL == archive->ar_File)
      continue;

    archive->ar_Gc = GcCounter;
    return archive;
  }

  for (ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];
    
    /* An unused archive is determined by a NULL file pointer. */
    if (NULL == archive->ar_File)
    {
      LoadArchive(archive, id);
      return archive;
    }
  }

  PARROT_ERR(
    "Unable to load assets!\n"
    "Reason: (1) Number of archives open exceed the maximum open archives"
    PARROT_ERR_INT("Archive"),
    id
  );

  return NULL;
}

STATIC BOOL NavigateToAssetList(struct Archive* archive, PtUnsigned32 classType)
{
  PtSigned32 err;
  struct SQUAWK_ASSET_LIST_HEADER hdr;

  err = Seek(archive->ar_File, 4, OFFSET_BEGINNING);

  while (TRUE)
  {
    Read(archive->ar_File, &hdr, sizeof(struct SQUAWK_ASSET_LIST_HEADER));

    if (hdr.al_Type == classType)
    {
      archive->ar_CurrentHeader = hdr;
      return TRUE;
    }

    if (hdr.al_Type == MAKE_ID('S', 'T', 'O', 'P'))
    {
      break;
    }

    Seek(archive->ar_File, hdr.al_Length, OFFSET_CURRENT);
  }

  return FALSE;
}

STATIC PtUnsigned32 NavigateToId(struct Archive* archive, PtUnsigned16 id)
{
  PtAsset hdr;
  PtUnsigned16 ii, count;

  count = archive->ar_CurrentHeader.al_Count;

  for (ii = 0; ii < count; ii++)
  {
    Read(archive->ar_File, &hdr, sizeof(PtAsset));

    if (hdr.as_Id != id)
    {
      Seek(archive->ar_File, hdr.as_Length - sizeof(PtAsset), OFFSET_CURRENT);
      continue;
    }

    Seek(archive->ar_File, -(sizeof(PtAsset)), OFFSET_CURRENT);
    return hdr.as_Length;
  }

  return 0;
}

STATIC PtUnsigned16 LoadAll(PtUnsigned32 classType, struct Archive* archive, struct ARENA* arena, PtAsset** outAssets, PtUnsigned16 outCapacity)
{
  PtUnsigned16 count, ii;
  PtAsset hdr;
  PtUnsigned32 memSize;
  PtAsset* asset;

  archive->ar_Gc = GcCounter;
  
  if (NavigateToAssetList(archive, classType) == FALSE)
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (1) No assets of type are in the given archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id
      );
  }

  count = outCapacity;

  if (archive->ar_CurrentHeader.al_Count < count)
  {
    count = archive->ar_CurrentHeader.al_Count;
  }

  for (ii = 0; ii < count; ii++)
  {
    Read(archive->ar_File, &hdr, sizeof(PtAsset));
    
    memSize = sizeof(PtAsset) + hdr.as_Length;
    asset = (PtAsset*) NewObject(arena, memSize, FALSE);
    asset->as_Id = hdr.as_Id;
    asset->as_Flags = hdr.as_Flags;
    asset->as_Length = hdr.as_Length;

    Read(archive->ar_File, (APTR) (&asset[1]), hdr.as_Length);

    outAssets[ii] = asset;
  }

  return count;

}

STATIC PtAsset* Load(PtUnsigned32 classType, struct Archive* archive, struct ARENA* arena, PtUnsigned16 id)
{
  PtAsset* asset;
  PtUnsigned32 assetLength;

  asset = NULL;

  archive->ar_Gc = GcCounter;

  if (NavigateToAssetList(archive, classType) == FALSE)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (2) No assets of type are in the given archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32) archive->ar_Id,
      (PtUnsigned32) id
    );
  }

  assetLength = NavigateToId(archive, id);

  if (0 == assetLength)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (3) No assets of this id are in the given asset list in this archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id,
      (PtUnsigned32)id
    );
  }
  
  asset = (PtAsset*) NewObject(arena, assetLength, FALSE);
  
  Read(archive->ar_File, (APTR)asset, assetLength);
  
  return asset;
}


STATIC BOOL LoadInto(PtUnsigned32 classType, struct Archive* archive, PtAsset* asset, PtUnsigned32 assetSize, PtUnsigned16 id)
{
  PtUnsigned32 assetLength;

  archive->ar_Gc = GcCounter;

  if (NavigateToAssetList(archive, classType) == FALSE)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (2) No assets of type are in the given archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id,
      (PtUnsigned32)id
    );
  }

  assetLength = NavigateToId(archive, id);

  if (0 == assetLength)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (3) No assets of this id are in the given asset list in this archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id,
      (PtUnsigned32)id
    );
  }

  if (assetLength > assetSize)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (4) Asset does not fit in given asset"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("Asset Size (Given)")
      PARROT_ERR_INT("Asset Size (Stored"),
      IDtoStr(classType, strType),
      (PtUnsigned32) id,
      (PtUnsigned32) assetSize,
      (PtUnsigned32) assetLength
    );
  }

  TRACEF("About to read %lx into %lx of length %ld", archive, asset, assetLength);
  
  Read(archive->ar_File, (APTR)asset, assetLength);
  
  return TRUE;
}


STATIC BOOL LoadInto_Callback(PtUnsigned32 classType, struct Archive* archive, PtAsset* asset, PtUnsigned32 assetSize, PtUnsigned16 id, LoadSpecialCallback cb)
{
  PtUnsigned32 assetLength;

  archive->ar_Gc = GcCounter;

  if (NavigateToAssetList(archive, classType) == FALSE)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (2) No assets of type are in the given archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id,
      (PtUnsigned32)id
    );
  }

  assetLength = NavigateToId(archive, id);

  TRACEF("About to read %lx into %lx of length %ld", archive, asset, assetLength);

  Read(archive->ar_File, (APTR)asset, assetSize);

  PtUnsigned16 counter = 0;

  while (TRUE)
  {
    PtUnsigned32 readLength = 0;
    APTR readInto = 0;
    cb(asset, counter, &readLength, &readInto);
    
    if (readLength == 0)
      break;

    PtSigned32 r = Read(archive->ar_File, readInto, readLength);

    TRACEF("Read %ld of %ld into %lx", r, readLength, readInto);

    PtUnsigned32* rx = readInto;
    *rx = 0x55555555;

    counter++;
  }

  return TRUE;
}

STATIC BOOL LoadIntoRaster(PtUnsigned32 classType, struct Archive* archive, PtAsset* asset, PtUnsigned32 assetSize, PtUnsigned16 id)
{
  PtUnsigned32 assetLength;

  archive->ar_Gc = GcCounter;

  if (NavigateToAssetList(archive, classType) == FALSE)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (2) No assets of type are in the given archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id,
      (PtUnsigned32)id
    );
  }

  assetLength = NavigateToId(archive, id);

  if (0 == assetLength)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (3) No assets of this id are in the given asset list in this archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive")
      PARROT_ERR_INT("Asset"),
      IDtoStr(classType, strType),
      (PtUnsigned32)archive->ar_Id,
      (PtUnsigned32)id
    );
  }

  if (assetLength > assetSize)
  {
    PARROT_ERR(
      "Unable to load asset!\n"
      "Reason: (4) Asset does not fit in given asset"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("Asset Size (Given)")
      PARROT_ERR_INT("Asset Size (Stored"),
      IDtoStr(classType, strType),
      (PtUnsigned32)id,
      (PtUnsigned32)assetSize,
      (PtUnsigned32)assetLength
    );
  }

  TRACEF("About to read %lx into %lx of length %ld", archive, asset, assetLength);

  Read(archive->ar_File, (APTR)asset, assetLength);

  return TRUE;
}
/* Public */

void Archives_Initialise()
{
  struct Archive* archive;

  for (PtUnsigned16 ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];

    archive->ar_Id = 0;
    archive->ar_Gc = 0;
    archive->ar_File = 0;
    archive->ar_CurrentHeader.al_Length = 0;
    archive->ar_CurrentHeader.al_Type = 0;
  }
}


PtUnsigned16 Asset_LoadAll(PtUnsigned32 classType, PtUnsigned16 archiveId, struct ARENA* arena, PtAsset** outAssets, PtUnsigned16 outCapacity)
{
  struct Archive* archive;

  archive = GetOrOpenArchive(archiveId);

  return LoadAll(classType, archive, arena, outAssets, outCapacity);
}

PtAsset* Asset_Load_KnownArchive(PtUnsigned32 classType, PtUnsigned16 archiveId, PtUnsigned16 id, struct ARENA* arena)
{
  struct Archive* archive;
  archive = GetOrOpenArchive(archiveId);

  return Load(classType, archive, arena, id);
}

BOOL Asset_LoadInto_KnownArchive(PtUnsigned16 id, PtUnsigned16 archiveId, PtUnsigned32 classType, PtAsset* outAsset, PtUnsigned32 assetSize)
{
  struct Archive* archive;
  archive = GetOrOpenArchive(archiveId);

  TRACEF("ASSET LoadIntoKnownArchive. Id=%ld, ArchiveId=%ld, ClassType=%s", id, archiveId, IDtoStr(classType, strType));

  return LoadInto(classType, archive, outAsset, assetSize, id);
}

BOOL Asset_LoadInto_Callback_KnownArchive(PtUnsigned16 id, PtUnsigned16 archiveId, PtUnsigned32 classType, PtAsset* outAsset, PtUnsigned32 assetSize, LoadSpecialCallback cb)
{
  struct Archive* archive;
  archive = GetOrOpenArchive(archiveId);

  TRACEF("ASSET LoadIntoKnownRasterArchive. Id=%ld, ArchiveId=%ld, ClassType=%s", id, archiveId, IDtoStr(classType, strType));

  return LoadInto_Callback(classType, archive, outAsset, assetSize, id, cb);
}

void Archives_Unload(PtUnsigned16 olderThan)
{
  struct Archive* archive;

  for (PtUnsigned16 ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];

    if (archive->ar_Gc > 0 && archive->ar_Gc <= olderThan)
    {
      UnloadArchive(archive);
    }
  }
}


PtAsset* Asset_Load(PtUnsigned16 id, PtUnsigned16 chapter, PtUnsigned32 assetType, struct ARENA* arena)
{
  PtUnsigned16 archiveId;

  TRACEF("ASSET GetAsset. Id = %ld, Chapter = %ld, AssetType = %s", id, chapter, IDtoStr(assetType, strType));

  archiveId = AssetTables_FindArchive(id, chapter, assetType);

  if (NO_ARCHIVE == archiveId)
  {
    PARROT_ERR(
      "Unable to load asset\n"
      "Reason: (1) Asset Id was not found in any asset table"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("Chapter"),
      IDtoStr(assetType, strType),
      (PtUnsigned32)id,
      (PtUnsigned32)chapter
    );
  }

  /* FUTURE - CACHING */

  return Asset_Load_KnownArchive(assetType, archiveId, id, arena);
}

BOOL Asset_LoadInto(PtUnsigned16 id, PtUnsigned16 chapter, PtUnsigned32 assetType, PtAsset* asset, PtUnsigned32 assetSize)
{
  PtUnsigned16 archiveId;

  TRACEF("ASSET LoadInto. Id = %ld, Chapter = %ld, AssetType = %s", id, chapter, IDtoStr(assetType, strType));

  archiveId = AssetTables_FindArchive(id, chapter, assetType);

  if (NO_ARCHIVE == archiveId)
  {
    PARROT_ERR(
      "Unable to load asset\n"
      "Reason: (1) Asset Id was not found in any asset table"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("Chapter"),
      IDtoStr(assetType, strType),
      (PtUnsigned32)id,
      (PtUnsigned32)chapter
    );
  }

  return Asset_LoadInto_KnownArchive(id, archiveId, assetType, asset, assetSize);
}

void Asset_Unload(PtAsset* asset)
{
  TRACEF("ReleaseAsset. Id = %ld", asset != NULL ? asset->as_Id : 0);

  /* FUTURE */
}


BOOL Asset_LoadInto_Callback(PtUnsigned16 id, PtUnsigned16 chapter, PtUnsigned32 assetType, PtAsset* asset, PtUnsigned32 assetSize, LoadSpecialCallback cb)
{

  PtUnsigned16 archiveId;

  TRACEF("ASSET LoadInto. Id = %ld, Chapter = %ld, AssetType = %s", id, chapter, IDtoStr(assetType, strType));

  archiveId = AssetTables_FindArchive(id, chapter, assetType);

  if (NO_ARCHIVE == archiveId)
  {
    PARROT_ERR(
      "Unable to load asset\n"
      "Reason: (1) Asset Id was not found in any asset table"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("Chapter"),
      IDtoStr(assetType, strType),
      (PtUnsigned32)id,
      (PtUnsigned32)chapter
    );
  }

  return Asset_LoadInto_Callback_KnownArchive(id, archiveId, assetType, asset, assetSize, cb);
}



PtAsset* Asset_New(PtUnsigned32 size)
{
  PtAsset* asset;

  asset = AllocMem(size, MEMF_CLEAR);

  asset->as_Id = 0;
  asset->as_Flags = PT_AF_INSTANCE | PT_AF_ARCH_ANY;
  asset->as_Length = size;

  return asset;
}

PtAsset* Asset_New1(PtUnsigned32 size, PtUnsigned32 dataSize, PtUnsigned32 numDataElements)
{
  PtAsset* asset;
  PtUnsigned32 realSize;

  realSize = size + (dataSize * numDataElements);

  asset = AllocMem(realSize, MEMF_CLEAR);

  asset->as_Id = 0;
  asset->as_Flags = PT_AF_INSTANCE | PT_AF_ARCH_ANY | PT_AF_HAS_DATA;
  asset->as_Length = realSize;

  return asset;
}

void Asset_Release(PtAsset* asset)
{
  if (asset == NULL)
  {
    WARNING("Tried to release a NULL asset!");
    return;
  }

  if (asset->as_Flags & PT_AF_INSTANCE)
  {
    TRACEF("ASSET Release. Releasing Instance %ld", asset->as_Id);
    FreeMem(asset, asset->as_Length);
    return;
  }
  else
  {
    TRACEF("ASSET Release. Asset %ld was not released! Due to unimplementation", asset->as_Id);
  }

}
