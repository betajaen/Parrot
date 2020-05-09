#ifndef PARROT_PROTOS_H
#define PARROT_PROTOS_H

#include <exec/types.h>

VOID Lib_Parrot_Initialise(struct PARROT_CONTEXT* parrot);
VOID Lib_Parrot_Shutdown(struct PARROT_CONTEXT* parrot);
VOID Lib_Parrot_Event(struct PARROT_CONTEXT* parrot, ULONG event, ULONG user);

#endif
