/**
    $Id: Main.c, 1.0, 2020/05/13 07:16:00, betajaen Exp $

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

#define SAVE_PATH "PROGDIR:%ld.Parrot"

struct SquawkFile
{
  BPTR   sq_File;
  ULONG  sq_ListPos;
  struct SQUAWK_ASSET_LIST_HEADER sq_ListHeader;
};

#define SQUAWK_HEADER "SQWK"
#define SQUAWK_FOOTER "STOP\0\0\0\0\0\0\0\0"

STATIC CHAR strtype[5];

SquawkPtr OpenSquawkFile(UWORD id)
{
  SquawkPtr squawk;
  LONG err;
  CHAR path[1 + sizeof(SAVE_PATH) + 6];

  squawk = AllocMem(sizeof(struct SquawkFile), MEMF_CLEAR);

  StrFormat(path, sizeof(path), SAVE_PATH, (ULONG)id);

  squawk->sq_File = Open(path, MODE_NEWFILE);

  if (squawk->sq_File == 0)
  {
    PARROT_ERR(
      "Unable open squawk file for writing.\n"
      "Reason: (2) File was not opened for writing"
      PARROT_ERR_INT("Id")
      PARROT_ERR_PTR("Ptr")
      PARROT_ERR_STR("Path")
      PARROT_ERR_INT("IoErr"),
      id,
      squawk->sq_File,
      path,
      IoErr()
    );
  }
  
  Write(squawk->sq_File, SQUAWK_HEADER, literal_strlen(SQUAWK_HEADER));
  
  return squawk;
}
VOID EndAssetList(SquawkPtr squawk);

VOID CloseSquawkFile(SquawkPtr squawk)
{
  if (squawk->sq_ListPos > 0)
  {
    EndAssetList(squawk);
  }

  /* Footer */
  Write(squawk->sq_File, SQUAWK_FOOTER, literal_strlen(SQUAWK_FOOTER));

  Close(squawk->sq_File);

  FreeMem(squawk, sizeof(struct SquawkFile));
}

VOID StartAssetList(SquawkPtr squawk, ULONG classType, UWORD chapter)
{
  ULONG zero;

  if (squawk->sq_ListPos != 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Asset list is already open"
      PARROT_ERR_STR("Current"),
      IDtoStr(squawk->sq_ListHeader.al_Type, strtype)
    );
  }

  squawk->sq_ListHeader.al_Type = classType;
  squawk->sq_ListHeader.al_Chapter = chapter;
  squawk->sq_ListHeader.al_Count = 0;
  squawk->sq_ListHeader.al_Length = 0;

  squawk->sq_ListPos = Seek(squawk->sq_File, 0, OFFSET_CURRENT);

  Write(squawk->sq_File, &squawk->sq_ListHeader, sizeof(struct SQUAWK_ASSET_LIST_HEADER));
}

VOID EndAssetList(SquawkPtr squawk)
{
  ULONG len;

  if (squawk->sq_ListPos > 0)
  {
    len = Seek(squawk->sq_File, 0, OFFSET_CURRENT);
    len = len - squawk->sq_ListPos;
    len -= sizeof(struct SQUAWK_ASSET_LIST_HEADER);

    squawk->sq_ListHeader.al_Length = len;

    Seek(squawk->sq_File, squawk->sq_ListPos, OFFSET_BEGINNING);
    Write(squawk->sq_File, &squawk->sq_ListHeader, sizeof(struct SQUAWK_ASSET_LIST_HEADER));
    Seek(squawk->sq_File, 0, OFFSET_END);

    squawk->sq_ListHeader.al_Type = 0;
    squawk->sq_ListHeader.al_Chapter = 0;
    squawk->sq_ListHeader.al_Count = 0;
    squawk->sq_ListHeader.al_Length = 0;
    squawk->sq_ListPos = 0;
  }
  else
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Asset list is already open"
      PARROT_ERR_STR("Current"),
      IDtoStr(squawk->sq_ListHeader.al_Type, strtype)
    );
  }
}

VOID SaveAsset(SquawkPtr squawk, struct ANY_ASSET* asset, ULONG assetSize)
{
  asset->as_Length = assetSize;

  Write(squawk->sq_File, asset, assetSize);

  squawk->sq_ListHeader.al_Count++;
}

VOID SaveAssetExtra(SquawkPtr squawk, struct ANY_ASSET* asset, ULONG assetSize, APTR data, ULONG dataLength)
{
  asset->as_Length = assetSize + dataLength;
  asset->as_Flags |= CHUNK_FLAG_HAS_DATA;

  Write(squawk->sq_File, asset, assetSize);
  Write(squawk->sq_File, data, dataLength);

  squawk->sq_ListHeader.al_Count++;
}
