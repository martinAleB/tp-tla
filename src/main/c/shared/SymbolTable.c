#include "SymbolTable.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct TNode *TList;

typedef struct
{
    void *elem;
    TList tail;
} TNode;

typedef struct
{
    TList subqueries;
} SymbolTableCDT;

SymbolTable initializeSymbolTable()
{
    return calloc(1, sizeof(SymbolTableCDT));
}

static TList addToList(TList list, void *elem, int (*cmp)(void *, void *), int *added)
{
    int c;
    if (list == NULL || (c = cmp(elem, list->elem)) == 0)
    {
        *added = 0;
        return NULL;
    }
    if (c < 0)
    {
        TList aux = malloc(sizeof(TNode));
        aux->elem = elem;
        aux->tail = list;
        *added = 1;
        return aux;
    }
    if (c == 0)
    {
        *added = 0;
        return list;
    }
    list->tail = addToList(list->tail, elem, cmp, added);
    return list;
}

static int strcmpWrapper(void *e1, void *e2)
{
    return strcmp((char *)e1, (char *)e2);
}

int addSubquery(SymbolTable symbolTable, char *name)
{
    int added;
    symbolTable->subqueries = addToList(symbolTable->subqueries, (void *)name, &added);
    return added;
}

static int existsInList(TList list, void *elem, int (*cmp)(void *, void *))
{
    int c;
    if (list == NULL || (c = cmp(elem, list->elem)) < 0)
        return 0;
    if (c == 1)
        return 1;
    return existsInList(list->tail, elem, cmp);
}

int existsSubquery(SymbolTable symbolTable, char *name)
{
    return existsInList(symbolTable->subqueries, (void *)name, strcmpWrapper);
}

static void freeList(TList list)
{
    if (list != NULL)
    {
        freeList(list->tail);
        free(list);
    }
}

void freeSymbolTable(SymbolTable symbolTable)
{
    freeList(symbolTable->firstSubquery);
    free(symbolTable);
}
