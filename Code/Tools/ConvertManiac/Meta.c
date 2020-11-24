/**
    $Id: Metadata.c, 1.0, 2020/05/13 07:16:00, betajaen Exp $

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

struct GAME_INFO GameInfo;

VOID ReadGameInfo()
{
  GameInfo.gi_Title = PushDialogue(LANG_ENGLISH, literal_strlen(MM_META_TITLE), MM_META_TITLE);
  GameInfo.gi_ShortTitle = PushDialogue(LANG_ENGLISH, literal_strlen(MM_META_SHORT_TITLE), MM_META_SHORT_TITLE);
  GameInfo.gi_Author = PushDialogue(LANG_ENGLISH, literal_strlen(MM_META_AUTHOR), MM_META_AUTHOR);
  GameInfo.gi_Release = PushDialogue(LANG_ENGLISH, literal_strlen(MM_META_RELEASE), MM_META_RELEASE);

  GameInfo.gi_Width = 320;
  GameInfo.gi_Height = 200;
  GameInfo.gi_Depth = 4;
  GameInfo.gi_StartPalette = 0;
  GameInfo.gi_StartCursorPalette = 0;
  GameInfo.gi_StartScript = 0;
  GameInfo.gi_NumAssetTables = 0;

}

VOID ExportGameInfo(IffPtr master)
{
  StartAssetList(master, CT_GAME_INFO);
  SaveAssetQuick(master, (APTR)&GameInfo, sizeof(GameInfo), CT_GAME_INFO, 1, CHUNK_FLAG_ARCH_ANY);
  EndAssetList(master);
}

STATIC VOID ExportGamePalette(IffPtr master, UWORD id)
{
  struct PALETTE_TABLE pal;
  ULONG* pData;

  InitStackVar(pal);

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


  SaveAssetQuick(master, (APTR)&pal, sizeof(pal), CT_PALETTE, id, CHUNK_FLAG_ARCH_ANY);
}

STATIC VOID ExportCursorPalette(IffPtr iff, UWORD id)
{
  struct PALETTE_TABLE  pal;
  ULONG* pData;

  InitStackVar(pal)

  pData = (ULONG*)&pal.pt_Data[0];

  *pData++ = (1l << 16) | 18;

  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;
  *pData++ = 0xFFFFFFFF;

  *pData = 0;

  pal.pt_Begin = 17;
  pal.pt_End = 18;

  SaveAssetQuick(iff, (APTR)&pal, sizeof(pal), CT_PALETTE, id, CHUNK_FLAG_ARCH_ANY);
}


VOID ExportPalettes(IffPtr iff)
{
  StartAssetList(iff, CT_PALETTE);
  ExportGamePalette(iff, MM_PALETTE_ID);
  ExportCursorPalette(iff, MM_PALETTE_ID + 1);
  EndAssetList(iff);

  GameInfo.gi_StartPalette = MM_PALETTE_ID;
  GameInfo.gi_StartCursorPalette = MM_PALETTE_ID + 1;
}
