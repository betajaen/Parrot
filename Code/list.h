#ifndef __PARROT_LIST_H__
#define __PARROT_LIST_H__

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>

#define LL_XY(X, Y) X##Y

#define LL_NULL_LIST { NULL, NULL, NULL, 0, 0 }

#define LL_INIT_LIST(LL_PTR) \
	do { \
    	struct List* ll_0 = (struct List*) (LL_PTR); \
        ll_0->lh_Head = (struct Node*) &ll_0->lh_Tail; \
        ll_0->lh_Tail = NULL; \
        ll_0->lh_TailPred = (struct Node*) &ll_0->lh_Head; \
	} while(0)

#define LL_CAST(LL_PTR) \
    ((struct List*) LL_PTR)

#define LN_CAST(LN_PTR) \
    ((struct Node*) LN_PTR)

#define LL_HAS_ANY(LL_PTR) \
    (LL_CAST(LL_PTR)->lh_TailPred != LN_CAST(LL_PTR))

#define LL_IS_NULL(LL_PTR) \
    ((LL_PTR)->lh_Head == NULL)

#define LL_FOREACH(LL, LN) \
	for(struct Node* LN=((struct List*)LL)->lh_Head,*LL_XY(ll_,__LINE__);(LL_XY(ll_,__LINE__)=LN->ln_Succ)!=NULL;LN=LL_XY(ll_,__LINE__))

#define LL_CONST_FOREACH(LL, LN) \
	for(struct Node* LN=((struct List*)LL)->lh_Head;(LN!=NULL||LN->ln_Succ!=NULL);LN=LN->ln_Succ)

#endif

