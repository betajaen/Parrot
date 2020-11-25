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

#include <Parrot/Config.h>

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

#define MAKE_LANGUAGE_CODE(a,b)	\
    ((USHORT) (a)<<8 | (USHORT) (b))

/*
    Version
*/

struct VERSION_PARTS_INFO
{
  UBYTE gv_Game;
  UBYTE gv_Feature;
  UBYTE gv_Build;
};

union VERSION_INFO
{
  struct VERSION_PARTS_INFO gv_Version;
  ULONG gv_Num;
};

/*
  Dialogue
*/

#define DIALOGUE_MAGIC 0x00FC

typedef ULONG DIALOGUE;

union DIALOGUE_TEXT
{
  struct DIALOGUE_PARTS
  {
    UWORD   dp_Magic;
    UBYTE   dp_Table;
    UBYTE   dp_Item;
  } dt_Parts;
  DIALOGUE  dt_AssetId;
};

/*
  Squawk Files
*/

#define ASSET_HEADER           \
  UWORD             as_Id;     \
  UWORD             as_Flags;  \
  ULONG             as_Length

struct SQUAWK_ASSET_LIST_HEADER
{
  ULONG             al_Type;
  UWORD             al_Count;
  UWORD             al_Chapter;
  ULONG             al_Length;
};

struct SQUAWK_ASSET_HEADER
{
  ASSET_HEADER;
};

struct ANY_ASSET
{
  ASSET_HEADER;
};

#define GET_ASSET_ID(X) (((struct ANY_ASSET*) (X))->as_Id)
#define GET_ASSET_TYPE(X) (((struct ANY_ASSET*) (X))->as_Type)


/**
    SDL Banned Functions
*/
#define memcpy      __unsafe_memcpy   /* DO NOT USE     */
#define strcat      __unsafe_strcat   /* DO NOT USE     */
#define strcpy      __unsafe_strcpy   /* DO NOT USE     */
#define sprintf     __unsafe_sprintf  /* Use StrFormat  */
#define strlen      __unsafe_strlen   /* Use StrLength  */
#define strtok      __unsafe_strtok   /* DO NOT USE     */

#define literal_strlen(TEXT) (sizeof(TEXT)-1)

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

#define CT_COUNT          8
#define CT_GAME_INFO      MAKE_NODE_ID('G','A','M','E')
#define CT_ROOM           MAKE_NODE_ID('R','O','O','M')
#define CT_IMAGE          MAKE_NODE_ID('I','M','G','E')
#define CT_PALETTE        MAKE_NODE_ID('P','A','L','4')
#define CT_TABLE          MAKE_NODE_ID('T','B','L','E')
#define CT_ENTITY         MAKE_NODE_ID('E','N','T','Y')
#define CT_SCRIPT         MAKE_NODE_ID('S','C','P','T')
#define CT_STRING_TABLE   MAKE_NODE_ID('S','T','R','T')

#define IET_KEYDOWN    1
#define IET_KEYUP      2
#define IET_SELECTDOWN 3
#define IET_SELECTUP   4
#define IET_SELECT     5
#define IET_MENUDOWN   6
#define IET_MENUUP     7
#define IET_MENU       8
#define IET_CURSOR     9

struct INPUTEVENT
{
  UWORD             ie_Type;
  UWORD             ie_Code;
  WORD              ie_CursX;
  WORD              ie_CursY;
};

#define IS_REF_LOADED(REF) (NULL != (REF).ar_Ptr)

#define CHUNK_FLAG_ARCH_ECS       (1 << 0)
#define CHUNK_FLAG_ARCH_AGA       (1 << 1)
#define CHUNK_FLAG_ARCH_RTG       (1 << 2)
#define CHUNK_FLAG_ARCH_ANY       (CHUNK_FLAG_ARCH_ECS | CHUNK_FLAG_ARCH_AGA | CHUNK_FLAG_ARCH_RTG)

#define CHUNK_FLAG_HAS_DATA       (1 << 14)
#define CHUNK_FLAG_IGNORE         (1 << 15)

/*
      Game Info
*/

#define GAME_INFO_FIELDS \
  ULONG                     gi_GameId;\
  ULONG                     gi_GameVersion;\
  DIALOGUE                  gi_Title;\
  DIALOGUE                  gi_ShortTitle;\
  DIALOGUE                  gi_Author;\
  DIALOGUE                  gi_Release;\
  UWORD                     gi_Width;\
  UWORD                     gi_Height;\
  UWORD                     gi_Depth;\
  UWORD                     gi_NumAssetTables;\
  UWORD                     gi_StartPalette;\
  UWORD                     gi_StartCursorPalette;\
  UWORD                     gi_StartScript

struct GAME_INFO
{
  GAME_INFO_FIELDS;
};

struct GAME_INFO_ASSET
{
  ASSET_HEADER;
  GAME_INFO_FIELDS;
};

/*
    Asset Table
*/

