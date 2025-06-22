%{

#include "FlexActions.h"

%}

/**
 * Enable start condition manipulation functions.
 */
%option stack



/**
 * Reusable patterns.
 *
 * @see https://westes.github.io/flex/manual/Matching.html
 * @see https://westes.github.io/flex/manual/Patterns.html
 */

%%



"\"from\""                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), FROM); }
"\"attributes\""                    { return KeyWordLexemeAction(createLexicalAnalyzerContext(), ATTRIBUTES); }
"\"where\""                         { return KeyWordLexemeAction(createLexicalAnalyzerContext(), WHERE); }
"\"order_by\""                      { return KeyWordLexemeAction(createLexicalAnalyzerContext(), ORDER_BY); }
"\"group_by\""                      { return KeyWordLexemeAction(createLexicalAnalyzerContext(), GROUP_BY); }
"\"auxiliary\""                     { return KeyWordLexemeAction(createLexicalAnalyzerContext(), AUXILIARY); }

"\"table\""                         { return KeyWordLexemeAction(createLexicalAnalyzerContext(), TABLE); }
"\"as\""                            { return KeyWordLexemeAction(createLexicalAnalyzerContext(), AS); }

"\"attribute\""                     { return KeyWordLexemeAction(createLexicalAnalyzerContext(), ATTRIBUTE); }
"\"name\""                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), NAME); }
"\"function\""                      { return KeyWordLexemeAction(createLexicalAnalyzerContext(), FUNCTION); }
"\"order\""                         { return KeyWordLexemeAction(createLexicalAnalyzerContext(), ORDER); }
"\"count\""                         { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), COUNT); }
"\"sum\""                           { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), SUM); }
"\"average\""                       { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), AVERAGE); }
"\"min\""                           { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), MIN); }
"\"max\""                           { return AggregationFunctionLexemeAction(createLexicalAnalyzerContext(), MAX); }

"\"asc\""                               { return OrderByFunctionLexemeAction(createLexicalAnalyzerContext(), ASC); }
"\"desc\""                              { return OrderByFunctionLexemeAction(createLexicalAnalyzerContext(), DESC); }

"null"                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), NULLV); }
"true"                          { return BooleanLexemeAction(createLexicalAnalyzerContext(), TRUE); }
"false"                         { return BooleanLexemeAction(createLexicalAnalyzerContext(), FALSE); }
"\"value1\""                        { return KeyWordLexemeAction(createLexicalAnalyzerContext(), FIRST_VALUE); }
"\"value2\""                        { return KeyWordLexemeAction(createLexicalAnalyzerContext(), SECOND_VALUE); }
"\"operator\""                      { return KeyWordLexemeAction(createLexicalAnalyzerContext(), OPERATOR); }
"\"not\""                           { return KeyWordLexemeAction(createLexicalAnalyzerContext(), NOT); }
"\"and\""                           { return PreconditionalLexemeAction(createLexicalAnalyzerContext(), AND); }
"\"or\""                            { return PreconditionalLexemeAction(createLexicalAnalyzerContext(), OR); }
"\"in\""                            { return KeyWordLexemeAction(createLexicalAnalyzerContext(), IN); }
"\"is\""                            { return KeyWordLexemeAction(createLexicalAnalyzerContext(), IS); }
"\"join\""                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), JOIN); }
"\"type\""                          { return KeyWordLexemeAction(createLexicalAnalyzerContext(), TYPE); }
"\"outer\""                         { return KeyWordLexemeAction(createLexicalAnalyzerContext(), OUTER); }
"\"right\""                         { return JoinTypeLexemeAction(createLexicalAnalyzerContext(), RIGHT); }
"\"left\""                          { return JoinTypeLexemeAction(createLexicalAnalyzerContext(), LEFT); }
"\"inner\""                         { return JoinTypeLexemeAction(createLexicalAnalyzerContext(), INNER); }
"\"condition\""                     { return KeyWordLexemeAction(createLexicalAnalyzerContext(), CONDITION); }

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


[[:digit:]]+						{ return IntegerLexemeAction(createLexicalAnalyzerContext()); }
"-?[0-9]+(\.[0-9]+)?"               { return NumberLexemeAction(createLexicalAnalyzerContext()); }

[[:space:]]+						{ IgnoredLexemeAction(createLexicalAnalyzerContext()); }
.									{ return UnknownLexemeAction(createLexicalAnalyzerContext()); }

%%

#include "FlexExport.h"
