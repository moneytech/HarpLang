#include <stdio.h>

#include "tests.h"
#include "harp.h"
#include "lexer.h"
#include "ast.h"
#include "interp.h"

void print_node_as_value(harp_node_t* node) {
    if (!node) printf("nil");
    switch(node->type) {

        case NT_REAL_LITERAL:
            printf("%f", node->value.number);
            break;

        case NT_ATOM:
            printf("%.*s", (int)node->value.string.len, node->value.string.data);
            break;

        case NT_BOOLEAN_LITERAL:
            printf("%s", node->value.boolean?"#t":"#f");
            break;

        case NT_LIST: {
            if (!node->child) {
                printf("()");
            } else {
                harp_node_t* it = harp_car(node);
                printf("(");
                while (it) {
                    if (node->flags &= FLAG_QUOTED)
                        it->flags |= FLAG_QUOTED;

                    print_node_as_value(it);
                    it = it->next;
                    if (it) { printf(" "); }
                }
                printf(")");
            }
            break;
        }
        case NT_EXPRESSION: {
            if (node->flags &= FLAG_QUOTED) node->type = NT_LIST;
            harp_node_t *node_ev = node->flags & FLAG_QUOTED
                ? node
                : harp_eval_expr(node);
            print_node_as_value(node_ev);
            break;
        }
        default: {
            printf("none::[%s]", harp_node_type_names[node->type]);
            break;
        }
    }
}

void run_repl() {
    while (1) {
        printf("> ");
        char buff[512];
        fgets(buff, sizeof(buff), stdin);
        if (strcmp(buff, "(exit)") == 0){
            //@HACK
            break;
        }
        print_node_as_value(harp_eval_string(buff, strlen(buff)));
        printf("\n");
    }
}

int main() {
    harp_run_all_tests();

    printf("Harp programming language, Version: %s\n", HARP_VERSION);

    run_repl();

    return 0;
}
