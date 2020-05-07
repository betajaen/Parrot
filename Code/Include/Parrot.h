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

#include <exec/types.h>

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
