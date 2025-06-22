#include "AbstractSyntaxTree.h"
#include <stdio.h>
/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeAbstractSyntaxTreeModule()
{
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseJson(Json *json)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (json != NULL)
	{
		releaseClauseList(json->clauseList);
		free(json);
	}
}

void releaseProgram(Program *program)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL)
	{
		releaseJson(program->json);

		free(program);
	}
}

void releaseClauseArgsList(ClauseArgsList *clauseArgsList)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (clauseArgsList != NULL)
	{
		ClauseArgsList *next = clauseArgsList->next;
		releaseClauseValue(clauseArgsList->clauseValue);
		free(clauseArgsList);
		clauseArgsList = next;
	}
}

void releaseTableRename(TableRename *tableRename)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	free(tableRename->name);
	free(tableRename->rename);
	free(tableRename);
}

void releaseClauseValue(ClauseValue *clauseValue)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (clauseValue != NULL)
	{
		switch (clauseValue->clauseType)
		{
		case FROM_CLAUSE:
			switch (clauseValue->fromClauseValue->fromClauseValueType)
			{
			case STR:
				free(clauseValue->fromClauseValue->string);
				break;
			case TABLE_RENAME:
				releaseTableRename(clauseValue->fromClauseValue->tableRename);
				break;
			}
			free(clauseValue->fromClauseValue);
			break;
		case ATTRIBUTES_CLAUSE:
			if (clauseValue->attributesClauseValue->name != NULL)
			{
				free(clauseValue->attributesClauseValue->name);
			}
			if (clauseValue->attributesClauseValue->rename != NULL)
			{
				free(clauseValue->attributesClauseValue->rename);
			}
			if (clauseValue->attributesClauseValue->table != NULL)
			{
				free(clauseValue->attributesClauseValue->table);
			}
			free(clauseValue->attributesClauseValue);
			break;
		case GROUP_BY_CLAUSE:
			free(clauseValue->groupByClauseValue->string);
			free(clauseValue->groupByClauseValue);
			break;
		case ORDER_BY_CLAUSE:
			switch (clauseValue->orderByClauseValue->orderByClauseValueType)
			{
			case ORDER_BY_STRING:
				free(clauseValue->orderByClauseValue->string);
				break;
			case ORDER_BY_COMPOSITE:
				if (clauseValue->orderByClauseValue->compositeOrderByClause != NULL)
				{
					free(clauseValue->orderByClauseValue->compositeOrderByClause->string);
					free(clauseValue->orderByClauseValue->compositeOrderByClause);
				}
				break;
			case ORDER_BY_AGGR_FUNC:
				if (clauseValue->orderByClauseValue->compositeOrderByClause != NULL)
				{
					free(clauseValue->orderByClauseValue->compositeOrderByClause->string);
					free(clauseValue->orderByClauseValue->compositeOrderByClause);
				}
				break;
			}
			free(clauseValue->orderByClauseValue);
			break;

		case AUXILIARY_CLAUSE:
			releaseJson(clauseValue->auxiliaryClauseValue->auxQuery);
			free(clauseValue->auxiliaryClauseValue->auxQueryName);
			free(clauseValue->auxiliaryClauseValue);
			break;
		case JOIN_CLAUSE:
			releaseJoinClauseValue(clauseValue->joinClauseValue);
			break;
		}
		free(clauseValue);
	}
}

void releaseClause(Clause *clause)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (clause != NULL)
	{
		switch (clause->type)
		{
		case WHERE_CLAUSE:
			releaseWhereCondition(clause->whereCondition);
			break;
		default:
			releaseClauseArgsList(clause->clauseArgsList);
			break;
		}
	}
	free(clause);
}

void releaseClauseList(ClauseList *clauseList)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	ClauseList *aux = NULL;
	while (clauseList->next != NULL)
	{
		releaseClause(clauseList->clause);
		aux = clauseList;
		clauseList = clauseList->next;
		free(aux);
	}
	releaseClause(clauseList->clause);
	free(clauseList);
}

void releaseAggregationFunction(AggregationFunction *aggFunc)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (aggFunc != NULL)
	{
		free(aggFunc->attribute);
		free(aggFunc);
	}
}

void releaseAttributeRename(AttributeRename *attrRename)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (attrRename != NULL)
	{
		releaseClauseValue(attrRename->value);
		free(attrRename->rename);
		free(attrRename);
	}
}

void releaseWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (whereBinaryCondition != NULL)
	{
		releaseWhereConditionValue(whereBinaryCondition->value1);
		releaseWhereConditionValue(whereBinaryCondition->value2);
		free(whereBinaryCondition);
	}
}

void releaseWhereConditionValue(WhereConditionValue *whereConditionValue)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (whereConditionValue != NULL)
	{
		switch (whereConditionValue->type)
		{
		case CONDITION_VALUE_STRING:
			free(whereConditionValue->string);
			break;
		case CONDITION_VALUE_BINARY_CONDITION:
			releaseWhereBinaryCondition(whereConditionValue->binaryCondition);
			break;
		case CONDITION_VALUE_NOT_CONDITION:
			releaseWhereNotCondition(whereConditionValue->notCondition);
			break;
		case CONDITION_VALUE_AGGREGATION_FUNCTION:
			releaseAggregationFunction(whereConditionValue->aggregationFunction);
			break;
		case CONDITION_VALUE_ATTRIBUTE:
			free(whereConditionValue->attribute);
			break;
		}

		free(whereConditionValue);
	}
}

void releaseWhereNotCondition(WhereNotCondition *whereNotCondition)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (whereNotCondition != NULL)
	{
		switch (whereNotCondition->nodeSelected)
		{
		case NOT_NODE:
			releaseWhereNotCondition(whereNotCondition->not);
			break;

		case CONDITION_NODE:
			releaseWhereCondition(whereNotCondition->condition);
			break;

		case BINARY_CONDITION_NODE:
			releaseWhereBinaryCondition(whereNotCondition->whereBinaryCondition);
			break;
		case IN_NODE:
			releaseWhereInCondition(whereNotCondition->in);
			break;
		}
		free(whereNotCondition);
	}
}

void releaseWhereCondition(WhereCondition *whereCondition)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (whereCondition != NULL)
	{
		switch (whereCondition->nodeType)
		{
		case NODE_TYPE_WHERE_BINARY_CONDITION:
			releaseWhereBinaryCondition(whereCondition->binaryCondition);
			break;
		case NODE_TYPE_WHERE_CONDITION:
			releaseWhereCondition(whereCondition->whereCondition);
			break;
		case NODE_TYPE_WHERE_NOT_CONDITION:
			releaseWhereNotCondition(whereCondition->whereNotCondition);
			break;
		case NODE_TYPE_WHERE_IN_CONDITION:
			releaseWhereInCondition(whereCondition->whereInCondition);
			break;
		}
		releaseWhereCondition(whereCondition->next);
		free(whereCondition);
	}
}

void releaseWhereInCondition(WhereInCondition *whereInCondition)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (whereInCondition != NULL)
	{
		switch (whereInCondition->type)
		{
		case WHERE_IN_CONDITION_QUERY:
			releaseJson(whereInCondition->query);
			break;
		case WHERE_IN_CONDITION_AUX_QUERY_NAME:
			free(whereInCondition->auxQueryName);
			break;
		}
		free(whereInCondition->attribute);
		free(whereInCondition);
	}
}

void releaseJoinClauseValue(JoinClauseValue *joinClauseValue)
{
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (joinClauseValue != NULL)
	{
		releaseWhereBinaryCondition(joinClauseValue->condition);
		free(joinClauseValue->table);
		free(joinClauseValue);
	}
}