struct ASSET_TABLE_ENTRY
{
  UWORD  ti_Id;
  UWORD  ti_Archive;
};

struct ASSET_TABLE
{
  ULONG  at_AssetType;
  UWORD  at_Chapter;
  UWORD  at_Count;
  UWORD  at_Lowest;
  UWORD  at_Highest;
  struct ASSET_TABLE_ENTRY at_Assets[];
};

/*
  
  String Tables
  
*/

struct STRING_TABLE
{
  UWORD   st_Language;
  UWORD   st_Offsets[256];
  CHAR    st_Text[];
};

struct STRING_TABLE_ASSET
{
  ASSET_HEADER;
  struct STRING_TABLE as_Table;
};

#define LANG_ENGLISH  MAKE_LANGUAGE_CODE('e','n')
#define LANG_FRENCH   MAKE_LANGUAGE_CODE('f','r')
#define LANG_ITALIAN  MAKE_LANGUAGE_CODE('i','t')
#define LANG_GERMAN   MAKE_LANGUAGE_CODE('d','e')
#define LANG_SPANISH  MAKE_LANGUAGE_CODE('e','s')

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

struct PALETTE_TABLE_ASSET
{
  ASSET_HEADER;
  struct PALETTE_TABLE as_Palette;
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

struct IMAGE_ASSET
{
  ASSET_HEADER;
  struct IMAGE as_Image;
};

/*

  Verbs

*/

#define VERB_NONE 0
#define VERB_WALK 1

struct VERBS
{
  UWORD  vb_Allowed;
  UWORD  vb_Selected;
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

#define ET_ANY        MAKE_ENTITY_ID('A','N')
#define ET_EXIT       MAKE_ENTITY_ID('E','X')
#define ET_ACTIVATOR  MAKE_ENTITY_ID('A','C')

#define ETF_IS_NAMED  (1 << 0)
#define ETF_IS_LOCKED (1 << 14)
#define ETF_IS_OPEN   (1 << 15)

#define STRUCT_ENTITY_COMMON \
  UWORD                en_Size;\
  UWORD                en_Type;\
  struct RECT          en_HitBox;\
  UWORD                en_Flags;\
  UWORD                en_Context;\
  DIALOGUE             en_Name;\
  UWORD                en_Images[MAX_ENTITY_IMAGES];\
  UWORD                en_Scripts[MAX_ENTITY_SCRIPTS]

struct NEW_ANY_ENTITY
{
  STRUCT_ENTITY_COMMON;
  LONG                 en_Params[];
};

struct NEW_ANY_ENTITY_ASSET
{
  ASSET_HEADER;
  STRUCT_ENTITY_COMMON;
};

struct NEW_EXIT_ENTITY
{
  STRUCT_ENTITY_COMMON;
  LONG                 ex_Target;
};

struct NEW_EXIT_ENTITY_ASSET
{
  ASSET_HEADER;
  STRUCT_ENTITY_COMMON;
  LONG                 ex_Target;
};

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
  UWORD               rm_Scripts[MAX_ROOM_SCRIPTS];
};

struct ROOM_ASSET
{
  ASSET_HEADER;
  struct ROOM as_Room;
};

#define UFLG_DEBUG    1
#define UFLG_SCENE    2
#define UFLG_SCROLL   4
#define UFLG_CAPTION  8
#define UFLG_ALL      (2 | 4 | 8)

struct UNPACKED_ROOM
{
  struct ROOM*        ur_Room;
  struct IMAGE*       ur_Backdrops[MAX_ROOM_BACKDROPS];
  struct EXIT*        ur_Exits[MAX_ROOM_EXITS];
  struct ENTITIES*    ur_Entities[MAX_ROOM_ENTITIES];
  struct SCRIPT*      ur_Scripts[MAX_ROOM_SCRIPTS];
  struct VERBS        ur_Verbs;
  UWORD               ur_Id;
  ULONG               ur_Unpacked;
  WORD                ur_CamX;
  WORD                ur_CamY;
  struct ENTITY*      ur_HoverEntity;
  UWORD               ur_UpdateFlags;
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

/*
      Script
*/

struct SCRIPT
{
  UWORD  sc_Type;
  UWORD  sc_Flags;
  ULONG  sc_Length;
};

#define SCRIPT_TYPE_ROOM   1
#define SCRIPT_TYPE_ENTITY 2
#define SCRIPT_TYPE_ACTOR  3
#define SCRIPT_TYPE_UI     4
#define SCRIPT_TYPE_SCENE  5

struct VIRTUAL_MACHINE
{
  UWORD            vm_State;
  UWORD            vm_PC;
  struct SCRIPT*   vm_Script;
  UWORD            vm_Timer;
  UWORD            vm_StackHead;
  UBYTE*           vm_Instructions;
  UWORD            vm_Stack[MAX_VM_STACK_SIZE];
};

#define VM_STATE_FREE  0
#define VM_STATE_TIMER 1
#define VM_STATE_RUN   2
#define VM_STATE_STOP  3

#endif
