#ifndef __SYMBOL__TABLE__
#define __SYMBOL__TABLE__

typedef struct SymbolTableCDT *SymbolTable;

SymbolTable initializeSymbolTable();
int addSubquery(SymbolTable symbolTable, char *name);
int existsSubquery(SymbolTable symbolTable, char *name);
void freeSymbolTable(SymbolTable symbolTable);

#endif
