#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>

#include "text.h"

static const ULONG PutChar = 0x16c04e75;
static const ULONG CountChar = 0x52934e75;
extern TEXT u_Text[1024];
static TEXT u_TagStr[5]={0,0,0,0,0};
static struct EasyStruct s_EasyStruct = {
	sizeof(struct EasyStruct),
    0,
    "Parrot",
    NULL,
    NULL
};

STRPTR u_Tag2Str(ULONG tag) {
    ULONG* d = (ULONG*) &u_TagStr;
    *d = tag;
    return (STRPTR) d;
}

LONG u_Requester(CONST_STRPTR options, CONST_STRPTR text) {
	if (u_IsNullOrEmpty(options)) {
		s_EasyStruct.es_GadgetFormat = (BYTE*) "Okay";
	}
	else {
		s_EasyStruct.es_GadgetFormat = (BYTE*) options;
	}
    if (u_IsNullOrEmpty(text)) {
        s_EasyStruct.es_TextFormat = "No Message.";
    }
	else {
		s_EasyStruct.es_TextFormat = (BYTE*) text;
	}
    return EasyRequest(NULL, &s_EasyStruct, NULL);
}

LONG u_RequesterFormat(CONST_STRPTR options, CONST_STRPTR fmt, ...) {
    ULONG textSize;
    TEXT** arg;

    textSize = 0;
    arg = (TEXT**)(&fmt + 1);
    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &CountChar, (STRPTR) &textSize);

    if (textSize >= sizeof(u_Text)) {
        u_Text[0] = '\0';
    }
	else {
    	RawDoFmt((STRPTR) fmt, arg, (void(*)()) &PutChar, (STRPTR) u_Text);
    }

    return u_Requester(options, u_Text);
}

