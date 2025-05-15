%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	float number;
	char * string;
	Token token;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Clause * clause;
	ClauseList * clauseList;
	ClauseArgsList * clauseArgsList;
	ClauseValue * clauseValue;
	Json * json;
	Program * program;
}

/**
 * Destructors. This functions are executed after the parsing ends, so if the
 * AST must be used in the following phases of the compiler you shouldn't used
 * this approach for the AST root node ("program" non-terminal, in this
 * grammar), or it will drop the entire tree even if the parse succeeds.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Destructor-Decl.html
 */
%destructor { releaseConstant($$); } <constant>
%destructor { releaseExpression($$); } <expression>
%destructor { releaseFactor($$); } <factor>
%destructor { releaseJson($$); } <json>
%destructor { releaseClauseArgsList($$); } <clauseArgsList>
%destructor { releaseClauseValue($$); } <clauseValue>
%destructor { releaseClause($$); } <clause>
%destructor { releaseClauseList($$); } <clauseList>

/** Terminals. */
%token <integer> INTEGER
%token <token> ADD
%token <token> CLOSE_PARENTHESIS
%token <token> DIV
%token <token> MUL
%token <token> OPEN_PARENTHESIS
%token <token> SUB
%token <token> OPEN_CURLY_BRACE
%token <token> CLOSE_CURLY_BRACE
%token <token> FROM
%token <token> ATTRIBUTES
%token <token> WHERE
%token <token> TABLE
%token <token> AS
%token <token> COLON
%token <token> COMMA
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <string> STRING
%token <number> NUMBER

%token <token> UNKNOWN

/** Non-terminals. */
%type <constant> constant
%type <expression> expression
%type <factor> factor
%type <json> json
%type <clause> clause
%type <clauseList> clauseList

%type <clauseArgsList> fromClauseArgsList
%type <clauseArgsList> fromClauseValues
%type <clauseValue> fromClauseValue

%type <clauseArgsList> attributesClauseArgsList
%type <clauseArgsList> attributesClauseValues
%type <clauseValue> attributesClauseValue

%type <program> program

/**
 * Precedence and associativity.
 *
 * @see https://www.gnu.org/software/bison/manual/html_node/Precedence.html
 */
%left ADD SUB
%left MUL DIV

%%

// IMPORTANT: To use λ in the following grammar, use the %empty symbol.


//GENERAL JSON
program: expression													{ $$ = ExpressionProgramSemanticAction(currentCompilerState(), $1); }
	| json															{ $$ = JsonProgramSemanticAction(currentCompilerState(), $1); }
	;

json: OPEN_CURLY_BRACE clauseList CLOSE_CURLY_BRACE 				{ $$ = JsonSemanticAction($2);}
	;


//CLAUSES
clauseList: clause													{ $$ = ClauseListSemanticAction($1, NULL); }	
	| clause COMMA clauseList										{ $$ = ClauseListSemanticAction($1, $3); }
	;

clause: FROM COLON fromClauseArgsList               				{ $$ = ClauseSemanticAction($3, FROM_CLAUSE); }
	| ATTRIBUTES COLON attributesClauseArgsList						{ $$ = ClauseSemanticAction($3, ATTRIBUTES_CLAUSE); }
	;


//FROM CLAUSE
fromClauseValue: STRING												{ $$ = StringFromClauseValueSemanticAction($1); }
	| OPEN_CURLY_BRACE TABLE COLON STRING[strTable] COMMA AS COLON STRING[strRename] CLOSE_CURLY_BRACE	{ $$ = TableRenameFromClauseValueSemanticAction($strTable, $strRename); }
	;

fromClauseValues: fromClauseValue									{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| fromClauseValue COMMA fromClauseValues						{ $$ = ClauseArgsListSemanticAction($1, $3); }
	;

fromClauseArgsList: fromClauseValue									{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| OPEN_BRACKET fromClauseValues CLOSE_BRACKET					{ $$ = $2; }
	;


//ATTRIBUTES CLAUSE
attributesClauseValue: STRING										{ $$ = StringAttributesClauseValueSemanticAction($1); }
	
	;

attributesClauseValues: attributesClauseValue						{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| attributesClauseValue COMMA attributesClauseValues			{ $$ = ClauseArgsListSemanticAction($1, $3); }
	;

attributesClauseArgsList: attributesClauseValue						{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| OPEN_BRACKET attributesClauseValues CLOSE_BRACKET				{ $$ = $2; }
	;

// THE OG, KEEP THEM AS REFERENCE
expression: expression[left] ADD expression[right]					{ $$ = ArithmeticExpressionSemanticAction($left, $right, ADDITION); }
	| expression[left] DIV expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, DIVISION); }
	| expression[left] MUL expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, MULTIPLICATION); }
	| expression[left] SUB expression[right]						{ $$ = ArithmeticExpressionSemanticAction($left, $right, SUBTRACTION); }
	| factor														{ $$ = FactorExpressionSemanticAction($1); }
	;

factor: OPEN_PARENTHESIS expression CLOSE_PARENTHESIS				{ $$ = ExpressionFactorSemanticAction($2); }
	| constant														{ $$ = ConstantFactorSemanticAction($1); }
	;

constant: INTEGER													{ $$ = IntegerConstantSemanticAction($1); }
	;

%%
