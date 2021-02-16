/**
    $Id: Tables.c, 1.0, 2020/05/13 07:16:00, betajaen Exp $

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

#include <Squawk/Squawk.h>

#define MAX_TABLES 16
#define NEW_TABLE_CAPACITY 1024

STATIC PtChar strtype[5];

STATIC PtUnsigned16 NextArchiveId;
STATIC PtUnsigned16 NextRoomId;
STATIC PtUnsigned16 NextEntityId;
STATIC PtUnsigned16 NextImageId;
STATIC PtUnsigned16 NextScriptId;
STATIC PtUnsigned16 NextPaletteId;
STATIC PtUnsigned16 NextTableId;

struct WRITE_ASSET_TABLE
{
  PtUnsigned32 at_Capacity;
  struct AssetTable at_Table;
};

STATIC struct WRITE_ASSET_TABLE    Table[MAX_TABLES];
STATIC struct AssetTableEntry*   TableData[MAX_TABLES];
STATIC char strtype[5];

STATIC void NewTable(PtUnsigned16 idx, PtUnsigned32 classType, PtUnsigned16 chapter)
{
  struct WRITE_ASSET_TABLE* tbl;
  struct AssetTableEntry* ent;

  tbl = &Table[idx];

  tbl->at_Capacity = NEW_TABLE_CAPACITY;
  tbl->at_Table.at_AssetType = classType;
  tbl->at_Table.at_Count = 0;
  tbl->at_Table.at_Highest = 0;
  tbl->at_Table.at_Lowest = 65535;
  tbl->at_Table.at_Chapter = chapter;

  ent = TableData[idx];

  TableData[idx] = AllocMem(tbl->at_Capacity * sizeof(struct AssetTableEntry), MEMF_CLEAR);
}

STATIC void GrowTable(PtUnsigned16 idx)
{
  struct WRITE_ASSET_TABLE* tbl;
  struct AssetTableEntry* ent, *newEnt, *dst;
  PtUnsigned32 newCapacity, capacity, ii;

  tbl = &Table[idx];

  capacity = tbl->at_Capacity;
  newCapacity = capacity * 2;

  ent = TableData[idx];
  newEnt = AllocMem(newCapacity * sizeof(struct AssetTableEntry), MEMF_CLEAR);

  for (ii = 0; ii < capacity; ii++)
  {
    newEnt[ii] = ent[ii];
  }

  FreeMem(ent, capacity * sizeof(struct AssetTableEntry));

  TableData[idx] = newEnt;
  tbl->at_Capacity = newCapacity;
}

STATIC PtUnsigned32 GetOrAddTableIdx(PtUnsigned32 classType, PtUnsigned16 chapter)
{
  PtUnsigned16 ii;
  struct WRITE_ASSET_TABLE* tbl;

  for (ii = 0; ii < MAX_TABLES;ii++)
  {
    tbl = &Table[ii];

    if (tbl->at_Capacity == 0)
      continue;

    if (tbl->at_Table.at_AssetType != classType)
      continue;

    if (tbl->at_Table.at_Chapter != chapter)
      continue;

    if (tbl->at_Table.at_Count == tbl->at_Capacity)
    {
      GrowTable(ii);
    }

    return ii;
  }

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    if (tbl->at_Capacity != 0)
      continue;

    NewTable(ii, classType, chapter);
    return ii;
  }

  PARROT_ERR(
    "Unable to save table data.\n"
    "Reason: (1) All tables are full"
    PARROT_ERR_STR("Class Type")
    PARROT_ERR_INT("Chapter"),
    IDtoStr(classType, strtype),
    (PtUnsigned32) chapter
  );
}

void StartTables()
{
  struct WRITE_ASSET_TABLE* tbl;
  PtUnsigned16 ii;

  NextRoomId = 1;
  NextEntityId = 1;
  NextImageId = 1;
  NextScriptId = 1;
  NextArchiveId = 1;
  NextPaletteId = 1;
  NextTableId = 1;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    tbl->at_Capacity = 0;
    tbl->at_Table.at_AssetType = 0;
    tbl->at_Table.at_Chapter = 0;
    tbl->at_Table.at_Count = 0;
    tbl->at_Table.at_Highest = 0;
    tbl->at_Table.at_Lowest = 0;

    TableData[ii] = NULL;
  }
}

void EndTables()
{
  PtUnsigned16 ii;
  struct WRITE_ASSET_TABLE* tbl;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    if (tbl->at_Capacity == 0)
      continue;

    FreeMem(TableData[ii], tbl->at_Table.at_Count * sizeof(struct AssetTableEntry));

    TableData[ii] = NULL;

    tbl->at_Capacity = 0;
    tbl->at_Table.at_AssetType = 0;
    tbl->at_Table.at_Chapter = 0;
    tbl->at_Table.at_Count = 0;
    tbl->at_Table.at_Highest = 0;
    tbl->at_Table.at_Lowest = 0;
  }
}

PtUnsigned16 GenerateArchiveId()
{
  return NextArchiveId++;
}

PtUnsigned16 GenerateAssetId(PtUnsigned32 classType)
{
  switch (classType)
  {
    case PT_AT_ROOM:  return NextRoomId++;
    case PT_AT_ENTITY: return NextEntityId++;
    case PT_AT_IMAGE: return NextImageId++;
    case PT_AT_SCRIPT: return NextScriptId++;
    case PT_AT_PALETTE: return NextPaletteId++;
    case PT_AT_TABLE: return NextTableId++;
  }

  PARROT_ERR(
    "Unable to allocate Asset ID\n"
    "Reason: Unsupported type"
    PARROT_ERR_STR("Type"),
    IDtoStr(classType, strtype)
  );

  return 0;
}

void ExportTables(SquawkPtr squawk)
{
  PtUnsigned16 ii;
  struct WRITE_ASSET_TABLE* tbl;
  
  StartAssetList(squawk, PT_AT_TABLE, 0);
  
  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    if (tbl == NULL)
      continue;

    if (tbl->at_Table.at_Count == 0)
      continue;
    
    tbl->at_Table.as_Id = GenerateAssetId(PT_AT_TABLE);
    tbl->at_Table.as_Flags = PT_AF_ARCH_ANY;
    
    SaveAssetExtra(squawk,
      (PtAsset*) &tbl->at_Table,
      sizeof(struct AssetTable),
      TableData[ii],
      tbl->at_Table.at_Count * sizeof(struct AssetTableEntry)
    );
  }

  EndAssetList(squawk);
}

void AddToTable(PtUnsigned32 classType, PtUnsigned16 id, PtUnsigned16 archive, PtUnsigned16 chapter)
{
  PtUnsigned16 tableIdx;
  struct WRITE_ASSET_TABLE* tbl;
  struct AssetTableEntry* ent;

  tableIdx = GetOrAddTableIdx(classType, chapter);

  tbl = &Table[tableIdx];
  ent = &TableData[tableIdx][tbl->at_Table.at_Count];

  ent->ti_Archive = archive;
  ent->ti_Id = id;

  if (id > tbl->at_Table.at_Highest)
  {
    tbl->at_Table.at_Highest = id;
  }

  if (id < tbl->at_Table.at_Lowest)
  {
    tbl->at_Table.at_Lowest = id;
  }

  ++tbl->at_Table.at_Count;
}

PtUnsigned16 GetNumTables()
{
  PtUnsigned16 ii;
  struct WRITE_ASSET_TABLE* tbl;
  PtUnsigned16 count = 0;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    if (tbl == NULL)
      continue;

    if (tbl->at_Table.at_Count == 0)
      continue;

    ++count;
  }

  return count;
}
