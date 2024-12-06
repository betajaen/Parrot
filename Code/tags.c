#include "tags.h"
#include "debug.h"

#define TAG_SIZE 64

static struct TagItem s_tags[TAG_SIZE];
static ULONG s_tag_idx = 0UL;

VOID u_clear_tags() {
    for(ULONG i=0;i < TAG_SIZE;i++) {
		s_tags[s_tag_idx].ti_Tag = TAG_DONE;
		s_tags[s_tag_idx].ti_Data = 0UL;
    }
    s_tag_idx = 0UL;
}

struct TagItem* u_start_tags() {
    return &s_tags[s_tag_idx];
}

VOID u_push_tagu(Tag tag, ULONG data) {
    if (s_tag_idx >= TAG_SIZE) {
        U_ERROR("Out of tag space!");
        return;
    }
	s_tags[s_tag_idx].ti_Tag = tag;
	s_tags[s_tag_idx].ti_Data = data;
	s_tag_idx++;
}

VOID u_end_tags() {
    u_push_tagu(TAG_DONE, 0UL);
}
