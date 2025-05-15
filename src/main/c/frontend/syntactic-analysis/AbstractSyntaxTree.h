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
typedef enum OrderByType OrderByType;
typedef enum OrderByFunctionType OrderByFunctionType;
typedef enum OrderByClauseValueType OrderByClauseValueType;


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
typedef struct OrderByClauseValue  OrderByClauseValue;
typedef struct CompositeOrderByClause CompositeOrderByClause;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

enum AggregationType {
	COUNT_F,
	SUM_F,
	AVERAGE_F
};

enum OrderByType{
	ASC_T,
	DESC_T
};

enum OrderByFunctionType {
	ORDER_BY_ASC,
	ORDER_BY_DESC
};

enum OrderByClauseValueType{
	ORDER_BY_STRING,
	ORDER_BY_COMPOSITE,
	ORDER_BY_AGGR_FUNC
};

enum ExpressionType {
	ADDITION,
	DIVISION,
	FACTOR,
	MULTIPLICATION,
	SUBTRACTION
};

enum FactorType {
	CONSTANT,
	EXPRESSION
};

enum ProgramType {
	PROGRAM_EXPRESSION,
	JSON
};

enum ClauseType {
	FROM_CLAUSE,
	ATTRIBUTES_CLAUSE,
	GROUP_BY_CLAUSE
	ATTRIBUTES_CLAUSE,
	ORDER_BY_CLAUSE
};

enum FromClauseValueType {
	STR,
	TABLE_RENAME
};

enum AttributesClauseValueType {
	ATTR_STR,
	AGGR_FUNC,
	ATTR_RENAME
};
struct Constant {
	int value;
};

struct Factor {
	union {
		Constant * constant;
		Expression * expression;
	};
	FactorType type;
};

struct TableRename {
	char * name;
	char * rename;
};

struct AttributeRename {
	ClauseValue * value;
	char * rename;
};

struct AggregationFunction {
	AggregationType token;
	char * attribute;
};
struct FromClauseValue {
	union {
		char * string;
		TableRename * tableRename;
	};
	FromClauseValueType fromClauseValueType;
};

struct CompositeOrderByClause{
	char* string;
	AggregationFunction * aggrFunc;
	OrderByFunctionType orderByFunctionType;
};


struct OrderByClauseValue {
	union {
		char * string;
		CompositeOrderByClause * compositeOrderByClause;
	};
	OrderByClauseValueType orderByClauseValueType;
};
	

struct AttributesClauseValue {
	union {
		char * string;
		AggregationFunction * aggrFunc;
		AttributeRename * attrRename;
	};
	AttributesClauseValueType attributeClauseValueType;
};

struct ClauseArgsList {
	ClauseValue * clauseValue;
	ClauseArgsList * next;
};

struct ClauseValue {
	union {
		FromClauseValue * fromClauseValue;
		AttributesClauseValue * attributesClauseValue;
		GroupByClauseValue * groupByClauseValue;
	}; 
		OrderByClauseValue * orderByClauseValue;
	};
	ClauseType clauseType;
};

struct Clause {
	ClauseArgsList * clauseArgsList;
	ClauseType type;
};

struct ClauseList {
	Clause * clause;
	ClauseList * next;
};

struct GroupByClauseValue {
	char * string;
};

struct Json {
	ClauseList * clauseList;
};

struct Expression {
	union {
		Factor * factor;
		struct {
			Expression * leftExpression;
			Expression * rightExpression;
		};
	};
	ExpressionType type;
};

struct Program {
	union {
		Json * json;
		Expression * expression;
	};
	ProgramType type;
};

/**
 * Node recursive destructors.
 */
void releaseConstant(Constant * constant);
void releaseExpression(Expression * expression);
void releaseFactor(Factor * factor);
void releaseJson(Json *json);
void releaseProgram(Program * program);
void releaseClauseArgsList(ClauseArgsList * fromArray);
void releaseClauseValue(ClauseValue * fromArrayValue);
void releaseClause(Clause * clause);
void releaseClauseList(ClauseList * clauseList);

void releaseOrderByClauseValue(OrderByClauseValue * orderBy);

void releaseTableRename(TableRename * tableRename);
void releaseAttributeRename(AttributeRename * attrRename);
void releaseAggregationFunction(AggregationFunction * aggFunc);

#endif
