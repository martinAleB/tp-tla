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
typedef enum AttributesClauseValueType AttributesClauseValueType;
typedef enum AggregationType AggregationType;
typedef enum BooleanTypes BooleanTypes;
typedef enum ConditionValueType ConditionValueType;
typedef enum BinaryConditionOperator BinaryConditionOperator;
typedef enum WhereConditionPreconditional WhereConditionPreconditional;
typedef enum WhereConditionNodeType WhereConditionNodeType;
typedef enum NotNodeSelected NotNodeSelected;

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
typedef struct WhereCondition WhereCondition;
typedef struct WhereNotCondition WhereNotCondition;
typedef struct WhereConditionValue WhereConditionValue;
typedef struct WhereBinaryCondition WhereBinaryCondition;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum AggregationType
{
	COUNT_F,
	SUM_F,
	AVERAGE_F
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
	WHERE_CLAUSE
};

enum FromClauseValueType
{
	STR,
	TABLE_RENAME
};

enum AttributesClauseValueType
{
	ATTR_STR,
	AGGR_FUNC,
	ATTR_RENAME
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
	CONDITION_VALUE_NOT_CONDITION
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
	NODE_TYPE_WHERE_NOT_CONDITION
};

enum NotNodeSelected
{
	NOT_NODE,
	CONDITION_NODE,
	BINARY_CONDITION_NODE
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
	AggregationType token;
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

struct AttributesClauseValue
{
	union
	{
		char *string;
		AggregationFunction *aggrFunc;
		AttributeRename *attrRename;
	};
	AttributesClauseValueType attributeClauseValueType;
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
		WhereCondition *whereClauseValue;
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
		// @TODO: ver de agregar el Value tambien para contemplar los booleanos
		WhereBinaryCondition *binaryCondition;
		WhereCondition *whereCondition;
		WhereNotCondition *whereNotCondition;
	};
	WhereConditionNodeType nodeType;
	WhereCondition *next;
	WhereConditionPreconditional preconditional;
};

struct WhereNotCondition
{
	union
	{
		WhereNotCondition * not;
		WhereCondition *condition; // admite NOT <ALGO_NO_BOOLEANO>, pero luego se valida en backend
		WhereBinaryCondition *whereBinaryCondition;
	};
	NotNodeSelected nodeSelected;
};

struct WhereConditionValue
{
	union
	{
		char *string;
		float number;
		int integer;
		boolean bool;
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
void releaseConstant(Constant *constant);
void releaseExpression(Expression *expression);
void releaseFactor(Factor *factor);
void releaseJson(Json *json);
void releaseProgram(Program *program);
void releaseClauseArgsList(ClauseArgsList *fromArray);
void releaseClauseValue(ClauseValue *fromArrayValue);
void releaseClause(Clause *clause);
void releaseClauseList(ClauseList *clauseList);
void releaseTableRename(TableRename *tableRename);
void releaseAttributeRename(AttributeRename *attrRename);
void releaseAggregationFunction(AggregationFunction *aggFunc);
void releaseWhereBinaryCondition(WhereBinaryCondition *whereBinaryCondition);
void releaseWhereConditionValue(WhereConditionValue *whereConditionValue);
void releaseWhereNotCondition(WhereNotCondition *whereNotCondition);
void releaseWhereCondition(WhereCondition *whereCondition);

#endif
