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

STATIC CHAR strtype[5];

STATIC UWORD NextArchiveId;
STATIC UWORD NextRoomId;
STATIC UWORD NextEntityId;
STATIC UWORD NextImageId;
STATIC UWORD NextScriptId;
STATIC UWORD NextPaletteId;
STATIC UWORD NextTableId;

struct WRITE_ASSET_TABLE
{
  ULONG at_Capacity;
  struct ASSET_TABLE at_Table;
};

STATIC struct WRITE_ASSET_TABLE    Table[MAX_TABLES];
STATIC struct ASSET_TABLE_ENTRY*   TableData[MAX_TABLES];
STATIC char strtype[5];

STATIC VOID NewTable(UWORD idx, ULONG classType, UWORD chapter)
{
  struct WRITE_ASSET_TABLE* tbl;
  struct ASSET_TABLE_ENTRY* ent;

  tbl = &Table[idx];

  tbl->at_Capacity = NEW_TABLE_CAPACITY;
  tbl->at_Table.at_AssetType = classType;
  tbl->at_Table.at_Count = 0;
  tbl->at_Table.at_Highest = 0;
  tbl->at_Table.at_Lowest = 65535;
  tbl->at_Table.at_Chapter = chapter;

  ent = TableData[idx];

  TableData[idx] = AllocMem(tbl->at_Capacity * sizeof(struct ASSET_TABLE_ENTRY), MEMF_CLEAR);
}

STATIC VOID GrowTable(UWORD idx)
{
  struct WRITE_ASSET_TABLE* tbl;
  struct ASSET_TABLE_ENTRY* ent, *newEnt, *dst;
  ULONG newCapacity, capacity, ii;

  tbl = &Table[idx];

  capacity = tbl->at_Capacity;
  newCapacity = capacity * 2;

  ent = TableData[idx];
  newEnt = AllocMem(newCapacity * sizeof(struct ASSET_TABLE_ENTRY), MEMF_CLEAR);

  for (ii = 0; ii < capacity; ii++)
  {
    newEnt[ii] = ent[ii];
  }

  FreeMem(ent, capacity * sizeof(struct ASSET_TABLE_ENTRY));

  TableData[idx] = newEnt;
  tbl->at_Capacity = newCapacity;
}

STATIC ULONG GetOrAddTableIdx(ULONG classType, UWORD chapter)
{
  UWORD ii;
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
    (ULONG) chapter
  );
}

VOID StartTables()
{
  struct WRITE_ASSET_TABLE* tbl;
  UWORD ii;

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

VOID EndTables()
{
  UWORD ii;
  struct WRITE_ASSET_TABLE* tbl;

  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    if (tbl->at_Capacity == 0)
      continue;

    FreeMem(TableData[ii], tbl->at_Table.at_Count * sizeof(struct ASSET_TABLE_ENTRY));

    TableData[ii] = NULL;

    tbl->at_Capacity = 0;
    tbl->at_Table.at_AssetType = 0;
    tbl->at_Table.at_Chapter = 0;
    tbl->at_Table.at_Count = 0;
    tbl->at_Table.at_Highest = 0;
    tbl->at_Table.at_Lowest = 0;
  }
}

UWORD GenerateArchiveId()
{
  return NextArchiveId++;
}

UWORD GenerateAssetId(ULONG classType)
{
  switch (classType)
  {
    case CT_ROOM:  return NextRoomId++;
    case CT_ENTITY: return NextEntityId++;
    case CT_IMAGE: return NextImageId++;
    case CT_SCRIPT: return NextScriptId++;
    case CT_PALETTE: return NextPaletteId++;
    case CT_TABLE: return NextTableId++;
  }

  PARROT_ERR(
    "Unable to allocate Asset ID\n"
    "Reason: Unsupported type"
    PARROT_ERR_STR("Type"),
    IDtoStr(classType, strtype)
  );

  return 0;
}

VOID ExportTables(SquawkPtr squawk)
{
  UWORD ii;
  struct WRITE_ASSET_TABLE* tbl;
  
  StartAssetList(squawk, CT_TABLE, 0);
  
  for (ii = 0; ii < MAX_TABLES; ii++)
  {
    tbl = &Table[ii];

    if (tbl == NULL)
      continue;

    if (tbl->at_Table.at_Count == 0)
      continue;
    
    tbl->at_Table.as_Id = GenerateAssetId(CT_TABLE);
    tbl->at_Table.as_Flags = CHUNK_FLAG_ARCH_ANY;
    
    SaveAssetExtra(squawk,
      (struct ANY_ASSET*) &tbl->at_Table,
      sizeof(struct ASSET_TABLE),
      TableData[ii],
      tbl->at_Table.at_Count * sizeof(struct ASSET_TABLE_ENTRY)
    );
  }

  EndAssetList(squawk);
}

VOID AddToTable(ULONG classType, UWORD id, UWORD archive, UWORD chapter)
{
  UWORD tableIdx;
  struct WRITE_ASSET_TABLE* tbl;
  struct ASSET_TABLE_ENTRY* ent;

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

UWORD GetNumTables()
{
  UWORD ii;
  struct WRITE_ASSET_TABLE* tbl;
  UWORD count = 0;

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
