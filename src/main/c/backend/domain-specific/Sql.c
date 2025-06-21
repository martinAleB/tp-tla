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
            res = 0;
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
        if ((clause->type != JOIN_CLAUSE && clauseCounter[clause->type] > 1) || !validateClause(clause))
        {
            logError(_logger, "Repeated statements detected");
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
        ConditionType type = FIRST_WHERE_CONDITION_TYPE;
        return validateWhereCondition(clause->whereCondition, &type);
    }
    return validateClauseArgsList(clause->clauseArgsList);
}

boolean validateWhereCondition(WhereCondition *whereCondition, ConditionType *type)
{
    WhereCondition *current = whereCondition;
    while (current != NULL)
    {
        ConditionType defType;
        if (*type == FIRST_WHERE_CONDITION_TYPE)
        {
            defType = UNDEFINED_CONDITION_TYPE;
        }
        else
        {
            defType = *type;
        }
        switch (current->nodeType)
        {
        case NODE_TYPE_WHERE_IN_CONDITION:
            if (!validateWhereInCondition(current->whereInCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_IS_CONDITION:
            if (!validateWhereIsCondition(current->whereIsCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_NOT_CONDITION:
            if (!validateWhereNotCondition(current->whereNotCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_BINARY_CONDITION:
            if (!validateWhereBinaryCondition(current->binaryCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_CONDITION:
            if (!validateWhereCondition(current->whereCondition, &defType))
            {
                return false;
            }
            break;
        }
        current = current->next;
        if (*type != FIRST_WHERE_CONDITION_TYPE)
        {
            *type = defType;
        }
    }
    return true;
}

boolean validateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition, ConditionType *type)
{
    if (*type == UNDEFINED_CONDITION_TYPE)
    {
        *type = (whereBinaryCondition->value1->type == CONDITION_VALUE_AGGREGATION_FUNCTION || whereBinaryCondition->value2->type == CONDITION_VALUE_AGGREGATION_FUNCTION) ? HAVING_CONDITION_TYPE : WHERE_CONDITION_TYPE;
        if (*type == HAVING_CONDITION_TYPE)
        {
            setScopeHavingClause();
        }
    }
    if (*type == WHERE_CONDITION_TYPE && (whereBinaryCondition->value1->type == CONDITION_VALUE_AGGREGATION_FUNCTION || whereBinaryCondition->value2->type == CONDITION_VALUE_AGGREGATION_FUNCTION))
    {
        logError(_logger, "Detected where and having conditions mixed");
        return false;
    }
    if (*type == HAVING_CONDITION_TYPE && (whereBinaryCondition->value1->type != CONDITION_VALUE_AGGREGATION_FUNCTION && whereBinaryCondition->value2->type != CONDITION_VALUE_AGGREGATION_FUNCTION))
    {
        logError(_logger, "Detected where and having conditions mixed");
        return false;
    }
    return true;
}

boolean validateWhereNotCondition(WhereNotCondition *whereNotCondition, ConditionType *type)
{
    switch (whereNotCondition->nodeSelected)
    {
    case NOT_NODE:
        return validateWhereNotCondition(whereNotCondition->not, type);
    case IN_NODE:
        return validateWhereInCondition(whereNotCondition->in, type);
    case CONDITION_NODE:
        return validateWhereCondition(whereNotCondition->condition, type);
    case BINARY_CONDITION_NODE:
        return validateWhereBinaryCondition(whereNotCondition->whereBinaryCondition, type);
    }
    return true;
}

boolean validateWhereIsCondition(WhereIsCondition *whereIsCondition, ConditionType *type)
{
    return validateWhereNotCondition(whereIsCondition->whereNotCondition, type);
}

boolean validateWhereInCondition(WhereInCondition *whereInCondition, ConditionType *type)
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
    }
    return true;
}

boolean validateAuxiliaryClauseValue(AuxiliaryClauseValue *auxiliaryClauseValue)
{
    return validateJson(auxiliaryClauseValue->auxQuery);
}

boolean validateGroupByClauseValue(GroupByClauseValue *GroupByClauseValue)
{
    setScopeGroupByClause();
    return true;
}
