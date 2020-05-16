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
STATIC UBYTE* SrcFileData;
STATIC UBYTE* SrcFilePos;

ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);
ULONG StrCopy(CHAR* pDst, ULONG pDstCapacity, CONST CHAR* pSrc);

STATIC VOID MemClear(APTR pMem, ULONG size);

STATIC VOID OpenParrotIff(UBYTE id);
STATIC VOID CloseParrotIff();
STATIC VOID ExportGame();
STATIC VOID ExportPalette();
STATIC VOID ExportRoom();
STATIC VOID ExportBackdrop();
STATIC VOID ReadImageData(UBYTE* tgt, UWORD w, UWORD h);
STATIC UWORD ReadUWORDBE();
STATIC UWORD ReadUWORDLE();
STATIC UBYTE ReadUBYTE();
STATIC VOID Chunky2Planar(UBYTE* chunky, UBYTE* planar, UWORD w, UWORD h);

STATIC LONG DebugF(CONST_STRPTR pFmt, ...);

STATIC VOID OpenFile(CONST CHAR* path);
STATIC VOID CloseFile();
STATIC VOID SeekFile(ULONG pos);

INT main()
{
  INT rc;

  struct Process* process;
  struct Message* wbMsg;
  struct CHUNK_GAME_INFO version;

  SrcFile = 0;
  DstIff = NULL;
  rc = RETURN_OK;

  SysBase = *(struct ExecBase**) 4L;

  process = (struct Process*) FindTask(NULL);

  if (process->pr_CLI != NULL)
  {
    DOSBase = (struct DosLibrary*) OpenLibrary("dos.library", 0);
    Write(Output(), "Maniac Mansion Converter can only be launched from Workbench\n", 44);
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

  OpenParrotIff(0);
  ExportGame();
  ExportPalette(1);
  CloseParrotIff();

  OpenFile("PROGDIR:01.LFL");

  OpenParrotIff(1);
  ExportRoom();
  ExportBackdrop();
  CloseParrotIff();

  CloseFile();

CLEAN_EXIT:

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

STATIC VOID Chunky2Planar(UBYTE* chunky, UBYTE* planar, UWORD w, UWORD h)
{
}


STATIC UWORD ReadUWORDBE()
{
  UWORD r = 0;

  /* Read(SrcFile, &r, 2); */
  r = SrcFilePos[0] << 8 | SrcFilePos[1];
  SrcFilePos += 2;

  r ^= 0xFFFF;  /* Copy Protection */

  return r;
}

STATIC UWORD ReadUWORDLE()
{
  UWORD r = 0;

  //Read(SrcFile, &r, 2)

  r = SrcFilePos[1] << 8 | SrcFilePos[0];
  SrcFilePos += 2;


  r ^= 0xFFFF;  /* Copy Protection */
  // r = ( (r >> 8) | (r << 8) ) & 0xFFFF; /* Little Endian to Big Endian */

  return r;
}

STATIC UBYTE ReadUBYTE()
{
  UBYTE r = 0;

  // Read(SrcFile, &r, 1);

  r = SrcFilePos[0];
  SrcFilePos += 1;

  r ^= 0xFF;  /* Copy Protection */
  
  return r;
}

STATIC VOID OpenParrotIff(UBYTE id)
{
  CHAR filename[26];

  RoomNo = id;
  RoomAssetCounter = 1;
  RoomWidth = 0;
  RoomHeight = 0;
  RoomBackdropOffset = 0;
  RoomMaskOffset = 0;
  RoomNumObjs = 0;

  StrFormat(filename, sizeof(filename), "PROGDIR:%ld.Parrot", (ULONG)id);
  
  DstIff = AllocIFF();
  DstIff->iff_Stream = Open(filename, MODE_NEWFILE);

  InitIFFasDOS(DstIff);
  OpenIFF(DstIff, IFFF_WRITE);

  PushChunk(DstIff, ID_SQWK, ID_FORM, IFFSIZE_UNKNOWN);

}

STATIC VOID CloseParrotIff()
{
  if (NULL != DstIff)
  {
    PopChunk(DstIff); /* ID_FORM */

    CloseIFF(DstIff);
    Close(DstIff->iff_Stream);
    FreeIFF(DstIff);
    DstIff = NULL;
  }
}

STATIC VOID ExportPalette(UWORD id)
{
  struct CHUNK_COLOUR_PALETTE palette;

  palette.Header.Schema = CHUNK_COLOUR_PALETTE_SCHEMA;
  palette.Header.MinVersion = CHUNK_COLOUR_PALETTE_MIN_VERSION;
  palette.Header.Id = MAKE_ASSET_ID(RoomNo, ASSET_TYPE_COLOUR_PALETTE, id);
  
  palette.NumColours = 16;
  
  /* Black */
  palette.Palette[0] = 0x00;
  palette.Palette[1] = 0x00;
  palette.Palette[2] = 0x00;

  /* Blue */
  palette.Palette[3] = 0x00;
  palette.Palette[4] = 0x00;
  palette.Palette[5] = 0xAA;

  /* Green */
  palette.Palette[6] = 0x00;
  palette.Palette[7] = 0xAA;
  palette.Palette[8] = 0x00;

  /* Cyan */
  palette.Palette[9] = 0x00;
  palette.Palette[10] = 0xAA;
  palette.Palette[11] = 0xAA;

  /* Red */
  palette.Palette[12] = 0xAA;
  palette.Palette[13] = 0x00;
  palette.Palette[14] = 0x00;

  /* Magenta */
  palette.Palette[15] = 0xAA;
  palette.Palette[16] = 0x00;
  palette.Palette[17] = 0xAA;

  /* Brown */
  palette.Palette[18] = 0xAA;
  palette.Palette[19] = 0x55;
  palette.Palette[20] = 0x00;

  /* Light Gray */
  palette.Palette[21] = 0xAA;
  palette.Palette[22] = 0xAA;
  palette.Palette[23] = 0xAA;

  /* Dark Gray */
  palette.Palette[24] = 0x55;
  palette.Palette[25] = 0x55;
  palette.Palette[26] = 0x55;

  /* Bright Blue */
  palette.Palette[27] = 0x55;
  palette.Palette[28] = 0x55;
  palette.Palette[29] = 0xFF;

  /* Bright Green */
  palette.Palette[30] = 0x55;
  palette.Palette[31] = 0xFF;
  palette.Palette[32] = 0x55;
  
  /* Bright Cyan */
  palette.Palette[33] = 0x55;
  palette.Palette[34] = 0xFF;
  palette.Palette[35] = 0xFF;

  /* Bright Red */
  palette.Palette[36] = 0xFF;
  palette.Palette[37] = 0x55;
  palette.Palette[38] = 0x55;

  /* Bright Magenta */
  palette.Palette[39] = 0xFF;
  palette.Palette[40] = 0x55;
  palette.Palette[41] = 0xFF;

  /* Bright Yellow */
  palette.Palette[42] = 0xFF;
  palette.Palette[43] = 0xFF;
  palette.Palette[44] = 0x55;

  /* Bright White */
  palette.Palette[45] = 0xFF;
  palette.Palette[46] = 0xFF;
  palette.Palette[47] = 0xFF;

  PushChunk(DstIff, ID_SQWK, CHUNK_COLOUR_PALETTE_ID, sizeof(struct CHUNK_HEADER) + sizeof(UBYTE) + 48);  /* backdrop.ImageAssetId, size); */
  WriteChunkBytes(DstIff, &palette, sizeof(struct CHUNK_HEADER) + sizeof(UBYTE) + 48);
  PopChunk(DstIff);
}

STATIC VOID ExportGame()
{
  struct CHUNK_GAME_INFO info;
  MemClear(&info, sizeof(struct CHUNK_GAME_INFO));

  info.Header.Schema = CHUNK_GAME_INFO_SCHEMA_VERSION;
  info.Header.MinVersion = CHUNK_GAME_INFO_MIN_VERSION;
  info.Header.Id = MAKE_ASSET_ID(0, ASSET_TYPE_GAME, 1);

  StrCopy(&info.Title[0], sizeof(info.Title), "Maniac Mansion");
  StrCopy(&info.ShortTitle[0], sizeof(info.ShortTitle), "Maniac");
  StrCopy(&info.Author[0], sizeof(info.Author), "Lucasfilm Games LLC");
  StrCopy(&info.Release[0], sizeof(info.Release), "Commodore Amiga");

  info.Width = 320;
  info.Height = 200;
  info.Depth = 4;

  PushChunk(DstIff, ID_SQWK, CHUNK_GAME_INFO_ID, sizeof(struct CHUNK_GAME_INFO));
  WriteChunkBytes(DstIff, &info, sizeof(struct CHUNK_GAME_INFO));
  PopChunk(DstIff);

}

STATIC VOID ExportBackdrop()
{
  struct CHUNK_BACKDROP backdrop;
  ULONG  chunkySize, planarSize, p;
  UBYTE* chunky, * planar;
  UWORD  x, y;
  UBYTE  r, col, len, ii;

  MemClear(&backdrop, sizeof(struct CHUNK_BACKDROP));

  SeekFile(4);
  RoomWidth = ReadUWORDLE();
  RoomHeight = ReadUWORDLE();

  SeekFile(10);
  RoomBackdropOffset = ReadUWORDLE();

  x = 0;
  y = 0;
  chunkySize = RoomWidth * RoomHeight;
  planarSize = chunkySize / 2;

  chunky = AllocVec(chunkySize, MEMF_CLEAR);
  planar = AllocVec(planarSize, MEMF_CLEAR);

  backdrop.Header.Schema = CHUNK_BACKDROP_SCHEMA;
  backdrop.Header.MinVersion = CHUNK_BACKDROP_MIN_VERSION;
  backdrop.Header.Id = MAKE_ASSET_ID(RoomNo, ASSET_TYPE_BACKDROP, 1);

  backdrop.Width = RoomWidth;
  backdrop.Height = RoomHeight;
  backdrop.PaletteId = MAKE_ASSET_ID(0, ASSET_TYPE_COLOUR_PALETTE, 1);

  PushChunk(DstIff, ID_SQWK, CHUNK_BACKDROP_ID, IFFSIZE_UNKNOWN);
  WriteChunkBytes(DstIff, &backdrop, sizeof(struct CHUNK_BACKDROP));
  SeekFile(RoomBackdropOffset);
  ReadImageData(chunky, RoomWidth, RoomHeight);
  WriteChunkBytes(DstIff, chunky, chunkySize);
  PopChunk(DstIff);


  FreeVec(chunky);
  FreeVec(planar);
}


STATIC VOID ExportRoom()
{
  struct CHUNK_ROOM room;

  MemClear(&room, sizeof(struct CHUNK_ROOM));

  SeekFile(4);
  RoomWidth = ReadUWORDLE();
  RoomHeight = ReadUWORDLE();
  
  room.Header.Schema = CHUNK_ROOM_SCHEMA;
  room.Header.MinVersion = CHUNK_ROOM_MIN_VERSION;
  room.Header.Id = CHUNK_ROOM_ID;

  room.Width = RoomWidth;
  room.Height = RoomHeight;
  room.Backdrops[0] = MAKE_ASSET_ID(RoomNo, ASSET_TYPE_BACKDROP, 1);

  PushChunk(DstIff, ID_SQWK, CHUNK_ROOM_ID, sizeof(struct CHUNK_ROOM));
  WriteChunkBytes(DstIff, &room, sizeof(struct CHUNK_ROOM));
  PopChunk(DstIff);

}


STATIC VOID ReadImageData(UBYTE* tgt, UWORD w, UWORD h)
{
  UWORD x, y;
  UBYTE r, len, col, ii;
  ULONG offset;

  x = 0;
  y = 0;

  offset = 0;

  while (x < w)
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

      for(ii=0;ii < len;ii++)
      {
        //offset = x + (y * w);
        tgt[offset] = col;
        y++;
        offset += w;
        if (y >= h)
        {
          y = 0;
          x++;
          offset = x;
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
        //offset = (x-1) + (y * w);
        col = tgt[offset - 1];
        //offset = (x) + (y * w);
        tgt[offset] = col;
        offset += w;
        y++;
        if (y >= h)
        {
          y = 0;
          x++;
          offset = x;
        }
      }
    }

  }
}


STATIC VOID MemClear(APTR pMem, ULONG size)
{
  ULONG ii;
  UBYTE* m;

  m = (UBYTE*)pMem;

  for (ii = 0; ii < size; ii++)
  {
    m[ii] = 0;
  }
}

STATIC VOID OpenFile(CONST CHAR* path)
{
  ULONG len;

  SrcFile = Open(path, MODE_OLDFILE);
  Seek(SrcFile, 0, OFFSET_END);
  len = Seek(SrcFile, 0, OFFSET_BEGINING);

  SrcFileData = AllocVec(len, 0);
  SrcFilePos = SrcFileData;

  Read(SrcFile, SrcFileData, len);
}

STATIC VOID CloseFile()
{
  if (NULL != SrcFile)
  {
    Close(SrcFile);
  }

  if (NULL != SrcFileData)
  {
    FreeVec(SrcFileData);
    SrcFileData = NULL;
  }
}


STATIC VOID SeekFile(ULONG pos)
{
  SrcFilePos = SrcFileData + pos;
}
