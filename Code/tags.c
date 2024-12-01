#include "tags.h"
#define TAG_SIZE 64

static struct TagItem s_tags[TAG_SIZE];
static ULONG s_tag_idx = 0;

VOID u_clear_tags() {
	const ULONG size2 = (TAG_SIZE << 1);
	ULONG* t;

	t = (ULONG*) s_tags;
	for(ULONG i=0;i < size2;i++) {
		*t++ = 0UL;
	}
	s_tag_idx = 0;
}

struct TagItem* u_start_tags() {
    return s_tags + s_tag_idx;
}

VOID u_push_tagu(Tag tag, ULONG data) {
	s_tags[s_tag_idx].ti_Tag = tag;
	s_tags[s_tag_idx].ti_Data = data;
	s_tag_idx++;
}

VOID u_end_tags() {
    u_push_tagu(TAG_DONE, 0UL);
}
