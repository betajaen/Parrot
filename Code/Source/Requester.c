/**
    $Id: Requester.c, 0.1, 2020/05/07 08:54:00, betajaen Exp $

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

#include "Common.h"

#include <proto/exec.h>
#include <proto/intuition.h>

struct EasyStruct EasyRequesterStruct =
{
  sizeof(struct EasyStruct),
  0,
  "Parrot",
  NULL,
  NULL,
};

STATIC CHAR RequesterText[1024] = { 0 };

LONG Requester(CONST_STRPTR pOptions, CONST_STRPTR pText)
{
  if (NULL == pOptions || '\0' == pOptions[0])
  {
    EasyRequesterStruct.es_GadgetFormat = (UBYTE*)"Okay";
  }
  else
  {
    EasyRequesterStruct.es_GadgetFormat = (UBYTE*)pOptions;
  }

  if (NULL == pText || '\0' == pText[0])
  {
    EasyRequesterStruct.es_TextFormat = (UBYTE*)"No Message.";
  }
  else
  {
    EasyRequesterStruct.es_TextFormat = (UBYTE*)pText;
  }

  return EasyRequest(NULL, &EasyRequesterStruct, NULL);
}

#if defined(IS_M68K)
STATIC CONST ULONG PutChar = 0x16c04e75;
STATIC CONST ULONG CountChar = 0x52934E75;
#endif

LONG RequesterF(CONST_STRPTR pOptions, CONST_STRPTR pFmt, ...)
{
#if defined(IS_M68K)
  LONG size;;
  STRPTR* arg;

  size = 0;
  arg = (STRPTR*)(&pFmt + 1);
  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) & CountChar, (STRPTR )&size);

  if (size >= sizeof(RequesterText) || (0 == size))
  {
    return 0;
  }

  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) & PutChar, (STRPTR) &RequesterText[0]);

  return Requester(pOptions, RequesterText);
#else
  return 0; /* Unimplemented */
#endif
}

EXPORT LONG TestRequester(LONG x)
{
  return RequesterF("Okay", "This is a test. Value given is = %ld", x);
}
