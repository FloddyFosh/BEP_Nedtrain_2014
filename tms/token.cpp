#include <stdio.h>
#include "token.h"

void print_token(Token *t) {
	fprintf(stderr, "type [%d]  text [%s]\n", t->type, t->text);
}

