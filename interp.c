#include "interp.h"

harp_node_t* eval_expr(harp_node_t* node) {
    switch(node->type) {
        case NT_ATOM: {
            return node;
        }
        case NT_REAL_LITERAL: {
            return node;
        }
        default: { break; }
    }
    return node;
}

harp_node_t* harp_eval_expr(char* buff, size_t len) {
    harp_lexer_t lex = harp_create_lexer(buff, len);
    harp_node_t* ast = harp_get_node(&lex);

    assert(ast);
    assert(ast->type == NT_LIST);
    if (ast->child == NULL) return harp_new_node(NT_NONE);

    harp_node_t* next = ast->child;

    if (!next->next) return eval_expr(next);

    return NULL;
}
