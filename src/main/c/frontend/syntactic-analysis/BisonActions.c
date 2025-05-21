#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeBisonActionsModule()
{
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char *functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char *functionName)
{
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant *IntegerConstantSemanticAction(const int value)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant *constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression *ArithmeticExpressionSemanticAction(Expression *leftExpression, Expression *rightExpression, ExpressionType type)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression *expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression *FactorExpressionSemanticAction(Factor *factor)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression *expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor *ConstantFactorSemanticAction(Constant *constant)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor *factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor *ExpressionFactorSemanticAction(Expression *expression)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor *factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program *ExpressionProgramSemanticAction(CompilerState *compilerState, Expression *expression)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program *program = calloc(1, sizeof(Program));
	program->expression = expression;
	program->type = PROGRAM_EXPRESSION;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext())
	{
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else
	{
		compilerState->succeed = true;
	}
	return program;
}

// Our own Bison Actions

// JSON

Program *JsonProgramSemanticAction(CompilerState *compilerState, Json *json)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program *program = calloc(1, sizeof(Program));
	program->json = json;
	program->type = JSON;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext())
	{
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else
	{
		compilerState->succeed = true;
	}
	return program;
}

Json *JsonSemanticAction(ClauseList *clauseList)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Json *json = calloc(1, sizeof(Json));
	json->clauseList = clauseList;
	return json;
}

// CLAUSES (GENERAL)

ClauseList *ClauseListSemanticAction(Clause *clause, ClauseList *clauseList)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseList *newClauseList = calloc(1, sizeof(ClauseList));
	newClauseList->clause = clause;
	newClauseList->next = clauseList;
	return newClauseList;
}

Clause *ClauseSemanticAction(ClauseArgsList *newClauseArgsList, Token token)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Clause *clause = calloc(1, sizeof(Clause));
	clause->type = token;
	clause->clauseArgsList = newClauseArgsList;
	return clause;
}

Clause *WhereClauseSemanticAction(WhereCondition *whereCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Clause *clause = calloc(1, sizeof(Clause));
	clause->type = WHERE_CLAUSE;
	clause->whereCondition = whereCondition;
	return clause;
}

ClauseArgsList *ClauseArgsListSemanticAction(ClauseValue *clauseValue, ClauseArgsList *clauseValuesNext)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList *clauseArgsList = calloc(1, sizeof(ClauseArgsList));
	clauseArgsList->clauseValue = clauseValue;
	clauseArgsList->next = clauseValuesNext;
	return clauseArgsList;
}

ClauseValue *AttributeClauseValueSemanticAction(AttributesClauseValue *attributeClauseValue)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->attributesClauseValue = attributeClauseValue;
	clauseValue->clauseType = ATTRIBUTES_CLAUSE;
	return clauseValue;
}

// FROM CLAUSE VALUES

ClauseValue *StringFromClauseValueSemanticAction(char *string)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->fromClauseValue = calloc(1, sizeof(FromClauseValue));
	clauseValue->fromClauseValue->string = string;
	clauseValue->clauseType = FROM_CLAUSE;
	return clauseValue;
}

ClauseValue *TableRenameFromClauseValueSemanticAction(char *trueName, char *alias)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->fromClauseValue = calloc(1, sizeof(FromClauseValue));
	clauseValue->fromClauseValue->tableRename = calloc(1, sizeof(TableRename));
	clauseValue->fromClauseValue->tableRename->name = trueName;
	clauseValue->fromClauseValue->tableRename->rename = alias;
	clauseValue->fromClauseValue->fromClauseValueType = TABLE_RENAME;
	clauseValue->clauseType = FROM_CLAUSE;
	return clauseValue;
}

// ATTRIBUTES CLAUSE VALUES

AttributesClauseValue *EmptyAttributeOptionSemanticAction()
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	return calloc(1, sizeof(AttributesClauseValue));
}

