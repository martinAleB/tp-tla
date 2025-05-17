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
	OrderByClauseValueType orderByType;
	//CompositeOrderByClause compositeOrderByClause;
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
	Json * json;
	Program * program;
	WhereConditionValue * whereConditionValue;
	WhereBinaryCondition * whereBinaryCondition;
	WhereNotCondition * whereNotCondition;
	WhereCondition * whereCondition;
	WhereInCondition * whereInCondition;
	WhereIsCondition * whereIsCondition;
	AttributesClauseValue * attributeClauseValue;
	CompositeOrderByClause * orderByExplicit;
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
%destructor { releaseWhereBinaryCondition($$); } <whereBinaryCondition>
%destructor { releaseWhereNotCondition($$); } <whereNotCondition>
%destructor { releaseWhereConditionValue($$); } <whereConditionValue>
%destructor { releaseWhereCondition($$); } <whereCondition>
%destructor { releaseWhereInCondition($$); } <whereInCondition>
%destructor { releaseWhereIsCondition($$); } <whereIsCondition>

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
%token <token> GROUP_BY
%token <token> ORDER_BY
%token <token> ORDER
%token <token> TABLE
%token <token> AS
%token <token> FUNCTION
%token <token> ATTRIBUTE
%token <string> COUNT
%token <string> SUM
%token <string> AVERAGE
%token <token> ASC
%token <token> DESC
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
%token <token> NOT
%token <token> IN
%token <token> IS
%token <token> NAME

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
%type <attributeClauseValue> attribute
%type <attributeClauseValue> attributeOptions
%type <attributeClauseValue> aggregation

%type <string> aggregationFunction
%type <clauseArgsList> orderByClauseArgsList
%type <clauseArgsList> orderByClauseValues
%type <clauseValue> orderByClauseValue
%type <orderByExplicit> orderByExplicit

%type <clauseArgsList> attributesClauseArgsList
%type <clauseArgsList> attributesClauseValues
%type <clauseValue> attributesClauseValue
%type <whereConditionValue> whereConditionValue
%type <whereBinaryCondition> whereBinaryCondition
%type <whereNotCondition> whereNotCondition
%type <whereCondition> whereCondition
%type <whereCondition> colonWhere
%type <whereCondition> whereConditionWithPrecond
%type <whereCondition> whereConditionWithPrecondAfter
%type <whereInCondition> whereInCondition
%type <whereIsCondition> whereIsCondition

%type <clauseArgsList> groupByClause
%type <clauseArgsList> groupByValues
%type <clauseValue> groupByValue

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
	| GROUP_BY COLON groupByClause									{ $$ = ClauseSemanticAction($3, GROUP_BY_CLAUSE); }
	| ORDER_BY COLON orderByClauseArgsList							{ $$ = ClauseSemanticAction($3, ORDER_BY_CLAUSE); }
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
	;

whereBinaryCondition: OPEN_CURLY_BRACE OPERATOR[op] COLON BINARY_OPERATOR COMMA FIRST_VALUE COLON whereConditionValue[value1] COMMA SECOND_VALUE COLON whereConditionValue[value2] CLOSE_CURLY_BRACE	{ $$ = WhereBinaryConditionSemanticAction($value1, $value2, $op); }
	;

whereNotCondition: OPEN_CURLY_BRACE NOT COLON whereBinaryCondition CLOSE_CURLY_BRACE				{ $$ = WhereNotConditionWithBinaryConditionSemanticAction($4); }
	| OPEN_CURLY_BRACE NOT COLON whereCondition CLOSE_CURLY_BRACE									{ $$ = WhereNotConditionWithWhereConditionSemanticAction($4); }
	| OPEN_CURLY_BRACE NOT COLON whereNotCondition CLOSE_CURLY_BRACE								{ $$ = WhereNotConditionWithNotConditionSemanticAction($4); }
	| OPEN_CURLY_BRACE NOT COLON whereInCondition CLOSE_CURLY_BRACE									{ $$ = WhereNotConditionWithInConditionSemanticAction($4); }
	;

colonWhere:	COLON whereCondition CLOSE_CURLY_BRACE whereConditionWithPrecondAfter			{ $$ = CurrentAndNextWhereConditionsSemanticAction($2, $4); }
	| COLON whereBinaryCondition CLOSE_CURLY_BRACE whereConditionWithPrecondAfter			{ $$ = BinaryConditionAndNextWhereConditionSemanticAction($2, $4); }
	| COLON whereNotCondition CLOSE_CURLY_BRACE whereConditionWithPrecondAfter				{ $$ = NotConditionAndNextWhereConditionSemanticAction($2, $4); }
	| COLON whereIsCondition CLOSE_CURLY_BRACE whereConditionWithPrecondAfter				{ $$ = IsConditionAndNextWhereConditionSemanticAction($2, $4); }
	;

whereConditionWithPrecond: OPEN_CURLY_BRACE AND colonWhere[node1]												{ $$ = PreconditionalWhereConditionSemanticAction($node1, $2); }
	| OPEN_CURLY_BRACE OR colonWhere[node2]																		{ $$ = PreconditionalWhereConditionSemanticAction($node2, $2); }
	;

whereConditionWithPrecondAfter: COMMA whereConditionWithPrecond													{ $$ = $2; }
	| %empty																									{ $$ = NULL; }
	;

