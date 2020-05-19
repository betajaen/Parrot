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
STATIC ULONG  NextRoomId;
STATIC ULONG  NextBackdropId;

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
  ExportGame(1);
  ExportPalette(1);
  ExportCursorPalette(1);
  CloseParrotIff();

  if (OpenFile("PROGDIR:01.LFL") > 0)
  {

    OpenParrotIff(1);
    ExportRoom(1, NextBackdropId);
    ExportBackdrop(NextBackdropId++, 1);
    NextBackdropId++;
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
  struct CHUNK_HEADER hdr;
  struct PALETTE32_TABLE aga;

  MemClear((APTR)&aga, sizeof(aga));
  
  hdr.Id = id;
  hdr.Flags = CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG;

  aga.Count_Start = (16l << 16) | 0;

  /* Black */
  aga.Index[0] = 0x00000000;
  aga.Index[1] = 0x00000000;
  aga.Index[2] = 0x00000000;

  /* Blue */
  aga.Index[3] = 0x00000000;
  aga.Index[4] = 0x00000000;
  aga.Index[5] = 0xAAAAAAAA;

  /* Green */
  aga.Index[6] = 0x00000000;
  aga.Index[7] = 0xAAAAAAAA;
  aga.Index[8] = 0x00000000;

  /* Cyan */
  aga.Index[9] = 0x00000000;
  aga.Index[10] = 0xAAAAAAAA;
  aga.Index[11] = 0xAAAAAAAA;

  /* Red */
  aga.Index[12] = 0xAAAAAAAA;
  aga.Index[13] = 0x00000000;
  aga.Index[14] = 0x00000000;

  /* Magenta */
  aga.Index[15] = 0xAAAAAAAA;
  aga.Index[16] = 0x00000000;
  aga.Index[17] = 0xAAAAAAAA;

  /* Brown */
  aga.Index[18] = 0xAAAAAAAA;
  aga.Index[19] = 0x55555555;
  aga.Index[20] = 0x00000000;

  /* Light Gray */
  aga.Index[21] = 0xAAAAAAAA;
  aga.Index[22] = 0xAAAAAAAA;
  aga.Index[23] = 0xAAAAAAAA;

  /* Dark Gray */
  aga.Index[24] = 0x55555555;
  aga.Index[25] = 0x55555555;
  aga.Index[26] = 0x55555555;

  /* Bright Blue */
  aga.Index[27] = 0x55555555;
  aga.Index[28] = 0x55555555;
  aga.Index[29] = 0xFFFFFFFF;

  /* Bright Green */
  aga.Index[30] = 0x55555555;
  aga.Index[31] = 0xFFFFFFFF;
  aga.Index[32] = 0x55555555;

  /* Bright Cyan */
  aga.Index[33] = 0x55555555;
  aga.Index[34] = 0xFFFFFFFF;
  aga.Index[35] = 0xFFFFFFFF;

  /* Bright Red */
  aga.Index[36] = 0xFFFFFFFF;
  aga.Index[37] = 0x55555555;
  aga.Index[38] = 0x55555555;

  /* Bright Magenta */
  aga.Index[39] = 0xFFFFFFFF;
  aga.Index[40] = 0x55555555;
  aga.Index[41] = 0xFFFFFFFF;

  /* Bright Yellow */
  aga.Index[42] = 0xFFFFFFFF;
  aga.Index[43] = 0xFFFFFFFF;
  aga.Index[44] = 0x55555555;

  /* Bright White */
  aga.Index[45] = 0xFFFFFFFF;
  aga.Index[46] = 0xFFFFFFFF;
  aga.Index[47] = 0xFFFFFFFF;

  aga.Terminator = 0;

  PushChunk(DstIff, ID_SQWK, CT_PALETTE32, sizeof(aga) + sizeof(hdr));
  WriteChunkBytes(DstIff, &hdr, sizeof(hdr));
  WriteChunkBytes(DstIff, &aga, sizeof(aga));
  PopChunk(DstIff);
}

