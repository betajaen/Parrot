#ifndef _VBCCINLINE_PARROT_H
#define _VBCCINLINE_PARROT_H

#ifndef EXEC_TYPES_H
#include <exec/types.h>
#endif

VOID __TestRequester(__reg("a6") struct Library*, __reg("d0") ULONG x) = "\tjsr\t-30(a6)";
#define TestRequester(x) __TestRequester(ParrotBase, x)


#endif /*  _VBCCINLINE_PARROT_H  */
