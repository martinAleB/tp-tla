#ifndef __SQL__
#define __SQL__

#include "../../frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"

typedef enum ConditionType
{
    WHERE_CONDITION_TYPE,
    HAVING_CONDITION_TYPE,
    UNDEFINED_CONDITION_TYPE,
    FIRST_WHERE_CONDITION_TYPE
} ConditionType;

void initializeSqlModule();
void shutdownSqlModule();

boolean computeSql(Program *program);
boolean computeJson(Json *json);
boolean computeClauseList(ClauseList *clauseList);
boolean computeClause(Clause *clause);
boolean computeWhereCondition(WhereCondition *whereCondition, ConditionType *type);
boolean computeWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition, ConditionType *type);
boolean computeWhereNotCondition(WhereNotCondition *whereNotCondition, ConditionType *type);
boolean computeWhereIsCondition(WhereIsCondition *whereIsCondition, ConditionType *type);
boolean computeWhereInCondition(WhereInCondition *whereInCondition, ConditionType *type);
boolean computeClauseArgsList(ClauseArgsList *clauseArgsList);
boolean computeClauseValue(ClauseValue *clauseValue);
boolean computeAuxiliaryClauseValue(AuxiliaryClauseValue *auxiliaryClauseValue);

#endif
