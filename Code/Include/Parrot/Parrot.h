/**
    $Id: Parrot.h 1.5 2021/02/21 10:19:00, betajaen Exp $

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

#ifndef _PARROT_H_
#define _PARROT_H_

#define PARROT_VERSION "Parrot 1.5 (21.02.2021)"

#include <exec/types.h>
#include <Parrot/sdi/SDI_compiler.h>

typedef BYTE   int8;
typedef WORD   int16;
typedef LONG   int32;

typedef UBYTE  uint8;
typedef UWORD  uint16;
typedef ULONG  uint32;

typedef ULONG  uint;
typedef BOOL   bool;

#define PARROT_STRINGIFY(a) PARROT_STRINGIFY_(a)
#define PARROT_STRINGIFY_(a) #a

#endif

