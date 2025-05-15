#include "BisonActions.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeBisonActionsModule() {
	_logger = createLogger("BisonActions");
}

void shutdownBisonActionsModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** IMPORTED FUNCTIONS */

extern unsigned int flexCurrentContext(void);

/* PRIVATE FUNCTIONS */

static void _logSyntacticAnalyzerAction(const char * functionName);

/**
 * Logs a syntactic-analyzer action in DEBUGGING level.
 */
static void _logSyntacticAnalyzerAction(const char * functionName) {
	logDebugging(_logger, "%s", functionName);
}

/* PUBLIC FUNCTIONS */

Constant * IntegerConstantSemanticAction(const int value) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Constant * constant = calloc(1, sizeof(Constant));
	constant->value = value;
	return constant;
}

Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->leftExpression = leftExpression;
	expression->rightExpression = rightExpression;
	expression->type = type;
	return expression;
}

Expression * FactorExpressionSemanticAction(Factor * factor) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Expression * expression = calloc(1, sizeof(Expression));
	expression->factor = factor;
	expression->type = FACTOR;
	return expression;
}

Factor * ConstantFactorSemanticAction(Constant * constant) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->constant = constant;
	factor->type = CONSTANT;
	return factor;
}

Factor * ExpressionFactorSemanticAction(Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Factor * factor = calloc(1, sizeof(Factor));
	factor->expression = expression;
	factor->type = EXPRESSION;
	return factor;
}

Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Program * program = calloc(1, sizeof(Program));
	program->expression = expression;
	program->type = PROGRAM_EXPRESSION;
	compilerState->abstractSyntaxtTree = program;
	if (0 < flexCurrentContext()) {
		logError(_logger, "The final context is not the default (0): %d", flexCurrentContext());
		compilerState->succeed = false;
	}
	else {
		compilerState->succeed = true;
	}
	return program;
}


//Our own Bison Actions


//JSON

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

Json *JsonSemanticAction(ClauseList * clauseList)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Json *json = calloc(1, sizeof(Json));
	json->clauseList = clauseList;
	return json;
}

//CLAUSES (GENERAL)

ClauseList * ClauseListSemanticAction(Clause * clause, ClauseList * clauseList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseList * newClauseList = calloc(1, sizeof(ClauseList));
	newClauseList->clause = clause;
	newClauseList->next = clauseList;
	return newClauseList;
}

Clause * ClauseSemanticAction(ClauseArgsList * newClauseArgsList, Token token) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Clause * clause = calloc(1, sizeof(Clause));
	clause->type = token;
	clause->clauseArgsList = newClauseArgsList;
	return clause;
}

ClauseArgsList * ClauseArgsListSemanticAction(ClauseValue * clauseValue, ClauseArgsList * clauseValuesNext) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList * clauseArgsList = calloc(1, sizeof(ClauseArgsList));
	clauseArgsList->clauseValue = clauseValue;
	clauseArgsList->next = clauseValuesNext;
	return clauseArgsList;
}

//FROM CLAUSE VALUES

ClauseValue * StringFromClauseValueSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->fromClauseValue = calloc(1, sizeof(FromClauseValue));
	clauseValue->fromClauseValue->string = string;
	clauseValue->clauseType = FROM_CLAUSE;
	return clauseValue;
}

ClauseValue * TableRenameFromClauseValueSemanticAction(char * trueName, char * alias) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->fromClauseValue = calloc(1, sizeof(FromClauseValue));
	clauseValue->fromClauseValue->tableRename = calloc(1, sizeof(TableRename));
	clauseValue->fromClauseValue->tableRename->name = trueName;
	clauseValue->fromClauseValue->tableRename->rename = alias;
	clauseValue->fromClauseValue->fromClauseValueType = TABLE_RENAME;
	clauseValue->clauseType = FROM_CLAUSE;
	return clauseValue;
}

//ATTRIBUTES CLAUSE VALUES

AggregationFunction * AggregationFunctionSemanticAction(AggregationType aggrType) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AggregationFunction * aggrFunc = calloc(1, sizeof(AggregationFunction));
	aggrFunc->token = aggrType;
	return aggrFunc;
}

AttributeRename * AttributeRenameSemanticAction(ClauseValue * clausevalue, char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	AttributeRename * attrRename = calloc(1, sizeof(AttributeRename));
	attrRename->value = clausevalue;
	attrRename->rename = string;
	return attrRename;
}

ClauseValue * AggregationFunctionAttributesClauseValueSemanticAction(AggregationFunction * aggrFunc, char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->attributesClauseValue = calloc(1, sizeof(AttributesClauseValue));
	clauseValue->attributesClauseValue->aggrFunc = aggrFunc;
	clauseValue->attributesClauseValue->aggrFunc->attribute = string;
	clauseValue->attributesClauseValue->attributeClauseValueType = AGGR_FUNC;
	clauseValue->clauseType = ATTRIBUTES_CLAUSE;
	return clauseValue;
}

ClauseValue * AttributeRenameAttributesClauseValueSemanticAction(AttributeRename * attrRename) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->attributesClauseValue = calloc(1, sizeof(AttributesClauseValue));
	clauseValue->attributesClauseValue->attrRename = attrRename;
	clauseValue->attributesClauseValue->attributeClauseValueType = ATTR_RENAME;
	clauseValue->clauseType = ATTRIBUTES_CLAUSE;
	return clauseValue;
}

ClauseValue * StringAttributesClauseValueSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->attributesClauseValue = calloc(1, sizeof(AttributesClauseValue));
	clauseValue->attributesClauseValue->string = string;
	clauseValue->attributesClauseValue->attributeClauseValueType = ATTR_STR;
	clauseValue->clauseType = ATTRIBUTES_CLAUSE;
	return clauseValue;
}

// GROUP BY CLAUSE VALUES

ClauseArgsList * StringGroupByClauseSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList * clauseArgsList = calloc(1, sizeof(ClauseArgsList));
	clauseArgsList->clauseValue = calloc(1, sizeof(ClauseValue));
	clauseArgsList->clauseValue->groupByClauseValue = calloc(1, sizeof(GroupByClauseValue));
	clauseArgsList->clauseValue->groupByClauseValue->string = string;
	clauseArgsList->clauseValue->clauseType = GROUP_BY_CLAUSE;
	return clauseArgsList;
}

ClauseValue * GroupByValueSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * clauseValue = calloc(1, sizeof(ClauseValue));
	clauseValue->groupByClauseValue = calloc(1, sizeof(GroupByClauseValue));
	clauseValue->groupByClauseValue->string = string;
	clauseValue->clauseType = GROUP_BY_CLAUSE;
	return clauseValue;
}