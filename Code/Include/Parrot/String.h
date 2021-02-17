/**
    $Id: String.h 1.2 2020/05/17 16:08:00, betajaen Exp $

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

#ifndef PARROT_STRING_H
#define PARROT_STRING_H

PtUnsigned32 StrLength(CONST_STRPTR pStr);

PtUnsigned32 StrLengthFormat(PtChar* pFmt, ...);

PtUnsigned32 StrFormat(PtChar* pBuffer, PtSigned32 pBufferCapacity, PtChar* pFmt, ...);

PtUnsigned32 StrLen(CONST PtChar* pSrc);

PtUnsigned32 StrCopy(PtChar* pDst, PtUnsigned32 pDstCapacity, CONST PtChar* pSrc);

PtUnsigned32 StrHash(PtUnsigned8 length, STRPTR text);

PtBool StrStartsWith(CONST PtChar* pTest, PtChar value);

PtBool StrEndsWith(CONST PtChar* test, PtChar value);

PtBool StrStartsWithStr(CONST PtChar* pTest, CONST PtChar* pPrefix);

void FillMem(PtUnsigned8* pDst, PtUnsigned32 length, PtUnsigned8 value);

#define ClearMem(X) FillMem((PtUnsigned8*) &X, sizeof(X), 0);

PtChar* StrDuplicate(CONST PtChar* pSrc);

PtUnsigned32 UWordToId(PtUnsigned16 value);

#endif
