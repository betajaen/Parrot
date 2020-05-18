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

STATIC struct IFFHandle* DstIff;
STATIC UBYTE  ArchiveId;
STATIC UBYTE* SrcFileData;
STATIC UBYTE* SrcFilePos;
STATIC UBYTE* SrcFileEnd;

ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);
ULONG StrCopy(CHAR* pDst, ULONG pDstCapacity, CONST CHAR* pSrc);

STATIC VOID MemClear(APTR pMem, ULONG size);

STATIC VOID OpenParrotIff(UBYTE id);
STATIC VOID CloseParrotIff();
STATIC VOID ExportGame();
STATIC VOID ExportPalette(UWORD id);
STATIC VOID ExportCursorPalette(UWORD id);
STATIC VOID ExportRoom();
STATIC VOID ExportBackdrop();
STATIC VOID ReadImageData(UBYTE* tgt, UWORD w, UWORD h);
STATIC VOID ConvertImageDataToPlanar(UBYTE* src, UBYTE* dst, UWORD w, UWORD h);
STATIC UWORD ReadUWORDBE();
STATIC UWORD ReadUWORDLE();
STATIC UBYTE ReadUBYTE();

STATIC LONG DebugF(CONST_STRPTR pFmt, ...);

STATIC ULONG OpenFile(CONST CHAR* path);
STATIC VOID CloseFile();
STATIC BOOL SeekFile(ULONG pos);

