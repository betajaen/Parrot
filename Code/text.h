#ifndef __PARROT_TEXT__
#define __PARROT_TEXT__

#include <exec/types.h>

BOOL   u_IsNullOrEmpty(CONST_STRPTR text);
STRPTR u_Format(CONST_STRPTR fmt, ...);
STRPTR u_FormatInto(STRPTR text, ULONG textMaxSize, CONST_STRPTR fmt, ...);

#endif

