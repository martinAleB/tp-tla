#ifndef __SCOPE__
#define __SCOPE__

typedef struct ScopeManagerCDT *ScopeManager;

void initializeScopeManager();
ScopeManager currentScopeManager();
void pushScope();
int isMainScope();
int scopeGroupByValidation();
void setScopeGroupByClause();
void setScopeHavingClause();
void popScope();
void freeScopeManager();

#endif