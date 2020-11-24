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

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>
#include <exec/lists.h>

extern UWORD GcCounter;

struct NEW_ARCHIVE
{
  BPTR              ar_File;
  struct IFFHandle* ar_Iff;
  UWORD             ar_Id;
  UWORD             ar_Gc;
  ULONG             ar_CurrentAssetListType;
};

STATIC char strType[5];
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

  FreeIFF(archive->ar_Iff);
  Close(archive->ar_File);

  archive->ar_File = NULL;
  archive->ar_Iff = NULL;
  archive->ar_Id = 0;
  archive->ar_Gc = 0;
  archive->ar_CurrentAssetListType = 0;
}

STATIC VOID LoadArchive(struct NEW_ARCHIVE* archive, UWORD id)
{
  CHAR path[128];

  archive->ar_Id = id;
  archive->ar_Gc = GcCounter;
  archive->ar_CurrentAssetListType = 0;

  if (0 == StrFormat(&path[0], sizeof(path), DEFAULT_ARCHIVE_PATH_FSTR, (ULONG)id))
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (4) Base path is to long to be assembled into an archive path"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Base Path"),
      (ULONG)id,
      DEFAULT_ARCHIVE_PATH_FSTR
    );
  }

  archive->ar_File = Open(&path[0], MODE_OLDFILE);

  if (NULL == archive->ar_File)
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (5) Path does not exist or file cannot be opened"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      (ULONG)id,
      &path[0]
    );
  }

  archive->ar_Iff = AllocIFF();

  if (NULL == archive->ar_Iff)
  {
    PARROT_ERR(
      "Unable to load assets!\n"
      "Reason: (6) IFF archive could not be allocated"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      (ULONG)id,
      &path[0]
    );
  }

  archive->ar_Iff->iff_Stream = archive->ar_File;
  InitIFFasDOS(archive->ar_Iff);
}

STATIC struct NEW_ARCHIVE* GetOrOpenArchive(UWORD id)
{
  struct NEW_ARCHIVE* archive;
  UWORD ii;

  for (ii = 0; ii < MAX_OPEN_ARCHIVES; ii++)
  {
    archive = &Archives[ii];

    if (id == archive->ar_Id)
    {
      archive->ar_Gc = GcCounter;
      return archive;
    }
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
  archive->ar_Gc = GcCounter;

  /* ... */

  return FALSE;
}

STATIC UWORD LoadAll(ULONG classType, struct NEW_ARCHIVE* archive, struct NEW_ASSET* outAssets, UWORD outCapacity)
{
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

  /* ... */

  return 0;
}

STATIC APTR Load(ULONG classType, struct NEW_ARCHIVE* archive, UWORD id)
{
  APTR asset;

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

  return asset;
}

/* Public */

UWORD GetAllAssetsFromArchive(ULONG classType, UWORD archiveId, struct ARENA* arena, APTR outAssets, UWORD outCapacity)
{
  struct NEW_ARCHIVE* archive;

  archive = GetOrOpenArchive(archiveId);

  return LoadAll(classType, archive, outAssets, outCapacity);
}

APTR GetAssetFromArchive(ULONG classType, UWORD archiveId, UWORD id, struct ARENA* arena)
{
  struct NEW_ARCHIVE* archive;
  archive = GetOrOpenArchive(archiveId);

  return Load(classType, archive, id);
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
