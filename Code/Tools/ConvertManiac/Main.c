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
STATIC UBYTE* SrcFileData;
STATIC UBYTE* SrcFilePos;
STATIC UBYTE* SrcFileEnd;
STATIC ULONG  NextRoomId;
STATIC ULONG  NextBackdropId;
STATIC UWORD  CurrentArchiveId;

ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);
ULONG StrCopy(CHAR* pDst, ULONG pDstCapacity, CONST CHAR* pSrc);

STATIC VOID MemClear(APTR pMem, ULONG size);

STATIC VOID OpenParrotIff(UWORD id);
STATIC VOID CloseParrotIff();
STATIC VOID ExportGame(UWORD id, struct OBJECT_TABLE_REF* tables, UWORD mainPalette, UWORD cursorPalette);
STATIC VOID ExportPalette(UWORD id);
STATIC VOID ExportCursorPalette(UWORD id);
STATIC VOID ExportRoom(UWORD id, UWORD backdrop);
STATIC VOID ExportBackdrop(UWORD id, UWORD palette);
STATIC VOID ReadImageData(UBYTE* tgt, UWORD w, UWORD h);
STATIC VOID ConvertImageDataToPlanar(UBYTE* src, UBYTE* dst, UWORD w, UWORD h);
STATIC UWORD ReadUWORDBE();
STATIC UWORD ReadUWORDLE();
STATIC UBYTE ReadUBYTE();
STATIC VOID AddToTable(struct OBJECT_TABLE* table, UWORD id, UWORD archive, UWORD flags, ULONG size);
STATIC VOID InitTable(struct OBJECT_TABLE* table, ULONG type);
STATIC VOID ExportTable(struct OBJECT_TABLE* table, UWORD id, UWORD tableRefSlot);

STATIC LONG DebugF(CONST_STRPTR pFmt, ...);

STATIC ULONG OpenFile(CONST CHAR* path);
STATIC VOID CloseFile();
STATIC BOOL SeekFile(ULONG pos);

STATIC struct OBJECT_TABLE_REF TableRefs[16];
STATIC struct OBJECT_TABLE RoomTable;
STATIC struct OBJECT_TABLE ImageTable;
STATIC struct OBJECT_TABLE PaletteTable;

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
  
  NextBackdropId = 1;

  MemClear((APTR)&RoomTable, sizeof(RoomTable));
  MemClear((APTR)&ImageTable, sizeof(ImageTable));
  MemClear((APTR)&PaletteTable, sizeof(PaletteTable));

  InitTable(&RoomTable, CT_ROOM);
  InitTable(&ImageTable, CT_IMAGE);
  InitTable(&PaletteTable, CT_PALETTE);

  if (OpenFile("PROGDIR:01.LFL") > 0)
  {

    OpenParrotIff(1);
    ExportRoom(1, NextBackdropId);
    ExportBackdrop(NextBackdropId++, 1);
    NextBackdropId++;
    CloseParrotIff();

    CloseFile();
  }

  OpenParrotIff(0);

  ExportPalette(1);
  ExportCursorPalette(2);
  ExportTable(&PaletteTable, 1, 0);
  ExportTable(&RoomTable, 2, 1);
  ExportTable(&ImageTable, 3, 2);

  ExportGame(1, &TableRefs[0], 1, 2);

  CloseParrotIff();

  DebugF("Converted.");

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

