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

#include <exec/types.h>
#include <Parrot/Requester.h>

#define MAX_SCRIPT_WRITER_SIZE 256

STATIC BYTE ScriptData[MAX_SCRIPT_WRITER_SIZE];
STATIC UWORD ScriptSize = 0;
extern VOID exit();

STATIC VOID ScriptEnforceSpace(UWORD length)
{
  if (ScriptSize + length >= MAX_SCRIPT_WRITER_SIZE)
  {
    DebugF("OK", "Ran out of script space when compiling script");
    exit();
  }
}

STATIC VOID ScriptWriteByte2(UBYTE first, UBYTE second)
{
  ScriptEnforceSpace(2);
  ScriptData[ScriptSize++] = first;
  ScriptData[ScriptSize++] = second;
}

STATIC VOID ScriptWriteWord(WORD value)
{
  ScriptEnforceSpace(2);
  ScriptData[ScriptSize++] = (value & 0xFF00) >> 8;
  ScriptData[ScriptSize++] = value & 0xFF;
}

STATIC VOID ScriptWriteLong(LONG value)
{
  ScriptEnforceSpace(4);
  ScriptData[ScriptSize++] = ((value) >> 24) & 0xFF;
  ScriptData[ScriptSize++] = ((value) >> 16) & 0xFF;
  ScriptData[ScriptSize++] = ((value) >> 8) & 0xFF;
  ScriptData[ScriptSize++] = (value & 0xFF);
}

STATIC VOID ScriptOpStop()
{
  ScriptWriteByte2(0, 0);
}

STATIC VOID ScriptOpRem(BYTE user)
{
  ScriptWriteByte2(1, user);
}

STATIC VOID ScriptOpPushByte(BYTE value)
{
  ScriptWriteByte2(2, value);
}

STATIC VOID ScriptOpPushWord(WORD value)
{
  ScriptWriteByte2(3, 0);
  ScriptWriteWord(value);
}

STATIC VOID ScriptOpPushLong(LONG value)
{
  ScriptWriteByte2(4, 0);
  ScriptWriteWord(value);
}

STATIC VOID ScriptOpPushStack(BYTE where)
{
  ScriptWriteByte2(5, where);
}

STATIC VOID ScriptOpPop(BYTE where)
{
  ScriptWriteByte2(6, 0);
}

/* 0x7 - Reserved */
/* 0x8 - j */
/* 0x9 - jz */
/* 0xA - jnz */
/* 0xB - je */
/* 0xC - jne */
/* 0xD - jg */
/* 0xE - jge */
/* 0xF - jl */
/* 0x10 - jle */
/* 0x11 - load */
/* 0x12 - save */
/* 0x13 - gload */
/* 0x14 - gsave */
/* 0x15 - Reserved */
/* 0x16 - Reserved */
/* 0x17 - Reserved */
/* 0x18 - Reserved */
/* 0x19 - Reserved */
/* 0x1A - OpAdd(vm) */
/* 0x1B - OpAddQuick(vm) */
/* 0x1C - OpSub(vm) */
/* 0x1D - OpSubQuick(vm) */

STATIC VOID ScriptOpRoom()
{
  ScriptWriteByte2(0x20, 0);
}

/* 0x40 - Audio */
/* 0x80 - Print */

STATIC VOID ScriptBegin()
{
  ScriptSize = 0;
}

STATIC VOID ScriptEnd()
{
  ScriptOpStop();
}
