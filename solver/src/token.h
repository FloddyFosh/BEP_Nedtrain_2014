#ifndef __TOKEN_H
#define __TOKEN_H

typedef struct Token {
	int type;
	char *text;
} Token;

void print_token(Token *t);

int yylex(void);
int yyparse(void);

extern FILE *yyin;
extern int error_counter;

#endif