STATIC VOID OpenParrotIff(UWORD id)
{
  CHAR filename[26];

  CurrentArchiveId = id;

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
  struct PALETTE_TABLE pal;
  ULONG* pData;

  MemClear((APTR)&pal, sizeof(pal));
  
  hdr.ch_Id = id;
  hdr.ch_Flags = CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG;

  pData = (ULONG*)&pal.pt_Data[0];

  *pData++ = (16l << 16) | 0;

  /* Black */
  *pData++ = 0x00000000;
  *pData++ = 0x00000000;
  *pData++ = 0x00000000;

  /* Blue */
  *pData++ = 0x00000000;
  *pData++ = 0x00000000;
  *pData++ = 0xAAAAAAAA;

  /* Green */
  *pData++ = 0x00000000;
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0x00000000;

  /* Cyan */
  *pData++ = 0x00000000;
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0xAAAAAAAA;

  /* Red */
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0x00000000;
  *pData++ = 0x00000000;

  /* Magenta */
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0x00000000;
  *pData++ = 0xAAAAAAAA;

  /* Brown */
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0x55555555;
  *pData++ = 0x00000000;

  /* Light Gray */
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0xAAAAAAAA;
  *pData++ = 0xAAAAAAAA;

  /* Dark Gray */
  *pData++ = 0x55555555;
  *pData++ = 0x55555555;
  *pData++ = 0x55555555;

  /* Bright Blue */
  *pData++ = 0x55555555;
  *pData++ = 0x55555555;
  *pData++ = 0xFFFFFFFF;

  /* Bright Green */
  *pData++ = 0x55555555;
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0x55555555;

  /* Bright Cyan */
  *pData++ = 0x55555555;
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;

  /* Bright Red */
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0x55555555;
  *pData++ = 0x55555555;

  /* Bright Magenta */
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0x55555555;
  *pData++ = 0xFFFFFFFF;

  /* Bright Yellow */
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0x55555555;

  /* Bright White */
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;

  /* Terminator */
  *pData = 0;

  pal.pt_Begin = 0;
  pal.pt_End = 15;

  PushChunk(DstIff, ID_SQWK, CT_PALETTE, sizeof(pal) + sizeof(hdr));
  WriteChunkBytes(DstIff, &hdr, sizeof(hdr));
  WriteChunkBytes(DstIff, &pal, sizeof(pal));
  PopChunk(DstIff);

  AddToTable(&PaletteTable, id, CurrentArchiveId, hdr.ch_Flags, sizeof(struct PALETTE_TABLE));
}

STATIC VOID ExportCursorPalette(UWORD id)
{
  struct CHUNK_HEADER   hdr;
  struct PALETTE_TABLE  pal;
  ULONG* pData;

  MemClear((APTR)&pal, sizeof(pal));

  hdr.ch_Id = id;
  hdr.ch_Flags = CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG;

  pData = (ULONG*) &pal.pt_Data[0];

  *pData++ = (1l << 16) | 18;

  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;

  *pData = 0;

  pal.pt_Begin = 17;
  pal.pt_End = 18;

  PushChunk(DstIff, ID_SQWK, CT_PALETTE, sizeof(hdr) + sizeof(pal));
  WriteChunkBytes(DstIff, &hdr, sizeof(hdr));
  WriteChunkBytes(DstIff, &pal, sizeof(pal));
  PopChunk(DstIff);

  AddToTable(&PaletteTable, id, CurrentArchiveId, hdr.ch_Flags, sizeof(struct PALETTE_TABLE));
}

STATIC VOID ExportGame(UWORD id, struct OBJECT_TABLE_REF* tables, UWORD startPalette, UWORD startCursorPalette)
{
  struct CHUNK_HEADER hdr;
  struct GAME_INFO info;
  UWORD  tableCount;

  tableCount = 0;
  MemClear(&info, sizeof(info));

  hdr.ch_Id = id;
  hdr.ch_Flags = CHUNK_FLAG_ARCH_ANY;

  StrCopy(&info.gi_Title[0], sizeof(info.gi_Title), "Maniac Mansion");
  StrCopy(&info.gi_ShortTitle[0], sizeof(info.gi_ShortTitle), "Maniac");
  StrCopy(&info.gi_Author[0], sizeof(info.gi_Author), "Lucasfilm Games LLC");
  StrCopy(&info.gi_Release[0], sizeof(info.gi_Release), "Commodore Amiga");

  info.gi_Width  = 320;
  info.gi_Height = 200;
  info.gi_Depth  = 4;
  info.gi_StartPalette = startPalette;
  info.gi_StartCursorPalette = startCursorPalette;

  while (tables->tr_ChunkHeaderId != 0 && tableCount < 16)
  {
    info.gi_StartTables[tableCount].tr_ArchiveId = tables->tr_ArchiveId;
    info.gi_StartTables[tableCount].tr_ChunkHeaderId = tables->tr_ChunkHeaderId;
    info.gi_StartTables[tableCount].tr_ClassType = tables->tr_ClassType;
    tableCount++;
    tables++;
  }

  PushChunk(DstIff, ID_SQWK, CT_GAME_INFO, sizeof(struct CHUNK_HEADER) + sizeof(struct GAME_INFO));
  WriteChunkBytes(DstIff, &hdr, sizeof(struct CHUNK_HEADER));
  WriteChunkBytes(DstIff, &info, sizeof(struct GAME_INFO));
  PopChunk(DstIff);

}

