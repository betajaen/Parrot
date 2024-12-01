#ifndef __PARROT_LIST_H__
#define __PARROT_LIST_H__

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>

#define LL_XY(X, Y) X##Y

#define LL_LIST(LL_PTR) \
	do { \
    	struct List* ll_ptr = (struct List*) LL_PTR; \
        ll_ptr->lh_Head = (struct Node*) &ll_ptr->lh_Tail; \
        ll_ptr->lh_Tail = NULL; \
        ll_ptr->lh_TailPred = (struct Node*) &ll_ptr->lh_Head; \
	while(0)

#define LL_CAST(LL_PTR) \
    ((struct List*) LL_PTR)

#define LN_CAST(LN_PTR) \
    ((struct Node*) LN_PTR)

#define LL_HAS_ANY(LL_PTR) \
    (LL_CAST(LL_PTR)->lh_TailPred != LN_CAST(LL_PTR))

#define LL_FOREACH(LL, LN) \
	for(struct Node* LN=((struct List*)LL)->lh_Head,*LL_XY(ll_,__LINE__);(LL_XY(ll_,__LINE__)=LN->ln_Succ)!=NULL;LN=LL_XY(ll_,__LINE__))

#define LL_CONST_FOREACH(LL, LN) \
	for(struct Node* LN=((struct List*)LL)->lh_Head;(LN!=NULL||LN->ln_Succ!=NULL);LN=LN->ln_Succ)

#endif

