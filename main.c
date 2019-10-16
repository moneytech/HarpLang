#include <stdio.h>

#include "tests.h"
#include "harp.h"
#include "lexer.h"
#include "ast.h"
#include "interp.h"

int main() {
    harp_run_all_tests();

    printf("Harp programming language, Version: %s\n", HARP_VERSION);

    return 0;
}
