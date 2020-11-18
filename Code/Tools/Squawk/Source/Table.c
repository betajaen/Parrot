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
#include <Parrot/Asset.h>

STATIC CHAR strtype[5];

STATIC UWORD NextArchiveId;
STATIC UWORD NextRoomId;
STATIC UWORD NextEntityId;
STATIC UWORD NextImageId;
STATIC UWORD NextScriptId;

VOID StartTables()
{
  NextRoomId = 1;
  NextEntityId = 1;
  NextImageId = 1;
  NextScriptId = 1;
  NextArchiveId = 1;
}

VOID EndTables()
{
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
  }

  PARROT_ERR(
    "Unable to allocate Asset ID\n"
    "Reason: Unsupported type"
    PARROT_ERR_STR("Type"),
    IDtoStr(classType, strtype)
  );

  return 0;
}

VOID ExportTables(IffPtr master)
{
  
}

VOID AddToTable(ULONG classType, UWORD id, UWORD archive, UWORD chapter)
{
  
}