CompositeOrderByClause *OrderBySemanticAction(OrderByType orderByType)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	CompositeOrderByClause *orderBy = calloc(1, sizeof(CompositeOrderByClause));
	orderBy->orderByFunctionType = orderByType;
	if (orderByType == ASC_T)
	{
		orderBy->string = NULL;
		orderBy->orderByFunctionType = ORDER_BY_ASC;
	}
	else if (orderByType == DESC_T)
	{
		orderBy->string = NULL;
		orderBy->orderByFunctionType = ORDER_BY_DESC;
	}
	return orderBy;
}

ClauseValue *StringOrderByClauseValueSemanticAction(char *string)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	OrderByClauseValue *orderBy = calloc(1, sizeof(OrderByClauseValue));
	clauseValue->clauseType = ORDER_BY_CLAUSE;
	clauseValue->orderByClauseValue = orderBy;
	orderBy->orderByClauseValueType = ORDER_BY_STRING;
	orderBy->string = string;
	return clauseValue;
}

ClauseValue *CompositeOrderByClauseValueSemanticAction(char *string, CompositeOrderByClause *compositeOrderByClause)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	OrderByClauseValue *orderBy = calloc(1, sizeof(OrderByClauseValue));
	clauseValue->clauseType = ORDER_BY_CLAUSE;
	clauseValue->orderByClauseValue = orderBy;
	orderBy->orderByClauseValueType = ORDER_BY_COMPOSITE;
	orderBy->compositeOrderByClause = compositeOrderByClause;
	orderBy->compositeOrderByClause->string = string;
	return clauseValue;
}

ClauseValue *AggregationFunctionOrderByClauseValueSemanticAction(char *aggrFunc, char *string, CompositeOrderByClause *compositeOrderByClause)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->clauseType = ORDER_BY_CLAUSE;
	clauseValue->orderByClauseValue = calloc(1, sizeof(OrderByClauseValue));
	clauseValue->orderByClauseValue->orderByClauseValueType = ORDER_BY_AGGR_FUNC;
	clauseValue->orderByClauseValue->compositeOrderByClause = compositeOrderByClause;
	clauseValue->orderByClauseValue->compositeOrderByClause->aggrFunc = aggrFunc;
	clauseValue->orderByClauseValue->compositeOrderByClause->string = string;
	return clauseValue;
}

AttributeRename *AttributeRenameSemanticAction(ClauseValue *clausevalue, char *string)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeRename *attrRename = calloc(1, sizeof(AttributeRename));
	attrRename->value = clausevalue;
	attrRename->rename = string;
	return attrRename;
}

AttributesClauseValue *OnlyTableAttributeOptionSemanticAction(char *table)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributesClauseValue *clauseValue = calloc(1, sizeof(AttributesClauseValue));
	clauseValue->table = table;
	return clauseValue;
}

AttributesClauseValue *OnlyAsAttributeOptionSemanticAction(char *rename)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributesClauseValue *clauseValue = calloc(1, sizeof(AttributesClauseValue));
	clauseValue->rename = rename;
	return clauseValue;
}

AttributesClauseValue *TableAndAsAttributeOptionSemanticAction(char *table, char *rename)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributesClauseValue *clauseValue = calloc(1, sizeof(AttributesClauseValue));
	clauseValue->table = table;
	clauseValue->rename = rename;
	return clauseValue;
}

AttributesClauseValue *AttributeSemanticAction(char *name, AttributesClauseValue *clauseValue)
{
	clauseValue->name = name;
	return clauseValue;
}

AttributesClauseValue *AggregationSemanticAction(char *function, AttributesClauseValue *clauseValue)
{
	clauseValue->aggregationFunction = function;
	return clauseValue;
}

// WHERE CONDITION VALUES

WhereConditionValue *StringWhereConditionValueSemanticAction(char *string)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->string = string;
	whereConditionValue->type = CONDITION_VALUE_STRING;
	return whereConditionValue;
}

