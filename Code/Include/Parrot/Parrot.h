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
#include <exec/nodes.h>

#include <Parrot/Private/SDI_compiler.h>

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

#ifndef EXPORT
#define EXPORT
#endif

/**
    Helper Macros
*/

#ifndef NewList
#define NewList(X)\
  (X)->lh_Head = (struct Node*) &(X)->lh_Tail;\
  (X)->lh_Tail = 0;\
  (X)->lh_TailPred = (struct Node*) &(X)->lh_Head;
#endif

#define MAKE_NODE_ID(a,b,c,d)	\
	((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

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

#define ARCHIVE_GLOBAL  0
#define ARCHIVE_UNKNOWN 65535

#define CT_GAME_INFO      MAKE_NODE_ID('G','A','M','E')
#define CT_ROOM           MAKE_NODE_ID('R','O','O','M')
#define CT_IMAGE          MAKE_NODE_ID('I','M','G','E')
#define CT_PALETTE32      MAKE_NODE_ID('P','A','L','5')
#define CT_PALETTE4       MAKE_NODE_ID('P','A','L','2')

struct ASSET
{
  struct MinNode    as_Node;
  UWORD             as_Id;
  UWORD             as_Arch;
};

struct ASSET_REF
{
  APTR              ar_Ptr;
  UWORD             ar_Id;
};

#define IS_REF_LOADED(REF) (NULL != (REF).ar_Ptr)

/*
  A chunk header is a 4-byte header after the IFF chunk containing the 16-bit ID
  and 16-bit chunk flags.
*/
struct CHUNK_HEADER
{
  UWORD Id;
  UWORD Flags;
};

#define CHUNK_FLAG_ARCH_ECS  (1 << 0)
#define CHUNK_FLAG_ARCH_AGA  (1 << 1)
#define CHUNK_FLAG_ARCH_RTG  (1 << 2)
#define CHUNK_FLAG_ARCH_ANY  (CHUNK_FLAG_ARCH_ECS | CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG)

#define CHUNK_FLAG_IGNORE    (1 << 15)


/*
      Game Info
*/

struct GAME_INFO
{
  ULONG             GameId;
  ULONG             GameVersion;
  CHAR              Title[64];
  CHAR              ShortTitle[16];
  CHAR              Author[128];
  CHAR              Release[128];
  UWORD             Width;
  UWORD             Height;
  UWORD             Depth;
};

/*
    Palette 32 Table

*/
struct PALETTE32_TABLE
{
  ULONG Count_Start;
  ULONG Index[32 * 3];
  ULONG Terminator;
};

/*
  Palette 4 Table
*/
struct PALETTE4_TABLE
{
  ULONG Count_Start;
  ULONG Index[4 * 3];
  ULONG Terminator;
};


/*
      Image
*/

struct IMAGE
{
  struct ASSET      as_Asset;

  UWORD             im_Width;
  UWORD             im_Height;
  UBYTE             im_Depth;
  UBYTE             im_Format;
  BYTE*             im_Data;
};

struct IMAGE_REF
{
  struct IMAGE*     ar_Ptr;
  UWORD             ar_Id;
};

struct IMAGE_CHUNK
{
  struct CHUNK_HEADER Header;

  UWORD Width;
  UWORD Height;
  UWORD Palette;
};



struct ROOM
{
  UWORD             rm_Width;
  UWORD             rm_Height;
  struct IMAGE_REF  rm_Backdrops[4];
};

struct ROOM_ASSET
{
  struct ASSET      as_Asset;
  struct ROOM       as_Data;
};

struct ROOM_REF
{
  struct ROOM*      ar_Ptr;
  UWORD             ar_Id;
};

#define UNPACK_ROOM_BACKDROPS  1

#endif
