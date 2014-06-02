#include <stdio.h>
#include "token.h"

void print_token(Token *t) {
    printf("type [%d]  text [%s]\n", t->type, t->text);
    fflush(stdout);
}

