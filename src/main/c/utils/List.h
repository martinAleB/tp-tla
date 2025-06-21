#ifndef __LIST__
#define __LIST__

typedef struct TNode *TList;

typedef struct TNode
{
    void *elem;
    TList tail;
} TNode;

void freeList(TList list);

#endif