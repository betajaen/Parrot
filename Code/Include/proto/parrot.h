#ifndef PROTO_PARROT_H
#define PROTO_PARROT_H

extern struct Library* ParrotBase;

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

ULONG IMPORT_ScreenNew(__reg("a6") struct ParrotBase*, __reg("a0") struct SCREEN_INFO* info)="\tjsr\t-60(a6)";
#define ScreenNew(info) IMPORT_ScreenNew(ParrotBase, info)

VOID IMPORT_ScreenDelete(__reg("a6") struct ParrotBase*, __reg("d0") ULONG screen)="\tjsr\t-66(a6)";
#define ScreenDelete(screen) IMPORT_ScreenDelete(ParrotBase, screen)

#endif /* PROTO_PARROT_H */
