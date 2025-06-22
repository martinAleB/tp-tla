#include "backend/code-generation/SqlGenerator.h"
#include "backend/domain-specific/Sql.h"
#include "frontend/lexical-analysis/FlexActions.h"
#include "frontend/syntactic-analysis/AbstractSyntaxTree.h"
#include "frontend/syntactic-analysis/BisonActions.h"
#include "frontend/syntactic-analysis/SyntacticAnalyzer.h"
#include "shared/CompilerState.h"
#include "shared/ScopeManager.h"
#include "shared/Environment.h"
#include "shared/SymbolTable.h"
#include "shared/Logger.h"
#include "shared/String.h"

/**
 * The main entry-point of the entire application. If you use "strtok" to
 * parse anything inside this project instead of using Flex and Bison, I will
 * find you, and I will kill you (Bryan Mills; "Taken", 2008).
 */
const int main(const int count, const char **arguments)
{
	Logger *logger = createLogger("EntryPoint");
	initializeFlexActionsModule();
	initializeBisonActionsModule();
	initializeSyntacticAnalyzerModule();
	initializeAbstractSyntaxTreeModule();
	initializeSqlModule();
	initializeGeneratorModule();

	// Logs the arguments of the application.
	for (int k = 0; k < count; ++k)
	{
		logDebugging(logger, "Argument %d: \"%s\"", k, arguments[k]);
	}

	// Begin compilation process.
	CompilerState compilerState = {
		.symbolTable = initializeSymbolTable(),
		.abstractSyntaxtTree = NULL,
		.succeed = false,
		.value = NULL};

	const SyntacticAnalysisStatus syntacticAnalysisStatus = parse(&compilerState);
	CompilationStatus compilationStatus = SUCCEED;
	Program *program = compilerState.abstractSyntaxtTree;
	if (syntacticAnalysisStatus == ACCEPT)
	{
		// ----------------------------------------------------------------------------------------
		// Beginning of the Backend... ------------------------------------------------------------

		initializeScopeManager();
		logDebugging(logger, "Checking expression through the SymbolTable and scopes...");
		if (hasSubqueryRedefinition(compilerState.symbolTable))
		{
			logError(logger, "Auxiliary query redefinition");
			compilationStatus = FAILED;
		}
		else if (!allUsedSubqueriesDefined(compilerState.symbolTable))
		{
			logError(logger, "Undefined reference to auxiliary query");
			compilationStatus = FAILED;
		}
		else if (!validateSql(program))
		{
			logError(logger, "Query validation failed");
			compilationStatus = FAILED;
		}
		else
		{
			generate(&compilerState);
		}
		freeScopeManager();
		// ...end of the Backend. -----------------------------------------------------------------
		// ----------------------------------------------------------------------------------------
	}
	else
	{
		logError(logger, "The syntactic-analysis phase rejects the input program.");
		compilationStatus = FAILED;
	}
	logDebugging(logger, "Releasing AST resources...");
	releaseProgram(program);
	logDebugging(logger, "Releasing modules resources...");
	freeSymbolTable(compilerState.symbolTable);
	shutdownGeneratorModule();
	shutdownSqlModule();
	shutdownAbstractSyntaxTreeModule();
	shutdownSyntacticAnalyzerModule();
	shutdownBisonActionsModule();
	shutdownFlexActionsModule();
	logDebugging(logger, "Compilation is done.");
	destroyLogger(logger);
	return compilationStatus;
}
