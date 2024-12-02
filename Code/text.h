#ifndef __PARROT_TEXT__
#define __PARROT_TEXT__

#include <exec/types.h>

STRPTR u_Format(CONST_STRPTR fmt, ...);
STRPTR u_FormatInto(STRPTR text, ULONG textMaxSize, CONST_STRPTR fmt, ...);

#endif

