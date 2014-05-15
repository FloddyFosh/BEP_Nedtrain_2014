/* Lexical analyzer that creates Token structs for all matched tokens
   and prints error messages for unmatched tokens. Tries to match as
   much of GNU 'as' AT&T syntax assembly as possible. */

%option nounput

%{

#include <string.h>

#include "token.h"
#include <string.h>
#include "grammar.tab.hpp"	/* import token definitions from Yacc */

#define YY_NO_INPUT

Token *new_token(int);

%}

TYPE		[RJTAQPpSDFG]
STRING		\"[^\n"]*\"
INT			(0|([1-9][0-9]*))
WHITESPACE	[\t\r ]
COMMENT		#.*

%%

\n				{ yylval.token = new_token(T_LINE_SEP); return yylval.token->type; }

{TYPE}			{ yylval.token = new_token(yytext[0]); return yylval.token->type; }
{INT}			{ yylval.token = new_token(T_INT); return yylval.token->type; }
{STRING}		{ yylval.token = new_token(T_STRING); return yylval.token->type; }

{WHITESPACE}+
{COMMENT}

.				{ fprintf(stderr, "error: unexpected character in input: \"%s\"\n", yytext); }

%%

/*
 * creates Token struct, sets type, copies yytext
 * and sets context info (line and column number)
 */
Token *new_token(int type) {
	Token *token = new Token;
	token->type = type;
	token->text = strdup(yytext);
	return token;
}
