#include "SymbolTable.h"
#include "Logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../utils/List.h"

typedef struct SymbolTableCDT
{
    TList subqueriesDefinition;
    TList subqueriesUsages;
    int subqueryRedefinition;
} SymbolTableCDT;

typedef struct SubqueryDefinition
{
    char *name;
    void *query;
} SubqueryDefinition;

SymbolTable initializeSymbolTable()
{
    return calloc(1, sizeof(SymbolTableCDT));
}

static TList addToList(TList list, void *elem, int (*cmp)(void *, void *), int *added)
{
    int c;
    if (list == NULL || (c = cmp(elem, list->elem)) < 0)
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

static int subqueryDefinitionCmp(void *def1, void *def2)
{
    return strcmp(((SubqueryDefinition *)def1)->name, ((SubqueryDefinition *)def2)->name);
}

static int strcmpWrapper(void *e1, void *e2)
{
    return strcmp((char *)e1, (char *)e2);
}

int addSubqueryDefinition(SymbolTable symbolTable, char *name, void *query)
{
    int added;
    SubqueryDefinition *definition = malloc(sizeof(SubqueryDefinition));
    definition->name = name;
    definition->query = query;
    symbolTable->subqueriesDefinition = addToList(symbolTable->subqueriesDefinition, (void *)definition, subqueryDefinitionCmp, &added);
    if (!added)
    {
        symbolTable->subqueryRedefinition = 1;
        free(definition);
    }
    return added;
}

void *getSubqueryByName(SymbolTable symbolTable, char *name)
{
    for (TList current = symbolTable->subqueriesDefinition; current != NULL; current = current->tail)
    {
        SubqueryDefinition *definition = current->elem;
        if (!strcmp(definition->name, name))
        {
            return definition->query;
        }
    }
    return NULL;
}

int addSubqueryUsage(SymbolTable symbolTable, char *name)
{
    int added;
    symbolTable->subqueriesUsages = addToList(symbolTable->subqueriesUsages, (void *)name, strcmpWrapper, &added);
    return added;
}

int allUsedSubqueriesDefined(SymbolTable symbolTable)
{
    TList node = symbolTable->subqueriesUsages;
    while (node != NULL)
    {
        char *subquery = node->elem;
        if (!existsSubqueryDefinition(symbolTable, subquery))
            return 0;
        node = node->tail;
    }
    return 1;
}

static int existsInList(TList list, void *elem, int (*cmp)(void *, void *))
{
    int c;
    if (list == NULL || (c = cmp(elem, list->elem)) < 0)
        return 0;
    if (c == 0)
        return 1;
    return existsInList(list->tail, elem, cmp);
}

int existsSubqueryDefinition(SymbolTable symbolTable, char *name)
{
    SubqueryDefinition *def = malloc(sizeof(SubqueryDefinition));
    def->name = name;
    boolean res = existsInList(symbolTable->subqueriesDefinition, (void *)def, subqueryDefinitionCmp);
    free(def);
    return res;
}

int hasSubqueryRedefinition(SymbolTable symbolTable)
{
    return symbolTable->subqueryRedefinition;
}

void freeSymbolTable(SymbolTable symbolTable)
{
    freeList(symbolTable->subqueriesDefinition, free);
    freeList(symbolTable->subqueriesUsages, NULL);
    free(symbolTable);
}
