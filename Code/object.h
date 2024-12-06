#ifndef __PARROT_OBJECT_H__
#define __PARROT_OBJECT_H__

#include <exec/types.h>

#define TAG(A,B,C,D) ((BYTE) A << 24 | (BYTE) B << 16 | (BYTE) C << 8 | (BYTE) D)

VOID* u__AllocObject(ULONG size, ULONG tag);
VOID  u__FreeObject(VOID* object, ULONG tag);

#define U_ALLOC_OBJECT(T, TAG) \
	u__AllocObject(sizeof(T), TAG)

#define U_FREE_OBJECT(OBJ,TAG) \
	do { \
		u__FreeObject((VOID*)OBJ, TAG); \
		OBJ = NULL; \
	} while(0)

#endif

