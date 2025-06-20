#include "Generator.h"

/* MODULE INTERNAL STATE */

static Logger *_logger = NULL;

void initializeGeneratorModule()
{
	_logger = createLogger("Generator");
}

void shutdownGeneratorModule()
{
	if (_logger != NULL)
	{
		destroyLogger(_logger);
	}
}

/** PRIVATE FUNCTIONS */

static void _generateQuery(Json *json);
static void _generateClauseList(ClauseList *clauseList);
static void _generateClause(Clause* clause);
static void _generateFromClause(ClauseArgsList *clauseArgsList);
static void _generateFromClauseValue(ClauseValue *clauseValue);
static void _generateAttributesClause(ClauseArgsList *clauseArgsList);
static void _generateAttributesClauseValue(ClauseValue *clauseValue);
static void _generateGroupByClause(ClauseArgsList *clauseArgsList);
static void _generateGroupByClauseValue(ClauseValue *clauseValue);
static void _generateOrderByClause(ClauseArgsList *clauseArgsList);
static void _generateOrderByClauseValue(ClauseValue *clauseValue);
static void _generateOrderByClauseValueString(OrderByClauseValue *orderByClauseValue);
static void _generateOrderByClauseValueComposite(OrderByClauseValue *orderByClauseValue);
static void _generateOrderByClauseValueAggrFunc(OrderByClauseValue *orderByClauseValue);

static void _generateQuery(Json *json) {
	if (json == NULL || json->clauseList == NULL) {
		logError(_logger, "Empty JSON clause list.");
		return;
	}
	_generateClauseList(json->clauseList);
	printf(";\n");
}

static void _generateClauseList(ClauseList *clauseList) {
	//@TODO: Handle clauseList order
	while (clauseList != NULL) {
		_generateClause(clauseList->clause);
		clauseList = clauseList->next;
	}
}

static void _generateClause(Clause* clause) {
	if (clause->clauseArgsList == NULL) {
		logError(_logger, "Empty FROM clause arguments.");
		return;
	}
	switch (clause->type) {
		case FROM_CLAUSE:
			_generateFromClause(clause->clauseArgsList);
			break;
		case ATTRIBUTES_CLAUSE:
			_generateAttributesClause(clause->clauseArgsList);
			break;
		case GROUP_BY_CLAUSE:
			_generateGroupByClause(clause->clauseArgsList);
			break;
		case ORDER_BY_CLAUSE:
			_generateOrderByClause(clause->clauseArgsList);
			break;
		case WHERE_CLAUSE:
			break;
		case AUXILIARY_CLAUSE:
			break;
		case JOIN_CLAUSE:
			break;
		default:
			logError(_logger, "Unsupported clause type: %d", clause->type);
			break;
	}
}

static void _generateFromClause(ClauseArgsList *clauseArgsList) {
	printf("FROM ");
	while (clauseArgsList != NULL) {
		if (clauseArgsList->clauseValue != NULL) {
			_generateFromClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL) {
				printf(", ");
			}
		} else {
			logError(_logger, "Null clause argument in FROM clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateFromClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != FROM_CLAUSE) {
		logError(_logger, "Incorrect clause type in FROM clause: %d", clauseValue->clauseType);
	}

	if (clauseValue->fromClauseValue->fromClauseValueType == STR ) {
		printf("%s", clauseValue->fromClauseValue->string);
	}

	else if (clauseValue->fromClauseValue->fromClauseValueType == TABLE_RENAME) {
		printf("%s AS %s",
			clauseValue->fromClauseValue->tableRename->name,
			clauseValue->fromClauseValue->tableRename->rename);
	}
}

static void _generateAttributesClause(ClauseArgsList *clauseArgsList) {
	if (clauseArgsList == NULL) {
		printf("* ");
	}
	else {
		while (clauseArgsList != NULL) {
			if (clauseArgsList->clauseValue != NULL) {
				_generateAttributesClauseValue(clauseArgsList->clauseValue);
				if (clauseArgsList->next != NULL) {
					printf(", ");
				}
			} else {
				logError(_logger, "Null clause argument in ATTRIBUTES clause.");
			}
			clauseArgsList = clauseArgsList->next;
		}
	}
}

static void _generateAttributesClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != ATTRIBUTES_CLAUSE) {
		logError(_logger, "Incorrect clause type in ATTRIBUTES clause: %d", clauseValue->clauseType);
	}
	if (clauseValue->attributesClauseValue->name == NULL) {
		logError(_logger, "Name attribute cannot be NULL");
	}

	if (clauseValue->attributesClauseValue->aggregationFunction != NULL) {
		printf("%s(", clauseValue->attributesClauseValue->aggregationFunction);
	}
	if (clauseValue->attributesClauseValue->table != NULL) {
		printf("%s.",clauseValue->attributesClauseValue->table);
	}
	printf(clauseValue->attributesClauseValue->name);
	if (clauseValue->attributesClauseValue->aggregationFunction != NULL) {
		printf(")");
	}
	if (clauseValue->attributesClauseValue->rename != NULL) {
		printf("AS %s", clauseValue->attributesClauseValue->rename);
	}
}

