#include <proto/exec.h>

#include "debug.h"

struct PObjectBase {
    struct Node node;
    ULONG size;
    ULONG tag;
    BYTE data[];
};

VOID* u__AllocObject(ULONG size, ULONG tag) {

    ULONG allocSize;

    if (size < sizeof(struct PObjectBase)) {
        U_ERROR_FORMAT("Object size for %s is to small!", u_Tag2Str(tag));
        return NULL;
    }

    struct PObjectBase* obj = (struct PObjectBase*) AllocVec(size, MEMF_CLEAR);
    obj->size = size;
    obj->tag = (tag ^ (ULONG) obj);
    return (VOID*) obj;
}

VOID  u__FreeObject(VOID* object, ULONG tag) {
    if (object != NULL) {
        struct PObjectBase* obj = (struct PObjectBase*) object;
        if (tag != (obj->tag ^ (ULONG) obj)) {
            U_ERROR_FORMAT("Object corruption for %s!\nObject will not be deleted.", u_Tag2Str(tag));
            return;
        }
        FreeVec(object);
    }
}
