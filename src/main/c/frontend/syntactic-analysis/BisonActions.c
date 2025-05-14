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
Json *JsonSemanticAction(ClauseList * clauseList)
{
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Json *json = calloc(1, sizeof(Json));
	json->clauseList = clauseList;
	return json;
}

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

ClauseList * ClauseListSemanticAction(Clause * clause, ClauseList * clauseList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseList * newClauseList = calloc(1, sizeof(ClauseList));
	newClauseList->clause = clause;
	newClauseList->next = clauseList;
	return newClauseList;
}

ClauseValue * FromClauseValueSemanticAction(char * string) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseValue * fromClauseValue = calloc(1, sizeof(ClauseValue));
	fromClauseValue->string = string;
	fromClauseValue->clauseType = FROM_CLAUSE;
	return fromClauseValue;
}

ClauseArgsList * ClauseArgsListSemanticAction(ClauseValue * clauseValue, ClauseArgsList * clauseValuesNext) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	ClauseArgsList * clauseArgsList = calloc(1, sizeof(ClauseArgsList));
	clauseArgsList->clauseValue = clauseValue;
	clauseArgsList->next = clauseValuesNext;
	return clauseArgsList;
}

Clause * FromClauseSemanticAction(ClauseArgsList * clauseArgsList) {
	_logSyntacticAnalyzerAction(__FUNCTION__);
	Clause * clause = calloc(1, sizeof(Clause));
	clause->type = FROM_CLAUSE;
	clause->fromClauseArgsList = clauseArgsList;
	return clause;
}
