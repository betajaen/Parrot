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

VOID ExportGameInfo(IffPtr master)
{
  struct GAME_INFO gi;
  
  InitStackVar(struct GAME_INFO, gi);

  StrCopy(&gi.gi_Title[0], sizeof(gi.gi_Title), MM_META_TITLE );
  StrCopy(&gi.gi_ShortTitle[0], sizeof(gi.gi_ShortTitle), MM_META_SHORT_TITLE);
  StrCopy(&gi.gi_Author[0], sizeof(gi.gi_Author), MM_META_AUTHOR);
  StrCopy(&gi.gi_Release[0], sizeof(gi.gi_Release), MM_META_RELEASE);

  gi.gi_Width = 320;
  gi.gi_Height = 200;
  gi.gi_Depth = 4;
  gi.gi_RoomCount = MM_NUM_ROOMS;
  gi.gi_StartPalette = 1;
  gi.gi_StartCursorPalette = 1;
  gi.gi_StartRoom = 1;
  gi.gi_StartScript = 1;

  StartAssetList(master, CT_GAME_INFO);
  SaveAssetQuick(master, (APTR)&gi, sizeof(gi), CT_GAME_INFO, 1, CHUNK_FLAG_ARCH_ANY);
  EndAssetList(master);
}
