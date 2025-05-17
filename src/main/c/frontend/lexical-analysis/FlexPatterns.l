%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack

/**
 * Flex contexts (a.k.a. start conditions).
 *
 * @see https://westes.github.io/flex/manual/Start-Conditions.html
 */
%x MULTILINE_COMMENT

/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%

"/*"								{ BEGIN(MULTILINE_COMMENT); BeginMultilineCommentLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>"*/"				{ EndMultilineCommentLexemeAction(createLexicalAnalyzerContext()); BEGIN(INITIAL); }
<MULTILINE_COMMENT>[[:space:]]+		{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>[^*]+			{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
<MULTILINE_COMMENT>.				{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }

"\"from\""                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), FROM); }
"\"attributes\""                    { return KeyWordLexemeAction(createLexicalAnalyzerContext(), ATTRIBUTES); }
"\"where\""                         { return KeyWordLexemeAction(createLexicalAnalyzerContext(), WHERE); }

"\"table\""                         { return KeyWordLexemeAction(createLexicalAnalyzerContext(), TABLE); }
"\"as\""                            { return KeyWordLexemeAction(createLexicalAnalyzerContext(), AS); }

"\"attribute\""                     { return KeyWordLexemeAction(createLexicalAnalyzerContext(), ATTRIBUTE); }
"\"function\""                      { return KeyWordLexemeAction(createLexicalAnalyzerContext(), FUNCTION); }

"\"count\""                         { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), COUNT); }
"\"sum\""                           { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), SUM); }
"\"average\""                       { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), AVERAGE); }

"\"null\""                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), NULLV); }
"\"true\""                          { return BooleanLexemeAction(createLexicalAnalyzerContext(), TRUE); }
"\"false\""                         { return BooleanLexemeAction(createLexicalAnalyzerContext(), FALSE); }
"\"value1\""                        { return KeyWordLexemeAction(createLexicalAnalyzerContext(), FIRST_VALUE); }
"\"value2\""                        { return KeyWordLexemeAction(createLexicalAnalyzerContext(), SECOND_VALUE); }
"\"operator\""                      { return KeyWordLexemeAction(createLexicalAnalyzerContext(), OPERATOR); }
"\"not\""                           { return KeyWordLexemeAction(createLexicalAnalyzerContext(), NOT); }
"\"and\""                           { return PreconditionalLexemeAction(createLexicalAnalyzerContext(), AND); }
"\"or\""                            { return PreconditionalLexemeAction(createLexicalAnalyzerContext(), OR); }
"\"in\""                            { return KeyWordLexemeAction(createLexicalAnalyzerContext(), IN); }
"\"is\""                            { return KeyWordLexemeAction(createLexicalAnalyzerContext(), IS); }

"\"<\""                                 { return BinaryConditionOperatorLexemeAction(createLexicalAnalyzerContext(), LOWER); }
"\">\""                                 { return BinaryConditionOperatorLexemeAction(createLexicalAnalyzerContext(), GREATER); }
"\"<=\""                                { return BinaryConditionOperatorLexemeAction(createLexicalAnalyzerContext(), LOWER_OR_EQUAL); }
"\">=\""                                { return BinaryConditionOperatorLexemeAction(createLexicalAnalyzerContext(), GREATER_OR_EQUAL); }
"\"=\""                                 { return BinaryConditionOperatorLexemeAction(createLexicalAnalyzerContext(), EQUAL); }
"\"!=\""                                { return BinaryConditionOperatorLexemeAction(createLexicalAnalyzerContext(), NOT_EQUAL); }

"{"                                 { return KeyWordLexemeAction(createLexicalAnalyzerContext(), OPEN_CURLY_BRACE); }
"}"                                 { return KeyWordLexemeAction(createLexicalAnalyzerContext(), CLOSE_CURLY_BRACE); }
":"                                 { return KeyWordLexemeAction(createLexicalAnalyzerContext(), COLON); }
","                                 { return KeyWordLexemeAction(createLexicalAnalyzerContext(), COMMA); }        
"["                                 { return KeyWordLexemeAction(createLexicalAnalyzerContext(), OPEN_BRACKET); }
"]"                                 { return KeyWordLexemeAction(createLexicalAnalyzerContext(), CLOSE_BRACKET); }    

\"([^\\\"]|\\.)*\"                  { return StringLexemeAction(createLexicalAnalyzerContext()); }


"-"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), SUB); }
"*"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), MUL); }
"/"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), DIV); }
"+"									{ return ArithmeticOperatorLexemeAction(createLexicalAnalyzerContext(), ADD); }

"("									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), OPEN_PARENTHESIS); }
")"									{ return ParenthesisLexemeAction(createLexicalAnalyzerContext(), CLOSE_PARENTHESIS); }

[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }
"-?[0-9]+(\.[0-9]+)?"               { return NumberLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
