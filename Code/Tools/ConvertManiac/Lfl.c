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

STATIC PtByte*  LflData;
STATIC PtUnsigned32  LflLength;
STATIC PtUnsigned32  LflCapacity;
STATIC SquawkPtr Archive;
STATIC PtUnsigned16  ArchiveId;

STATIC PtUnsigned16  RoomId;
STATIC PtUnsigned16  RoomWidth;
STATIC PtUnsigned16  RoomHeight;
STATIC PtUnsigned16  BackdropId;
STATIC PtUnsigned16  NumObjects;
STATIC PtUnsigned32  ObjectOffset;
STATIC PtUnsigned32  GraphicsOffset;

STATIC PtUnsigned8* ChunkyData;
STATIC PtUnsigned32  ChunkyCapacity;

STATIC PtUnsigned16* PlanarData;
STATIC PtUnsigned32  PlanarCapacity;

STATIC void OpenLflFile(STRPTR lflPath, PtUnsigned16 lflNum);
STATIC void CloseLflFile();
STATIC void ExportRoom();
STATIC void ExportEntities();
STATIC void ExportImageBackdrop(PtUnsigned16 id);
STATIC void ReadRoomData();
STATIC void DecodeBackdrop(PtUnsigned8* src, PtUnsigned8* dst, PtUnsigned16 w, PtUnsigned16 h);
STATIC void ConvertImageDataToPlanar(PtUnsigned8* src, PtUnsigned16* dst, PtUnsigned16 w, PtUnsigned16 h);

STATIC PtUnsigned8 ReadByte(PtUnsigned32 offset)
{
  PtUnsigned8 r = 0;

  r = LflData[offset];
  r ^= 0xFF;  /* Copy Protection */

  return r;
}

STATIC PtUnsigned16 ReadUWordLE(PtUnsigned32 offset)
{
  PtUnsigned16 r = 0;

  r = LflData[offset + 1] << 8 | LflData[offset]; /* Little Endian to Big Endian */
  r ^= 0xFFFF;  /* Copy Protection */

  return r;
}

