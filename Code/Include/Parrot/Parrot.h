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
#include <Parrot/Private/SDI_compiler.h>

#if defined(__M68K__)
#define IS_M68K
#else
#error "Unsupported Arch"
#endif

#include <Parrot/Config.h>

typedef unsigned char  PtUnsigned8;
typedef unsigned short PtUnsigned16;
typedef unsigned int   PtUnsigned32;

typedef char  PtSigned8;
typedef short PtSigned16;
typedef int   PtSigned32;

typedef APTR PtPtr;
typedef char PtChar;
typedef char PtByte;
typedef BOOL PtBool;

#if defined(EXTERN)
#define PtPublic EXTERN
#else
#define PtPublic extern
#endif

#if defined(STATIC)
#define PtPrivate STATIC
#else
#define PtPrivate static
#endif

#if defined(INLINE)
#define PtInline INLINE
#else
#define PtInline inline
#endif

/**
    Undefine Unsafe Functions
*/
#define memcpy    /* DO NOT USE     */
#define strcat    /* DO NOT USE     */
#define strcpy    /* DO NOT USE     */
#define sprintf   /* Use StrFormat  */
#define strlen    /* Use StrLength  */
#define strtok    /* DO NOT USE     */

#define Pt_literal_strlen(TEXT) (sizeof(TEXT)-1)

/**
    Helper Macros
*/

#include <exec/nodes.h>

#define NEW_LIST(list)\
    list.lh_Head          = (struct Node *) &list.lh_Tail;\
    list.lh_Tail          = 0;\
    list.lh_TailPred      = (struct Node*) &list.lh_Head;\

#define NEW_MIN_LIST(list)\
    list.mlh_Head          = (struct MinNode *) &list.mlh_Tail;\
    list.mlh_Tail          = 0;\
    list.mlh_TailPred      = (struct MinNode*) &list.mlh_Head;

#define MAKE_NODE_ID(a,b,c,d)	\
    ((PtUnsigned32) (a)<<24 | (PtUnsigned32) (b)<<16 | (PtUnsigned32) (c)<<8 | (PtUnsigned32) (d))

#define MAKE_ENTITY_ID(a,b)	\
    ((USHORT) (a)<<8 | (USHORT) (b))

#define MAKE_LANGUAGE_CODE(a,b)	\
    ((USHORT) (a)<<8 | (USHORT) (b))

/*
  Dialogue
*/

#define Pt_DialogueMagic 0x00FC

typedef PtUnsigned32 PtDialogue;

#define Pt_DialogueString(STR) ((PtDialogue) STR)

union PtDialogueText_t
{
  struct
  {
    PtUnsigned16   dp_Magic;
    PtUnsigned8   dp_Table;
    PtUnsigned8   dp_Item;
  } dt_Parts;
  PtDialogue  dt_AssetId;
};

/*
  Squawk Files
*/

#define PtAssetHeader        \
  PtUnsigned16      as_Id;    \
  PtUnsigned16      as_Flags; \
  PtUnsigned32      as_Length

typedef struct PtAsset_t PtAsset;

struct PtAsset_t
{
  PtAssetHeader;
};

enum PtCursorType
{
  PT_CT_NONE   =  0,
  PT_CT_SELECT =  1,
  PT_CT_BUSY    = 2
};

#define ARCHIVE_GLOBAL  0
#define ARCHIVE_UNKNOWN 65535

enum PtAssetType
{
  PT_AT_GAME_INFO    = MAKE_NODE_ID('G','A','M','E'),
  PT_AT_ROOM         = MAKE_NODE_ID('R','O','O','M'),
  PT_AT_IMAGE        = MAKE_NODE_ID('I','M','G','E'),
  PT_AT_PALETTE      = MAKE_NODE_ID('P','A','L','4'),
  PT_AT_TABLE        = MAKE_NODE_ID('T','B','L','E'),
  PT_AT_ENTITY       = MAKE_NODE_ID('E','N','T','Y'),
  PT_AT_SCRIPT       = MAKE_NODE_ID('S','C','P','T'),
  PT_AT_STRING_TABLE = MAKE_NODE_ID('S','T','R','T'),
  
  PT_AT_COUNT        = 8
};

enum PtAssetFlags
{
  /* Arch is Amiga ECS */
  PT_AF_ARCH_ECS  = (1 << 0),
  /* Arch is Amiga AGA */
  PT_AF_ARCH_AGA  = (1 << 1),
  /* Arch is Amiga RTG */
  PT_AF_ARCH_RTG  = (1 << 2),
  /* Arch is Any */
  PT_AF_ARCH_ANY  = (PT_AF_ARCH_ECS | PT_AF_ARCH_AGA | PT_AF_ARCH_RTG),

