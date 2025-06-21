#include "Generator.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeGeneratorModule()
{
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static void _generateQuery(Json *json);
static void _generateClauseList(ClauseList *clauseList);
static void _generateClause(Clause *clause);
static void _generateFromClause(ClauseArgsList *clauseArgsList);
static void _generateFromClauseValue(ClauseValue *clauseValue);
static void _generateJoinClause(ClauseArgsList *clauseArgsList);
static void _generateJoinClauseValue(ClauseValue *clauseValue);
static void _generateAttributesClause(ClauseArgsList *clauseArgsList);
static void _generateAttributesClauseValue(ClauseValue *clauseValue);
static void _generateGroupByClause(ClauseArgsList *clauseArgsList);
static void _generateGroupByClauseValue(ClauseValue *clauseValue);
static void _generateOrderByClause(ClauseArgsList *clauseArgsList);
static void _generateOrderByClauseValue(ClauseValue *clauseValue);
static void _generateOrderByClauseValueString(OrderByClauseValue *orderByClauseValue);
static void _generateOrderByClauseValueComposite(OrderByClauseValue *orderByClauseValue);
static void _generateOrderByClauseValueAggrFunc(OrderByClauseValue *orderByClauseValue);
static void _generateWhereClause(WhereCondition *whereCondition);
static void _generateWhereCondition(WhereCondition *whereCondition);
static void _generateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition);
static void _generateWhereIsCondition(WhereIsCondition *whereIsCondition);
static void _generateWhereInCondition(WhereInCondition *whereInCondition);
static void _generateWhereNotCondition(WhereNotCondition *whereNotCondition);
static void _generateWhereConditionValue(WhereConditionValue *whereConditionValue);
static void _generateBinaryConditionOperator(BinaryConditionOperator *binaryConditionOperator);
static void _generateAuxQueryByName(char *auxQueryName);
static boolean _whereConditionHasAggregationFunction(WhereCondition *whereCondition);
static boolean _whereBinaryConditionHasAggregationFunction(WhereBinaryCondition *whereBinaryCondition);
static boolean _notConditionHasAggregateFunction(WhereNotCondition *whereNotCondition);

static void _generateQuery(Json *json)
{
	if (json == NULL || json->clauseList == NULL)
	{
		logError(_logger, "Empty JSON clause list.");
		return;
	}
	_generateClauseList(json->clauseList);
	printf(";\n");
}

static void _generateClauseList(ClauseList *clauseList)
{
	//@TODO: Handle clauseList order
	while (clauseList != NULL)
	{
		_generateClause(clauseList->clause);
		clauseList = clauseList->next;
	}
}

static void _generateClause(Clause *clause)
{
	if (clause->clauseArgsList == NULL)
	{
		logError(_logger, "Empty FROM clause arguments.");
		return;
	}
	switch (clause->type)
	{
	case FROM_CLAUSE:
		_generateFromClause(clause->clauseArgsList);
		break;
	case ATTRIBUTES_CLAUSE:
		_generateAttributesClause(clause->clauseArgsList);
		break;
	case GROUP_BY_CLAUSE:
		_generateGroupByClause(clause->clauseArgsList);
		break;
	case ORDER_BY_CLAUSE:
		_generateOrderByClause(clause->clauseArgsList);
		break;
	case WHERE_CLAUSE:
		_generateWhereClause(clause->whereCondition);
		break;
	case AUXILIARY_CLAUSE:
		break;
	case JOIN_CLAUSE:
			_generateJoinClause(clause->clauseArgsList);
		break;
	default:
		logError(_logger, "Unsupported clause type: %d", clause->type);
		break;
	}
}