static void _generateGroupByClause(ClauseArgsList *clauseArgsList) {
	printf("GROUP BY ");
	while (clauseArgsList != NULL) {
		if (clauseArgsList->clauseValue != NULL) {
			_generateGroupByClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL) {
				printf(", ");
			}
		} else {
			logError(_logger, "Null clause argument in GROUP BY clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateGroupByClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != GROUP_BY_CLAUSE) {
		logError(_logger, "Incorrect clause type in ORDER BY clause: %d", clauseValue->clauseType);
	}
	if (clauseValue->groupByClauseValue->string == NULL) {
		logError(_logger, "Group by string cannot be NULL");
	}
	printf("%s", clauseValue->groupByClauseValue->string);
}

static void _generateOrderByClause(ClauseArgsList *clauseArgsList) {
	printf("ORDER BY ");
	while (clauseArgsList != NULL) {
		if (clauseArgsList->clauseValue != NULL) {
			_generateOrderByClauseValue(clauseArgsList->clauseValue);
			if (clauseArgsList->next != NULL) {
				printf(", ");
			}
		} else {
			logError(_logger, "Null clause argument in ORDER BY clause.");
		}
		clauseArgsList = clauseArgsList->next;
	}
}

static void _generateOrderByClauseValue(ClauseValue *clauseValue) {
	if (clauseValue->clauseType != ORDER_BY_CLAUSE) {
		logError(_logger, "Incorrect clause type in ORDER BY clause: %d", clauseValue->clauseType);
	}

	switch (clauseValue->orderByClauseValue->orderByClauseValueType) {
		case ORDER_BY_STRING:
			_generateOrderByClauseValueString(clauseValue->orderByClauseValue);
			break;
		case ORDER_BY_COMPOSITE:
			_generateOrderByClauseValueComposite(clauseValue->orderByClauseValue);
			break;
		case ORDER_BY_AGGR_FUNC:
			_generateOrderByClauseValueAggrFunc(clauseValue->orderByClauseValue);
			break;
	}
}

static void _generateOrderByClauseValueString(OrderByClauseValue *orderByClauseValue) {
	printf("%s", orderByClauseValue->string);
}

static void _generateOrderByClauseValueComposite(OrderByClauseValue *orderByClauseValue) {
	printf("%s ", orderByClauseValue->string);
	if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_ASC) {
		printf("ASC");
	}
	else if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_DESC) {
		printf("DESC");
	}
}

static void _generateOrderByClauseValueAggrFunc(OrderByClauseValue *orderByClauseValue) {
	printf("%s(%s) ", orderByClauseValue->compositeOrderByClause->aggrFunc, orderByClauseValue->compositeOrderByClause->string);
	if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_ASC) {
		printf("ASC");
	}
	else if (orderByClauseValue->compositeOrderByClause->orderByFunctionType == ORDER_BY_DESC) {
		printf("DESC");
	}
}

/** PUBLIC FUNCTIONS */

void generate(CompilerState *compilerState)
{
	//@TODO: Delete log errors, generate doesnt validate
	logDebugging(_logger, "Starting SQL generation...");
	Program *program = (Program *) compilerState->abstractSyntaxtTree;

	if (program->type != JSON) {
		logError(_logger, "Expected a JSON AST in program node.");
		return;
	}

	_generateQuery(program->json);
	logDebugging(_logger, "Generation is done.");
}
