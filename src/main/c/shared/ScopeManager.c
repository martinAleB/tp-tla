#include "ScopeManager.h"
#include "../utils/List.h"
#include <stdlib.h>

typedef struct Scope
{
    int hasGroupByClause;
    int hasHavingClause;
} Scope;

typedef struct ScopeManagerCDT
{
    TList top;
    size_t size;
} ScopeManagerCDT;

static ScopeManager scopeManager;

void initializeScopeManager()
{
    scopeManager = calloc(1, sizeof(ScopeManagerCDT));
}

ScopeManager currentScopeManager()
{
    return scopeManager;
}

static TList addToStack(TList stack, void *elem)
{
    TList aux = malloc(sizeof(TNode));
    aux->elem = elem;
    aux->tail = stack;
    return aux;
}

void pushScope()
{
    Scope *scope = calloc(1, sizeof(Scope));
    scopeManager->top = addToStack(scopeManager->top, (void *)scope);
    scopeManager->size++;
}

int isMainScope()
{
    return scopeManager->size == 1;
}

int scopeGroupByValidation()
{
    Scope *scope = (Scope *)scopeManager->top->elem;
    return !scope->hasHavingClause || scope->hasGroupByClause;
}

void setScopeGroupByClause()
{
    Scope *scope = (Scope *)scopeManager->top->elem;
    scope->hasGroupByClause = 1;
}

void setScopeHavingClause()
{
    Scope *scope = (Scope *)scopeManager->top->elem;
    scope->hasHavingClause = 1;
}

static void *popFromStack(TList *stack)
{
    void *elem = (*stack)->elem;
    TList aux = *stack;
    *stack = (*stack)->tail;
    free(aux);
    return elem;
}

void popScope()
{
    Scope *scope = popFromStack(&scopeManager->top);
    free(scope);
    scopeManager->size--;
}

void freeScopeManager()
{
    freeList(scopeManager->top, free);
    free(scopeManager);
}
