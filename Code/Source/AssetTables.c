/**
    $Id: Arena.c, 1.2 2020/05/11 10:48:00, betajaen Exp $

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
#include <Parrot/Squawk.h>
#include <Parrot/Log.h>

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/iffparse.h>

#define NO_ARCHIVE 0xFFFF

STATIC struct AssetTable* AssetTables[256];
STATIC PtUnsigned16  NumAssetTables;
STATIC char strType[5];


PtUnsigned16 AssetTables_FindArchive(PtUnsigned16 assetId, PtUnsigned16 chapter, PtUnsigned32 assetType)
{
  PtUnsigned16 ii, jj;
  struct AssetTable* tbl;
  struct AssetTableEntry* ent;

  for (ii = 0; ii < NumAssetTables; ii++)
  {
    tbl = AssetTables[ii];
    
    //RequesterF("OK", "Table %ld, Type=%s, Count=%ld, Chapter=%ld",
    //  (PtUnsigned32)ii, IDtoStr(tbl->at_AssetType, strType), (PtUnsigned32)tbl->at_Count, (PtUnsigned32)tbl->at_Chapter);

    if (tbl->at_AssetType != assetType)
      continue;

    if (tbl->at_Chapter != chapter)
      continue;

    for (jj = 0; jj < tbl->at_Count; jj++)
    {
      ent = &tbl->at_Assets[jj];
      
      if (ent->ti_Id != assetId)
        continue;

      return ent->ti_Archive;
    }
  }

  return NO_ARCHIVE;
}

void AssetTables_Load(PtUnsigned16 archive, PtUnsigned16 chapter, PtUnsigned16 count)
{
  PtUnsigned16 ii;
  NumAssetTables = count;
  struct AssetTable* tbl;

  TRACEF("LoadAssetTables. Archive = %ld, Chapter = %ld, Count = %ld", archive, chapter, count);

  for (ii = 0; ii < count; ii++)
  {
    tbl = (struct AssetTable*)Asset_Load_KnownArchive(PT_AT_TABLE, archive, 1 + ii, ArenaGame);

    if (NULL == tbl)
    {
      PARROT_ERR(
        "Unable to load asset table!\n"
        "Reason: (1) Asset Table does not exist in archive"
        PARROT_ERR_INT("Asset Type")
        PARROT_ERR_INT("Archive"),
        (PtUnsigned32)(1+ii),
        (PtUnsigned32)archive
      );
    }

    TRACEF("LoadAssetTables. Loaded. Id = %ld, Count = %ld, Chapter = %ld, AssetType = %s", tbl->as_Id, tbl->at_Count, tbl->at_Chapter, IDtoStr(tbl->at_AssetType, strType));

    AssetTables[ii] = tbl;
  }
}

