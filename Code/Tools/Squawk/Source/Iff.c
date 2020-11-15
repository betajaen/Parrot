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

struct IFFHandle* SquawkIff;

#define SAVE_PATH "PROGDIR:%ld.Parrot"

IffPtr OpenSquawkFile(UWORD id)
{
  IffPtr iff;
  CHAR path[1 + sizeof(SAVE_PATH) + 6];

  StrFormat(path, sizeof(path), SAVE_PATH, (ULONG)id);

  iff = AllocIFF();
  iff->iff_Stream = Open(path, MODE_NEWFILE);

  InitIFFasDOS(iff);
  OpenIFF(iff, IFFF_WRITE);

  PushChunk(iff, ID_SQWK, ID_FORM, IFFSIZE_UNKNOWN);

  return iff;
}

VOID CloseSquawkFile(IffPtr iff)
{
  if (NULL != iff)
  {
    PopChunk(iff); /* ID_FORM */

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

  if (NULL != iff)
  {
    PushChunk(iff, ID_SQWK, classType, IFFSIZE_UNKNOWN);

    PopChunk(iff);
  }
  else
  {
    PARROT_ERR(
      "Unable start serialising assets.\n"
      "Reason: Iff pointer is null"
      PARROT_ERR_STR("Type"),
      IDtoStr(classType, strtype)
    );
  }
}

VOID EndAssetList(IffPtr iff)
{
  char strtype[5];

  if (NULL != iff)
  {
    PopChunk(iff);
  }
  else
  {
    PARROT_ERR0(
      "Unable stop serialising assets.\n"
      "Reason: Iff pointer is null"
    );
  }
}

VOID SaveAssetQuick(IffPtr iff, APTR data, ULONG dataLength, ULONG classType, UWORD id, UWORD chunkHeaderflags)
{
  struct CHUNK_HEADER hdr;
  
  hdr.ch_Id = id;
  hdr.ch_Flags = chunkHeaderflags;

  PushChunk(iff, ID_SQWK, classType, sizeof(struct CHUNK_HEADER) + dataLength);
  WriteChunkBytes(iff, &hdr, sizeof(struct CHUNK_HEADER));
  WriteChunkBytes(iff, data, dataLength);
  PopChunk(iff);
}
