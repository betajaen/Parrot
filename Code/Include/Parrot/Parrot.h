/**
    $Id: Parrot.h 1.2 2020/05/07 06:49:00, betajaen Exp $

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

#define MAX_SCREENS            2
#define MAX_ROOM_BACKDROPS     2
#define MAX_ROOM_EXITS         10
#define MAX_ROOM_ENTITIES      20
#define MAX_ENTITY_NAME_LENGTH 29
#define MAX_VIEW_LAYOUTS       2
#define MAX_INPUT_EVENT_SIZE   32

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

#ifndef EXTERN
#define EXTERN extern
#endif

/**
    Helper Macros
*/

#define NEW_LIST(list)\
    list.lh_Head          = (struct Node *) &list.lh_Tail;\
    list.lh_Tail          = 0;\
    list.lh_TailPred      = (struct Node*) &list.lh_Head;\

#define NEW_MIN_LIST(list)\
    list.mlh_Head          = (struct MinNode *) &list.mlh_Tail;\
    list.mlh_Tail          = 0;\
    list.mlh_TailPred      = (struct MinNode*) &list.mlh_Head;

#define MAKE_NODE_ID(a,b,c,d)	\
	((ULONG) (a)<<24 | (ULONG) (b)<<16 | (ULONG) (c)<<8 | (ULONG) (d))

#define MAKE_ENTITY_ID(a,b)	\
	((USHORT) (a)<<8 | (USHORT) (b))

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

struct VIEW_LAYOUT
{
  UWORD  vl_Width;
  UWORD  vl_Height;
  UWORD  vl_BitMapWidth;
  UWORD  vl_BitmapHeight;
  WORD   vl_Horizontal;
  WORD   vl_Vertical;
  UWORD  vl_Depth;
};

struct VIEW_LAYOUTS
{
  LONG               v_Left;
  LONG               v_Top;
  LONG               v_Width;
  LONG               v_Height;
  UWORD              v_NumLayouts;
  struct VIEW_LAYOUT v_Layouts[MAX_VIEW_LAYOUTS];
};

#define CURSOR_NONE   0
#define CURSOR_SELECT 1
#define CURSOR_BUSY   2

#define ARCHIVE_GLOBAL  0
#define ARCHIVE_UNKNOWN 65535

#define CT_GAME_INFO      MAKE_NODE_ID('G','A','M','E')
#define CT_ROOM           MAKE_NODE_ID('R','O','O','M')
#define CT_IMAGE          MAKE_NODE_ID('I','M','G','E')
#define CT_PALETTE        MAKE_NODE_ID('P','A','L','4')
#define CT_TABLE          MAKE_NODE_ID('T','B','L','E')
#define CT_ENTITY         MAKE_NODE_ID('E','N','T','Y')

#define IET_KEYDOWN    1
#define IET_KEYUP      2
#define IET_SELECTDOWN 3
#define IET_SELECTUP   4
#define IET_MENUDOWN   5
#define IET_MENUUP     6
#define IET_CURSOR       7

struct INPUTEVENT
{
  UWORD             ie_Type;
  UWORD             ie_Code;
  WORD              ie_CursX;
  WORD              ie_CursY;
};

struct ASSET
{
  ULONG             as_ClassType;
  UWORD             as_Arch;
  UWORD             as_Id;
};

#define GET_ASSET_ID(X) (((UWORD*)X)[-1])

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
  UWORD ch_Id;
  UWORD ch_Flags;
};

#define CHUNK_FLAG_ARCH_ECS       (1 << 0)
#define CHUNK_FLAG_ARCH_AGA       (1 << 1)
#define CHUNK_FLAG_ARCH_RTG       (1 << 2)
#define CHUNK_FLAG_ARCH_ANY       (CHUNK_FLAG_ARCH_ECS | CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG)

#define CHUNK_FLAG_HAS_DATA       (1 << 14)
#define CHUNK_FLAG_IGNORE         (1 << 15)

struct OBJECT_TABLE_REF
{
  ULONG tr_ClassType;
  UWORD tr_ChunkHeaderId;
  UWORD tr_ArchiveId;
};

/*
      Game Info
*/

