#include <proto/exec.h>

#include "debug.h"

struct PObjectBase {
    struct Node node;
    ULONG tag;
    BYTE data[];
};

VOID* u__AllocObject(ULONG size, ULONG tag) {
    struct PObjectBase* obj = (struct PObjectBase*) AllocVec(size, MEMF_CLEAR);
    obj->tag = (tag ^ (ULONG) obj);
    return (VOID*) obj;
}

VOID  u__FreeObject(VOID* object, ULONG tag) {
    struct PObjectBase* obj = (struct PObjectBase*) object;
    if (tag != (obj->tag ^ (ULONG) obj)) {
        U_ERROR("Object corruption!\nObject will not be deleted.");
        return;
    }
    FreeVec(object);
}
