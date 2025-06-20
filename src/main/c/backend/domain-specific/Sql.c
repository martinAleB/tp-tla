#include "Sql.h"

static Logger *_logger = NULL;

void initializeSqlModule()
{
    _logger = createLogger("Sql");
}

void shutdownSqlModule()
{
    if (_logger != NULL)
    {
        destroyLogger(_logger);
    }
}

boolean computeSql(Program *program)
{
    return computeJson(program->json);
}

boolean computeJson(Json *json)
{
    return computeClauseList(json->clauseList);
}

boolean computeClauseList(ClauseList *clauseList)
{
    int clauseCounter[CLAUSE_TYPE_COUNT] = {0};
    ClauseList *current = clauseList;
    while (current != NULL)
    {
        Clause *clause = current->clause;
        clauseCounter[clause->type]++;
        if (clauseCounter[clause->type] > 1 || !computeClause(clause))
        {
            logError(_logger, "Repeated statements detected");
            return false;
        }
        current = current->next;
    }
    if (clauseCounter[FROM_CLAUSE] == 0)
    {
        logError(_logger, "Missing 'FROM' statement");
    }
    return clauseCounter[FROM_CLAUSE] == 1;
}

boolean computeClause(Clause *clause)
{
    return true;
}