struct GAME_INFO
{
  ULONG                     gi_GameId;
  ULONG                     gi_GameVersion;
  CHAR                      gi_Title[64];
  CHAR                      gi_ShortTitle[16];
  CHAR                      gi_Author[128];
  CHAR                      gi_Release[128];
  UWORD                     gi_Width;
  UWORD                     gi_Height;
  UWORD                     gi_Depth;
  UWORD                     gi_RoomCount;
  struct OBJECT_TABLE_REF   gi_StartTables[16];
  UWORD                     gi_StartPalette;
  UWORD                     gi_StartCursorPalette;
  UWORD                     gi_StartRoom;
};

/*
    Object Table
*/

struct OBJECT_TABLE_ITEM
{
  UWORD ot_Id;
  UWORD ot_Archive;
  UWORD ot_Flags;
  ULONG ot_Size;
  APTR  ot_Ptr;
};

#define MAX_ITEMS_PER_TABLE 64

struct OBJECT_TABLE
{
  APTR                      ot_Next;
  struct OBJECT_TABLE_REF   ot_NextRef;
  ULONG                     ot_ClassType;
  UWORD                     ot_IdMin;
  UWORD                     ot_IdMax;
  struct OBJECT_TABLE_ITEM  ot_Items[MAX_ITEMS_PER_TABLE];
};

/*
    Palette Table

    Upto 16 Colours
*/
struct PALETTE_TABLE
{
  UBYTE pt_Begin;
  UBYTE pt_End;
  ULONG pt_Data[64];
};

/*
      Image
*/

struct IMAGE
{
  UWORD             im_BytesPerRow;
  UWORD             im_Height;
  UBYTE             im_Flags;
  UBYTE             im_Depth;
  UWORD             im_pad;
  UBYTE*            im_Planes[8];
  UWORD             im_Width;
  UWORD             im_Palette;
  ULONG             im_PlaneSize;
};

/*

      Room

*/

#define DIR_NW     0
#define DIR_N      1
#define DIR_NE     2
#define DIR_E      3
#define DIR_SE     4
#define DIR_S      5
#define DIR_SW     6
#define DIR_W      7
#define DIR_UP     DIR_N
#define DIR_DOWN   DIR_S

struct RECT
{
  WORD rt_Left;
  WORD rt_Top;
  WORD rt_Right;
  WORD rt_Bottom;
};

struct POINT
{
  WORD pt_Left;
  WORD pt_Top;
};

#define ET_EXIT MAKE_ENTITY_ID('E','X')

#define ETF_IS_NAMED  (1 << 0)
#define ETF_IS_LOCKED (1 << 14)
#define ETF_IS_OPEN   (1 << 15)

struct ENTITY
{
  UWORD                en_Type;
  UWORD                en_Flags;
  struct RECT          en_HitBox;
  UBYTE                en_Name[MAX_ENTITY_NAME_LENGTH + 1];
};

struct EXIT
{
  UWORD                ex_Type;
  UWORD                ex_Flags;
  struct RECT          ex_HitBox;
  UBYTE                ex_Name[MAX_ENTITY_NAME_LENGTH + 1];
  UWORD                ex_Target;
};

struct ROOM
{
  UWORD               rm_Width;
  UWORD               rm_Height;
  UWORD               rm_Backdrops[MAX_ROOM_BACKDROPS];
  UWORD               rm_Exits[MAX_ROOM_EXITS];
  UWORD               rm_Entities[MAX_ROOM_ENTITIES];
};

struct UNPACKED_ROOM
{
  struct ROOM*        ur_Room;
  struct IMAGE*       ur_Backdrops[MAX_ROOM_BACKDROPS];
  struct EXIT* ur_Exits[MAX_ROOM_EXITS];
  UWORD               ur_Id;
  ULONG               ur_Unpacked;
  WORD                ur_CamX;
  WORD                ur_CamY;
};

struct ENTRANCE
{
  UWORD   en_Room;
  UWORD   en_Exit;
};

#define UNPACK_ROOM_ASSET      1
#define UNPACK_ROOM_BACKDROPS  2
#define UNPACK_ROOM_ENTITIES   4

#define UNPACK_ROOM_ALL        (UNPACK_ROOM_ASSET | UNPACK_ROOM_BACKDROPS | UNPACK_ROOM_ENTITIES)

#endif
