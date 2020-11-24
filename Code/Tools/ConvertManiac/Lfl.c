/**
    $Id: Lfl.c, 1.0, 2020/05/13 15:42:00, betajaen Exp $

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
#include <Squawk/Squawk.h>
#include "Maniac.h"

#define MAX_PATH   256
#define MM_CHAPTER 1

#define MM_LFL_OBJOFF_SIZE    0x0
#define MM_LFL_OBJOFF_NUM     0x2
#define MM_LFL_OBJOFF_X       0x7
#define MM_LFL_OBJOFF_Y       0x8
#define MM_LFL_OBJOFF_WIDTH   0x9
#define MM_LFL_OBJOFF_HEIGHT  0xD
#define MM_LFL_OBJOFF_NAME    0xE

STATIC BYTE*  LflData;
STATIC ULONG  LflLength;
STATIC ULONG  LflCapacity;
STATIC IffPtr Archive;
STATIC UWORD  ArchiveId;

STATIC UWORD  RoomId;
STATIC UWORD  RoomWidth;
STATIC UWORD  RoomHeight;
STATIC UWORD  BackdropId;
STATIC UWORD  NumObjects;
STATIC ULONG  ObjectOffset;
STATIC ULONG  GraphicsOffset;

STATIC UBYTE* ChunkyData;
STATIC ULONG  ChunkyCapacity;

STATIC UWORD* PlanarData;
STATIC ULONG  PlanarCapacity;

STATIC VOID OpenLflFile(STRPTR lflPath, UWORD lflNum);
STATIC VOID CloseLflFile();
STATIC VOID ExportRoom();
STATIC VOID ExportEntities();
STATIC VOID ExportImageBackdrop(UWORD id);
STATIC VOID ReadRoomData();
STATIC VOID DecodeBackdrop(UBYTE* src, UBYTE* dst, UWORD w, UWORD h);
STATIC VOID ConvertImageDataToPlanar(UBYTE* src, UWORD* dst, UWORD w, UWORD h);

STATIC UBYTE ReadByte(ULONG offset)
{
  UBYTE r = 0;

  r = LflData[offset];
  r ^= 0xFF;  /* Copy Protection */

  return r;
}

STATIC UWORD ReadUWordLE(ULONG offset)
{
  UWORD r = 0;

  r = LflData[offset + 1] << 8 | LflData[offset]; /* Little Endian to Big Endian */
  r ^= 0xFFFF;  /* Copy Protection */

  return r;
}

STATIC ULONG ReadLflNameStringToDialogue(ULONG offset)
{
  UWORD len;
  UBYTE ch;
  CHAR text[255];

  for (len = 0; len < 255; len++)
  {
    ch = ReadByte(offset++);

    /* Break at null or @ */
    if (ch == 0 || ch == '@')
      break;

    /* Only use strings with printable characters. */
    if (ch < 32 || ch > 127)
      return 0;

    text[len] = ch;
  }

  text[len] = 0;

  if (len == 0)
    return 0;

  return PushDialogue(LANG_ENGLISH, len, &text[0]);
}

VOID StartLfl()
{
  LflData = NULL;
  LflCapacity = 0;
  ChunkyData = NULL;
  ChunkyCapacity = 0;
  PlanarData = NULL;
  PlanarCapacity = 0;
}

VOID EndLfl()
{
  if (NULL != PlanarData)
  {
    FreeMem(PlanarData, PlanarCapacity);
    PlanarCapacity = 0;
    PlanarData = NULL;
  }
  if (NULL != ChunkyData)
  {
    FreeMem(ChunkyData, ChunkyCapacity);
    ChunkyCapacity = 0;
    ChunkyData = NULL;
  }
  if (NULL != LflData)
  {
    FreeMem(LflData, LflCapacity);
    LflCapacity = 0;
    LflData = NULL;
  }
}

VOID ExportLfl(UWORD lflNum)
{
  OpenLflFile("PROGDIR:", lflNum);

  ArchiveId = GenerateArchiveId();
  Archive = OpenSquawkFile(ArchiveId);

  ReadRoomData();

  ExportRoom();
  ExportEntities();

  StartAssetList(Archive, CT_IMAGE);
  ExportImageBackdrop(BackdropId);
  EndAssetList(Archive);

  CloseSquawkFile(Archive);
  Archive = NULL;
  CloseLflFile();
}

