#include "tests.c"

#include <stdio.h>

#include "harp.h"
#include "lexer.h"

int main() {
    printf("Harp programming language, Version: %s\n", HARP_VERSION);

    harp_lexer_t lex = harp_create_lexer("Hello", 5);

    return 0;
}