WhereConditionValue *IntegerWhereConditionValueSemanticAction(int integer)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->integer = integer;
	whereConditionValue->type = CONDITION_VALUE_INTEGER;
	return whereConditionValue;
}
WhereConditionValue *NumberWhereConditionValueSemanticAction(float number)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->number = number;
	whereConditionValue->type = CONDITION_VALUE_NUMBER;
	return whereConditionValue;
}
WhereConditionValue *BooleanWhereConditionValueSemanticAction(boolean bool)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->bool = bool;
	whereConditionValue->type = CONDITION_VALUE_BOOLEAN;
	return whereConditionValue;
}
WhereConditionValue *NullWhereConditionValueSemanticAction()
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->type = CONDITION_VALUE_NULL;
	return whereConditionValue;
}
WhereConditionValue *AttributeWhereConditionValueSemanticAction(char *attribute)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->attribute = attribute;
	whereConditionValue->type = CONDITION_VALUE_ATTRIBUTE;
	return whereConditionValue;
}
WhereConditionValue *AggregationFunctionWhereConditionValueSemanticAction(char *aggr, char *attribute)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->aggregationFunction = calloc(1, sizeof(AggregationFunction));
	whereConditionValue->aggregationFunction->aggr = aggr;
	whereConditionValue->aggregationFunction->attribute = attribute;
	whereConditionValue->type = CONDITION_VALUE_AGGREGATION_FUNCTION;
	return whereConditionValue;
}

WhereConditionValue *WhereBinaryConditionWhereConditionValueSemanticAction(WhereBinaryCondition *whereBinaryCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereConditionValue *whereConditionValue = calloc(1, sizeof(WhereConditionValue));
	whereConditionValue->binaryCondition = whereBinaryCondition;
	whereConditionValue->type = CONDITION_VALUE_BINARY_CONDITION;
	return whereConditionValue;
}

WhereBinaryCondition *WhereBinaryConditionSemanticAction(WhereConditionValue *value1, WhereConditionValue *value2, BinaryConditionOperator operator)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereBinaryCondition *whereBinaryCondition = calloc(1, sizeof(WhereBinaryCondition));
	whereBinaryCondition->value1 = value1;
	whereBinaryCondition->value2 = value2;
	whereBinaryCondition->operator = operator;
	return whereBinaryCondition;
}

WhereNotCondition *WhereNotConditionWithBinaryConditionSemanticAction(WhereBinaryCondition *whereBinaryCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereNotCondition *newWhereNotCondition = calloc(1, sizeof(WhereNotCondition));
	newWhereNotCondition->nodeSelected = BINARY_CONDITION_NODE;
	newWhereNotCondition->whereBinaryCondition = whereBinaryCondition;
	return newWhereNotCondition;
}

WhereNotCondition *WhereNotConditionWithNotConditionSemanticAction(WhereNotCondition *whereNotCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereNotCondition *newWhereNotCondition = calloc(1, sizeof(WhereNotCondition));
	newWhereNotCondition->nodeSelected = NOT_NODE;
	newWhereNotCondition->not = whereNotCondition;
	return newWhereNotCondition;
}

WhereNotCondition *WhereNotConditionWithInConditionSemanticAction(WhereInCondition *whereInCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereNotCondition *newWhereNotCondition = calloc(1, sizeof(WhereNotCondition));
	newWhereNotCondition->nodeSelected = IN_NODE;
	newWhereNotCondition->in = whereInCondition;
	return newWhereNotCondition;
}

WhereNotCondition *WhereNotConditionWithWhereConditionSemanticAction(WhereCondition *whereCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereNotCondition *newWhereNotCondition = calloc(1, sizeof(WhereNotCondition));
	newWhereNotCondition->nodeSelected = CONDITION_NODE;
	newWhereNotCondition->condition = whereCondition;
	return newWhereNotCondition;
}

WhereCondition *FirstNotConditionAndNextWhereConditionSemanticAction(WhereNotCondition *whereNotCondition, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = NotConditionAndNextWhereConditionSemanticAction(whereNotCondition, next);
	whereCondition->preconditional = PRECONDITIONAL_FIRST;
	return whereCondition;
}

