#ifndef __SYMBOL__TABLE__
#define __SYMBOL__TABLE__

typedef struct SymbolTableCDT *SymbolTable;

SymbolTable initializeSymbolTable();
int addSubqueryDefinition(SymbolTable symbolTable, char *name, void *query);
void *getSubqueryByName(SymbolTable symbolTable, char *name);
int existsSubqueryDefinition(SymbolTable symbolTable, char *name);
int addSubqueryUsage(SymbolTable symbolTable, char *name);
int allUsedSubqueriesDefined(SymbolTable symbolTable);
int hasSubqueryRedefinition(SymbolTable symbolTable);
void freeSymbolTable(SymbolTable symbolTable);

#endif