static void _generateFromClause(ClauseArgsList *clauseArgsList)
{
	printf("FROM ");
	while (clauseArgsList != NULL)
	{
		if (clauseArgsList->clauseValue != NULL)
		{
			_generateFromClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL)
			{
				printf(", ");
			}
		}
		else
		{
			logError(_logger, "Null clause argument in FROM clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateJoinClause(ClauseArgsList *clauseArgsList) {
	while (clauseArgsList != NULL) {
		if (clauseArgsList->clauseValue != NULL) {
			_generateJoinClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL) {
				printf("\n");
			}
		} else {
			logError(_logger, "Null clause argument in JOIN clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateJoinClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != JOIN_CLAUSE) {
		logError(_logger, "Incorrect clause type in JOIN clause: %d", clauseValue->clauseType);
	}

	printf("%s %s JOIN %s ON ", clauseValue->joinClauseValue->type, clauseValue->joinClauseValue->outer? "OUTER" : "", clauseValue->joinClauseValue->table);
	_generateWhereBinaryCondition(clauseValue->joinClauseValue->condition);
}

static void _generateFromClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != FROM_CLAUSE) {
		logError(_logger, "Incorrect clause type in FROM clause: %d", clauseValue->clauseType);
	}

	if (clauseValue->fromClauseValue->fromClauseValueType == STR)
	{
		printf("%s", clauseValue->fromClauseValue->string);
	}

	else if (clauseValue->fromClauseValue->fromClauseValueType == TABLE_RENAME)
	{
		printf("%s AS %s",
			   clauseValue->fromClauseValue->tableRename->name,
			   clauseValue->fromClauseValue->tableRename->rename);
	}
}

static void _generateAttributesClause(ClauseArgsList *clauseArgsList)
{
	if (clauseArgsList == NULL)
	{
		printf("* ");
	}
	else
	{
		while (clauseArgsList != NULL)
		{
			if (clauseArgsList->clauseValue != NULL)
			{
				_generateAttributesClauseValue(clauseArgsList->clauseValue);
				if (clauseArgsList->next != NULL)
				{
					printf(", ");
				}
			}
			else
			{
				logError(_logger, "Null clause argument in ATTRIBUTES clause.");
			}
			clauseArgsList = clauseArgsList->next;
		}
	}
}

static void _generateAttributesClauseValue(ClauseValue *clauseValue)
{
	if (clauseValue->clauseType != ATTRIBUTES_CLAUSE)
	{
		logError(_logger, "Incorrect clause type in ATTRIBUTES clause: %d", clauseValue->clauseType);
	}
	if (clauseValue->attributesClauseValue->name == NULL)
	{
		logError(_logger, "Name attribute cannot be NULL");
	}

	if (clauseValue->attributesClauseValue->aggregationFunction != NULL)
	{
		printf("%s(", clauseValue->attributesClauseValue->aggregationFunction);
	}
	if (clauseValue->attributesClauseValue->table != NULL)
	{
		printf("%s.", clauseValue->attributesClauseValue->table);
	}
	printf(clauseValue->attributesClauseValue->name);
	if (clauseValue->attributesClauseValue->aggregationFunction != NULL)
	{
		printf(")");
	}
	if (clauseValue->attributesClauseValue->rename != NULL)
	{
		printf("AS %s", clauseValue->attributesClauseValue->rename);
	}
}

static void _generateGroupByClause(ClauseArgsList *clauseArgsList)
{
	printf("GROUP BY ");
	while (clauseArgsList != NULL)
	{
		if (clauseArgsList->clauseValue != NULL)
		{
			_generateGroupByClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL)
			{
				printf(", ");
			}
		}
		else
		{
			logError(_logger, "Null clause argument in GROUP BY clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateGroupByClauseValue(ClauseValue *clauseValue)
{
	if (clauseValue->clauseType != GROUP_BY_CLAUSE)
	{
		logError(_logger, "Incorrect clause type in ORDER BY clause: %d", clauseValue->clauseType);
	}
	if (clauseValue->groupByClauseValue->string == NULL)
	{
		logError(_logger, "Group by string cannot be NULL");
	}
	printf("%s", clauseValue->groupByClauseValue->string);
}

static void _generateOrderByClause(ClauseArgsList *clauseArgsList)
{
	printf("ORDER BY ");
	while (clauseArgsList != NULL)
	{
		if (clauseArgsList->clauseValue != NULL)
		{
			_generateOrderByClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL)
			{
				printf(", ");
			}
		}
		else
		{
			logError(_logger, "Null clause argument in ORDER BY clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateOrderByClauseValue(ClauseValue *clauseValue)
{
	if (clauseValue->clauseType != ORDER_BY_CLAUSE)
	{
		logError(_logger, "Incorrect clause type in ORDER BY clause: %d", clauseValue->clauseType);
	}

	switch (clauseValue->orderByClauseValue->orderByClauseValueType)
	{
	case ORDER_BY_STRING:
		_generateOrderByClauseValueString(clauseValue->orderByClauseValue);
		break;
	case ORDER_BY_COMPOSITE:
		_generateOrderByClauseValueComposite(clauseValue->orderByClauseValue);
		break;
	case ORDER_BY_AGGR_FUNC:
		_generateOrderByClauseValueAggrFunc(clauseValue->orderByClauseValue);
		break;
	}
}

static void _generateOrderByClauseValueString(OrderByClauseValue *orderByClauseValue)
{
	printf("%s", orderByClauseValue->string);
}

static void _generateOrderByClauseValueComposite(OrderByClauseValue *orderByClauseValue)
{
	printf("%s ", orderByClauseValue->string);
	if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_ASC)
	{
		printf("ASC");
	}
	else if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_DESC)
	{
		printf("DESC");
	}
}

static void _generateOrderByClauseValueAggrFunc(OrderByClauseValue *orderByClauseValue)
{
	printf("%s(%s) ", orderByClauseValue->compositeOrderByClause->aggrFunc, orderByClauseValue->compositeOrderByClause->string);
	if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_ASC)
	{
		printf("ASC");
	}
	else if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_DESC)
	{
		printf("DESC");
	}
}

static boolean _notConditionHasAggregateFunction(WhereNotCondition *whereNotCondition)
{
	return whereNotCondition->nodeSelected == BINARY_CONDITION_NODE &&
			   _whereBinaryConditionHasAggregationFunction(whereNotCondition->whereBinaryCondition) ||
		   whereNotCondition->nodeSelected == NOT_NODE &&
			   _notConditionHasAggregateFunction(whereNotCondition->not) ||
		   whereNotCondition->nodeSelected == CONDITION_NODE &&
			   _whereConditionHasAggregationFunction(whereNotCondition->condition);
}

static boolean _whereBinaryConditionHasAggregationFunction(WhereBinaryCondition *whereBinaryCondition)
{

	return whereBinaryCondition->value1->type == CONDITION_VALUE_AGGREGATION_FUNCTION ||
		   whereBinaryCondition->value2->type == CONDITION_VALUE_AGGREGATION_FUNCTION ||
		   whereBinaryCondition->value1->type == CONDITION_VALUE_BINARY_CONDITION &&
			   _whereBinaryConditionHasAggregationFunction(whereBinaryCondition->value1->binaryCondition) ||
		   whereBinaryCondition->value2->type == CONDITION_VALUE_BINARY_CONDITION &&
			   _whereBinaryConditionHasAggregationFunction(whereBinaryCondition->value2->binaryCondition) ||
		   whereBinaryCondition->value1->type == CONDITION_VALUE_NOT_CONDITION &&
			   _notConditionHasAggregationFunction(whereBinaryCondition->value1->notCondition) ||
		   whereBinaryCondition->value2->type == CONDITION_VALUE_NOT_CONDITION &&
			   _notConditionHasAggregationFunction(whereBinaryCondition->value2->notCondition);
	// Si es in es atributo y no tiene funcion de agregacion
}

static boolean _whereConditionHasAggregationFunction(WhereCondition *whereCondition)
{
	boolean hasAggregationFunction = false;
	switch (whereCondition->nodeType)
	{
	case NODE_TYPE_WHERE_BINARY_CONDITION:
		return _whereBinaryConditionHasAggregationFunction(whereCondition->binaryCondition);
		break;
	case NODE_TYPE_WHERE_IN_CONDITION:
		return false; // Solo tiene atributos aca
		break;
	case NODE_TYPE_WHERE_IS_CONDITION:
		return _notConditionHasAggregateFunction(whereCondition->whereIsCondition->whereNotCondition);
		break;
	case NODE_TYPE_WHERE_NOT_CONDITION:
		return _notConditionHasAggregateFunction(whereCondition->whereNotCondition);
		break;
	case NODE_TYPE_WHERE_CONDITION:
		return _whereConditionHasAggregationFunction(whereCondition->whereCondition);
	}

	if (whereCondition->next)
		return (hasAggregationFunction || _whereConditionHasAggregationFunction(whereCondition->next));
	else
		return hasAggregationFunction;
}

static void _generateWhereClause(WhereCondition *whereCondition)
{
	if (_whereConditionHasAggregationFunction(whereCondition))
		printf("HAVING ");
	else
		printf("WHERE ");
	_generateWhereCondition(whereCondition);
}

static void _generateWhereCondition(WhereCondition *whereCondition)
{
	switch (whereCondition->preconditional)
	{
	case PRECONDITIONAL_AND:
		printf("AND ");
		break;
	case PRECONDITIONAL_OR:
		printf("OR ");
		break;
	}
	printf("(");
	switch (whereCondition->nodeType)
	{
	case NODE_TYPE_WHERE_BINARY_CONDITION:
		_generateWhereBinaryCondition(whereCondition->binaryCondition);
		break;
	case NODE_TYPE_WHERE_IN_CONDITION:
		_generateWhereInCondition(whereCondition->whereInCondition);
		break;
	case NODE_TYPE_WHERE_IS_CONDITION:
		_generateWhereIsCondition(whereCondition->whereIsCondition);
		break;
	case NODE_TYPE_WHERE_NOT_CONDITION:
		_generateWhereNotCondition(whereCondition->whereNotCondition);
		break;
	case NODE_TYPE_WHERE_CONDITION:
		_generateWhereCondition(whereCondition->whereCondition);
		break;
	}
	printf(")");
	if (whereCondition->next)
		_generateWhereCondition(whereCondition->next);
}

static void _generateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition)
{
	_generateWhereConditionValue(whereBinaryCondition->value1);
	_generateBinaryConditionOperator(whereBinaryCondition->operator);
	_generateWhereConditionValue(whereBinaryCondition->value1);
}
static void _generateWhereIsCondition(WhereIsCondition *whereIsCondition)
{
	printf("%s ", whereIsCondition->attribute);
	_generateWhereNotCondition(whereIsCondition->whereNotCondition);
}
static void _generateWhereInCondition(WhereInCondition *whereInCondition)
{
	printf("%s IN (", whereInCondition->attribute);

	switch (whereInCondition->type)
	{
	case WHERE_IN_CONDITION_QUERY:
		_generateQuery(whereInCondition->query);
		break;
	case WHERE_IN_CONDITION_AUX_QUERY_NAME:
		_generateAuxQueryByName(whereInCondition->auxQueryName);
		break;
	}
	printf(")");
}
static void _generateWhereNotCondition(WhereNotCondition *whereNotCondition)
{
	printf("NOT ");
	switch (whereNotCondition->nodeSelected)
	{
	case IN_NODE:
		_generateWhereInCondition(whereNotCondition->in);
		break;
	case NOT_NODE:
		_generateWhereNotCondition(whereNotCondition->not);
		break;
	case CONDITION_NODE:
		_generateWhereCondition(whereNotCondition->condition);
		break;
	case BINARY_CONDITION_NODE:
		_generateWhereBinaryCondition(whereNotCondition->whereBinaryCondition);
		break;
	}
}

static void _generateWhereConditionValue(WhereConditionValue *whereConditionValue)
{
	switch (whereConditionValue->type)
	{
	case CONDITION_VALUE_STRING:
		printf("'%s'", whereConditionValue->string);
		break;
	case CONDITION_VALUE_ATTRIBUTE:
		printf("%s", whereConditionValue->attribute);
		break;
	case CONDITION_VALUE_NUMBER:
		printf("%f", whereConditionValue->number);
		break;
	case CONDITION_VALUE_INTEGER:
		printf("%d", whereConditionValue->integer);
		break;
	case CONDITION_VALUE_BOOLEAN:
		printf(whereConditionValue->bool ? "TRUE" : "FALSE");
		break;
	case CONDITION_VALUE_AGGREGATION_FUNCTION:
		printf("%s(%s)", whereConditionValue->aggregationFunction->aggr, whereConditionValue->aggregationFunction->attribute);
		break;
	case CONDITION_VALUE_BINARY_CONDITION:
		_generateWhereBinaryCondition(whereConditionValue->binaryCondition);
		break;
	case CONDITION_VALUE_NOT_CONDITION:
		_generateWhereNotCondition(whereConditionValue->notCondition);
		break;
	}
}
static void _generateBinaryConditionOperator(BinaryConditionOperator *binaryConditionOperator)
{
	switch (*binaryConditionOperator)
	{
	case BINARY_CONDITION_OPERATOR_LOWER:
		printf(" < ");
		break;
	case BINARY_CONDITION_OPERATOR_GREATER:
		printf(" > ");
		break;
	case BINARY_CONDITION_OPERATOR_LOWER_OR_EQUAL:
		printf(" <= ");
		break;
	case BINARY_CONDITION_OPERATOR_GREATER_OR_EQUAL:
		printf(" >= ");
		break;
	case BINARY_CONDITION_OPERATOR_EQUAL:
		printf(" = ");
		break;
	case BINARY_CONDITION_OPERATOR_NOT_EQUAL:
		printf(" != ");
		break;
	}
}

/** PUBLIC FUNCTIONS */

void generate(CompilerState *compilerState)
{
	//@TODO: Delete log errors, generate doesnt validate
	logDebugging(_logger, "Starting SQL generation...");
	Program *program = (Program *)compilerState->abstractSyntaxtTree;

	if (program->type != JSON)
	{
		logError(_logger, "Expected a JSON AST in program node.");
		return;
	}

	_generateQuery(program->json);
	logDebugging(_logger, "Generation is done.");
}
