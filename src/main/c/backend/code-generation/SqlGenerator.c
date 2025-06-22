#include "Generator.h"
/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;
static SymbolTable _symbolTable = NULL;

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

static void _generateQuery(Json *json, int isMainQuery);
static void _generateClauseList(ClauseList *clauseList);
static void _generateClause(Clause *clause, int *havingFlag);
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
static void _generateWhereClause(WhereCondition *whereCondition, int *havingFlag);
static void _generateWhereCondition(WhereCondition *whereCondition);
static void _generateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition);
static void _generateWhereIsCondition(WhereIsCondition *whereIsCondition);
static void _generateWhereInCondition(WhereInCondition *whereInCondition, int isNotCondition);
static void _generateWhereNotCondition(WhereNotCondition *whereNotCondition);
static void _generateWhereConditionValue(WhereConditionValue *whereConditionValue);
static void _generateNullValuedWhereCondition(WhereBinaryCondition* whereBinaryCondition);
static void _generateBinaryConditionOperator(BinaryConditionOperator binaryConditionOperator);
static void _generateBinaryConditionOperatorForNullValues(BinaryConditionOperator binaryConditionOperator);
static void _generateAuxQueryByName(char *auxQueryName);
static boolean _whereConditionHasAggregationFunction(WhereCondition *whereCondition);
static boolean _whereBinaryConditionHasAggregationFunction(WhereBinaryCondition *whereBinaryCondition);
static boolean _notConditionHasAggregateFunction(WhereNotCondition *whereNotCondition);
static void _generateAggregationFunction(AggregationType aggType);

static void _outputSql(const char *const format, ...)
{
	va_list arguments;
	va_start(arguments, format);
	vfprintf(stdout, format, arguments);
	fflush(stdout);
	va_end(arguments);
}

static void _generateQuery(Json *json, int isMainQuery)
{
	if (json == NULL || json->clauseList == NULL)
	{
		logError(_logger, "Empty JSON clause list.");
		return;
	}
	_generateClauseList(json->clauseList);
	if (isMainQuery)
		_outputSql(";\n");
}

static void _generateClauseList(ClauseList *clauseList)
{
	Clause *clauses[CLAUSE_TYPE_COUNT] = {0};
	while(clauseList != NULL) {
		clauses[clauseList->clause->type] = clauseList->clause;
		clauseList = clauseList->next;
	}
	
	_generateAttributesClause(clauses[ATTRIBUTES_CLAUSE] != NULL ? clauses[ATTRIBUTES_CLAUSE]->clauseArgsList : NULL);

	int havingFlag = 0;
	for (int i=1; i<CLAUSE_TYPE_COUNT; i++) {
		if (clauses[i]) {
			_generateClause(clauses[i], &havingFlag);
			if (i == GROUP_BY_CLAUSE && havingFlag) {
				_generateClause(clauses[WHERE_CLAUSE], &havingFlag);
			}
		}
	}
}

static void _generateClause(Clause *clause, int *havingFlag)
{
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
		_generateWhereClause(clause->whereCondition, havingFlag);
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
	_outputSql("FROM ");
	while (clauseArgsList != NULL)
	{
		if (clauseArgsList->clauseValue != NULL)
		{
			_generateFromClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL)
			{
				_outputSql(", ");
			}
			else {
				_outputSql(" ");
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
			_outputSql(" ");
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

	switch(clauseValue->joinClauseValue->joinType) {
		case INNER_JOIN:
			_outputSql("INNER ");
			break;
		case LEFT_JOIN:
			_outputSql("LEFT ");
			break;
		case RIGHT_JOIN:
			_outputSql("RIGHT ");
			break;
	}
	_outputSql("%s JOIN %s ON ", clauseValue->joinClauseValue->outer? "OUTER" : "", clauseValue->joinClauseValue->table);
	_generateWhereBinaryCondition(clauseValue->joinClauseValue->condition);
}

static void _generateFromClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != FROM_CLAUSE) {
		logError(_logger, "Incorrect clause type in FROM clause: %d", clauseValue->clauseType);
	}

	if (clauseValue->fromClauseValue->fromClauseValueType == STR)
	{
		_outputSql("%s", clauseValue->fromClauseValue->string);
	}

	else if (clauseValue->fromClauseValue->fromClauseValueType == TABLE_RENAME)
	{
		_outputSql("%s AS %s",
			   clauseValue->fromClauseValue->tableRename->name,
			   clauseValue->fromClauseValue->tableRename->rename);
	}
}

