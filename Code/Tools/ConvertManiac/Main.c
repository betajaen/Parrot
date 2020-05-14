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

#include <exec/types.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/iffparse.h>
#include <libraries/iffparse.h>

#include <Asset.h>

struct ExecBase* SysBase;
struct DosLibrary* DOSBase;
struct Library* IFFParseBase;
struct IntuitionBase* IntuitionBase;




#define RC_OKAY RETURN_OK
#define RC_FAIL RETURN_FAIL

#ifndef INT
#define INT int
#endif

#ifndef CHAR
#define CHAR char
#endif

STATIC BPTR SrcFile;
STATIC struct IFFHandle* DstIff;
STATIC UBYTE RoomNo;
STATIC UWORD RoomAssetCounter;
STATIC UWORD RoomWidth, RoomHeight;
STATIC ULONG RoomBackdropOffset;
STATIC ULONG RoomMaskOffset;
STATIC ULONG RoomNumObjs;

STATIC VOID DecodeRoomData();
STATIC VOID DecodeRoomBackground();
STATIC UWORD ReadUWORDBE();
STATIC UWORD ReadUWORDLE();
STATIC UBYTE ReadUBYTE();

STATIC LONG DebugF(CONST_STRPTR pFmt, ...);

INT main()
{
  INT rc;

  struct Process* process;
  struct Message* wbMsg;
  struct CHUNK_VERSION version;

  SrcFile = 0;
  DstIff = NULL;
  rc = RETURN_OK;

  SysBase = *(struct ExecBase**) 4L;

  process = (struct Process*) FindTask(NULL);

  if (process->pr_CLI != NULL)
  {
    DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);
    Write(Output(), "Maniac Game can only be launched from Workbench\n", 44);
    goto CLEAN_EXIT;
  }

  WaitPort(&process->pr_MsgPort);
  wbMsg = GetMsg(&process->pr_MsgPort);

  DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);

  if (NULL == DOSBase)
  {
    goto CLEAN_EXIT;
  }

  IntuitionBase = (struct IntuitionBase*) OpenLibrary("intuition.library", 0);

  if (NULL == IntuitionBase)
  {
    goto CLEAN_EXIT;
  }

  IFFParseBase = (struct Library*) OpenLibrary("iffparse.library", 0);

  if (NULL == IFFParseBase)
  {
    goto CLEAN_EXIT;
  }

  SrcFile = Open("PROGDIR:01.LFL", MODE_OLDFILE);
  
  RoomNo = 1;
  RoomAssetCounter = 1;
  RoomWidth = 0;
  RoomHeight = 0;
  RoomBackdropOffset = 0;
  RoomMaskOffset = 0;
  RoomNumObjs = 0;

  if (0 == SrcFile)
  {
    goto CLEAN_EXIT;
  }

  DstIff = AllocIFF();
  DstIff->iff_Stream = Open("PROGDIR:01.Parrot", MODE_NEWFILE);
  
  InitIFFasDOS(DstIff);
  OpenIFF(DstIff, IFFF_WRITE);
  
  PushChunk(DstIff, ID_MNIC, ID_FORM, IFFSIZE_UNKNOWN);

  version.GameId = ID_MNIC;
  version.ParrotVersionMajor = 1;
  version.ParrotVersionMinor = 0;

  PushChunk(DstIff, ID_MNIC, CHUNK_VERSION_ID, sizeof(struct CHUNK_VERSION));
  WriteChunkBytes(DstIff, &version, sizeof(struct CHUNK_VERSION));
  PopChunk(DstIff);

  DecodeRoomBackground();

CLEAN_EXIT:

  if (0 != SrcFile)
  {
    Close(SrcFile);
    SrcFile = 0;
  }

  if (NULL != DstIff)
  {
    PopChunk(DstIff); /* ID_FORM */

    CloseIFF(DstIff);
    Close(DstIff->iff_Stream);
    FreeIFF(DstIff);
    DstIff = NULL;
  }

  if (NULL != IFFParseBase)
  {
    CloseLibrary((struct Library*) IFFParseBase);
    IFFParseBase = NULL;
  }

  if (NULL != wbMsg)
  {
    Forbid();
    ReplyMsg(wbMsg);
  }

  if (NULL != IntuitionBase)
  {
    CloseLibrary((struct Library*) IntuitionBase);
    IntuitionBase = NULL;
  }

  if (NULL != DOSBase)
  {
    CloseLibrary((struct Library*) DOSBase);
    DOSBase = NULL;
  }

  return RETURN_OK;
}