  /* Asset does not have a serialised form; i.e. created via AllocVec */
  PT_AF_INSTANCE  = (1 << 13),

  /* Asset has data with it */
  PT_AF_HAS_DATA   = (1 << 14),

  /* Ignore this asset when loading */
  PT_AF_IGNORE     = (1 << 15),
};

/*
      Game Info
*/

typedef struct PtGameInfo_t PtGameInfo;

struct PtGameInfo_t
{
  PtAssetHeader;
  
  PtUnsigned32  gi_GameId;
  PtUnsigned32  gi_GameVersion;
  PtDialogue    gi_Title;
  PtDialogue    gi_ShortTitle;
  PtDialogue    gi_Author;
  PtDialogue    gi_Release;
  PtUnsigned16  gi_Width;
  PtUnsigned16  gi_Height;
  PtUnsigned16  gi_Depth;
  PtUnsigned16  gi_NumAssetTables;
  PtUnsigned16  gi_StartScript;
};

extern PtGameInfo GameInfo;

/*
    Asset Table
*/

struct AssetTableEntry
{
  PtUnsigned16  ti_Id;
  PtUnsigned16  ti_Archive;
};

struct AssetTable
{
  PtAssetHeader;

  PtUnsigned32  at_AssetType;
  PtUnsigned16  at_Chapter;
  PtUnsigned16  at_Count;
  PtUnsigned16  at_Lowest;
  PtUnsigned16  at_Highest;

  struct AssetTableEntry at_Assets[];
};

/*
  
  String Tables
  
*/

struct StringTable
{
  PtAssetHeader;

  PtUnsigned16   st_Language;
  PtUnsigned16   st_Offsets[256];
  PtChar    st_Text[];
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
struct PaletteTable
{
  PtAssetHeader;

  PtUnsigned8 pt_Begin;
  PtUnsigned8 pt_End;
  PtUnsigned32 pt_Data[64];
};

/*
      Image
*/

struct IMAGE
{
  PtAssetHeader;

  PtUnsigned16             im_BytesPerRow;
  PtUnsigned16             im_Height;
  PtUnsigned8             im_Flags;
  PtUnsigned8             im_Depth;
  PtUnsigned16             im_pad;
  PtUnsigned8*            im_Planes[8];
  PtUnsigned16             im_Width;
  PtUnsigned16             im_Palette;
  PtUnsigned32             im_PlaneSize;
};

#define CAST_IMAGE_TO_BITMAP(IMG) ((struct BitMap*)(&IMG->im_BytesPerRow))


/*

  Verbs

*/

#define VERB_NONE 0
#define VERB_WALK 1

struct Verbs
{
  PtUnsigned16  vb_Allowed;
  PtUnsigned16  vb_Selected;
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

struct Rect
{
  PtSigned16 rt_Left;
  PtSigned16 rt_Top;
  PtSigned16 rt_Right;
  PtSigned16 rt_Bottom;
};

struct Point
{
  PtSigned16 pt_Left;
  PtSigned16 pt_Top;
};

#define ET_ANY        MAKE_ENTITY_ID('A','N')
#define ET_EXIT       MAKE_ENTITY_ID('E','X')
#define ET_ACTIVATOR  MAKE_ENTITY_ID('A','C')

#define ETF_IS_NAMED  (1 << 0)
#define ETF_IS_LOCKED (1 << 14)
#define ETF_IS_OPEN   (1 << 15)

#define STRUCT_ENTITY_COMMON \
  PtAssetHeader;               \
  PtUnsigned16                en_Size;\
  PtUnsigned16                en_Type;\
  struct Rect          en_HitBox;\
  PtUnsigned16                en_Flags;\
  PtUnsigned16                en_Context;\
  PtDialogue             en_Name;\
  PtUnsigned16                en_Images[MAX_ENTITY_IMAGES];\
  PtUnsigned16                en_Scripts[MAX_ENTITY_SCRIPTS]

struct AnyEntity
{
  STRUCT_ENTITY_COMMON;
  PtSigned32                 en_Params[];
};

struct AnyEntityAsset
{
  STRUCT_ENTITY_COMMON;
};

struct RoomExitEntity
{
  STRUCT_ENTITY_COMMON;
  PtSigned32                 ex_Target;
};


struct Entity
{
  PtUnsigned16                en_Type;
  PtUnsigned16                en_Flags;
  struct Rect                 en_HitBox;
  PtUnsigned8                 en_Name[MAX_ENTITY_NAME_LENGTH + 1];
};

struct RoomExit
{
  PtUnsigned16                ex_Type;
  PtUnsigned16                ex_Flags;
  struct Rect                 ex_HitBox;
  PtUnsigned8                 ex_Name[MAX_ENTITY_NAME_LENGTH + 1];
  PtUnsigned16                ex_Target;
};

struct Room
{
  PtAssetHeader;

