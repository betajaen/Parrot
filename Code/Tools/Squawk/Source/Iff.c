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

IffPtr OpenSquawkFile(UWORD id)
{
  IffPtr iff;
  LONG err;
  BPTR file;
  CHAR path[1 + sizeof(SAVE_PATH) + 6];

  StrFormat(path, sizeof(path), SAVE_PATH, (ULONG)id);

  iff = AllocIFF();

  if (iff == NULL)
  {
    PARROT_ERR(
      "Unable open squawk file for writing.\n"
      "Reason: (1) IFF pointer could not be allocated"
      PARROT_ERR_INT("Id")
      PARROT_ERR_STR("Path"),
      id,
      path
    );
  }

  file = Open(path, MODE_NEWFILE);

  if (file == 0)
  {
    PARROT_ERR(
      "Unable open squawk file for writing.\n"
      "Reason: (2) File was not opened for writing"
      PARROT_ERR_INT("Id")
      PARROT_ERR_PTR("Ptr")
      PARROT_ERR_STR("Path")
      PARROT_ERR_INT("IoErr"),
      id,
      file,
      path,
      IoErr()
    );
  }

  iff->iff_Stream = file;

  InitIFFasDOS(iff);

  err = OpenIFF(iff, IFFF_WRITE);

  if (err != 0)
  {
    PARROT_ERR(
      "Unable open squawk file for writing.\n"
      "Reason: (3) Could not Open IFF DOS File"
      PARROT_ERR_INT("Id")
      PARROT_ERR_INT("Err")
      PARROT_ERR_STR("Path"),
      id,
      err,
      path
    );
  }

  err = PushChunk(iff, MAKE_ID('I', 'L', 'B', 'M'), ID_LIST, IFFSIZE_UNKNOWN);
  
  if (err != 0)
  {
    PARROT_ERR(
      "Unable open squawk file for writing.\n"
      "Reason: (4) Opening chunk could not be written"
      PARROT_ERR_INT("Id")
      PARROT_ERR_INT("Err")
      PARROT_ERR_STR("Path"),
      id,
      err,
      path
    );
  }

  return iff;
}

VOID CloseSquawkFile(IffPtr iff)
{
  LONG err;

  if (NULL != iff)
  {
    err = PopChunk(iff); /* ID_LIST */

    if (err != 0)
    {
      PARROT_ERR(
        "Unable close squawk file for writing.\n"
        "Reason: (5) Closing chunk could not be finalised"
        PARROT_ERR_INT("Err"),
        err
      );
    }

    CloseIFF(iff);
    Close(iff->iff_Stream);
    FreeIFF(iff);
  }
  else
  {
    PARROT_ERR0(
      "Unable to close Squawk File.\n"
      "Reason: Null pointer given to CloseSquawkFunction"
    );
  }
}

VOID StartAssetList(IffPtr iff, ULONG classType)
{
  char strtype[5];
  LONG err;

  err = PushChunk(iff, classType, ID_FORM, IFFSIZE_UNKNOWN);

  if (err != 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Asset list could not be started"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Err"),
      IDtoStr(classType, strtype),
      err
    );
  }
}

VOID EndAssetList(IffPtr iff)
{
  LONG err;

  err = PopChunk(iff);

  if (err != 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Asset list could not be ended"
      PARROT_ERR_INT("Err"),
      err
    );
  }
}

VOID SaveAssetQuick(IffPtr iff, APTR data, ULONG dataLength, ULONG classType, UWORD id, UWORD chunkHeaderflags)
{
  struct ASSET_HEADER hdr;
  LONG err;
  char strtype[5];
  
  hdr.ah_Id = id;
  hdr.ah_AssetFlags = chunkHeaderflags;

  PushChunk(iff, ID_SQWK, UWordToId(id), sizeof(struct ASSET_HEADER) + dataLength);
  err = WriteChunkBytes(iff, &hdr, sizeof(struct ASSET_HEADER));

  if (err != sizeof(struct ASSET_HEADER))
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Header was not written"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id"),
      IDtoStr(classType, strtype),
      id
    );
  }

  err = WriteChunkBytes(iff, data, dataLength);

  if (err != dataLength)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Data was not written"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id"),
      IDtoStr(classType, strtype),
      id
    );
  }

  err = PopChunk(iff);

  if (err != 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Data chunk was not popped"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id")
      PARROT_ERR_INT("Err"),
      IDtoStr(classType, strtype),
      id,
      (LONG) err
    );
  }
}


VOID SaveAssetWithData(IffPtr iff, APTR data, ULONG dataLength, APTR data2, ULONG data2Length, ULONG classType, UWORD id, UWORD chunkHeaderflags)
{
  struct ASSET_HEADER hdr;
  LONG err;
  char strtype[5];

  hdr.ah_Id = id;
  hdr.ah_AssetFlags = chunkHeaderflags | CHUNK_FLAG_HAS_DATA;

  PushChunk(iff, ID_SQWK, UWordToId(id), sizeof(struct ASSET_HEADER) + dataLength + data2Length);
  err = WriteChunkBytes(iff, &hdr, sizeof(struct ASSET_HEADER));

  if (err != sizeof(struct ASSET_HEADER))
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Header was not written"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id"),
      IDtoStr(classType, strtype),
      id
    );
  }

  err = WriteChunkBytes(iff, data, dataLength);

  if (err != dataLength)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Data (1) was not written"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id"),
      IDtoStr(classType, strtype),
      id
    );
  }

  err = WriteChunkBytes(iff, data2, data2Length);

  if (err != data2Length)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Data (2) was not written"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id"),
      IDtoStr(classType, strtype),
      id
    );
  }

  err = PopChunk(iff);

  if (err != 0)
  {
    PARROT_ERR(
      "Unable serialise assets.\n"
      "Reason: Data and Data2 chunks was not popped"
      PARROT_ERR_STR("Type")
      PARROT_ERR_INT("Id")
      PARROT_ERR_INT("Err"),
      IDtoStr(classType, strtype),
      id,
      (LONG)err
    );
  }
}