STATIC CONST ULONG PutChar = 0x16c04e75;
STATIC CONST ULONG CountChar = 0x52934E75;
char RequesterText[1024];

struct EasyStruct EasyRequesterStruct =
{
  sizeof(struct EasyStruct),
  0,
  "Parrot",
  &RequesterText[0],
  "Ok",
};

STATIC LONG DebugF(CONST_STRPTR pFmt, ...)
{
  LONG size;;
  STRPTR* arg;

  size = 0;
  arg = (STRPTR*)(&pFmt + 1);
  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) & CountChar, (STRPTR)&size);

  if (size >= sizeof(RequesterText) || (0 == size))
  {
    return 0;
  }

  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) & PutChar, (STRPTR)&RequesterText[0]);

  EasyRequest(NULL, &EasyRequesterStruct, NULL);

}

STATIC VOID DecodeRoomBackground()
{
  struct CHUNK_BACKDROP backdrop;
  ULONG  size, p;
  UBYTE* dst;
  UWORD  x, y;
  UBYTE  r, col, len, ii;

  Seek(SrcFile, 4, OFFSET_BEGINNING);
  RoomWidth = ReadUWORDLE();
  RoomHeight = ReadUWORDLE();

  Seek(SrcFile, 10, OFFSET_BEGINNING);
  RoomBackdropOffset = ReadUWORDLE();
  
  x = 0;
  y = 0;
  size = RoomWidth * RoomHeight;
  dst = AllocVec(size, MEMF_CLEAR);

  backdrop.Width = RoomWidth;
  backdrop.Height = RoomHeight;
  backdrop.Format = IMAGE_FORMAT_PALETTE;
  backdrop.Compression = IMAGE_COMPRESSION_NONE;
  backdrop.PaletteAssetId = MAKE_ASSET_ID(ASSET_TYPE_ROOM_PALETTE, 0, 1);
  backdrop.ImageAssetId = MAKE_ASSET_ID(ASSET_TYPE_ROOM_BACKDROP, 0, RoomAssetCounter);
  RoomAssetCounter++;
  
  PushChunk(DstIff, ID_MNIC, CHUNK_BACKDROP_ID, sizeof(struct CHUNK_BACKDROP));
  WriteChunkBytes(DstIff, &backdrop, sizeof(struct CHUNK_BACKDROP));
  PopChunk(DstIff);

  Seek(SrcFile, RoomBackdropOffset, OFFSET_BEGINNING);
  while (x < RoomWidth)
  {
    r = ReadUBYTE();

    if (r < 0x80)
    {
      len = (r >> 4) & 0xF;
      col = r & 0xF;

      if (0 == len)
      {
        len = ReadUBYTE();
      }


      for (ii = 0; ii < len; ii++)
      {
        p = x + (y * RoomWidth);
        dst[p] = col;
        y++;
        if (y >= RoomHeight)
        {
          y = 0;
          x++;
        }
      }

    }
    else
    {
      len = r & 0x7F;

      if (0 == len)
      {
        len = ReadUBYTE();
      }

      for (ii = 0; ii < len; ii++)
      {
        p = x - 1 + (y * RoomWidth);
        col = dst[p];
        p = x + (y * RoomWidth);
        dst[p] = col;
        y++;
        if (y >= RoomHeight)
        {
          y = 0;
          x++;
        }
      }
    }

  }
  

  DebugF("Converted = %ld", Seek(SrcFile, 0, OFFSET_CURRENT));


  PushChunk(DstIff, ID_MNIC, MAKE_ID('X', 'X', 'X', 'X'), size);  /* backdrop.ImageAssetId, size); */
  WriteChunkBytes(DstIff, dst, size);
  PopChunk(DstIff);


  FreeVec(dst);
}

STATIC UWORD ReadUWORDBE()
{
  UWORD r = 0;

  Read(SrcFile, &r, 2);

  r ^= 0xFFFF;  /* Copy Protection */

  return r;
}

STATIC UWORD ReadUWORDLE()
{
  UWORD r = 0;

  Read(SrcFile, &r, 2);

  r ^= 0xFFFF;  /* Copy Protection */
  r = ( (r >> 8) | (r << 8) ) & 0xFFFF; /* Little Endian to Big Endian */

  return r;
}

STATIC UBYTE ReadUBYTE()
{
  UBYTE r = 0;

  Read(SrcFile, &r, 1);

  r ^= 0xFF;  /* Copy Protection */
  
  return r;
}
