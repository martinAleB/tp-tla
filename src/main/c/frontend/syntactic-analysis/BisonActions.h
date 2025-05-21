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

Constant *IntegerConstantSemanticAction(const int value);
Expression *ArithmeticExpressionSemanticAction(Expression *leftExpression, Expression *rightExpression, ExpressionType type);
Expression *FactorExpressionSemanticAction(Factor *factor);
Factor *ConstantFactorSemanticAction(Constant *constant);
Factor *ExpressionFactorSemanticAction(Expression *expression);
Program *ExpressionProgramSemanticAction(CompilerState *compilerState, Expression *expression);

// JSON
Program *JsonProgramSemanticAction(CompilerState *compilerState, Json *json);
Json *JsonSemanticAction(ClauseList *clauseList);

// CLAUSES
ClauseList *ClauseListSemanticAction(Clause *clause, ClauseList *clauseList);
ClauseArgsList *ClauseArgsListSemanticAction(ClauseValue *fromClauseValue, ClauseArgsList *fromAClauseArgsList);
Clause *ClauseSemanticAction(ClauseArgsList *newClauseArgsList, Token token);

// FROM CLAUSE VALUES
ClauseValue *StringFromClauseValueSemanticAction(char *string);
ClauseValue *TableRenameFromClauseValueSemanticAction(char *trueName, char *alias);

// AUXILIARY CLAUSE VALUES
ClauseArgsList *SingleQueryAuxiliaryClauseArgsListSemanticAction(char *auxQueryName, Json *json);
ClauseArgsList *MultipleQueriesAuxiliaryClauseArgsListSemanticAction(char *auxQueryName, Json *json, ClauseArgsList *jsonList);

// ATTRIBUTES CLAUSE VALUES
AttributeRename *AttributeRenameSemanticAction(ClauseValue *clausevalue, char *string);
ClauseValue *AttributeRenameAttributesClauseValueSemanticAction(AttributeRename *attrRename);
ClauseValue *AggregationFunctionAttributesClauseValueSemanticAction(AggregationFunction *aggrFunc, char *string);
ClauseValue *StringAttributesClauseValueSemanticAction(char *string);
AttributesClauseValue *OnlyTableAttributeOptionSemanticAction(char *table);
AttributesClauseValue *OnlyAsAttributeOptionSemanticAction(char *rename);
AttributesClauseValue *TableAndAsAttributeOptionSemanticAction(char *table, char *rename);
AttributesClauseValue *AttributeSemanticAction(char *name, AttributesClauseValue *clauseValue);
AttributesClauseValue *AggregationSemanticAction(char *function, AttributesClauseValue *clauseValue);
ClauseValue *AttributeClauseValueSemanticAction(AttributesClauseValue *attributeClauseValue);
AttributesClauseValue *EmptyAttributeOptionSemanticAction();

// WHERE CLAUSE
Clause *WhereClauseSemanticAction(WhereCondition *whereCondition);
WhereConditionValue *StringWhereConditionValueSemanticAction(char *string);
WhereConditionValue *IntegerWhereConditionValueSemanticAction(int integer);
WhereConditionValue *NumberWhereConditionValueSemanticAction(float number);
WhereConditionValue *BooleanWhereConditionValueSemanticAction(boolean bool);
WhereConditionValue *NullWhereConditionValueSemanticAction();
WhereConditionValue *AttributeWhereConditionValueSemanticAction(char *attribute);
WhereConditionValue *AggregationFunctionWhereConditionValueSemanticAction(char *aggr, char *attribute);
WhereConditionValue *WhereBinaryConditionWhereConditionValueSemanticAction(WhereBinaryCondition *whereBinaryCondition);
WhereBinaryCondition *WhereBinaryConditionSemanticAction(WhereConditionValue *value1, WhereConditionValue *value2, BinaryConditionOperator operator);

WhereNotCondition *WhereNotConditionWithBinaryConditionSemanticAction(WhereBinaryCondition *whereBinaryCondition);
WhereNotCondition *WhereNotConditionWithNotConditionSemanticAction(WhereNotCondition *whereNotCondition);
WhereNotCondition *WhereNotConditionWithWhereConditionSemanticAction(WhereCondition *whereCondition);
WhereNotCondition *WhereNotConditionWithInConditionSemanticAction(WhereInCondition *whereInCondition);

WhereCondition *CurrentAndNextWhereConditionsSemanticAction(WhereCondition *current, WhereCondition *next);
WhereCondition *BinaryConditionAndNextWhereConditionSemanticAction(WhereBinaryCondition *condition, WhereCondition *next);
WhereCondition *NotConditionAndNextWhereConditionSemanticAction(WhereNotCondition *condition, WhereCondition *next);
WhereCondition *IsConditionAndNextWhereConditionSemanticAction(WhereIsCondition *condition, WhereCondition *next);
WhereCondition *InConditionAndNextWhereConditionSemanticAction(WhereInCondition *condition, WhereCondition *next);

WhereCondition *PreconditionalWhereConditionSemanticAction(WhereCondition *node, WhereConditionPreconditional preconditional);
WhereCondition *FirstCurrentAndNextWhereConditionsSemanticAction(WhereCondition *current, WhereCondition *next);
WhereCondition *FirstBinaryConditionAndNextWhereConditionSemanticAction(WhereBinaryCondition *condition, WhereCondition *next);
WhereCondition *FirstNotConditionAndNextWhereConditionSemanticAction(WhereNotCondition *whereNotCondition, WhereCondition *next);
WhereCondition *FirstIsConditionAndNextWhereConditionSemanticAction(WhereIsCondition *whereIsCondition, WhereCondition *next);
WhereCondition *FirstInConditionAndNextWhereConditionSemanticAction(WhereInCondition *whereInCondition, WhereCondition *next);

WhereInCondition *QueryWhereInConditionSemanticAction(char *attribute, Json *query);
WhereInCondition *AuxiliaryQueryWhereInConditionSemanticAction(char *attribute, char *auxQueryName);
WhereIsCondition *WhereNotConditionWhereIsConditionSemanticAction(WhereNotCondition *whereNotCondition);
WhereIsCondition *WhereInConditionWhereIsConditionSemanticAction(WhereInCondition *whereInCondition);

// GROUP BY CLAUSE VALUES
ClauseArgsList *StringGroupByClauseSemanticAction(char *string);
ClauseValue *GroupByValueSemanticAction(char *string);

ClauseValue *StringOrderByClauseValueSemanticAction(char *string);
ClauseValue *AggregationFunctionOrderByClauseValueSemanticAction(char *aggrFunc, char *string, CompositeOrderByClause *compositeOrderByClause);
ClauseValue *CompositeOrderByClauseValueSemanticAction(char *string, CompositeOrderByClause *compositeOrderByClause);
CompositeOrderByClause *OrderBySemanticAction(OrderByType orderByType);

// JOIN CLAUSE VALUES
ClauseValue *JoinClauseValueSemanticAction(char *table1, char *table2, char *joinType, boolean outer, WhereBinaryCondition *condition);

#endif
