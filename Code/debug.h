#ifndef __PARROT_DEBUG_H__
#define __PARROT_DEBUG_H__

#include <exec/types.h>

#define U_STR2(X) #X
#define U_STR(X) U_STR2(X)

STRPTR u_Tag2Str(ULONG tag);
LONG u_Requester(CONST_STRPTR options, CONST_STRPTR text);
LONG u_RequesterFormat(CONST_STRPTR options, CONST_STRPTR fmt, ...);

#if defined(PARROT_DEBUG) && PARROT_DEBUG==1
#define U_ERROR(MSG) \
    u_Requester("OK", MSG "\n" __FILE__ ":" U_STR(__LINE__) "")
#define U_ERROR_FORMAT(FMT, ...) \
    u_RequesterFormat("OK", FMT "\n" __FILE__ ":" U_STR(__LINE__) "", __VA_ARGS__)
#else                                                                             
#define U_ERROR(MSG)
#define U_ERROR_FORMAT(FMT, ...)
#endif

#endif

