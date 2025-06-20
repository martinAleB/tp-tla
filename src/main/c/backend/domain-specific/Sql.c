#include "Sql.h"

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

boolean computeSql(Program *program)
{
    return computeJson(program->json);
}

boolean computeJson(Json *json)
{
    return computeClauseList(json->clauseList);
}

boolean computeClauseList(ClauseList *clauseList)
{
    int clauseCounter[CLAUSE_TYPE_COUNT] = {0};
    ClauseList *current = clauseList;
    while (current != NULL)
    {
        Clause *clause = current->clause;
        clauseCounter[clause->type]++;
        if ((clause->type != JOIN_CLAUSE && clauseCounter[clause->type] > 1) || !computeClause(clause))
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

boolean computeClause(Clause *clause)
{
    if (clause->type == WHERE_CLAUSE)
    {
        ConditionType type = FIRST_WHERE_CONDITION_TYPE;
        return computeWhereCondition(clause->whereCondition, &type);
    }
    return computeClauseArgsList(clause->clauseArgsList);
}

boolean computeWhereCondition(WhereCondition *whereCondition, ConditionType *type)
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
            if (!computeWhereInCondition(current->whereInCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_IS_CONDITION:
            if (!computeWhereIsCondition(current->whereIsCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_NOT_CONDITION:
            if (!computeWhereNotCondition(current->whereNotCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_BINARY_CONDITION:
            if (!computeWhereBinaryCondition(current->binaryCondition, &defType))
            {
                return false;
            }
            break;
        case NODE_TYPE_WHERE_CONDITION:
            if (!computeWhereCondition(current->whereCondition, &defType))
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

boolean computeWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition, ConditionType *type)
{
    if (*type == UNDEFINED_CONDITION_TYPE)
    {
        *type = (whereBinaryCondition->value1->type == CONDITION_VALUE_AGGREGATION_FUNCTION || whereBinaryCondition->value2->type == CONDITION_VALUE_AGGREGATION_FUNCTION) ? HAVING_CONDITION_TYPE : WHERE_CONDITION_TYPE;
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

boolean computeWhereNotCondition(WhereNotCondition *whereNotCondition, ConditionType *type)
{
    switch (whereNotCondition->nodeSelected)
    {
    case NOT_NODE:
        return computeWhereNotCondition(whereNotCondition->not, type);
    case IN_NODE:
        return computeWhereInCondition(whereNotCondition->in, type);
    case CONDITION_NODE:
        return computeWhereCondition(whereNotCondition->condition, type);
    case BINARY_CONDITION_NODE:
        return computeWhereBinaryCondition(whereNotCondition->whereBinaryCondition, type);
    }
    return true;
}

boolean computeWhereIsCondition(WhereIsCondition *whereIsCondition, ConditionType *type)
{
    return computeWhereNotCondition(whereIsCondition->whereNotCondition, type);
}

boolean computeWhereInCondition(WhereInCondition *whereInCondition, ConditionType *type)
{
    if (whereInCondition->type == WHERE_IN_CONDITION_QUERY)
    {
        return computeJson(whereInCondition->query);
    }
    return true;
}

boolean computeClauseArgsList(ClauseArgsList *clauseArgsList)
{
    ClauseArgsList *current = clauseArgsList;
    while (current != NULL)
    {
        if (!computeClauseValue(clauseArgsList->clauseValue))
        {
            return false;
        }
        current = current->next;
    }
    return true;
}

boolean computeClauseValue(ClauseValue *clauseValue)
{
    switch (clauseValue->clauseType)
    {
    case AUXILIARY_CLAUSE:
        return computeAuxiliaryClauseValue(clauseValue->auxiliaryClauseValue);
    }
    return true;
}

boolean computeAuxiliaryClauseValue(AuxiliaryClauseValue *auxiliaryClauseValue)
{
    return computeJson(auxiliaryClauseValue->auxQuery);
}
