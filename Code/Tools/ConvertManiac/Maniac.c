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

STATIC SquawkPtr primary;

extern PtGameInfo GameInfo;

extern PtUnsigned16 GameLanguage;

void ExportTables(SquawkPtr primary);
void ReadGameInfo();
void ExportGameInfo(SquawkPtr primary);
void ExportPalettes(SquawkPtr primary);
void ExportPrimaryScripts(SquawkPtr primary);
void ExportLfl(PtUnsigned16 lflNum);
void StartTables();
void EndTables();
void StartLfl();
void EndLfl();
void StartDialogue();
void EndDialogue();
void ExportDialogue(SquawkPtr primary);
PtUnsigned16 GetNumTables();

void ConverterMain()
{
  PtUnsigned16 roomNum;
  PtChar language[2] = { 'e', 'n' };

  StartTables();
  StartDialogue();


  primary = OpenSquawkFile(0);

  ReadGameInfo();
  ExportPalettes(primary);
  ExportPrimaryScripts(primary);

  StartLfl();
  for (roomNum = 1; roomNum < MM_NUM_ROOMS; roomNum++)
  {
    ExportLfl(roomNum);
  }
  EndLfl();

  ExportDialogue(primary);
  ExportTables(primary);
  
  GameInfo.gi_NumAssetTables = GetNumTables();
  
  ExportGameInfo(primary);

  CloseSquawkFile(primary);
  primary = NULL;

  EndDialogue();
  EndTables();
  Requester("OK", "Converted");
}
