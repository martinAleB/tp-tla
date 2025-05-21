#ifndef ABSTRACT_SYNTAX_TREE_HEADER
#define ABSTRACT_SYNTAX_TREE_HEADER

#include "../../shared/Logger.h"
#include <stdlib.h>

/** Initialize module's internal state. */
void initializeAbstractSyntaxTreeModule();

/** Shutdown module's internal state. */
void shutdownAbstractSyntaxTreeModule();

/**
 * This typedefs allows self-referencing types.
 */

typedef enum ExpressionType ExpressionType;
typedef enum FactorType FactorType;
typedef enum ProgramType ProgramType;

typedef enum ClauseType ClauseType;
typedef enum FromClauseValueType FromClauseValueType;
typedef enum AggregationType AggregationType;
typedef enum OrderByType OrderByType;
typedef enum OrderByFunctionType OrderByFunctionType;
typedef enum OrderByClauseValueType OrderByClauseValueType;
typedef enum BooleanTypes BooleanTypes;
typedef enum ConditionValueType ConditionValueType;
typedef enum BinaryConditionOperator BinaryConditionOperator;
typedef enum WhereConditionPreconditional WhereConditionPreconditional;
typedef enum WhereConditionNodeType WhereConditionNodeType;
typedef enum NotNodeSelected NotNodeSelected;
typedef enum WhereInConditionType WhereInConditionType;

typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;

typedef struct Program Program;
typedef struct Json Json;
typedef struct FromClauseValue FromClauseValue;
typedef struct TableRename TableRename;
typedef struct AttributeRename AttributeRename;
typedef struct AggregationFunction AggregationFunction;
typedef struct AttributesClauseValue AttributesClauseValue;
typedef struct ClauseValue ClauseValue;
typedef struct ClauseArgsList ClauseArgsList;
typedef struct ClauseList ClauseList;
typedef struct Clause Clause;
typedef struct GroupByClauseValue GroupByClauseValue;
typedef struct OrderByClauseValue OrderByClauseValue;
typedef struct CompositeOrderByClause CompositeOrderByClause;
typedef struct WhereCondition WhereCondition;
typedef struct WhereNotCondition WhereNotCondition;
typedef struct WhereConditionValue WhereConditionValue;
typedef struct WhereBinaryCondition WhereBinaryCondition;
typedef struct WhereInCondition WhereInCondition;
typedef struct WhereIsCondition WhereIsCondition;
typedef struct AuxiliaryClauseValue AuxiliaryClauseValue;
typedef struct JoinClauseValue JoinClauseValue;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum OrderByType
{
	ASC_T,
	DESC_T
};

enum OrderByFunctionType
{
	ORDER_BY_ASC,
	ORDER_BY_DESC
};

enum OrderByClauseValueType
{
	ORDER_BY_STRING,
	ORDER_BY_COMPOSITE,
	ORDER_BY_AGGR_FUNC
};

