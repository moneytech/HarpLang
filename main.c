#include <stdio.h>

#include "tests.h"
#include "harp.h"
#include "lexer.h"
#include "ast.h"
#include "interp.h"

void print_node_as_value(harp_node_t* node) {
    if (!node) printf("none\n");
    switch(node->type) {
        case NT_REAL_LITERAL: {
            printf("%f\n", node->value.number);
            break;
        }
        case NT_ATOM: {
            printf("%.*s\n", (int)node->value.string.len, node->value.string.data);
            break;
        }
        default: {
            printf("none\n");
            break;
        }
    }
}

void run_repl() {
    while (1) {
        printf(">");
        char buff[512];
        fgets(buff, sizeof(buff), stdin);
        if (strcmp(buff, "(exit)") == 0){
            //@HACK
            break;
        }
        print_node_as_value(harp_eval_expr(buff, strlen(buff)));
    }
}

int main() {
    harp_run_all_tests();

    printf("Harp programming language, Version: %s\n", HARP_VERSION);

    run_repl();

    return 0;
}
