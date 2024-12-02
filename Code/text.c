#include <proto/exec.h>

static const ULONG PutChar = 0x16c04e75;
static const ULONG CountChar = 0x52934e75;
TEXT u_Text[1024] = { 0 };

BOOL u_IsNullOrEmpty(CONST_STRPTR text) {
    return text == NULL || text[0] == '\0';
}

STRPTR u_Format(CONST_STRPTR fmt, ...) {
    ULONG textSize;
    TEXT** arg;

    textSize = 0;
    arg = (TEXT**)(&fmt + 1);
    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &CountChar, (STRPTR) &textSize);

    if (textSize >= sizeof(u_Text)) {
        u_Text[0] = '\0';
        return u_Text;
    }

    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &PutChar, (STRPTR) u_Text);

    return u_Text;
}

STRPTR u_FormatInto(STRPTR text, ULONG textMaxSize, CONST_STRPTR fmt, ...) {
    ULONG textSize;
    TEXT** arg;

    if (text == NULL || textMaxSize == 0) {
		return NULL;
    }

    textSize = 0;
    arg = (TEXT**)(&fmt + 1);
    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &CountChar, (STRPTR) &textSize);

    if (textSize >= textMaxSize) {
        text[0] = '\0';
        return text;
    }

    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &PutChar, (STRPTR) text);

    return text;
}