enum ExpressionType
{
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType
{
	CONSTANT,
	EXPRESSION
};

enum ProgramType
{
	PROGRAM_EXPRESSION,
	JSON
};

enum ClauseType
{
	FROM_CLAUSE,
	ATTRIBUTES_CLAUSE,
	GROUP_BY_CLAUSE,
	ORDER_BY_CLAUSE,
	WHERE_CLAUSE,
	AUXILIARY_CLAUSE,
	JOIN_CLAUSE
};

enum FromClauseValueType
{
	STR,
	TABLE_RENAME
};

enum BinaryConditionOperator
{
	BINARY_CONDITION_OPERATOR_LOWER,
	BINARY_CONDITION_OPERATOR_GREATER,
	BINARY_CONDITION_OPERATOR_LOWER_OR_EQUAL,
	BINARY_CONDITION_OPERATOR_GREATHER_OR_EQUAL,
	BINARY_CONDITION_OPERATOR_EQUAL,
	BINARY_CONDITION_OPERATOR_NOT_EQUAL
};

enum ConditionValueType
{
	CONDITION_VALUE_STRING,
	CONDITION_VALUE_INTEGER,
	CONDITION_VALUE_NUMBER,
	CONDITION_VALUE_BOOLEAN,
	CONDITION_VALUE_NULL,
	CONDITION_VALUE_BINARY_CONDITION,
	CONDITION_VALUE_NOT_CONDITION,
	CONDITION_VALUE_ATTRIBUTE,
	CONDITION_VALUE_AGGREGATION_FUNCTION
};

enum WhereConditionPreconditional
{
	PRECONDITIONAL_AND,
	PRECONDITIONAL_OR,
	PRECONDITIONAL_FIRST
};

enum WhereConditionNodeType
{
	NODE_TYPE_WHERE_BINARY_CONDITION,
	NODE_TYPE_WHERE_CONDITION,
	NODE_TYPE_WHERE_NOT_CONDITION,
	NODE_TYPE_WHERE_IS_CONDITION,
	NODE_TYPE_WHERE_IN_CONDITION
};

enum NotNodeSelected
{
	IN_NODE,
	NOT_NODE,
	CONDITION_NODE,
	BINARY_CONDITION_NODE
};

enum WhereInConditionType
{
	WHERE_IN_CONDITION_QUERY,
	WHERE_IN_CONDITION_AUX_QUERY_NAME
};

struct Constant
{
	int value;
};

struct Factor
{
	union
	{
		Constant *constant;
		Expression *expression;
	};
	FactorType type;
};

struct TableRename
{
	char *name;
	char *rename;
};

struct AttributeRename
{
	ClauseValue *value;
	char *rename;
};

struct AggregationFunction
{
	char *aggr;
	char *attribute;
};
struct FromClauseValue
{
	union
	{
		char *string;
		TableRename *tableRename;
	};
	FromClauseValueType fromClauseValueType;
};

struct JoinClauseValue
{
	char *table1;
	char *table2;
	char *type;
	boolean outer;
	WhereBinaryCondition *condition;
};

struct AuxiliaryClauseValue
{
	Json *auxQuery;
	char *auxQueryName;
};

struct CompositeOrderByClause
{
	char *string;
	char *aggrFunc;
	OrderByFunctionType orderByFunctionType;
};

struct OrderByClauseValue
{
	union
	{
		char *string;
		CompositeOrderByClause *compositeOrderByClause;
	};
	OrderByClauseValueType orderByClauseValueType;
};

struct AttributesClauseValue
{
	char *name;
	char *table;
	char *aggregationFunction;
	char *rename;
};

struct ClauseArgsList
{
	ClauseValue *clauseValue;
	ClauseArgsList *next;
};

struct ClauseValue
{
	union
	{
		FromClauseValue *fromClauseValue;
		AttributesClauseValue *attributesClauseValue;
		GroupByClauseValue *groupByClauseValue;
		OrderByClauseValue *orderByClauseValue;
		WhereCondition *whereClauseValue;
		AuxiliaryClauseValue *auxiliaryClauseValue;
		JoinClauseValue *joinClauseValue;
	};
	ClauseType clauseType;
};

struct Clause
{
	union
	{
		ClauseArgsList *clauseArgsList;
		WhereCondition *whereCondition;
	};
	ClauseType type;
};

struct ClauseList
{
	Clause *clause;
	ClauseList *next;
};

struct GroupByClauseValue
{
	char *string;
};

struct Json
{
	ClauseList *clauseList;
};

struct Expression
{
	union
	{
		Factor *factor;
		struct
		{
			Expression *leftExpression;
			Expression *rightExpression;
		};
	};
	ExpressionType type;
};

struct Program
{
	union
	{
		Json *json;
		Expression *expression;
	};
	ProgramType type;
};

struct WhereCondition
{
	union
	{
		WhereBinaryCondition *binaryCondition;
		WhereCondition *whereCondition;
		WhereNotCondition *whereNotCondition;
		WhereIsCondition *whereIsCondition;
		WhereInCondition *whereInCondition;
	};
	WhereConditionNodeType nodeType;
	WhereCondition *next;
	WhereConditionPreconditional preconditional;
};

struct WhereNotCondition
{
	union
	{
		WhereInCondition *in;
		WhereNotCondition * not;
		WhereCondition *condition; // admite NOT <ALGO_NO_BOOLEANO>, pero luego se valida en backend
		WhereBinaryCondition *whereBinaryCondition;
	};
	NotNodeSelected nodeSelected;
};

struct WhereIsCondition
{
	WhereNotCondition *whereNotCondition;
};

struct WhereInCondition
{
	union
	{
		Json *query;
		char *auxQueryName;
	};
	char *attribute;
	WhereInConditionType type;
};

struct WhereConditionValue
{
	union
	{
		char *string;
		char *attribute;
		float number;
		int integer;
		boolean bool;
		AggregationFunction *aggregationFunction;
		WhereBinaryCondition *binaryCondition;
		WhereNotCondition *notCondition;
	};
	ConditionValueType type;
};

struct WhereBinaryCondition
{
	WhereConditionValue *value1;
	WhereConditionValue *value2;
	BinaryConditionOperator operator;
};

/**
 * Node recursive destructors.
 */
void releaseJson(Json *json);
void releaseProgram(Program *program);
void releaseClauseArgsList(ClauseArgsList *fromArray);
void releaseClauseValue(ClauseValue *fromArrayValue);
void releaseClause(Clause *clause);
void releaseClauseList(ClauseList *clauseList);

void releaseOrderByClauseValue(OrderByClauseValue *orderBy);
void releaseTableRename(TableRename *tableRename);
void releaseAttributeRename(AttributeRename *attrRename);
void releaseAggregationFunction(AggregationFunction *aggFunc);
void releaseWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition);
void releaseWhereConditionValue(WhereConditionValue *whereConditionValue);
void releaseWhereNotCondition(WhereNotCondition *whereNotCondition);
void releaseWhereCondition(WhereCondition *whereCondition);
void releaseWhereInCondition(WhereInCondition *whereInCondition);
void releaseWhereIsCondition(WhereIsCondition *whereIsCondition);
void releaseJoinClauseValue(JoinClauseValue *joinClauseValue);

#endif