static void _generateAttributesClause(ClauseArgsList *clauseArgsList)
{
	_outputSql("SELECT ");
	if (clauseArgsList == NULL)
	{
		_outputSql("* ");
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
					_outputSql(", ");
				}
				else {
					_outputSql(" ");
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

static void _generateAggregationFunction(AggregationType aggType) {
	switch(aggType) {
		case MIN_FUNC:
			_outputSql("MIN");
			break;
		case MAX_FUNC:
			_outputSql("MAX");
			break;
		case AVERAGE_FUNC:
			_outputSql("AVERAGE");
			break;
		case SUM_FUNC:
			_outputSql("SUM");
			break;
		case COUNT_FUNC:
			_outputSql("COUNT");
			break;
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

	if (clauseValue->attributesClauseValue->hasAggregationFunction)
	{
		_generateAggregationFunction(clauseValue->attributesClauseValue->aggregationFunction);
		_outputSql("(");
	}
	if (clauseValue->attributesClauseValue->table != NULL)
	{
		_outputSql("%s.", clauseValue->attributesClauseValue->table);
	}
	_outputSql(clauseValue->attributesClauseValue->name);
	if (clauseValue->attributesClauseValue->hasAggregationFunction)
	{
		_outputSql(")");
	}
	if (clauseValue->attributesClauseValue->rename != NULL)
	{
		_outputSql(" AS %s", clauseValue->attributesClauseValue->rename);
	}
}

static void _generateGroupByClause(ClauseArgsList *clauseArgsList)
{
	_outputSql("GROUP BY ");
	while (clauseArgsList != NULL)
	{
		if (clauseArgsList->clauseValue != NULL)
		{
			_generateGroupByClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL)
			{
				_outputSql(", ");
			}
			else {
				_outputSql(" ");
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
	_outputSql("%s", clauseValue->groupByClauseValue->string);
}

static void _generateOrderByClause(ClauseArgsList *clauseArgsList)
{
	_outputSql("ORDER BY ");
	while (clauseArgsList != NULL)
	{
		if (clauseArgsList->clauseValue != NULL)
		{
			_generateOrderByClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL)
			{
				_outputSql(", ");
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
	_outputSql("%s", orderByClauseValue->string);
}

static void _generateOrderByClauseValueComposite(OrderByClauseValue *orderByClauseValue)
{
	_outputSql("%s ", orderByClauseValue->compositeOrderByClause->string);
	if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_ASC)
	{
		_outputSql("ASC");
	}
	else if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_DESC)
	{
		_outputSql("DESC");
	}
}

static void _generateOrderByClauseValueAggrFunc(OrderByClauseValue *orderByClauseValue)
{
	_generateAggregationFunction(orderByClauseValue->compositeOrderByClause->aggrFunc);
	_outputSql("(%s) ", orderByClauseValue->compositeOrderByClause->string);
	if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_ASC)
	{
		_outputSql("ASC");
	}
	else if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_DESC)
	{
		_outputSql("DESC");
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
			   _notConditionHasAggregateFunction(whereBinaryCondition->value1->notCondition) ||
		   whereBinaryCondition->value2->type == CONDITION_VALUE_NOT_CONDITION &&
			   _notConditionHasAggregateFunction(whereBinaryCondition->value2->notCondition);
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

static void _generateWhereClause(WhereCondition *whereCondition, int *havingFlag)
{
	if (_whereConditionHasAggregationFunction(whereCondition)) {
		if (*havingFlag == 0)
			*havingFlag=1;
		else {
			_outputSql("HAVING ");
			_generateWhereCondition(whereCondition);
			*havingFlag = 0;
		}
	}
	else {
		_outputSql("WHERE ");	
		_generateWhereCondition(whereCondition);
	}
}

static void _generateWhereCondition(WhereCondition *whereCondition)
{
	switch (whereCondition->preconditional)
	{
	case PRECONDITIONAL_AND:
		_outputSql("AND ");
		break;
	case PRECONDITIONAL_OR:
		_outputSql("OR ");
		break;
	}
	_outputSql("(");
	switch (whereCondition->nodeType)
	{
	case NODE_TYPE_WHERE_BINARY_CONDITION:
		_generateWhereBinaryCondition(whereCondition->binaryCondition);
		break;
	case NODE_TYPE_WHERE_IN_CONDITION:
		_generateWhereInCondition(whereCondition->whereInCondition, false);
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
	_outputSql(") ");
	if (whereCondition->next)
		_generateWhereCondition(whereCondition->next);
}

static void _generateNullValuedWhereCondition(WhereBinaryCondition *whereBinaryCondition) {
	if (whereBinaryCondition->value1->type == CONDITION_VALUE_NULL) {
		WhereConditionValue* aux = whereBinaryCondition->value1;
		whereBinaryCondition->value1 = whereBinaryCondition->value2;
		whereBinaryCondition->value2 = aux;
	}
	
	_generateWhereConditionValue(whereBinaryCondition->value1);
	_generateBinaryConditionOperatorForNullValues(whereBinaryCondition->operator);
	_generateWhereConditionValue(whereBinaryCondition->value2);
}

static void _generateWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition)
{
	_outputSql("(");
	if (whereBinaryCondition->value1->type == CONDITION_VALUE_NULL || whereBinaryCondition->value2->type == CONDITION_VALUE_NULL) {
		_generateNullValuedWhereCondition(whereBinaryCondition);
	}
	else {
		_generateWhereConditionValue(whereBinaryCondition->value1);
		_generateBinaryConditionOperator(whereBinaryCondition->operator);
		_generateWhereConditionValue(whereBinaryCondition->value2);
	}
	_outputSql(")");
}
static void _generateWhereIsCondition(WhereIsCondition *whereIsCondition)
{
	_outputSql("%s ", whereIsCondition->attribute);
	_generateWhereNotCondition(whereIsCondition->whereNotCondition);
}
static void _generateWhereInCondition(WhereInCondition *whereInCondition, int isNotCondition)
{
	_outputSql("%s IN (", isNotCondition? "" : whereInCondition->attribute);

	switch (whereInCondition->type)
	{
	case WHERE_IN_CONDITION_QUERY:
		_generateQuery(whereInCondition->query, 0);
		break;
	case WHERE_IN_CONDITION_AUX_QUERY_NAME:
		_generateAuxQueryByName(whereInCondition->auxQueryName);
		break;
	}
	_outputSql(")");
}
static void _generateWhereNotCondition(WhereNotCondition *whereNotCondition)
{
	if (whereNotCondition->nodeSelected == IN_NODE)
		_outputSql("%s ", whereNotCondition->in->attribute);
	_outputSql("NOT%s", whereNotCondition->nodeSelected==IN_NODE ? "" : " ");
	switch (whereNotCondition->nodeSelected)
	{
	case IN_NODE:
		_generateWhereInCondition(whereNotCondition->in, true);
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
		_outputSql("%s", whereConditionValue->string);
		break;
	case CONDITION_VALUE_ATTRIBUTE:
		_outputSql("%s", whereConditionValue->attribute);
		break;
	case CONDITION_VALUE_NUMBER:
		_outputSql("%f", whereConditionValue->number);
		break;
	case CONDITION_VALUE_INTEGER:
		_outputSql("%d", whereConditionValue->integer);
		break;
	case CONDITION_VALUE_BOOLEAN:
		_outputSql(whereConditionValue->bool ? "TRUE" : "FALSE");
		break;
	case CONDITION_VALUE_NULL:
		_outputSql("NULL");
		break;
	case CONDITION_VALUE_AGGREGATION_FUNCTION:
		_generateAggregationFunction(whereConditionValue->aggregationFunction->aggr);
		_outputSql("(%s)", whereConditionValue->aggregationFunction->attribute);
		break;
	case CONDITION_VALUE_BINARY_CONDITION:
		_generateWhereBinaryCondition(whereConditionValue->binaryCondition);
		break;
	case CONDITION_VALUE_NOT_CONDITION:
		_generateWhereNotCondition(whereConditionValue->notCondition);
		break;
	}
}
static void _generateBinaryConditionOperator(BinaryConditionOperator binaryConditionOperator)
{
	switch (binaryConditionOperator)
	{
	case BINARY_CONDITION_OPERATOR_LOWER:
		_outputSql(" < ");
		break;
	case BINARY_CONDITION_OPERATOR_GREATER:
		_outputSql(" > ");
		break;
	case BINARY_CONDITION_OPERATOR_LOWER_OR_EQUAL:
		_outputSql(" <= ");
		break;
	case BINARY_CONDITION_OPERATOR_GREATER_OR_EQUAL:
		_outputSql(" >= ");
		break;
	case BINARY_CONDITION_OPERATOR_EQUAL:
		_outputSql(" = ");
		break;
	case BINARY_CONDITION_OPERATOR_NOT_EQUAL:
		_outputSql(" != ");
		break;
	}
}

static void _generateBinaryConditionOperatorForNullValues(BinaryConditionOperator binaryConditionOperator) {
	switch(binaryConditionOperator) 
	{
		case BINARY_CONDITION_OPERATOR_EQUAL:
			_outputSql(" IS ");
			break;
		case BINARY_CONDITION_OPERATOR_NOT_EQUAL:
			_outputSql(" IS NOT ");
			break;
	}
}

static void _generateAuxQueryByName(char *auxQueryName){
	_generateQuery((Json *)getSubqueryByName(_symbolTable, auxQueryName), 0);
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
	_symbolTable = compilerState->symbolTable;
	if (_symbolTable == NULL)
	{
		logError(_logger, "Symbol table is NULL.");
		return;
	}
	_generateQuery(program->json, 1);
	logDebugging(_logger, "Generation is done.");
}
