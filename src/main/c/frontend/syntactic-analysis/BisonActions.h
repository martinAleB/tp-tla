#ifndef BISON_ACTIONS_HEADER
#define BISON_ACTIONS_HEADER

#include "../../shared/CompilerState.h"
#include "../../shared/Logger.h"
#include "../../shared/Type.h"
#include "AbstractSyntaxTree.h"
#include "SyntacticAnalyzer.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeBisonActionsModule();

/** Shutdown module's internal state. */
void shutdownBisonActionsModule();

/**
 * Bison semantic actions.
 */

Constant * IntegerConstantSemanticAction(const int value);
Expression * ArithmeticExpressionSemanticAction(Expression * leftExpression, Expression * rightExpression, ExpressionType type);
Expression * FactorExpressionSemanticAction(Factor * factor);
Factor * ConstantFactorSemanticAction(Constant * constant);
Factor * ExpressionFactorSemanticAction(Expression * expression);
Program * ExpressionProgramSemanticAction(CompilerState * compilerState, Expression * expression);
Json * JsonSemanticAction();
Program * JsonProgramSemanticAction(CompilerState *compilerState, Json *json);
ClauseList * ClauseListSemanticAction(Clause * clause, ClauseList * clauseList);
Clause * FromClauseSemanticAction(ClauseArgsList * fromClauseArgsList);
ClauseValue * FromClauseValueSemanticAction(char * string);
ClauseArgsList * ClauseArgsListSemanticAction(ClauseValue * fromClauseValue, ClauseArgsList * fromAClauseArgsList);

#endif