STATIC VOID ExportCursorPalette(UWORD id)
{
  struct CHUNK_HEADER   hdr;
  struct PALETTE4_TABLE aga;

  MemClear((APTR)&aga, sizeof(aga));

  hdr.Id = id;
  hdr.Flags = CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG;

  aga.Count_Start = (4l << 16) | 17;

  aga.Index[0] = 0x00000000;
  aga.Index[1] = 0x00000000;
  aga.Index[2] = 0x00000000;

  aga.Index[3] = 0xFFFFFFFF;
  aga.Index[4] = 0xFFFFFFFF;
  aga.Index[5] = 0xFFFFFFFF;

  aga.Index[6] = 0xAAAAAAAA;
  aga.Index[7] = 0xAAAAAAAA;
  aga.Index[8] = 0xAAAAAAAA;

  aga.Terminator = 0;

  PushChunk(DstIff, ID_SQWK, CT_PALETTE4, sizeof(hdr) + sizeof(aga));
  WriteChunkBytes(DstIff, &hdr, sizeof(hdr));
  WriteChunkBytes(DstIff, &aga, sizeof(aga));
  PopChunk(DstIff);
}

STATIC VOID ExportGame(UWORD id)
{
  struct CHUNK_HEADER hdr;
  struct GAME_INFO info;
  MemClear(&info, sizeof(info));

  hdr.Id = id;
  hdr.Flags = CHUNK_FLAG_ARCH_ANY;

  StrCopy(&info.Title[0], sizeof(info.Title), "Maniac Mansion");
  StrCopy(&info.ShortTitle[0], sizeof(info.ShortTitle), "Maniac");
  StrCopy(&info.Author[0], sizeof(info.Author), "Lucasfilm Games LLC");
  StrCopy(&info.Release[0], sizeof(info.Release), "Commodore Amiga");

  info.Width  = 320;
  info.Height = 200;
  info.Depth  = 4;

  PushChunk(DstIff, ID_SQWK, CT_GAME_INFO, sizeof(struct CHUNK_HEADER) + sizeof(struct GAME_INFO));
  WriteChunkBytes(DstIff, &hdr, sizeof(struct CHUNK_HEADER));
  WriteChunkBytes(DstIff, &info, sizeof(struct GAME_INFO));
  PopChunk(DstIff);

}

STATIC VOID ExportBackdrop(UWORD id, UWORD palette)
{
  struct IMAGE_CHUNK backdrop;

  ULONG  chunkySize, planarSize, p, imgOffset;
  UBYTE* chunky, * planar;
  UWORD  x, y, w, h;
  UBYTE  r, col, len, ii;

  MemClear(&backdrop, sizeof(struct IMAGE_CHUNK));

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

  backdrop.Header.Id = id;
  backdrop.Header.Flags = CHUNK_FLAG_ARCH_ANY;

  backdrop.Width = w;
  backdrop.Height = h;
  backdrop.Palette = palette;

  PushChunk(DstIff, ID_SQWK, CT_IMAGE, IFFSIZE_UNKNOWN);
  WriteChunkBytes(DstIff, &backdrop, sizeof(struct IMAGE_CHUNK));
  SeekFile(imgOffset);
  ReadImageData(chunky, w, h);
  ConvertImageDataToPlanar(chunky, planar, w, h);
  WriteChunkBytes(DstIff, planar, planarSize);
  PopChunk(DstIff);


  FreeVec(chunky);
  FreeVec(planar);
}


STATIC VOID ExportRoom(UWORD id, UWORD backdrop)
{
  struct CHUNK_ROOM room;

  MemClear(&room, sizeof(struct CHUNK_ROOM));
  
  room.Header.Id = id;
  room.Header.Flags = CHUNK_FLAG_ARCH_ANY;

  SeekFile(4);
  room.Width = ReadUWORDLE();
  room.Height = ReadUWORDLE();
  room.Backdrops[0] = backdrop;

  PushChunk(DstIff, ID_SQWK, CT_ROOM, sizeof(struct CHUNK_ROOM));
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
