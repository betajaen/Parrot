/**
    $Id: Requester.h 0.1, 2020/05/17 16:09:00, betajaen Exp $

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

LONG Requester(CONST_STRPTR pOptions, CONST_STRPTR pText);

LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...);

VOID TraceF(CONST_STRPTR pFmt, ...);

VOID ErrorF(CONST_STRPTR pFmt, ...);

#define PARROT_SERIOUS_ERROR(MESSAGE, REASON, ...) \
  ErrorF(MESSAGE "\nReason:" REASON, __VA_ARGS__)

#define PARROT_ERR(MESSAGE, ...) \
  ErrorF(MESSAGE, __VA_ARGS__);

#define PARROT_ERR_PTR(NAME) \
  "\n " NAME " = %lx"

#define PARROT_ERR_INT(NAME) \
  "\n " NAME " = %ld"

#define PARROT_ERR_HEX(NAME) \
  "\n " NAME " = %ld"

#define PARROT_ERR_STR(NAME) \
  "\n " NAME " = %s"