WhereCondition *CurrentAndNextWhereConditionsSemanticAction(WhereCondition *current, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = calloc(1, sizeof(WhereCondition));
	whereCondition->whereCondition = current;
	whereCondition->nodeType = NODE_TYPE_WHERE_CONDITION;
	whereCondition->next = next;
	return whereCondition;
}

WhereCondition *BinaryConditionAndNextWhereConditionSemanticAction(WhereBinaryCondition *condition, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = calloc(1, sizeof(WhereCondition));
	whereCondition->binaryCondition = condition;
	whereCondition->nodeType = NODE_TYPE_WHERE_BINARY_CONDITION;
	whereCondition->next = next;
	return whereCondition;
}

WhereCondition *NotConditionAndNextWhereConditionSemanticAction(WhereNotCondition *condition, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = calloc(1, sizeof(WhereCondition));
	whereCondition->whereNotCondition = condition;
	whereCondition->nodeType = NODE_TYPE_WHERE_NOT_CONDITION;
	whereCondition->next = next;
	return whereCondition;
}

WhereCondition *IsConditionAndNextWhereConditionSemanticAction(WhereIsCondition *condition, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = calloc(1, sizeof(WhereCondition));
	whereCondition->whereIsCondition = condition;
	whereCondition->nodeType = NODE_TYPE_WHERE_IS_CONDITION;
	whereCondition->next = next;
	return whereCondition;
}

WhereCondition *InConditionAndNextWhereConditionSemanticAction(WhereInCondition *condition, WhereCondition *next)
{

	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = calloc(1, sizeof(WhereCondition));
	whereCondition->whereInCondition = condition;
	whereCondition->nodeType = NODE_TYPE_WHERE_IN_CONDITION;
	whereCondition->next = next;
	return whereCondition;
}

WhereCondition *PreconditionalWhereConditionSemanticAction(WhereCondition *node, WhereConditionPreconditional preconditional)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	node->preconditional = preconditional;
	return node;
}

WhereCondition *FirstCurrentAndNextWhereConditionsSemanticAction(WhereCondition *current, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = CurrentAndNextWhereConditionsSemanticAction(current, next);
	whereCondition->preconditional = PRECONDITIONAL_FIRST;
	return whereCondition;
}

WhereCondition *FirstBinaryConditionAndNextWhereConditionSemanticAction(WhereBinaryCondition *condition, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = BinaryConditionAndNextWhereConditionSemanticAction(condition, next);
	whereCondition->preconditional = PRECONDITIONAL_FIRST;
	return whereCondition;
}

WhereCondition *FirstIsConditionAndNextWhereConditionSemanticAction(WhereIsCondition *whereIsCondition, WhereCondition *next)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = IsConditionAndNextWhereConditionSemanticAction(whereIsCondition, next);
	whereCondition->preconditional = PRECONDITIONAL_FIRST;
	return whereCondition;
}

WhereCondition *FirstInConditionAndNextWhereConditionSemanticAction(WhereInCondition *whereInCondition, WhereCondition *next)
{

	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereCondition *whereCondition = InConditionAndNextWhereConditionSemanticAction(whereInCondition, next);
	whereCondition->preconditional = PRECONDITIONAL_FIRST;
	return whereCondition;
}

WhereInCondition *QueryWhereInConditionSemanticAction(char *attribute, Json *query)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereInCondition *whereInCondition = calloc(1, sizeof(WhereInCondition));
	whereInCondition->query = query;
	whereInCondition->attribute = attribute;
	whereInCondition->type = WHERE_IN_CONDITION_QUERY;
	return whereInCondition;
}