  PtUnsigned16               rm_Width;
  PtUnsigned16               rm_Height;
  PtUnsigned16               rm_Backdrops[MAX_ROOM_BACKDROPS];
  PtUnsigned16               rm_Exits[MAX_ROOM_EXITS];
  PtUnsigned16               rm_Entities[MAX_ROOM_ENTITIES];
  PtUnsigned16               rm_Scripts[MAX_ROOM_SCRIPTS];
};

#define UFLG_DEBUG     1
#define UFLG_SCENE     2
#define UFLG_SCROLL    4
#define UFLG_CAPTION   8
#define UFLG_MOUSEANIM 16
#define UFLG_ALL       (2 | 4 | 8)

struct UNPACKED_ROOM
{
  struct Room         lv_Room;

  struct IMAGE        lv_Backdrops[MAX_ROOM_BACKDROPS];
  struct RoomExit*    lv_Exits[MAX_ROOM_EXITS];
  struct Entity*      lv_Entities[MAX_ROOM_ENTITIES];
  struct SCRIPT*      lv_Scripts[MAX_ROOM_SCRIPTS];
  struct Verbs        lv_Verbs;

  PtUnsigned32               lv_InitialisationFlags;
  PtSigned16                lv_CamX;
  PtSigned16                lv_CamY;
  struct Entity*      lv_HoverEntity;
  PtUnsigned16               lv_UpdateFlags;
};

struct RoomEntrance
{
  PtUnsigned16   en_Room;
  PtUnsigned16   en_Exit;
};

enum RoomInitialisationFlags
{
  RIF_ASSET = 1,
  RIF_BACKDROPS = 2,
  RIF_ENTITIES = 4,

  RIF_ALL = (RIF_ASSET | RIF_BACKDROPS | RIF_ENTITIES)
};

/*
      Script
*/

typedef PtUnsigned16 OPCODE;

struct SCRIPT
{
  PtAssetHeader;
  
  PtUnsigned16  sc_NumOpcodes;
  PtSigned32   sc_Constants[MAX_CONSTANTS_PER_SCRIPT];
  OPCODE sc_Opcodes[];
};

extern PtSigned32 Vm_Globals[MAX_VM_GLOBALS];

struct VIRTUAL_MACHINE
{
  PtUnsigned16            vm_State;
  PtSigned16             vm_PC;
  PtUnsigned16            vm_Timer;
  PtUnsigned16            vm_StackHead;
  PtUnsigned16            vm_Cmp;
  PtUnsigned16            vm_Reserved;
  PtDialogue         vm_DialogueRegister;
  OPCODE*          vm_Opcodes;
  PtUnsigned16            vm_OpcodesLength;
  PtSigned32*            vm_Constants;
  struct SCRIPT*   vm_Script;
  PtSigned32             vm_Vars[MAX_VM_VARIABLES];
  PtSigned32             vm_Stack[MAX_VM_STACK_SIZE];
};

extern struct VIRTUAL_MACHINE* Vm_Current;

#define VM_STATE_END   0
#define VM_STATE_TIMER 1
#define VM_STATE_RUN   2
#define VM_STATE_STOPPING 3

#define VM_CMP_NONE 0
#define VM_CMP_EQUALS 1
#define VM_CMP_GT 2
#define VM_CMP_LT 4

#define SYSCALL_EXIT 0
#define SYSCALL_TRACE 1
#define SYSCALL_STOP_SCRIPT 2
#define SYSCALL_LOAD_GAME 3
#define SYSCALL_SAVE_GAME 4
#define SYSCALL_DELAY_TICKS 5
#define SYSCALL_DELAY_SECONDS 6


enum UiElementType
{
  UET_NONE           = 0,
  UET_TEXT           = 1,
  UET_BUTTON         = 2,
  UET_VERB           = 3,
  UET_SENTENCE       = 4,
  UET_INVENTORY_ITEM = 5,
};

enum UiFunction
{
  UF_None                = 0,
  UF_RunScript           = 1,
  UF_ReleaseThisPanel    = 2,
  UF_ReleasePanel        = 3,
};

struct UiElement
{
  PtUnsigned16    ue_Id;
  PtUnsigned16    ue_Type;
  PtUnsigned16    ue_X;
  PtUnsigned16    ue_Y;
  PtUnsigned16    ue_Width;
  PtUnsigned16    ue_Height;
  PtDialogue ue_Text;
  PtUnsigned16    ue_Function[2];
  PtUnsigned16    ue_Argument[2];
  PtUnsigned16    ue_Status;
};

struct UiPanel
{
  PtAssetHeader;

  PtUnsigned16                   ui_Screen;
  PtUnsigned16                   ui_NumElements;
  struct UiElement        ui_Elements[];
};

#endif
