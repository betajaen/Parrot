#ifndef __PARROT_DEBUG_H__
#define __PARROT_DEBUG_H__

#include <exec/types.h>

LONG u_Requester(CONST_STRPTR options, CONST_STRPTR text);
LONG u_RequesterFormat(CONST_STRPTR options, CONST_STRPTR fmt, ...);

#endif

