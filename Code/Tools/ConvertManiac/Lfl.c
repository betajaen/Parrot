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

#define MAX_PATH   256
#define MM_CHAPTER 1

STATIC UWORD  LflId;
STATIC BYTE*  LflData;
STATIC ULONG  LflLength;
STATIC IffPtr LflArchive;

STATIC VOID OpenLflFile(STRPTR lflPath);
STATIC VOID CloseLflFile();
STATIC VOID ExportRoom();
STATIC VOID ExportEntities();
STATIC VOID ExportBackdrop();

VOID ExportLfl(UWORD lflNum)
{
  LflId = lflNum;

  OpenLflFile("PROGDIR:");

  LflArchive = OpenSquawkFile(LflId);

  ExportRoom();
  ExportEntities();
  ExportBackdrop();

  CloseSquawkFile(LflArchive);
  LflArchive = NULL;
  CloseLflFile();
}

STATIC VOID ExportRoom()
{
  struct ROOM room;

  InitStackVar(room);

  StartAssetList(LflArchive, CT_ROOM);
  
  room.rm_Width = 320;
  room.rm_Height = 180;
  
  SaveAssetQuick(LflArchive, &room, sizeof(room), CT_ROOM, LflId, CHUNK_FLAG_ARCH_ANY);
  AddToTable(CT_ROOM, LflId, LflId, MM_CHAPTER);
  EndAssetList(LflArchive);
}

STATIC VOID ExportEntities()
{

}

STATIC VOID ExportBackdrop()
{

}

STATIC VOID OpenLflFile(STRPTR lflPath)
{
  CHAR path[MAX_PATH];
  BPTR  file;

  StrFormat(path, sizeof(path), "%s%02ld.LFL", lflPath, (ULONG)LflId);

  file = Open(path, MODE_OLDFILE);

  if (NULL == file)
  {
    PARROT_ERR(
      "Unable Open LFL file.\n"
      "Reason: LFL file could not be found in path"
      PARROT_ERR_STR("Path")
      PARROT_ERR_INT("LFL File"),
      lflPath,
      (ULONG) LflId
    );
  }

  Seek(file, 0, OFFSET_END);
  LflLength = Seek(file, 0, OFFSET_BEGINING);
  LflData = AllocVec(LflLength, 0);

  Read(file, LflData, LflLength);

  Close(file);
}

STATIC VOID CloseLflFile()
{
  FreeVec(LflData);
  LflLength = 0;
  LflData = NULL;
}
