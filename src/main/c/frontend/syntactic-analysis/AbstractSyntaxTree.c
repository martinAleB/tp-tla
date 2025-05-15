#include "AbstractSyntaxTree.h"

/* MODULE INTERNAL STATE */

static Logger * _logger = NULL;

void initializeAbstractSyntaxTreeModule() {
	_logger = createLogger("AbstractSyntxTree");
}

void shutdownAbstractSyntaxTreeModule() {
	if (_logger != NULL) {
		destroyLogger(_logger);
	}
}

/** PUBLIC FUNCTIONS */

void releaseConstant(Constant * constant) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (constant != NULL) {
		free(constant);
	}
}

void releaseExpression(Expression * expression) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (expression != NULL) {
		switch (expression->type) {
			case ADDITION:
			case DIVISION:
			case MULTIPLICATION:
			case SUBTRACTION:
				releaseExpression(expression->leftExpression);
				releaseExpression(expression->rightExpression);
				break;
			case FACTOR:
				releaseFactor(expression->factor);
				break;
		}
		free(expression);
	}
}

void releaseFactor(Factor * factor) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (factor != NULL) {
		switch (factor->type) {
			case CONSTANT:
				releaseConstant(factor->constant);
				break;
			case EXPRESSION:
				releaseExpression(factor->expression);
				break;
		}
		free(factor);
	}
}

void releaseJson(Json *json) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (json != NULL){
		releaseClauseList(json->clauseList);
		free(json);
	}
}

void releaseProgram(Program * program) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (program != NULL) {
		switch (program->type) {
			case PROGRAM_EXPRESSION:
				releaseExpression(program->expression);
				break;
			case JSON:
				releaseJson(program->json);
				break;
		}
		free(program);
	}
}

void releaseClauseArgsList(ClauseArgsList * clauseArgsList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	while (clauseArgsList != NULL) {
		ClauseArgsList * next = clauseArgsList->next;
		releaseClauseValue(clauseArgsList->clauseValue);
		free(clauseArgsList);
		clauseArgsList = next;
	}
}

void releaseTableRename(TableRename * tableRename) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	free(tableRename->name);
	free(tableRename->rename);
	free(tableRename);
}


void releaseClauseValue(ClauseValue * clauseValue) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (clauseValue != NULL) {
		switch(clauseValue->clauseType) {
			case FROM_CLAUSE:
				switch (clauseValue->fromClauseValue->fromClauseValueType) {
					case STR:
						free(clauseValue->fromClauseValue->string);
						break;
					case TABLE_RENAME:
						releaseTableRename(clauseValue->fromClauseValue->tableRename);
				}
				free(clauseValue->fromClauseValue);
				break;
			case ATTRIBUTES_CLAUSE:
				switch(clauseValue->attributesClauseValue->attributeClauseValueType) {
					case ATTR_STR:
						free(clauseValue->attributesClauseValue->string);
						break;
					case AGGR_FUNC:
						releaseAggregationFunction(clauseValue->attributesClauseValue->aggrFunc);
						break;
					case ATTR_RENAME:
						releaseAttributeRename(clauseValue->attributesClauseValue->attrRename);
						break;
				}
				free(clauseValue->attributesClauseValue);
				break;
		}
		free(clauseValue);
	}
}

void releaseClause(Clause * clause) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (clause != NULL) {
		releaseClauseArgsList(clause->clauseArgsList);
	}
	free(clause);
}

 void releaseClauseList(ClauseList * clauseList) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
    ClauseList * aux = NULL;
    while(clauseList->next != NULL) {
        releaseClause(clauseList->clause);
        aux = clauseList;
        clauseList = clauseList->next;
        free(aux);
    }
    releaseClause(clauseList->clause);
    free(clauseList);
}

void releaseAggregationFunction(AggregationFunction * aggFunc) {
	logDebugging(_logger, "Executing destructor: %s", __FUNCTION__);
	if (aggFunc != NULL) {
		free(aggFunc->attribute);
		free(aggFunc);
	}
}

void releaseAttributeRename(AttributeRename * attrRename) {
	if (attrRename != NULL) {
		releaseClauseValue(attrRename->value);
		free(attrRename->rename);
		free(attrRename);
	}
}