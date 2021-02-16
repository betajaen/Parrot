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

#include <Squawk/Squawk.h>
#include <Squawk/Writer/Script.h>


PtUnsigned16 ScriptData[MAX_SCRIPT_WRITER_SIZE];
PtUnsigned16 ScriptSize = 0;

PtUnsigned16 ScriptConstants[MAX_CONSTANTS_PER_SCRIPT];
PtUnsigned16 ScriptNumConstants = 0;

extern void exit();

void ScriptEnforceSpace()
{
  if (ScriptSize >= MAX_SCRIPT_WRITER_SIZE)
  {
    PARROT_ERR0("Ran out of script space when compiling script");
  }
}

void ScriptBegin()
{
  ScriptSize = 0;
  ScriptNumConstants = 0;
  
  for (PtUnsigned16 ii = 0; ii < MAX_CONSTANTS_PER_SCRIPT; ii++)
  {
    ScriptConstants[ii] = 0;
  }

}

void ScriptEnd()
{
}

void ScriptSave(SquawkPtr archive, PtUnsigned16 id, PtUnsigned16 chapter)
{
  struct SCRIPT script;

  PtUnsigned16 archiveId = 0;
  
  script.as_Id = id;
  script.as_Flags = PT_AF_ARCH_ANY;
  
  script.sc_NumOpcodes = ScriptSize;
  
  for (PtUnsigned16 ii = 0; ii < MAX_CONSTANTS_PER_SCRIPT; ii++)
  {
    script.sc_Constants[ii] = ScriptConstants[ii];
  }
  
  SaveAssetExtra(
    archive,
    (PtAsset*) &script,
    sizeof (struct SCRIPT),
    &ScriptData,
    ScriptSize * sizeof(PtUnsigned16)
  );

  AddToTable(PT_AT_SCRIPT, script.as_Id, archiveId, chapter);
}
