#ifndef PROTO_PARROT_H
#define PROTO_PARROT_H

extern struct ParrotBase* ParrotBase;

VOID IMPORT_GameStart(__reg("a6") struct ParrotBase*, __reg("a0") CONST_STRPTR name)="\tjsr\t-30(a6)";
#define GameStart(name) IMPORT_GameStart(ParrotBase, name)

LONG IMPORT_TestRequester(__reg("a6") struct ParrotBase*, __reg("d0") LONG x)="\tjsr\t-36(a6)";
#define TestRequester(x) IMPORT_TestRequester(ParrotBase, x)

APTR IMPORT_MemNew(__reg("a6") struct ParrotBase*, __reg("d0") ULONG size, __reg("d1") ULONG requirements)="\tjsr\t-42(a6)";
#define MemNew(size, requirements) IMPORT_MemNew(ParrotBase, size, requirements)

BOOL IMPORT_MemDelete(__reg("a6") struct ParrotBase*, __reg("a0") APTR pMem)="\tjsr\t-48(a6)";
#define MemDelete(pMem) IMPORT_MemDelete(ParrotBase, pMem)

ULONG IMPORT_MemSize(__reg("a6") struct ParrotBase*, __reg("a0") APTR pMem)="\tjsr\t-54(a6)";
#define MemSize(pMem) IMPORT_MemSize(ParrotBase, pMem)

APTR IMPORT_ScreenNew(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena, __reg("a1") struct SCREEN_INFO* info)="\tjsr\t-60(a6)";
#define ScreenNew(arena, info) IMPORT_ScreenNew(ParrotBase, arena, info)

VOID IMPORT_ScreenDelete(__reg("a6") struct ParrotBase*, __reg("a0") APTR obj)="\tjsr\t-66(a6)";
#define ScreenDelete(obj) IMPORT_ScreenDelete(ParrotBase, obj)

VOID IMPORT_ScreenSetCursor(__reg("a6") struct ParrotBase*, __reg("a0") APTR screen, __reg("d0") UBYTE cursor)="\tjsr\t-72(a6)";
#define ScreenSetCursor(screen, cursor) IMPORT_ScreenSetCursor(ParrotBase, screen, cursor)

UBYTE IMPORT_ScreenGetCursor(__reg("a6") struct ParrotBase*, __reg("a0") APTR screen)="\tjsr\t-78(a6)";
#define ScreenGetCursor(screen) IMPORT_ScreenGetCursor(ParrotBase, screen)

VOID IMPORT_ScreenSetColour(__reg("a6") struct ParrotBase*, __reg("a0") APTR screen, __reg("d0") UWORD index, __reg("d1") UBYTE r, __reg("d2") UBYTE g, __reg("d3") UBYTE b)="\tjsr\t-84(a6)";
#define ScreenSetColour(screen, index, r, g, b) IMPORT_ScreenSetColour(ParrotBase, screen, index, r, g, b)

VOID IMPORT_GameDelaySeconds(__reg("a6") struct ParrotBase*, __reg("d0") UWORD seconds)="\tjsr\t-90(a6)";
#define GameDelaySeconds(seconds) IMPORT_GameDelaySeconds(ParrotBase, seconds)

VOID IMPORT_GameDelayTicks(__reg("a6") struct ParrotBase*, __reg("d0") UWORD ticks)="\tjsr\t-96(a6)";
#define GameDelayTicks(ticks) IMPORT_GameDelayTicks(ParrotBase, ticks)

APTR IMPORT_ArenaNew(__reg("a6") struct ParrotBase*, __reg("d0") ULONG size, __reg("d1") ULONG requirements)="\tjsr\t-102(a6)";
#define ArenaNew(size, requirements) IMPORT_ArenaNew(ParrotBase, size, requirements)

BOOL IMPORT_ArenaDelete(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)="\tjsr\t-108(a6)";
#define ArenaDelete(arena) IMPORT_ArenaDelete(ParrotBase, arena)

ULONG IMPORT_ArenaSize(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)="\tjsr\t-114(a6)";
#define ArenaSize(arena) IMPORT_ArenaSize(ParrotBase, arena)

APTR IMPORT_ObjAlloc(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena, __reg("d0") ULONG size, __reg("d1") ULONG class)="\tjsr\t-120(a6)";
#define ObjAlloc(arena, size, class) IMPORT_ObjAlloc(ParrotBase, arena, size, class)

ULONG IMPORT_ObjGetClass(__reg("a6") struct ParrotBase*, __reg("a0") APTR alloc)="\tjsr\t-126(a6)";
#define ObjGetClass(alloc) IMPORT_ObjGetClass(ParrotBase, alloc)

ULONG IMPORT_ObjGetSize(__reg("a6") struct ParrotBase*, __reg("a0") APTR alloc)="\tjsr\t-132(a6)";
#define ObjGetSize(alloc) IMPORT_ObjGetSize(ParrotBase, alloc)

BOOL IMPORT_ArenaRollback(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)="\tjsr\t-138(a6)";
#define ArenaRollback(arena) IMPORT_ArenaRollback(ParrotBase, arena)

ULONG IMPORT_ArenaSpace(__reg("a6") struct ParrotBase*, __reg("a0") APTR arena)="\tjsr\t-144(a6)";
#define ArenaSpace(arena) IMPORT_ArenaSpace(ParrotBase, arena)

VOID IMPORT_ScreenClear(__reg("a6") struct ParrotBase*, __reg("a0") APTR obj)="\tjsr\t-150(a6)";
#define ScreenClear(obj) IMPORT_ScreenClear(ParrotBase, obj)

VOID IMPORT_ScreenSwapBuffers(__reg("a6") struct ParrotBase*, __reg("a0") APTR obj)="\tjsr\t-156(a6)";
#define ScreenSwapBuffers(obj) IMPORT_ScreenSwapBuffers(ParrotBase, obj)

#endif /* PROTO_PARROT_H */
