#ifndef __TOKEN_H
#define __TOKEN_H

/** @file token.h support file for lexer and parser */

#include <fstream>
using namespace std;

#include <QString>

/** Struct for passing data from lexer to parser. */
typedef struct Token {

        /** Type */
        int type;

        /** Value for integer tokens */
        unsigned int value;

        /** The original text of the token. */
        QString text;
} Token;

/** Lexer input file */
extern ifstream* lexin;

/** Lexer, manually written. */
int yylex(void);

/** Parser, generated by bison. */
int yyparse(void);

/** Number of lexing/parsing errors encountered, zero on successful parse. */
extern int error_counter;

#endif
