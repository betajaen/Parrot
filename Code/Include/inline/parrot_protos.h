#ifndef _VBCCINLINE_PARROT_H
#define _VBCCINLINE_PARROT_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

VOID __Lib_Parrot_Initialise(__reg("a6") struct Library *, __reg("a0") struct PARROT_CONTEXT* ctx)="\tjsr\t-30(a6)";
#define Lib_Parrot_Initialise(ctx) __Lib_Parrot_Initialise(ParrotBase, (ctx))

VOID __Lib_Parrot_Shutdown(__reg("a6") struct Library *, __reg("a0") struct PARROT_CONTEXT* ctx)="\tjsr\t-36(a6)";
#define Lib_Parrot_Shutdown(ctx) __Lib_Parrot_Shutdown(ParrotBase, (ctx))

VOID __Lib_Parrot_Event(__reg("a6") struct Library *, __reg("a0") struct PARROT_CONTEXT* ctx, __reg("d0") ULONG event, __reg("d1") ULONG user)="\tjsr\t-42(a6)";
#define Lib_Parrot_Event(ctx, event, user) __Lib_Parrot_Event(ParrotBase, (ctx), (event), (user))

#endif /*  _VBCCINLINE_PARROT_H  */