STATIC VOID ExportRoom()
{
  struct ROOM room;

  InitStackVar(room);

  RoomId = GenerateAssetId(CT_ROOM);
  BackdropId = GenerateAssetId(CT_IMAGE);

  room.rm_Width = RoomWidth;
  room.rm_Height = RoomHeight;

  StartAssetList(Archive, CT_ROOM);
  SaveAssetQuick(Archive, &room, sizeof(room), CT_ROOM, RoomId, CHUNK_FLAG_ARCH_ANY);
  AddToTable(CT_ROOM, RoomId, ArchiveId, MM_CHAPTER);
  EndAssetList(Archive);
}


STATIC VOID ReadLflObject(struct NEW_ANY_ENTITY* entity, ULONG dataOffset, ULONG graphicsOffset)
{
  ULONG nameOff, size;

  size = ReadUWordLE(dataOffset + MM_LFL_OBJOFF_SIZE);
  entity->en_Context = ReadUWordLE(dataOffset + MM_LFL_OBJOFF_NUM);

  entity->en_HitBox.rt_Left = ReadByte(dataOffset + MM_LFL_OBJOFF_X) << 3;
  entity->en_HitBox.rt_Top = (ReadByte(dataOffset + MM_LFL_OBJOFF_Y) & 0xF) << 3;
  entity->en_HitBox.rt_Right = ReadByte(dataOffset + MM_LFL_OBJOFF_WIDTH) << 3;
  entity->en_HitBox.rt_Bottom = ReadByte(dataOffset + MM_LFL_OBJOFF_HEIGHT) & 0xF8;

  entity->en_HitBox.rt_Right += entity->en_HitBox.rt_Left;
  entity->en_HitBox.rt_Bottom += entity->en_HitBox.rt_Top;

  nameOff = ReadByte(dataOffset + MM_LFL_OBJOFF_NAME);

  if (nameOff > 0xE && nameOff < size)
  {
    nameOff += dataOffset;
    entity->en_Name = ReadLflNameStringToDialogue(nameOff);
  }
  else
  {
    entity->en_Name = 0;
  }

}

STATIC VOID ExportEntity(UWORD id, ULONG dataOffset, ULONG graphicsOffset)
{
  struct NEW_ANY_ENTITY entity;
  InitStackVar(entity);

  ReadLflObject(&entity, dataOffset, graphicsOffset);

  entity.en_Type = ET_ANY;
  entity.en_Size = sizeof(struct NEW_ANY_ENTITY);
  
  SaveAssetQuick(Archive, &entity, entity.en_Size, CT_ENTITY, id, CHUNK_FLAG_ARCH_ANY);
  AddToTable(CT_ENTITY, id, ArchiveId, MM_CHAPTER);
}

STATIC VOID ExportEntities()
{
  UWORD ii;
  ULONG dataIdx, graphicsIdx, dataOffset, graphicsOffset;

  dataIdx = ObjectOffset;
  graphicsIdx = GraphicsOffset;

  StartAssetList(Archive, CT_ENTITY);
  for (ii = 0; ii < NumObjects; ii++)
  {
    dataOffset = ReadUWordLE(dataIdx);
    graphicsOffset = ReadUWordLE(graphicsIdx);

    ExportEntity(GenerateAssetId(CT_ENTITY), dataOffset, graphicsOffset);

    dataIdx += 2;
    graphicsIdx += 2;
  }
  EndAssetList(Archive);
}

