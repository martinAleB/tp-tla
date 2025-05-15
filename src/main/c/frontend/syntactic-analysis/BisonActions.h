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

//JSON
Program * JsonProgramSemanticAction(CompilerState *compilerState, Json *json);
Json * JsonSemanticAction(ClauseList * clauseList);

//CLAUSES
ClauseList * ClauseListSemanticAction(Clause * clause, ClauseList * clauseList);
ClauseArgsList * ClauseArgsListSemanticAction(ClauseValue * fromClauseValue, ClauseArgsList * fromAClauseArgsList);
Clause * ClauseSemanticAction(ClauseArgsList * newClauseArgsList, Token token);

//FROM CLAUSE VALUES
ClauseValue * StringFromClauseValueSemanticAction(char * string);
ClauseValue * TableRenameFromClauseValueSemanticAction(char * trueName, char * alias);

//ATTRIBUTES CLAUSE VALUES
AttributeRename * AttributeRenameSemanticAction(ClauseValue * clausevalue, char * string);
AggregationFunction * AggregationFunctionSemanticAction(AggregationType token);
ClauseValue * AttributeRenameAttributesClauseValueSemanticAction(AttributeRename * attrRename);
ClauseValue * AggregationFunctionAttributesClauseValueSemanticAction(AggregationFunction * aggrFunc, char * string);
ClauseValue * StringAttributesClauseValueSemanticAction(char * string);

//GROUP BY CLAUSE VALUES
ClauseArgsList * StringGroupByClauseSemanticAction(char * string);
ClauseValue * GroupByValueSemanticAction(char * string);

ClauseValue * StringOrderByClauseValueSemanticAction(char * string);
ClauseValue * AggregationFunctionOrderByClauseValueSemanticAction(AggregationFunction * aggrFunc, char * string, CompositeOrderByClause * compositeOrderByClause);
ClauseValue * CompositeOrderByClauseValueSemanticAction(char * string, CompositeOrderByClause * compositeOrderByClause);
CompositeOrderByClause * OrderBySemanticAction(OrderByType orderByType);

#endif