STATIC PtUnsigned32 ReadLflNameStringToDialogue(PtUnsigned32 offset)
{
  PtUnsigned16 len;
  PtUnsigned8 ch;
  PtChar text[255];

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

void StartLfl()
{
  LflData = NULL;
  LflCapacity = 0;
  ChunkyData = NULL;
  ChunkyCapacity = 0;
  PlanarData = NULL;
  PlanarCapacity = 0;
}

void EndLfl()
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

void ExportLfl(PtUnsigned16 lflNum)
{
  OpenLflFile("PROGDIR:", lflNum);

  ArchiveId = GenerateArchiveId();
  Archive = OpenSquawkFile(ArchiveId);

  ReadRoomData();

  ExportRoom();
  ExportEntities();

  StartAssetList(Archive, PT_AT_IMAGE, 1);
  ExportImageBackdrop(BackdropId);
  EndAssetList(Archive);

  CloseSquawkFile(Archive);
  Archive = NULL;
  CloseLflFile();
}

STATIC void ExportRoom()
{
  struct Room room;

  ClearMem(room);

  RoomId = GenerateAssetId(PT_AT_ROOM);
  BackdropId = GenerateAssetId(PT_AT_IMAGE);

  room.rm_Width = RoomWidth;
  room.rm_Height = RoomHeight;
  room.rm_Backdrops[0] = BackdropId;

  StartAssetList(Archive, PT_AT_ROOM, 1);

  room.as_Id = RoomId;
  room.as_Flags = PT_AF_ARCH_ANY;

  SaveAsset(Archive, (PtAsset*) &room, sizeof(struct Room));
  AddToTable(PT_AT_ROOM, RoomId, ArchiveId, MM_CHAPTER);
  EndAssetList(Archive);
}


STATIC void ReadLflObject(struct AnyEntity* entity, PtUnsigned32 dataOffset, PtUnsigned32 graphicsOffset)
{
  PtUnsigned32 nameOff, size;

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

STATIC void ExportEntity(PtUnsigned16 id, PtUnsigned32 dataOffset, PtUnsigned32 graphicsOffset)
{
  struct AnyEntity entity;
  ClearMem(entity);

  ReadLflObject(&entity, dataOffset, graphicsOffset);

  entity.en_Type = ET_ANY;
  entity.en_Size = sizeof(struct AnyEntity);
  
  entity.as_Id = id;
  entity.as_Flags = PT_AF_ARCH_ANY;

  SaveAsset(Archive, (PtAsset*)&entity, sizeof(struct AnyEntity));
  AddToTable(PT_AT_ENTITY, id, ArchiveId, MM_CHAPTER);
}

STATIC void ExportEntities()
{
  PtUnsigned16 ii;
  PtUnsigned32 dataIdx, graphicsIdx, dataOffset, graphicsOffset;

  dataIdx = ObjectOffset;
  graphicsIdx = GraphicsOffset;

  StartAssetList(Archive, PT_AT_ENTITY, 1);
  for (ii = 0; ii < NumObjects; ii++)
  {
    dataOffset = ReadUWordLE(dataIdx);
    graphicsOffset = ReadUWordLE(graphicsIdx);

    ExportEntity(GenerateAssetId(PT_AT_ENTITY), dataOffset, graphicsOffset);

    dataIdx += 2;
    graphicsIdx += 2;
  }
  EndAssetList(Archive);
}

STATIC void ExportImageBackdrop(PtUnsigned16 id)
{
  PtUnsigned32 offset;
  PtUnsigned32 chunkySize, planarSize;
  struct IMAGE backdrop;

  ClearMem(backdrop)

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

  backdrop.as_Id = id;
  backdrop.as_Flags = PT_AF_ARCH_ANY;
  backdrop.im_Width = RoomWidth;
  backdrop.im_Height = RoomHeight;
  backdrop.im_Depth = 4;
  backdrop.im_Palette = MM_PALETTE_ID;
  backdrop.im_BytesPerRow = (RoomWidth >> 3);
  backdrop.im_PlaneSize = backdrop.im_BytesPerRow * backdrop.im_Height;

  SaveAssetExtra(Archive, (PtAsset*) &backdrop, sizeof(backdrop), PlanarData, planarSize);
  AddToTable(PT_AT_IMAGE, id, ArchiveId, MM_CHAPTER);
  
}

STATIC void OpenLflFile(STRPTR lflPath, PtUnsigned16 lflNum)
{
  PtChar path[MAX_PATH];
  BPTR  file;

  StrFormat(path, sizeof(path), "%s%02ld.LFL", lflPath, (PtUnsigned32) lflNum);

  file = Open(path, MODE_OLDFILE);

  if (NULL == file)
  {
    PARROT_ERR(
      "Unable Open LFL file.\n"
      "Reason: LFL file could not be found in path"
      PARROT_ERR_STR("Path")
      PARROT_ERR_INT("LFL File"),
      lflPath,
      (PtUnsigned32) lflNum
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

STATIC void CloseLflFile()
{
  LflLength = 0;
}

STATIC void ReadRoomData()
{
  RoomWidth = ReadUWordLE(4);
  RoomHeight = ReadUWordLE(6);

  NumObjects = ReadByte(20);

  GraphicsOffset = 28;
  ObjectOffset = 28 + (NumObjects * 2);
}

STATIC void DecodeBackdrop(PtUnsigned8* src, PtUnsigned8* dst, PtUnsigned32 w, PtUnsigned32 h)
{
  PtUnsigned32 x, y, z;
  PtUnsigned8 r, len, col;
  
  x = 0;
  y = 0;

  while (x < w)
  {
    r = *src++;
    r ^= 0xFF; /* Copy Protection */

    if (r < 0x80)
    {
      len = (r >> 4);
      col = r & 0xF;

      if (0 == len)
      {
        len = *src++;
        len ^= 0xFF; /* Copy Protection */
      }

      for (z=0;z < len;z++)
      {
        dst[x + (y * w)] = col;
        y++;
        if (y >= h)
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
        len = *src++;
        len ^= 0xFF; /* Copy Protection */
      }

      for (z = 0; z < len; z++)
      {
        col = dst[(x - 1) + (y * w)];
        dst[x + (y * w)] = col;
        y++;
        if (y >= h)
        {
          y = 0;
          x++;
        }
      }
    }

  }

}

STATIC void ConvertImageDataToPlanar(PtUnsigned8* src, PtUnsigned16* dst, PtUnsigned16 w, PtUnsigned16 h)
{
  PtUnsigned32 idx;
  PtUnsigned8 bp, shift;
  PtUnsigned16 y, x, i;
  idx = 0;

  for (bp = 0; bp < 4; bp++)
  {
    PtUnsigned8 shift = 1 << bp;
    idx = 0;

    for (y = 0; y < h; y++)
    {
      for (x = 0; x < w; x += 16)
      {
        PtUnsigned16 word = 0;

        for (i = 0; i < 16; i++)
        {
          PtUnsigned8 col = src[idx + x + i];
          PtUnsigned8 bit = (col & shift) != 0 ? 1 : 0;

          if (bit)
            word |= (1 << (15 - i));
        }

        *dst++ = word;
      }
      idx += w;
    }

  }
}