STATIC VOID ExportImageBackdrop(UWORD id)
{
  ULONG offset;
  ULONG chunkySize, planarSize;
  struct IMAGE backdrop;

  InitStackVar(backdrop)

  offset = ReadUWordLE(10);

  chunkySize = RoomWidth * RoomHeight;
  planarSize = chunkySize >> 1;  /* chunkSize / 2 */

  GrowMem(ChunkyData, chunkySize, ChunkyCapacity);

  if (NULL == ChunkyData)
  {
    PARROT_ERR(
      "Unable Convert LFL File\n"
      "Reason: (1) Chunky Mem could not be allocated"
      PARROT_ERR_INT("LFL")
      PARROT_ERR_INT("Width")
      PARROT_ERR_INT("Height")
      PARROT_ERR_INT("Bytes"),
      id,
      RoomWidth,
      RoomHeight,
      chunkySize
    );
  }

  GrowMem(PlanarData, planarSize, PlanarCapacity);

  if (NULL == PlanarData)
  {
    PARROT_ERR(
      "Unable Convert LFL File\n"
      "Reason: (2) Planar Mem could not be allocated"
      PARROT_ERR_INT("LFL")
      PARROT_ERR_INT("Width")
      PARROT_ERR_INT("Height")
      PARROT_ERR_INT("Bytes"),
      id,
      RoomWidth,
      RoomHeight,
      planarSize
    );
  }

  DecodeBackdrop(LflData + offset, ChunkyData, RoomWidth, RoomHeight);
  ConvertImageDataToPlanar(ChunkyData, PlanarData, RoomWidth, RoomHeight);

  backdrop.im_Width = RoomWidth;
  backdrop.im_Height = RoomHeight;
  backdrop.im_Depth = 4;
  backdrop.im_Palette = MM_PALETTE_ID;
  backdrop.im_BytesPerRow = (RoomWidth >> 3);
  backdrop.im_PlaneSize = backdrop.im_BytesPerRow * backdrop.im_Height;

  SaveAssetWithData(Archive, &backdrop, sizeof(backdrop), ChunkyData, planarSize, CT_IMAGE, id, CHUNK_FLAG_ARCH_ANY | CHUNK_FLAG_HAS_DATA);
  AddToTable(CT_IMAGE, id, ArchiveId, MM_CHAPTER);
  
}

STATIC VOID OpenLflFile(STRPTR lflPath, UWORD lflNum)
{
  CHAR path[MAX_PATH];
  BPTR  file;

  StrFormat(path, sizeof(path), "%s%02ld.LFL", lflPath, (ULONG) lflNum);

  file = Open(path, MODE_OLDFILE);

  if (NULL == file)
  {
    PARROT_ERR(
      "Unable Open LFL file.\n"
      "Reason: LFL file could not be found in path"
      PARROT_ERR_STR("Path")
      PARROT_ERR_INT("LFL File"),
      lflPath,
      (ULONG) lflNum
    );
  }

  Seek(file, 0, OFFSET_END);
  LflLength = Seek(file, 0, OFFSET_BEGINING);
  GrowMem(LflData, LflLength, LflCapacity);

  if (NULL == LflData)
  {
    PARROT_ERR(
      "Unable Convert LFL File\n"
      "Reason: Lfl Mem could not be allocated"
      PARROT_ERR_INT("LFL")
      PARROT_ERR_INT("Bytes"),
      lflNum,
      LflLength
    );
  }

  Read(file, LflData, LflLength);

  Close(file);
}

STATIC VOID CloseLflFile()
{
  LflLength = 0;
}

STATIC VOID ReadRoomData()
{
  RoomWidth = ReadUWordLE(4);
  RoomHeight = ReadUWordLE(6);

  NumObjects = ReadByte(20);

  GraphicsOffset = 28;
  ObjectOffset = 28 + (NumObjects * 2);
}

STATIC VOID DecodeBackdrop(UBYTE* src, UBYTE* dst, UWORD w, UWORD h)
{
  UWORD x, y;
  UBYTE r, len, col;
  ULONG offset;
  
  x = 0;
  y = 0;
  offset = 0;

  while (x < w)
  {
    r = *src++;
    r ^= 0xFF; /* Copy Protection */

    if (r < 0x80)
    {
      len = (r >> 4) & 0xF;
      col = r & 0xF;

      if (0 == len)
      {
        r = *src++;
        r ^= 0xFF; /* Copy Protection */
      }

      while (len--)
      {
        dst[offset] = col;
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
        len = *src++;
        len ^= 0xFF; /* Copy Protection */
      }

      while (len--)
      {
        col = dst[offset - 1];
        dst[offset] = col;
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

STATIC VOID ConvertImageDataToPlanar(UBYTE* src, UWORD* dst, UWORD w, UWORD h)
{
  ULONG idx;
  UBYTE bp, shift;
  UWORD y, x, i;
  idx = 0;

  for (bp = 0; bp < 4; bp++)
  {
    UBYTE shift = 1 << bp;
    idx = 0;

    for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x += 16)
      {
        UWORD word = 0;

        for (i = 0; i < 16; i++)
        {
          UBYTE col = src[idx + x + i];
          UBYTE bit = (col & shift) != 0 ? 1 : 0;

          if (bit)
            word |= (1 << (15 - i));
        }

        *dst++ = word;
      }
      idx += w;
    }

  }
}
