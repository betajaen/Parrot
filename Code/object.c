#include <proto/exec.h>

#include "debug.h"

#define P_FREED 0xDEADEAD

typedef struct _PObject {
    ULONG size;
    ULONG tag;
} PObject;

typedef struct _PObjectFooter {
    ULONG tag;
} PObjectFooter;

static PObject* u__GetAndCheckPObject(void* mem, ULONG tag) {

    PObject* object;
    PObjectFooter* footer;

    if (mem == NULL) {
        return NULL;
    }

    object = ((PObject*) mem);
    object--;

    if ((object->tag ^ (ULONG) object) == P_FREED) {
		U_ERROR_FORMAT("Double free found for %s at %lx", u_Tag2Str(tag), (ULONG) object);
        return NULL;
    }

    if ((object->tag ^ (ULONG) object) != tag) {
		U_ERROR_FORMAT("Buffer underflow for %s at %lx", u_Tag2Str(tag), (ULONG) object);
        return NULL;
    }

    footer = (PObjectFooter*) ((((BYTE*) mem) + sizeof(PObject) + object->size));

    if (footer->tag != object->tag) {
        U_ERROR_FORMAT("Buffer overflow for %s at %lx", u_Tag2Str(tag), (ULONG) object);
        return NULL;
    }

    return object;
}

VOID* u__AllocObject(ULONG size, ULONG tag) {

    ULONG allocSize;
    PObject *obj;
    PObjectFooter *footer;

    if (size <= sizeof(ULONG)) {
        // Prevent U_ALLOC_OBJECT(sizeof(struct T), XYZW) usage.
        U_ERROR("Object size too small!");
        //U_ERROR_FORMAT("Object size for %s is to small! Setting to 256" , u_Tag2Str(tag));
        size = 256; // Prevent a crash.
    }

    allocSize = sizeof(PObject) + sizeof(PObjectFooter) + size;

    obj = (PObject*) AllocVec(allocSize, MEMF_CLEAR);
    obj->size = size;
    obj->tag = (tag ^ (ULONG) obj);

    footer = (PObjectFooter*) ((((BYTE*) obj) + sizeof(PObject) + size));
    footer->tag = obj->tag;

    return (VOID*) (obj+1);
}

VOID  u__FreeObject(VOID* object, ULONG tag) {

    PObject *obj;

    if (object != NULL) {
        return;
    }

    obj = u__GetAndCheckPObject(object, tag);

    if (obj == NULL) {
        return;
    }

    FreeVec(obj);
}
