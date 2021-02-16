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
#include <Parrot/Squawk.h>

#define SAVE_PATH "PROGDIR:%ld.Parrot"

struct Archive
{
  BPTR   sq_File;
  PtUnsigned32  sq_ListPos;
  PtChar   sq_Path[1 + sizeof(SAVE_PATH) + 6];
  struct SQUAWK_ASSET_LIST_HEADER sq_ListHeader;
};

#define SQUAWK_HEADER "SQWK"
#define SQUAWK_FOOTER "STOP\0\0\0\0\0\0\0\0"

STATIC PtChar strtype[5];

SquawkPtr OpenSquawkFile(PtUnsigned16 id)
{
  SquawkPtr squawk;
  PtSigned32 err;

  squawk = AllocMem(sizeof(struct Archive), MEMF_CLEAR);

  StrFormat(squawk->sq_Path, sizeof(squawk->sq_Path), SAVE_PATH, (PtUnsigned32)id);

  squawk->sq_File = Open(squawk->sq_Path, MODE_NEWFILE);

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
      squawk->sq_Path,
      IoErr()
    );
  }
  
  Write(squawk->sq_File, SQUAWK_HEADER, Pt_literal_strlen(SQUAWK_HEADER));
  
  return squawk;
}
void EndAssetList(SquawkPtr squawk);

void CloseSquawkFile(SquawkPtr squawk)
{
  if (squawk->sq_ListPos > 0)
  {
    EndAssetList(squawk);
  }

  /* Footer */
  Write(squawk->sq_File, SQUAWK_FOOTER, Pt_literal_strlen(SQUAWK_FOOTER));

  Close(squawk->sq_File);

  FreeMem(squawk, sizeof(struct Archive));
}

void StartAssetList(SquawkPtr squawk, PtUnsigned32 classType, PtUnsigned16 chapter)
{
  PtUnsigned32 zero;

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
  squawk->sq_ListHeader.al_Count = 0x55;
  squawk->sq_ListHeader.al_Length = 0xBEEFCAFE;

  squawk->sq_ListPos = Seek(squawk->sq_File, 0, OFFSET_CURRENT);

  Write(squawk->sq_File, &squawk->sq_ListHeader, sizeof(struct SQUAWK_ASSET_LIST_HEADER));
}

void EndAssetList(SquawkPtr squawk)
{
  PtUnsigned32 now, len;

  if (squawk->sq_ListPos != 0)
  {
    now = Seek(squawk->sq_File, 0, OFFSET_CURRENT);
    len = now - squawk->sq_ListPos - sizeof(struct SQUAWK_ASSET_LIST_HEADER);

    squawk->sq_ListHeader.al_Length = len;

    Seek(squawk->sq_File, squawk->sq_ListPos, OFFSET_BEGINNING);
    Write(squawk->sq_File, &squawk->sq_ListHeader, sizeof(struct SQUAWK_ASSET_LIST_HEADER));
    Seek(squawk->sq_File, now, OFFSET_BEGINNING);

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
      PARROT_ERR_STR("File")
      PARROT_ERR_STR("Current"),
      squawk->sq_Path,
      IDtoStr(squawk->sq_ListHeader.al_Type, strtype)
    );
  }
}

void SaveAsset(SquawkPtr squawk, PtAsset* asset, PtUnsigned32 assetSize)
{

  if (squawk->sq_ListPos == 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Asset list is not open (1)"
      PARROT_ERR_STR("File")
      PARROT_ERR_INT("Asset Id"),
      squawk->sq_Path,
      asset->as_Id
    );
    exit();
  }

  asset->as_Length = assetSize;

  Write(squawk->sq_File, asset, assetSize);

  squawk->sq_ListHeader.al_Count++;
}

void SaveAssetExtra(SquawkPtr squawk, PtAsset* asset, PtUnsigned32 assetSize, APTR data, PtUnsigned32 dataLength)
{
  if (squawk->sq_ListPos == 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Asset list is not open (2)"
      PARROT_ERR_STR("File")
      PARROT_ERR_INT("Asset Id")
      PARROT_ERR_INT("Asset Length"),
      squawk->sq_Path,
      (assetSize + dataLength)
    );
    exit();
  }

  asset->as_Length = assetSize + dataLength;
  asset->as_Flags |= PT_AF_HAS_DATA;

  Write(squawk->sq_File, asset, assetSize);
  Write(squawk->sq_File, data, dataLength);

  squawk->sq_ListHeader.al_Count++;
}
