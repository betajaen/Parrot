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
#include <Squawk/Squawk.h>
#include "Maniac.h"

STATIC IffPtr master;

VOID ExportGameInfo(IffPtr master);
VOID ExportPalettes(IffPtr master);
VOID ExportLfl(UWORD lflNum);
VOID StartLfl();
VOID EndLfl();
VOID StartDialogue();
VOID EndDialogue();
VOID ExportDialogue(IffPtr master);

VOID ConverterMain()
{
  UWORD roomNum;
  CHAR language[2] = { 'e', 'n' };

  StartDialogue();

  master = OpenSquawkFile(0);

  ExportGameInfo(master);
  ExportPalettes(master);

  // PushDialogue(LANG_ENGLISH, 6, "Parrot");

  StartLfl();
  for (roomNum = 1; roomNum < MM_NUM_ROOMS; roomNum++)
  {
    ExportLfl(roomNum);
  }
  EndLfl();

  ExportDialogue(master);


  CloseSquawkFile(master);
  master = NULL;

  EndDialogue();
  Requester("OK", "Converted");
}
