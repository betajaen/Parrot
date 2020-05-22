/**
    $Id: String.h 0.1, 2020/05/17 16:08:00, betajaen Exp $

    Parrot - Point and Click Adventure Game Player
    ==============================================

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

ULONG StrLength(CONST_STRPTR pStr);

ULONG StrLengthFormat(CHAR* pFmt, ...);

ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...);

ULONG StrLen(CONST CHAR* pSrc);

ULONG StrCopy(CHAR* pDst, ULONG pDstCapacity, CONST CHAR* pSrc);

BOOL StrStartsWith(CONST CHAR* pTest, CHAR value);

BOOL StrEndsWith(CONST CHAR* test, CHAR value);

VOID FillMem(UBYTE* pDst, ULONG length, UBYTE value);

#define InitStackVar(T, X) FillMem((UBYTE*) &X, sizeof(T), 0);

CHAR* StrDuplicate(CONST CHAR* pSrc);
