#ifndef __TOKEN_H
#define __TOKEN_H

typedef struct Token {
	int type;
	char *text;
} Token;

int yylex(void);
int yyparse(void);

extern FILE *yyin;
extern int error_counter;

#endif
