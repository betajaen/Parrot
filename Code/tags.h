#ifndef __PARROT_TAGS__
#define __PARROT_TAGS__

#include <exec/types.h>
#include <proto/utility.h>

VOID u_clear_tags();
struct TagItem* u_start_tags();
VOID u_push_tagu(Tag tag, ULONG data);

#define u_push_tags(TAG,DATA) u_push_tagu(TAG,(ULONG)DATA)
#define u_push_tagl(TAG,DATA) u_push_tagu(TAG,(ULONG)DATA)
#define u_push_tagp(TAG,DATA) u_push_tagu(TAG,(ULONG)DATA)

VOID u_end_tags();


#endif
