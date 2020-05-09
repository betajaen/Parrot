#ifndef _VBCCINLINE_PARROT_H
#define _VBCCINLINE_PARROT_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

VOID __GameInitialise(__reg("a6") struct Library *, __reg("a0") struct ParrotBase* parrotBase)="\tjsr\t-30(a6)";
#define GameInitialise(parrotBase) __GameInitialise(GameBase, parrotBase)

VOID __GameShutdown(__reg("a6") struct Library *)="\tjsr\t-36(a6)";
#define GameShutdown() __GameShutdown(GameBase)

VOID __OnGameEvent(__reg("a6") struct Library *, __reg("d0") ULONG event, __reg("d1") ULONG user)="\tjsr\t-42(a6)";
#define GameEvent(event, user) __OnGameEvent(GameBase, (event), (user))

#endif /*  _VBCCINLINE_PARROT_H  */
