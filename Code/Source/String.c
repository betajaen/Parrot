/**
    $Id: String.c, 0.1, 2020/05/07 08:43:00, betajaen Exp $

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

#include <Parrot/Parrot.h>

#include <proto/exec.h>

ULONG StrLength(CONST_STRPTR pStr)
{
  ULONG length;

  length = 0;

  if (NULL == pStr)
  {
    return length;
  }

  while ('\0' != *pStr)
  {
    pStr++;
    length++;
  }

  return length;
}

#if defined(IS_M68K)
STATIC CONST ULONG PutChar = 0x16c04e75;
STATIC CONST ULONG CountChar = 0x52934E75;
#endif

ULONG StrLengthFormat(CHAR* pFmt, ...)
{
#if defined(IS_M68K)
  LONG size;;
  STRPTR* arg;

  size = 0;
  arg = (STRPTR*)(&pFmt + 1);
  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) &CountChar, (STRPTR)&size);

  return size;
#else
  return 0; /* Unimplemented */
#endif
}

ULONG StrFormat(CHAR* pBuffer, LONG pBufferCapacity, CHAR* pFmt, ...)
{
#if defined(IS_M68K)
  LONG size;
  STRPTR* arg;
  
  if (0 == pBufferCapacity)
  {
    return 0;
  }
  
  size = 0;
  arg = (STRPTR*)(&pFmt + 1);
  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) &CountChar, (STRPTR)&size);

  if (size >= pBufferCapacity || (0 == size))
  {
    return 0;
  }

  RawDoFmt((STRPTR)pFmt, arg, (void (*)(void)) &PutChar, pBuffer);

  return size;
#else
  return 0; /* Unimplemented */
#endif
}

ULONG StrLen(CONST CHAR* pSrc)
{
  ULONG ii;
  
  ii = 0;

  if (NULL == pSrc)
  {
    goto CLEAN_EXIT;
  }

  while (*pSrc++ != '\0')
  {
    ii++;
  }

CLEAN_EXIT:
  return ii;
}

ULONG StrCopy(CHAR* pDst, ULONG pDstCapacity, CONST CHAR* pSrc)
{
  ULONG copyLen;
  
  copyLen = 0;

  if (NULL == pDst || NULL == pSrc || 0 == pDstCapacity)
  {
    goto CLEAN_EXIT;
  }

  copyLen = StrLen(pSrc);
  
  if ((copyLen + 1) > pDstCapacity)
  {
    copyLen = 0;
    pDst[0] = '\0';
    goto CLEAN_EXIT;
  }

  CopyMem(pSrc, pDst, copyLen);
  pDst[copyLen] = '\0';

  CLEAN_EXIT:
  
  return copyLen;
}
