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


typedef struct Constant Constant;
typedef struct Expression Expression;
typedef struct Factor Factor;

typedef struct Program Program;
typedef struct Json Json;
typedef struct FromClauseValue FromClauseValue;
typedef struct TableRename TableRename;
typedef struct AttributesClauseValue AttributesClauseValue;
typedef struct ClauseValue ClauseValue;
typedef struct ClauseArgsList ClauseArgsList;
typedef struct ClauseList ClauseList;
typedef struct Clause Clause;

/**
 * Node types for the Abstract Syntax Tree (AST).
 */

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
	ATTRIBUTES_CLAUSE
};

enum FromClauseValueType {
	STR,
	TABLE_RENAME
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
struct FromClauseValue {
	union {
		char * string;
		TableRename * tableRename;
	};
	FromClauseValueType fromClauseValueType;
};

struct AttributesClauseValue {
	char * string;
};

struct ClauseArgsList {
	ClauseValue * clauseValue;
	ClauseArgsList * next;
};

struct ClauseValue {
	union {
		FromClauseValue * fromClauseValue;
		AttributesClauseValue * attributesClauseValue;
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

void releaseTableRename(TableRename * tableRename);

#endif