whereCondition: OPEN_BRACKET whereCondition whereConditionWithPrecondAfter CLOSE_BRACKET			{ $$ = FirstCurrentAndNextWhereConditionsSemanticAction($2, $3); }						
	| OPEN_BRACKET whereBinaryCondition whereConditionWithPrecondAfter CLOSE_BRACKET				{ $$ = FirstBinaryConditionAndNextWhereConditionSemanticAction($2, $3); }
	| OPEN_BRACKET whereNotCondition whereConditionWithPrecondAfter CLOSE_BRACKET					{ $$ = FirstNotConditionAndNextWhereConditionSemanticAction($2, $3); }
	| OPEN_BRACKET whereIsCondition whereConditionWithPrecondAfter CLOSE_BRACKET					{ $$ = FirstIsConditionAndNextWhereConditionSemanticAction($2, $3); }
	;

whereInCondition: OPEN_CURLY_BRACE IN COLON json CLOSE_CURLY_BRACE									{ $$ = QueryWhereInConditionSemanticAction($4); }
	;

whereIsCondition: OPEN_CURLY_BRACE IS COLON whereNotCondition CLOSE_CURLY_BRACE						{ $$ = WhereNotConditionWhereIsConditionSemanticAction($4); }
	| OPEN_CURLY_BRACE IS COLON whereInCondition CLOSE_CURLY_BRACE									{ $$ = WhereInConditionWhereIsConditionSemanticAction($4); }
	;
//ORDER BY CLAUSE
orderByClauseArgsList: orderByClauseValue							{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| OPEN_BRACKET orderByClauseValues CLOSE_BRACKET				{ $$ = $2; }
	;

orderByClauseValues: orderByClauseValue								{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| orderByClauseValue COMMA orderByClauseValues					{ $$ = ClauseArgsListSemanticAction($1, $3); }
	;

orderByClauseValue: STRING											{ $$ = StringOrderByClauseValueSemanticAction($1); }
	//Funcion de agregacion
	| OPEN_CURLY_BRACE FUNCTION COLON aggregationFunction[aggr] COMMA ATTRIBUTE COLON STRING[attr] COMMA ORDER COLON orderByExplicit[order] CLOSE_CURLY_BRACE	{ $$ = AggregationFunctionOrderByClauseValueSemanticAction($aggr, $attr, $order); }
	//Solo atributo, como en sql y que defaultee a ASC (el motor de BD)
	| OPEN_CURLY_BRACE ATTRIBUTE COLON STRING[attr] CLOSE_CURLY_BRACE	{ $$ = StringOrderByClauseValueSemanticAction($attr); }
	//Atributo con order ASC o DESC
	| OPEN_CURLY_BRACE ATTRIBUTE COLON STRING[attr] COMMA ORDER COLON orderByExplicit[expl] CLOSE_CURLY_BRACE { $$ = CompositeOrderByClauseValueSemanticAction($attr, $expl); }
	;

orderByExplicit: ASC												{ $$ = OrderBySemanticAction($1); }
	| DESC															{ $$ = OrderBySemanticAction($1); }
;

//ATTRIBUTES CLAUSE
aggregationFunction: COUNT											{ $$ = $1; }
	| SUM 															{ $$ = $1; }
	| AVERAGE														{ $$ = $1; }
	;

aggregation: FUNCTION COLON aggregationFunction COMMA attribute		{ $$ = AggregationSemanticAction($3, $5); }

attributeOptions: COMMA TABLE COLON STRING							{ $$ = OnlyTableAttributeOptionSemanticAction($4); }
	| COMMA AS COLON STRING											{ $$ = OnlyAsAttributeOptionSemanticAction($4); }
	| COMMA TABLE COLON STRING COMMA AS COLON STRING				{ $$ = TableAndAsAttributeOptionSemanticAction($4, $8); }
	| %empty														{ $$ = EmptyAttributeOptionSemanticAction(); }
	;

attribute: NAME COLON STRING attributeOptions						{ $$ = AttributeSemanticAction($3, $4); }
	;

attributesClauseValue: OPEN_CURLY_BRACE attribute CLOSE_CURLY_BRACE		{ $$ = AttributeClauseValueSemanticAction($2); }
	| OPEN_CURLY_BRACE aggregation CLOSE_CURLY_BRACE					{ $$ = AttributeClauseValueSemanticAction($2); }
	;

attributesClauseValues: attributesClauseValue						{ $$ = ClauseArgsListSemanticAction($1, NULL); }
	| attributesClauseValue COMMA attributesClauseValues			{ $$ = ClauseArgsListSemanticAction($1, $3); }
	;

attributesClauseArgsList: OPEN_BRACKET attributesClauseValues CLOSE_BRACKET				{ $$ = $2; }
	;

//GROUP BY CLAUSE
groupByClause: STRING												{ $$ = StringGroupByClauseSemanticAction($1); }
	| OPEN_BRACKET groupByValues CLOSE_BRACKET						{ $$ = $2; }
	;

groupByValues: groupByValue											{ $$ = ClauseArgsListSemanticAction($1, NULL); }		
	| groupByValue COMMA groupByValues								{ $$ = ClauseArgsListSemanticAction($1, $3); }	
	;

groupByValue: STRING												{ $$ = GroupByValueSemanticAction($1); }
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
