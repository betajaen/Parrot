#include <proto/exec.h>

static const ULONG PutChar = 0x16c04e75;
static const ULONG CountChar = 0x52934e75;
static TEXT s_Text[1024] = { 0 };

STRPTR u_Format(CONST_STRPTR fmt, ...) {
    ULONG textSize;
    TEXT** arg;

    textSize = 0;
    arg = (TEXT**)(&fmt + 1);
    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &CountChar, (STRPTR) &textSize);

    if (textSize >= sizeof(s_Text)) {
        s_Text[0] = '\0';
        return s_Text;
    }

    RawDoFmt((STRPTR) fmt, arg, (void(*)()) &PutChar, (STRPTR) s_Text);

    return s_Text;
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