INT main()
{
  INT rc;

  struct Process* process;
  struct Message* wbMsg;
  struct CHUNK_GAME_INFO version;

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
  ExportCursorPalette(1);
  CloseParrotIff();

  if (OpenFile("PROGDIR:01.LFL") > 0)
  {

    OpenParrotIff(1);
    ExportRoom();
    ExportBackdrop();
    CloseParrotIff();

    CloseFile();
  }

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


STATIC UWORD ReadUWORDBE()
{
  UWORD r = 0;

  r = SrcFilePos[0] << 8 | SrcFilePos[1];
  r ^= 0xFFFF;  /* Copy Protection */

  SrcFilePos += 2;

  return r;
}

STATIC UWORD ReadUWORDLE()
{
  UWORD r = 0;

  r = SrcFilePos[1] << 8 | SrcFilePos[0]; /* Little Endian to Big Endian */
  r ^= 0xFFFF;  /* Copy Protection */

  SrcFilePos += 2;

  return r;
}

STATIC UBYTE ReadUBYTE()
{
  UBYTE r = 0;

  r = SrcFilePos[0];
  r ^= 0xFF;  /* Copy Protection */

  SrcFilePos += 1;

  return r;
}

STATIC VOID OpenParrotIff(UBYTE id)
{
  CHAR filename[26];

  ArchiveId = id;

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
  struct CHUNK_PALETTE_32 palette;
  MemClear((APTR)&palette, sizeof(struct CHUNK_PALETTE_32));

  palette.Header.Schema = CHUNK_PALETTE_32_SCHEMA;
  palette.Header.MinVersion = CHUNK_PALETTE_32_MIN_VERSION;
  palette.Header.Id = MAKE_ASSET_ID(ArchiveId, ASSET_TYPE_PALETTE_32, id);
  
  palette.NumColours = 16;
  
  /* Black */
  palette.Palette4[0] = 0x0;
  palette.Palette4[1] = 0x0;
  palette.Palette4[2] = 0x0;

  /* Blue */
  palette.Palette4[3] = 0x0;
  palette.Palette4[4] = 0x0;
  palette.Palette4[5] = 0xA;

  /* Green */
  palette.Palette4[6] = 0x0;
  palette.Palette4[7] = 0xA;
  palette.Palette4[8] = 0x0;

  /* Cyan */
  palette.Palette4[9] = 0x0;
  palette.Palette4[10] = 0xA;
  palette.Palette4[11] = 0xA;

  /* Red */
  palette.Palette4[12] = 0xA;
  palette.Palette4[13] = 0x0;
  palette.Palette4[14] = 0x0;

  /* Magenta */
  palette.Palette4[15] = 0xA;
  palette.Palette4[16] = 0x0;
  palette.Palette4[17] = 0xA;

  /* Brown */
  palette.Palette4[18] = 0xA;
  palette.Palette4[19] = 0x5;
  palette.Palette4[20] = 0x0;

  /* Light Gray */
  palette.Palette4[21] = 0xA;
  palette.Palette4[22] = 0xA;
  palette.Palette4[23] = 0xA;

  /* Dark Gray */
  palette.Palette4[24] = 0x5;
  palette.Palette4[25] = 0x5;
  palette.Palette4[26] = 0x5;

  /* Bright Blue */
  palette.Palette4[27] = 0x5;
  palette.Palette4[28] = 0x5;
  palette.Palette4[29] = 0xF;

  /* Bright Green */
  palette.Palette4[30] = 0x5;
  palette.Palette4[31] = 0xF;
  palette.Palette4[32] = 0x5;
  
  /* Bright Cyan */
  palette.Palette4[33] = 0x5;
  palette.Palette4[34] = 0xF;
  palette.Palette4[35] = 0xF;

  /* Bright Red */
  palette.Palette4[36] = 0xF;
  palette.Palette4[37] = 0x5;
  palette.Palette4[38] = 0x5;

  /* Bright Magenta */
  palette.Palette4[39] = 0xF;
  palette.Palette4[40] = 0x5;
  palette.Palette4[41] = 0xF;

  /* Bright Yellow */
  palette.Palette4[42] = 0xF;
  palette.Palette4[43] = 0xF;
  palette.Palette4[44] = 0x5;

  /* Bright White */
  palette.Palette4[45] = 0xF;
  palette.Palette4[46] = 0xF;
  palette.Palette4[47] = 0xF;


  palette.Palette32.Count_Start = (16l << 16) | 0;
  
  /* Black */
  palette.Palette32.Palette[0] = 0x00000000;
  palette.Palette32.Palette[1] = 0x00000000;
  palette.Palette32.Palette[2] = 0x00000000;

  /* Blue */
  palette.Palette32.Palette[3] = 0x00000000;
  palette.Palette32.Palette[4] = 0x00000000;
  palette.Palette32.Palette[5] = 0xAAAAAAAA;

  /* Green */
  palette.Palette32.Palette[6] = 0x00000000;
  palette.Palette32.Palette[7] = 0xAAAAAAAA;
  palette.Palette32.Palette[8] = 0x00000000;

  /* Cyan */
  palette.Palette32.Palette[9] = 0x00000000;
  palette.Palette32.Palette[10] = 0xAAAAAAAA;
  palette.Palette32.Palette[11] = 0xAAAAAAAA;

  /* Red */
  palette.Palette32.Palette[12] = 0xAAAAAAAA;
  palette.Palette32.Palette[13] = 0x00000000;
  palette.Palette32.Palette[14] = 0x00000000;

  /* Magenta */
  palette.Palette32.Palette[15] = 0xAAAAAAAA;
  palette.Palette32.Palette[16] = 0x00000000;
  palette.Palette32.Palette[17] = 0xAAAAAAAA;

  /* Brown */
  palette.Palette32.Palette[18] = 0xAAAAAAAA;
  palette.Palette32.Palette[19] = 0x55555555;
  palette.Palette32.Palette[20] = 0x00000000;

  /* Light Gray */
  palette.Palette32.Palette[21] = 0xAAAAAAAA;
  palette.Palette32.Palette[22] = 0xAAAAAAAA;
  palette.Palette32.Palette[23] = 0xAAAAAAAA;

  /* Dark Gray */
  palette.Palette32.Palette[24] = 0x55555555;
  palette.Palette32.Palette[25] = 0x55555555;
  palette.Palette32.Palette[26] = 0x55555555;

  /* Bright Blue */
  palette.Palette32.Palette[27] = 0x55555555;
  palette.Palette32.Palette[28] = 0x55555555;
  palette.Palette32.Palette[29] = 0xFFFFFFFF;

  /* Bright Green */
  palette.Palette32.Palette[30] = 0x55555555;
  palette.Palette32.Palette[31] = 0xFFFFFFFF;
  palette.Palette32.Palette[32] = 0x55555555;

  /* Bright Cyan */
  palette.Palette32.Palette[33] = 0x55555555;
  palette.Palette32.Palette[34] = 0xFFFFFFFF;
  palette.Palette32.Palette[35] = 0xFFFFFFFF;

  /* Bright Red */
  palette.Palette32.Palette[36] = 0xFFFFFFFF;
  palette.Palette32.Palette[37] = 0x55555555;
  palette.Palette32.Palette[38] = 0x55555555;

  /* Bright Magenta */
  palette.Palette32.Palette[39] = 0xFFFFFFFF;
  palette.Palette32.Palette[40] = 0x55555555;
  palette.Palette32.Palette[41] = 0xFFFFFFFF;

  /* Bright Yellow */
  palette.Palette32.Palette[42] = 0xFFFFFFFF;
  palette.Palette32.Palette[43] = 0xFFFFFFFF;
  palette.Palette32.Palette[44] = 0x55555555;

  /* Bright White */
  palette.Palette32.Palette[45] = 0xFFFFFFFF;
  palette.Palette32.Palette[46] = 0xFFFFFFFF;
  palette.Palette32.Palette[47] = 0xFFFFFFFF;

  palette.Palette32.Terminator = 0;

  PushChunk(DstIff, ID_SQWK, CHUNK_PALETTE_32_ID, sizeof(struct CHUNK_PALETTE_32));
  WriteChunkBytes(DstIff, &palette, sizeof(struct CHUNK_PALETTE_32));
  PopChunk(DstIff);
}

STATIC VOID ExportCursorPalette(UWORD id)
{
  struct CHUNK_SPRITE_PALETTE_32 palette;
  MemClear((APTR)&palette, sizeof(struct CHUNK_SPRITE_PALETTE_32));

  palette.Header.Schema = CHUNK_SPRITE_PALETTE_32_SCHEMA;
  palette.Header.MinVersion = CHUNK_SPRITE_PALETTE_32_MIN_VERSION;
  palette.Header.Id = MAKE_ASSET_ID(ArchiveId, ASSET_TYPE_SPRITE_PALETTE, id);

  palette.Palette.Count_Start = 4 << 16 | 17;
  palette.Palette.Palette[0] = 0x00000000;
  palette.Palette.Palette[1] = 0x00000000;
  palette.Palette.Palette[2] = 0x00000000;

  palette.Palette.Palette[3] = 0xFFFFFFFF;
  palette.Palette.Palette[4] = 0xFFFFFFFF;
  palette.Palette.Palette[5] = 0xFFFFFFFF;

  palette.Palette.Palette[6] = 0xAAAAAAAA;
  palette.Palette.Palette[7] = 0xAAAAAAAA;
  palette.Palette.Palette[8] = 0xAAAAAAAA;

  palette.Palette.Terminator = 0;

  PushChunk(DstIff, ID_SQWK, CHUNK_SPRITE_PALETTE_32_ID, sizeof(struct CHUNK_SPRITE_PALETTE_32));
  WriteChunkBytes(DstIff, &palette, sizeof(struct CHUNK_SPRITE_PALETTE_32));
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

  ULONG  chunkySize, planarSize, p, imgOffset;
  UBYTE* chunky, * planar;
  UWORD  x, y, w, h;
  UBYTE  r, col, len, ii;

  MemClear(&backdrop, sizeof(struct CHUNK_BACKDROP));

  SeekFile(4);
  w = ReadUWORDLE();
  h = ReadUWORDLE();

  SeekFile(10);
  imgOffset = ReadUWORDLE();

  x = 0;
  y = 0;
  chunkySize = w * h;
  planarSize = chunkySize / 2;

  chunky = AllocVec(chunkySize, MEMF_CLEAR);
  planar = AllocVec(planarSize, MEMF_CLEAR);

  backdrop.Header.Schema = CHUNK_BACKDROP_SCHEMA;
  backdrop.Header.MinVersion = CHUNK_BACKDROP_MIN_VERSION;
  backdrop.Header.Id = MAKE_ASSET_ID(ArchiveId, ASSET_TYPE_BACKDROP, 1);

  backdrop.Width = w;
  backdrop.Height = h;
  backdrop.PaletteId = MAKE_ASSET_ID(0, ASSET_TYPE_PALETTE_32, 1);

  PushChunk(DstIff, ID_SQWK, CHUNK_BACKDROP_ID, IFFSIZE_UNKNOWN);
  WriteChunkBytes(DstIff, &backdrop, sizeof(struct CHUNK_BACKDROP));
  SeekFile(imgOffset);
  ReadImageData(chunky, w, h);
  ConvertImageDataToPlanar(chunky, planar, w, h);
  WriteChunkBytes(DstIff, planar, planarSize);
  PopChunk(DstIff);


  FreeVec(chunky);
  FreeVec(planar);
}


STATIC VOID ExportRoom()
{
  struct CHUNK_ROOM room;

  MemClear(&room, sizeof(struct CHUNK_ROOM));
  
  room.Header.Schema = CHUNK_ROOM_SCHEMA;
  room.Header.MinVersion = CHUNK_ROOM_MIN_VERSION;
  room.Header.Id = CHUNK_ROOM_ID;

  SeekFile(4);
  room.Width = ReadUWORDLE();
  room.Height = ReadUWORDLE();
  room.Backdrops[0] = MAKE_ASSET_ID(ArchiveId, ASSET_TYPE_BACKDROP, 1);

  PushChunk(DstIff, ID_SQWK, CHUNK_ROOM_ID, sizeof(struct CHUNK_ROOM));
  WriteChunkBytes(DstIff, &room, sizeof(struct CHUNK_ROOM));
  PopChunk(DstIff);

}


STATIC VOID ReadImageData(UBYTE* tgt, UWORD w, UWORD h)
{
  UWORD x, y;
  UBYTE r, len, col;
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

      while(len--)
      {
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

      while (len--)
      {
        col = tgt[offset - 1];
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

STATIC VOID ConvertImageDataToPlanar(UBYTE* src, UBYTE* dst, UWORD w, UWORD h)
{
  ULONG idx;
  UBYTE bp, shift;
  UWORD y, x, i;

  idx = 0;

  for (y = 0; y < h; y++)
  {
    for (bp = 0; bp < 4; bp++)
    {
      UBYTE shift = 1 << bp;

      for (x = 0; x < w; x += 8)
      {
        UBYTE byte = 0;

        for (i = 0; i < 8; i++)
        {
          UBYTE col = src[idx + x + i];
          UBYTE bit = (col & shift) != 0 ? 1 : 0;

          if (bit)
            byte |= (1 << (7 - i));
        }

        *dst++ = byte;
      }
    }

    idx += w;
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

STATIC ULONG OpenFile(CONST CHAR* path)
{
  ULONG len;
  BPTR  file;

  file = Open(path, MODE_OLDFILE);

  if (NULL == file)
  {
    return 0ul;
  }

  Seek(file, 0, OFFSET_END);
  len = Seek(file, 0, OFFSET_BEGINING);

  SrcFileData = AllocVec(len, 0);
  SrcFilePos = SrcFileData;
  SrcFileEnd = SrcFileData + len;

  Read(file, SrcFileData, len);

  Close(file);

  return len;
}

STATIC VOID CloseFile()
{
  if (NULL != SrcFileData)
  {
    FreeVec(SrcFileData);
    SrcFileData = NULL;
  }
}


STATIC BOOL SeekFile(ULONG pos)
{
  UBYTE* nextPos;

  nextPos = SrcFileData + pos;

  if (nextPos < SrcFileEnd)
  {
    SrcFilePos = nextPos;

    return TRUE;
  }

  return FALSE;
}
