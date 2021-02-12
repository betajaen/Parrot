/**
    $Id: ScriptWriter, 1.0, 2020/11/14 06:45:00, betajaen Exp $

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

#pragma once

#ifndef SQUAWK_WRITER_SCRIPT_H
#define SQUAWK_WRITER_SCRIPT_H

#define MAX_SCRIPT_WRITER_SIZE 256

extern UWORD ScriptData[MAX_SCRIPT_WRITER_SIZE];
extern UWORD ScriptSize;

VOID ScriptEnforceSpace();

VOID ScriptBegin();

VOID ScriptEnd();

VOID ScriptSave(SquawkPtr archive, UWORD id, UWORD chapter);

inline UWORD ScriptU16tou10(UWORD value)
{
  return value << 6;
}

inline UWORD ScriptS16tos10(WORD value)
{
  UWORD v = value << 6;
  return v;
}

#include <Parrot/Vm_Opcodes.h>
#include "Script_Opcodes.inc"

#endif
