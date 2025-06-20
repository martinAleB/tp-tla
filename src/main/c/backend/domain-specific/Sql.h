#ifndef __SQL__
#define __SQL__

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"

void initializeSqlModule();
void shutdownSqlModule();

boolean computeSql(Program * program);
boolean computeJson(Json *json);
boolean computeClauseList(ClauseList * clauseList);
boolean computeClause(Clause * clause);

#endif