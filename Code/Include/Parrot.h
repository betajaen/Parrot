/**
    $Id: Parrot.h 0.1, 2020/05/07 06:49:00, betajaen Exp $

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
#ifndef PARROT_H
#define PARROT_H

#include <exec/types.h>

#include "SDI_compiler.h"

#if defined(__M68K__)
#define IS_M68K
#else
#error "Unsupported Arch"
#endif

/**
    Typename consistency
*/
#ifndef INT
#define INT int
#endif

#ifndef CHAR
#define CHAR char
#endif

#ifndef STATIC
#define STATIC static
#endif

/**
    SDL Banned Functions
*/
#define memcpy      __unsafe_memcpy   /* DO NOT USE     */
#define strcat      __unsafe_strcat   /* DO NOT USE     */
#define strcpy      __unsafe_strcpy   /* DO NOT USE     */
#define sprintf     __unsafe_sprintf  /* Use StrFormat  */
#define strlen      __unsafe_strlen   /* Use StrLength  */
#define strtok      __unsafe_strtok   /* DO NOT USE     */


#define SIF_IS_PUBLIC   1
#define SIF_IS_HIRES    2
#define SIF_IS_LACE     4

struct SCREEN_INFO
{
  LONG    si_Left, si_Top, si_Width, si_Height;
  STRPTR  si_Title;
  UWORD   si_Depth;
  UWORD   si_Flags;  /* See SIF_* */
};

#define CURSOR_NONE   0
#define CURSOR_POINT  1
#define CURSOR_BUSY   2
#define CURSOR_SELECT 3
#define CURSOR_NW     4
#define CURSOR_N      5
#define CURSOR_NE     6
#define CURSOR_E      7
#define CURSOR_SE     8
#define CURSOR_S      9
#define CURSOR_SW     10
#define CURSOR_W      11
#define CURSOR_ANY    12
#define CURSOR_MOUSE  13
#define CURSOR_LMB    14
#define CURSOR_RMB    15
#define CURSOR_USE    16
#define CURSOR_TALK   17
#define CURSOR_TAKE   18
#define CURSOR_GIVE   19
#define CURSOR_DROP   20
#define CURSOR_READ   21
#define CURSOR_LOOK   22
#define CURSOR_PULL   23
#define CURSOR_PUSH   24
#define CURSOR_OPEN   25
#define CURSOR_CLOSE  26
#define CURSOR_LOCK   27
#define CURSOR_UNLOCK 28

struct ROOM
{
  ULONG   rm_Id;
};

#endif
