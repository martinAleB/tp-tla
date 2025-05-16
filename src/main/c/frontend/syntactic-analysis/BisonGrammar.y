%{

#include "BisonActions.h"

%}

// You touch this, and you die.
%define api.value.union.name SemanticValue

%union {
	/** Terminals. */

	int integer;
	boolean bool;
	float number;
	char * string;
	AggregationType aggrType;
	Token token;
	BinaryConditionOperator binaryConditionOperator;
	WhereConditionPreconditional whereConditionPreconditional;

	/** Non-terminals. */

	Constant * constant;
	Expression * expression;
	Factor * factor;
	Clause * clause;
	ClauseList * clauseList;
	ClauseArgsList * clauseArgsList;
	ClauseValue * clauseValue;
	TableRename * tableRename;
	AttributeRename * attrRename;
	AggregationFunction * aggregationFunction;
	Json * json;
	Program * program;
	WhereConditionValue * whereConditionValue;
	WhereBinaryCondition * whereBinaryCondition;
	WhereCondition * whereCondition;
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
%destructor { releaseAggregationFunction($$); } <aggregationFunction>
%destructor { releaseAttributeRename($$); } <attrRename>
%destructor { releaseWhereBinaryCondition($$); } <whereBinaryCondition>
%destructor { releaseWhereConditionValue($$); } <whereConditionValue>
%destructor { releaseWhereCondition($$); } <whereCondition>

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
%token <token> FUNCTION
%token <token> ATTRIBUTE
%token <aggrType> COUNT
%token <aggrType> SUM
%token <aggrType> AVERAGE
%token <token> COLON
%token <token> COMMA
%token <token> OPEN_BRACKET
%token <token> CLOSE_BRACKET
%token <string> STRING
%token <number> NUMBER
%token <bool> BOOLEAN
%token <token> TRUE
%token <token> FALSE
%token <token> NULLV
%token <token> FIRST_VALUE
%token <token> SECOND_VALUE
%token <token> OPERATOR
%token <binaryConditionOperator> BINARY_OPERATOR
%token <token> LOWER
%token <token> GREATER
%token <token> LOWER_OR_EQUAL
%token <token> GREATER_OR_EQUAL
%token <token> EQUAL
%token <token> NOT_EQUAL
%token <token> AND
%token <token> OR

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

%type <attrRename> attributeRename

%type <aggregationFunction> aggregationFunction
%type <clauseArgsList> attributesClauseArgsList
%type <clauseArgsList> attributesClauseValues
%type <clauseValue> attributesClauseValue
%type <whereConditionValue> whereConditionValue
%type <whereBinaryCondition> whereBinaryCondition
%type <whereCondition> whereCondition
%type <whereCondition> colonWhere
%type <whereCondition> whereConditionWithPrecond
%type <whereCondition> whereConditionWithPrecondAfter

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
	| WHERE COLON whereCondition									{ $$ = WhereClauseSemanticAction($3); }
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

// WHERE CLAUSE
whereConditionValue: STRING											{ $$ = StringWhereConditionValueSemanticAction($1); }
	| INTEGER														{ $$ = IntegerWhereConditionValueSemanticAction($1); }
	| NUMBER														{ $$ = NumberWhereConditionValueSemanticAction($1); }
	| BOOLEAN														{ $$ = BooleanWhereConditionValueSemanticAction($1); }
	| NULLV															{ $$ = NullWhereConditionValueSemanticAction(); }
	| whereBinaryCondition											{ $$ = WhereBinaryConditionWhereConditionValueSemanticAction($1); }

whereBinaryCondition: OPEN_CURLY_BRACE OPERATOR[op] COLON BINARY_OPERATOR COMMA FIRST_VALUE COLON whereConditionValue[value1] COMMA SECOND_VALUE COLON whereConditionValue[value2] CLOSE_CURLY_BRACE	{ $$ = WhereBinaryConditionSemanticAction($value1, $value2, $op); }

colonWhere:	COLON whereCondition[current1] CLOSE_CURLY_BRACE whereConditionWithPrecondAfter[next1]		{ $$ = CurrentAndNextWhereConditionsSemanticAction($current1, $next1); }
	| COLON whereBinaryCondition[binary1] CLOSE_CURLY_BRACE whereConditionWithPrecondAfter[next2]		{ $$ = BinaryConditionAndNextWhereConditionSemanticAction($binary1, $next2); }

whereConditionWithPrecond: OPEN_CURLY_BRACE AND colonWhere[node1]												{ $$ = PreconditionalWhereConditionSemanticAction($node1, $2); }
	| OPEN_CURLY_BRACE OR colonWhere[node2]																		{ $$ = PreconditionalWhereConditionSemanticAction($node2, $2); }

whereConditionWithPrecondAfter: COMMA whereConditionWithPrecond													{ $$ = $2; }
	| %empty																									{ $$ = NULL; }

whereCondition: OPEN_BRACKET whereCondition[current2] whereConditionWithPrecondAfter[next3] CLOSE_BRACKET			{ $$ = FirstCurrentAndNextWhereConditionsSemanticAction($current2, $next3); }						
	| OPEN_BRACKET whereBinaryCondition[binary2] whereConditionWithPrecondAfter[next4] CLOSE_BRACKET				{ $$ = FirstBinaryConditionAndNextWhereConditionSemanticAction($binary2, $next4); }

//ATTRIBUTES CLAUSE
aggregationFunction: COUNT											{ $$ = AggregationFunctionSemanticAction($1); }
	| SUM 															{ $$ = AggregationFunctionSemanticAction($1); }
	| AVERAGE														{ $$ = AggregationFunctionSemanticAction($1); }
	;

attributeRename: ATTRIBUTE COLON attributesClauseValue[val] COMMA AS COLON STRING[str]	{ $$ = AttributeRenameSemanticAction($val, $str); } 
	;
attributesClauseValue: STRING										{ $$ = StringAttributesClauseValueSemanticAction($1); }
	| OPEN_CURLY_BRACE FUNCTION COLON aggregationFunction[aggr] COMMA ATTRIBUTE COLON STRING[attr] CLOSE_CURLY_BRACE	{ $$ = AggregationFunctionAttributesClauseValueSemanticAction($aggr, $attr); }
	| OPEN_CURLY_BRACE attributeRename[attrRename] CLOSE_CURLY_BRACE { $$ = AttributeRenameAttributesClauseValueSemanticAction($attrRename); }
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
