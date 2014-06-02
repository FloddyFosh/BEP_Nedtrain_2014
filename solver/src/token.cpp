#include <stdio.h>
#include "token.h"

void print_token(Token *t) {
    fprintf(stdout, "type [%d]  text [%s]\n", t->type, t->text);
}