STATIC VOID ExportBackdrop(UWORD id, UWORD palette)
{
  struct CHUNK_HEADER hdr;
  struct IMAGE backdrop;

  ULONG  chunkySize, planarSize, p, imgOffset;
  UBYTE* chunky, * planar;
  UWORD  x, y, w, h;
  UBYTE  r, col, len, ii;

  MemClear(&backdrop, sizeof(struct IMAGE));

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

  hdr.ch_Id = id;
  hdr.ch_Flags = CHUNK_FLAG_ARCH_ANY | CHUNK_FLAG_HAS_DATA;

  backdrop.im_Width = w;
  backdrop.im_Height = h;
  backdrop.im_Palette = palette;

  PushChunk(DstIff, ID_SQWK, CT_IMAGE, IFFSIZE_UNKNOWN);
  WriteChunkBytes(DstIff, &hdr, sizeof(struct CHUNK_HEADER));
  WriteChunkBytes(DstIff, &backdrop, sizeof(struct IMAGE));
  SeekFile(imgOffset);
  ReadImageData(chunky, w, h);
  ConvertImageDataToPlanar(chunky, planar, w, h);
  WriteChunkBytes(DstIff, planar, planarSize);
  PopChunk(DstIff);


  FreeVec(chunky);
  FreeVec(planar);


  AddToTable(&ImageTable, id, CurrentArchiveId, hdr.ch_Flags, sizeof(struct IMAGE) + planarSize);
}


STATIC VOID ExportRoom(UWORD id, UWORD backdrop)
{
  struct CHUNK_HEADER hdr;
  struct ROOM room;

  MemClear(&room, sizeof(struct ROOM));
  
  hdr.ch_Id = id;
  hdr.ch_Flags = CHUNK_FLAG_ARCH_ANY;

  SeekFile(4);
  room.rm_Width = ReadUWORDLE();
  room.rm_Height = ReadUWORDLE();
  room.rm_Backdrops[0] = backdrop;

  PushChunk(DstIff, ID_SQWK, CT_ROOM, sizeof(struct CHUNK_HEADER) + sizeof(struct ROOM));
  WriteChunkBytes(DstIff, &hdr, sizeof(struct CHUNK_HEADER));
  WriteChunkBytes(DstIff, &room, sizeof(struct ROOM));
  PopChunk(DstIff);

  AddToTable(&RoomTable, id, CurrentArchiveId, hdr.ch_Flags, sizeof(struct ROOM));
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
STATIC VOID AddToTable(struct OBJECT_TABLE* table, UWORD id, UWORD archive, UWORD flags, ULONG size)
{
  struct OBJECT_TABLE_ITEM* item;

  item = (struct OBJECT_TABLE_ITEM*) table->ot_Next;

  item->ot_Id = id;
  item->ot_Archive = archive;
  item->ot_Flags = flags;
  item->ot_Size = size;

  if (id > table->ot_IdMax)
  {
    table->ot_IdMax = id;
  }

  if (id < table->ot_IdMin)
  {
    table->ot_IdMin = id;
  }

  table->ot_Next = (APTR)(item + 1);
}

STATIC VOID InitTable(struct OBJECT_TABLE* table, ULONG classType)
{
  MemClear(table, sizeof(struct OBJECT_TABLE));

  table->ot_ClassType = classType;
  table->ot_IdMin = 65535;
  table->ot_IdMax = 0;
  table->ot_Next = &table->ot_Items[0];
}

STATIC VOID ExportTable(struct OBJECT_TABLE* table, UWORD id, UWORD tableRefSlot)
{
  struct CHUNK_HEADER hdr;
  struct OBJECT_TABLE_REF* ref;
  APTR t;
  ref = NULL;

  hdr.ch_Id = id;
  hdr.ch_Flags = CHUNK_FLAG_ARCH_ANY;

  if (tableRefSlot < 16)
  {
    ref = &TableRefs[tableRefSlot];
  }

  if (table->ot_IdMin > table->ot_IdMax)
  {
    table->ot_IdMin = 0;
    table->ot_IdMax = 0;
  }

  t = table->ot_Next;
  table->ot_Next = NULL;

  PushChunk(DstIff, ID_SQWK, CT_TABLE, sizeof(struct CHUNK_HEADER) + sizeof(struct OBJECT_TABLE));
  WriteChunkBytes(DstIff, &hdr, sizeof(struct CHUNK_HEADER));
  WriteChunkBytes(DstIff, table, sizeof(struct OBJECT_TABLE));
  PopChunk(DstIff);

  table->ot_Next = t;

  if (ref != NULL)
  {
    ref->tr_ArchiveId = CurrentArchiveId;
    ref->tr_ChunkHeaderId = hdr.ch_Id;
    ref->tr_ClassType = table->ot_ClassType;
  }

}
