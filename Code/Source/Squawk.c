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
#include <Parrot/Requester.h>
#include <Parrot/String.h>
#include <Parrot/Asset.h>
#include <Parrot/Arena.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <exec/lists.h>

extern UWORD GcCounter;

struct NEW_ARCHIVE
{
  BPTR              ar_File;
  UWORD             ar_Id;
  UWORD             ar_Gc;
  struct SQUAWK_ASSET_LIST_HEADER ar_CurrentHeader;
};

STATIC char strType[5];
STATIC char strType2[5];
STATIC struct NEW_ARCHIVE Archives[MAX_OPEN_ARCHIVES];

/* Private */

STATIC VOID UnloadArchive(struct NEW_ARCHIVE* archive)
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

STATIC VOID LoadArchive(struct NEW_ARCHIVE* archive, UWORD id)
{
  CHAR path[128];
  ULONG header;

  archive->ar_Id = id;
  archive->ar_Gc = GcCounter;

  if (0 == StrFormat(&path[0], sizeof(path), DEFAULT_ARCHIVE_PATH_FSTR, (ULONG)id))
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (1) Base path is to long to be assembled into an archive path"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Base Path"),
      (ULONG)id,
      DEFAULT_ARCHIVE_PATH_FSTR
    );
  }

  //RequesterF("OK", "Opening archive %s", path);

  archive->ar_File = Open(&path[0], MODE_OLDFILE);

  //RequesterF("OK", "Opening archive %ld", archive->ar_File);

  if (NULL == archive->ar_File)
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (2) Path does not exist or file cannot be opened"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      (ULONG)id,
      &path[0]
    );
  }

  Read(archive->ar_File, &header, sizeof(ULONG));

  if (header != ID_SQWK)
  {
    Close(archive->ar_File);

    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (3) File is not a Squawk file"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      (ULONG)id,
      &path[0]
    );
  }

}

STATIC struct NEW_ARCHIVE* GetOrOpenArchive(UWORD id)
{
  struct NEW_ARCHIVE* archive;
  UWORD ii;

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

STATIC BOOL NavigateToAssetList(struct NEW_ARCHIVE* archive, ULONG classType)
{
  LONG err;
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

STATIC ULONG NavigateToId(struct NEW_ARCHIVE* archive, UWORD id)
{
  struct ANY_ASSET hdr;
  UWORD ii, count;

  count = archive->ar_CurrentHeader.al_Count;

  for (ii = 0; ii < count; ii++)
  {
    Read(archive->ar_File, &hdr, sizeof(struct ANY_ASSET));

    if (hdr.as_Id != id)
    {
      Seek(archive->ar_File, hdr.as_Length - sizeof(struct ANY_ASSET), OFFSET_CURRENT);
      continue;
    }

    Seek(archive->ar_File, -(sizeof(struct ANY_ASSET)), OFFSET_CURRENT);
    return hdr.as_Length;
  }

  return 0;
}

STATIC UWORD LoadAll(ULONG classType, struct NEW_ARCHIVE* archive, struct ARENA* arena, struct ANY_ASSET** outAssets, UWORD outCapacity)
{
  UWORD count, ii;
  struct ANY_ASSET hdr;
  ULONG memSize;
  struct ANY_ASSET* asset;

  archive->ar_Gc = GcCounter;
  
  if (NavigateToAssetList(archive, classType) == FALSE)
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (1) No assets of type are in the given archive"
      PARROT_ERR_STR("Asset Type")
      PARROT_ERR_INT("Archive"),
      IDtoStr(classType, strType),
      (ULONG)archive->ar_Id
      );
  }

  count = outCapacity;

  if (archive->ar_CurrentHeader.al_Count < count)
  {
    count = archive->ar_CurrentHeader.al_Count;
  }

  for (ii = 0; ii < count; ii++)
  {
    Read(archive->ar_File, &hdr, sizeof(struct ANY_ASSET));
    
    memSize = sizeof(struct ANY_ASSET) + hdr.as_Length;
    asset = (struct ANY_ASSET*) NewObject(arena, memSize, FALSE);
    asset->as_Id = hdr.as_Id;
    asset->as_Flags = hdr.as_Flags;
    asset->as_Length = hdr.as_Length;

    Read(archive->ar_File, (APTR) (&asset[1]), hdr.as_Length);

    outAssets[ii] = asset;
  }

  return count;

}

STATIC struct ANY_ASSET* Load(ULONG classType, struct NEW_ARCHIVE* archive, struct ARENA* arena, UWORD id)
{
  struct ANY_ASSET* asset;
  ULONG assetLength;

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
      (ULONG) archive->ar_Id,
      (ULONG) id
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
      (ULONG)archive->ar_Id,
      (ULONG)id
    );
  }

  

  asset = (struct ANY_ASSET*) NewObject(arena, assetLength, FALSE);

  Read(archive->ar_File, (APTR)asset, assetLength);

  return asset;
}

/* Public */

VOID InitArchives()
{
  struct NEW_ARCHIVE* archive;

  for (UWORD ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];

    archive->ar_Id = 0;
    archive->ar_Gc = 0;
    archive->ar_File = 0;
    archive->ar_CurrentHeader.al_Length = 0;
    archive->ar_CurrentHeader.al_Type = 0;
  }
}

UWORD GetAllAssetsFromArchive(ULONG classType, UWORD archiveId, struct ARENA* arena, struct ANY_ASSET** outAssets, UWORD outCapacity)
{
  struct NEW_ARCHIVE* archive;

  archive = GetOrOpenArchive(archiveId);

  return LoadAll(classType, archive, arena, outAssets, outCapacity);
}

APTR GetAssetFromArchive(ULONG classType, UWORD archiveId, UWORD id, struct ARENA* arena)
{
  struct NEW_ARCHIVE* archive;
  archive = GetOrOpenArchive(archiveId);

  return Load(classType, archive, arena, id);
}

VOID GcArchives(UWORD olderThan)
{
  struct NEW_ARCHIVE* archive;

  for (UWORD ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];

    if (archive->ar_Gc > 0 && archive->ar_Gc <= olderThan)
    {
      UnloadArchive(archive);
    }
  }
}