WhereInCondition *AuxiliaryQueryWhereInConditionSemanticAction(char *attribute, char *auxQueryName)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereInCondition *whereInCondition = calloc(1, sizeof(WhereInCondition));
	whereInCondition->auxQueryName = auxQueryName;
	whereInCondition->attribute = attribute;
	whereInCondition->type = WHERE_IN_CONDITION_AUX_QUERY_NAME;
	return whereInCondition;
}

WhereIsCondition *WhereNotConditionWhereIsConditionSemanticAction(WhereNotCondition *whereNotCondition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	WhereIsCondition *whereIsCondition = calloc(1, sizeof(WhereIsCondition));
	whereIsCondition->whereNotCondition = whereNotCondition;
	whereIsCondition->type = WHERE_IS_CONDITION_NOT;
	return whereIsCondition;
}

// GROUP BY CLAUSE VALUES

ClauseArgsList *StringGroupByClauseSemanticAction(char *string)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList *clauseArgsList = calloc(1, sizeof(ClauseArgsList));
	clauseArgsList->clauseValue = calloc(1, sizeof(ClauseValue));
	clauseArgsList->clauseValue->groupByClauseValue = calloc(1, sizeof(GroupByClauseValue));
	clauseArgsList->clauseValue->groupByClauseValue->string = string;
	clauseArgsList->clauseValue->clauseType = GROUP_BY_CLAUSE;
	return clauseArgsList;
}

ClauseValue *GroupByValueSemanticAction(char *string)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->groupByClauseValue = calloc(1, sizeof(GroupByClauseValue));
	clauseValue->groupByClauseValue->string = string;
	clauseValue->clauseType = GROUP_BY_CLAUSE;
	return clauseValue;
}

// AUXILIARY CLAUSE
ClauseArgsList *SingleQueryAuxiliaryClauseArgsListSemanticAction(char *auxQueryName, Json *auxQuery)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList *auxiliaryArgsList = calloc(1, sizeof(ClauseArgsList));
	auxiliaryArgsList->clauseValue = calloc(1, sizeof(ClauseValue));
	auxiliaryArgsList->clauseValue->auxiliaryClauseValue = calloc(1, sizeof(AuxiliaryClauseValue));
	auxiliaryArgsList->clauseValue->auxiliaryClauseValue->auxQueryName = auxQueryName;
	auxiliaryArgsList->clauseValue->auxiliaryClauseValue->auxQuery = auxQuery;
	auxiliaryArgsList->clauseValue->clauseType = AUXILIARY_CLAUSE;
	return auxiliaryArgsList;
}

ClauseArgsList *MultipleQueriesAuxiliaryClauseArgsListSemanticAction(char *auxQueryName, Json *auxQuery, ClauseArgsList *auxQueriesList)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList *auxiliaryArgsList = calloc(1, sizeof(ClauseArgsList));
	auxiliaryArgsList->clauseValue = calloc(1, sizeof(ClauseValue));
	auxiliaryArgsList->clauseValue->auxiliaryClauseValue = calloc(1, sizeof(AuxiliaryClauseValue));
	auxiliaryArgsList->clauseValue->auxiliaryClauseValue->auxQueryName = auxQueryName;
	auxiliaryArgsList->clauseValue->auxiliaryClauseValue->auxQuery = auxQuery;
	auxiliaryArgsList->next = auxQueriesList;
	auxiliaryArgsList->clauseValue->clauseType = AUXILIARY_CLAUSE;
	return auxiliaryArgsList;
}

// JOIN CLAUSE VALUE
ClauseValue *JoinClauseValueSemanticAction(char *table1, char *table2, char *joinType, boolean outer, WhereBinaryCondition *condition)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue *clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->joinClauseValue = calloc(1, sizeof(JoinClauseValue));
	clauseValue->joinClauseValue->condition = condition;
	clauseValue->joinClauseValue->outer = outer;
	clauseValue->joinClauseValue->type = joinType;
	clauseValue->joinClauseValue->table1 = table1;
	clauseValue->joinClauseValue->table2 = table2;
	clauseValue->clauseType = JOIN_CLAUSE;
	return clauseValue;
}
