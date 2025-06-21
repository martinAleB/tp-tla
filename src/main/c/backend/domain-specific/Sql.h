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

boolean validateSql(Program *program);
boolean validateJson(Json *json);
boolean validateClauseList(ClauseList *clauseList);
boolean validateClause(Clause *clause);
boolean validateWhereCondition(WhereCondition *whereCondition, ConditionType *type);
boolean validateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition, ConditionType *type);
boolean validateWhereNotCondition(WhereNotCondition *whereNotCondition, ConditionType *type);
boolean validateWhereIsCondition(WhereIsCondition *whereIsCondition, ConditionType *type);
boolean validateWhereInCondition(WhereInCondition *whereInCondition, ConditionType *type);
boolean validateClauseArgsList(ClauseArgsList *clauseArgsList);
boolean validateClauseValue(ClauseValue *clauseValue);
boolean validateAuxiliaryClauseValue(AuxiliaryClauseValue *auxiliaryClauseValue);
boolean validateGroupByClauseValue(GroupByClauseValue *GroupByClauseValue);

#endif
