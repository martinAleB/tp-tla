#include "Sql.h"
#include "../../shared/ScopeManager.h"

static Logger *_logger = NULL;

void initializeSqlModule()
{
    _logger = createLogger("Sql");
}

void shutdownSqlModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }
}

boolean validateSql(Program *program)
{
    return validateJson(program->json);
}

boolean validateJson(Json *json)
{
    pushScope();
    boolean res = validateClauseList(json->clauseList);
    if (res)
    {
        if (!scopeGroupByValidation())
        {
            logError(_logger, "HAVING statement without GROUP BY");
            res = false;
        }
    }
    popScope();
    return res;
}

boolean validateClauseList(ClauseList *clauseList)
{
    int clauseCounter[CLAUSE_TYPE_COUNT] = {0};
    ClauseList *current = clauseList;
    while (current != NULL)
    {
        Clause *clause = current->clause;
        clauseCounter[clause->type]++;
        if ((clause->type != JOIN_CLAUSE && clauseCounter[clause->type] > 1))
        {
            logError(_logger, "Repeated statements detected");
            return false;
        }
        if (!validateClause(clause))
        {
            return false;
        }
        current = current->next;
    }
    if (clauseCounter[FROM_CLAUSE] == 0)
    {
        logError(_logger, "Missing 'from' statement");
    }
    return clauseCounter[FROM_CLAUSE] == 1;
}

boolean validateClause(Clause *clause)
{
    if (clause->type == WHERE_CLAUSE)
    {
        return validateWhereCondition(clause->whereCondition);
    }
    return validateClauseArgsList(clause->clauseArgsList);
}

boolean validateWhereCondition(WhereCondition *whereCondition)
{
    WhereCondition *current = whereCondition;
    while (current != NULL)
    {
        switch (current->nodeType)
        {
        case NODE_TYPE_WHERE_IN_CONDITION:
            if (!validateWhereInCondition(current->whereInCondition))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_NOT_CONDITION:
            if (!validateWhereNotCondition(current->whereNotCondition))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_BINARY_CONDITION:
            if (!validateWhereBinaryCondition(current->binaryCondition))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_CONDITION:
            if (!validateWhereCondition(current->whereCondition))
            {
                return false;
            }
            break;
        }
        current = current->next;
    }
    return true;
}

boolean validateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition)
{
    if (whereBinaryCondition->value1->type == CONDITION_VALUE_NULL || whereBinaryCondition->value2->type == CONDITION_VALUE_NULL)
    {
        if (whereBinaryCondition->operator != BINARY_CONDITION_OPERATOR_EQUAL && whereBinaryCondition->operator != BINARY_CONDITION_OPERATOR_NOT_EQUAL)
        {
            logError(_logger, "Cannot compare NULL value with >, <, >= or <=");
            return false;
        }
    }
    if (whereBinaryCondition->value1->type == CONDITION_VALUE_AGGREGATION_FUNCTION || whereBinaryCondition->value2->type == CONDITION_VALUE_AGGREGATION_FUNCTION)
    {
        setScopeHavingClause();
    }
    return true;
}

boolean validateWhereNotCondition(WhereNotCondition *whereNotCondition)
{
    switch (whereNotCondition->nodeSelected)
    {
    case NOT_NODE:
        return validateWhereNotCondition(whereNotCondition->not);
    case IN_NODE:
        return validateWhereInCondition(whereNotCondition->in);
    case CONDITION_NODE:
        return validateWhereCondition(whereNotCondition->condition);
    case BINARY_CONDITION_NODE:
        return validateWhereBinaryCondition(whereNotCondition->whereBinaryCondition);
    }
    return true;
}

boolean validateWhereInCondition(WhereInCondition *whereInCondition)
{
    if (whereInCondition->type == WHERE_IN_CONDITION_QUERY)
    {
        return validateJson(whereInCondition->query);
    }
    return true;
}

boolean validateClauseArgsList(ClauseArgsList *clauseArgsList)
{
    ClauseArgsList *current = clauseArgsList;
    while (current != NULL)
    {
        if (!validateClauseValue(clauseArgsList->clauseValue))
        {
            return false;
        }
        current = current->next;
    }
    return true;
}

boolean validateClauseValue(ClauseValue *clauseValue)
{
    switch (clauseValue->clauseType)
    {
    case AUXILIARY_CLAUSE:
        return validateAuxiliaryClauseValue(clauseValue->auxiliaryClauseValue);
    case GROUP_BY_CLAUSE:
        return validateGroupByClauseValue(clauseValue->groupByClauseValue);
    case JOIN_CLAUSE:
        return validateJoinClauseValue(clauseValue->joinClauseValue);
    }
    return true;
}

boolean validateAuxiliaryClauseValue(AuxiliaryClauseValue *auxiliaryClauseValue)
{
    if (!isMainScope())
    {
        logError(_logger, "Cannot declare AUXILIARY statement in an auxiliary query");
        return false;
    }
    return validateJson(auxiliaryClauseValue->auxQuery);
}

boolean validateGroupByClauseValue(GroupByClauseValue *GroupByClauseValue)
{
    setScopeGroupByClause();
    return true;
}

boolean validateJoinClauseValue(JoinClauseValue *joinClauseValue)
{
    boolean res = !joinClauseValue->outer || joinClauseValue->joinType != INNER_JOIN;
    if (!res)
    {
        logError(_logger, "Cannot use OUTER in an INNER JOIN clause");
    }
    return res;
}
