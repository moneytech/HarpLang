#include "interp.h"

harp_node_t* eval_expr(harp_node_t* node);
harp_node_t* eval_function(harp_node_t* atom, harp_node_t* args);
harp_node_t* harp_eq(harp_node_t* args);
harp_node_t* harp_eval_expr(harp_node_t* node) { return eval_expr(node); }

harp_node_t* harp_car(harp_node_t* list) {
    return eval_expr(list->child);
}

harp_node_t* harp_cdr(harp_node_t* list) {
    harp_node_t* result = harp_new_node(NT_LIST);
    harp_node_t* first = eval_expr(list);
    if (!first || !first->next) return result;
    result->child = first->next;
    return result;
}

// Learn more about cons and cons cells, new data type?
harp_node_t* harp_cons(harp_node_t* args) {
    return args;
}

harp_node_t* harp_test(harp_node_t* args) {
    harp_node_t* result = harp_new_node(NT_BOOLEAN_LITERAL);
    args = eval_expr(args);
    result->value.boolean = args->type == NT_REAL_LITERAL
        ? args->value.number != 0
        : ((args->type == NT_BOOLEAN_LITERAL)
          ? args->value.boolean
          : 0);
    return result;
}

harp_node_t* harp_len(harp_node_t* list) {
    assert(list->type == NT_LIST || list->type == NT_EXPRESSION);
    int n = 0;
    harp_node_t* it = list->child;
    while (it) {
        n++;
        it = it->next;
    }
    harp_node_t* result = harp_new_node(NT_REAL_LITERAL);
    result->value.number = n;
    return result;
}

void harp_free(harp_node_t* node) {
    if (node->type == NT_ATOM) {
        free(node->value.string.data);
        node->value.string.data = 0;
    }

    free(node);
    node = NULL;
}

harp_node_t* harp_eq_ab(harp_node_t* a, harp_node_t* b) {
    harp_node_t* result = harp_new_node(NT_BOOLEAN_LITERAL);
    int res_bool = 0;
    if (a->type != b->type) res_bool = 0;
    if (a->type == NT_BOOLEAN_LITERAL)
        res_bool = a->value.boolean == b->value.boolean;
    else if (a->type == NT_REAL_LITERAL)
        res_bool = a->value.number == b->value.number;
    else if (a->type == NT_LIST || a->type == NT_EXPRESSION) {
        harp_node_t* lena = harp_len(a);
        harp_node_t* lenb = harp_len(b);
        harp_node_t* cmp = harp_eq_ab(lena, lenb);
        if (!cmp->value.boolean) {
            res_bool = false;
        }
        else {
            harp_node_t* ita = a->child;
            harp_node_t* itb = b->child;
            while (ita && itb) {

                harp_node_t* cmp = harp_eq_ab(ita, itb);
                res_bool = cmp->value.boolean;
                harp_free(cmp);

                if (!res_bool) break; 

                ita = ita->next;
                itb = itb->next;
            }
        }
        harp_free(lena); harp_free(lenb); harp_free(cmp);
        lena = lenb = cmp = NULL;
    }
    else {
        printf("Cannot compare %s\n", harp_node_type_names[a->type]);
        assert(0);
    }
    result->value.boolean = res_bool;
    return result;
}

harp_node_t* harp_eq(harp_node_t* args) {
    harp_node_t* a = eval_expr(args);
    harp_node_t* b = eval_expr(args->next);
    return harp_eq_ab(a, b);
}

harp_node_t* eval_function(harp_node_t* atom, harp_node_t* args) {
    // We have next->next
    atom = eval_expr(atom);

    if (atom->type != NT_ATOM) {
        printf("illegal function call [%s]\n", harp_node_type_names[atom->type]);
        assert(0);
    }

    if (atom->value.string.len == 1){
        switch(atom->value.string.data[0]) {
            case '+': {
                double acc = 0.0;
                while (args) {
                    harp_node_t* a = args;
                    if (args->type == NT_EXPRESSION || args->type == NT_PROGN)
                        a = eval_expr(args);
                    if (a->type != NT_REAL_LITERAL)
                        printf("cannot add a non numaric type %s\n", harp_node_type_names[args->type]);
                    acc += a->value.number;
                    args = args->next;
                }
                harp_node_t* result = harp_new_node(NT_REAL_LITERAL);
                result->value.number = acc;
                return result;
            }
            case '-': {
                double acc = 0.0;
                bool fst = true;
                while (args) {
                    harp_node_t* a = args;
                    if (args->type == NT_EXPRESSION || args->type == NT_PROGN)
                        a = eval_expr(args);
                    if (a->type != NT_REAL_LITERAL)
                        printf("cannot minus a non numaric type %s\n", harp_node_type_names[args->type]);
                    if (!fst) acc -= a->value.number;
                    else { fst = false; acc = a->value.number; }
                    args = args->next;
                }
                harp_node_t* result = harp_new_node(NT_REAL_LITERAL);
                result->value.number = acc;
                return result;
            }
            case '*': {
                double acc = 1.0;
                while (args) {
                    harp_node_t* a = args;
                    if (args->type == NT_EXPRESSION || args->type == NT_PROGN)
                        a = eval_expr(args);
                    if (a->type != NT_REAL_LITERAL)
                        printf("cannot multiple by a non numaric type %s\n", harp_node_type_names[args->type]);
                    acc *= a->value.number;
                    args = args->next;
                }
                harp_node_t* result = harp_new_node(NT_REAL_LITERAL);
                result->value.number = acc;
                return result;
            }
            case '/': {
                double acc = 0.0;
                bool fst = true;
                while (args) {
                    harp_node_t* a = args;
                    if (args->type == NT_EXPRESSION || args->type == NT_PROGN)
                        a = eval_expr(args);
                    if (a->type != NT_REAL_LITERAL)
                        printf("cannot divide by a non numaric type %s\n", harp_node_type_names[args->type]);
                    if (!fst) acc /= a->value.number;
                    else { fst = false; acc = a->value.number; }
                    args = args->next;
                }
                harp_node_t* result = harp_new_node(NT_REAL_LITERAL);
                result->value.number = acc;
                return result;
            }
        }
    } else {
        if (strcmp(atom->value.string.data, "car") == 0) {
            return harp_car(args);
        } else if (strcmp(atom->value.string.data, "cdr") == 0) {
            return harp_cdr(args);
        } else if (strcmp(atom->value.string.data, "cons") == 0) {
            return harp_cons(args);
        } else if (strcmp(atom->value.string.data, "?") == 0) {
            // TODO(Dustin): Debug crashing??
            return harp_test(args);
        } else if (strcmp(atom->value.string.data, "test") == 0) {
            return harp_test(args);
        } else if (strcmp(atom->value.string.data, "eq?") == 0) {
            return harp_eq(args);
        } else if (strcmp(atom->value.string.data, "==") == 0) {
            return harp_eq(args);
        } else if (strcmp(atom->value.string.data, "len") == 0) {
            return harp_len(args);
        } else if (strcmp(atom->value.string.data, "neq?") == 0) {
            harp_node_t* res = harp_eq(args);
            res->value.boolean = !res->value.boolean;
            return res;
        } else if (strcmp(atom->value.string.data, "!=") == 0) {
            harp_node_t* res = harp_eq(args);
            res->value.boolean = !res->value.boolean;
            return res;
        }
    }

#undef BIN_OP

    return NULL;
}

harp_node_t* eval_expr(harp_node_t* node) {
    switch(node->type) {

        // Literals
        case NT_ATOM:
        case NT_REAL_LITERAL:
        case NT_BOOLEAN_LITERAL: {
            return node;
        }

        case NT_PROGN: {
            harp_node_t* it = eval_expr(node->child);
            harp_node_t* last_v = NULL;

            while (it) {
                last_v = eval_expr(it);
                it = it->next;
            }

            return last_v;
        }
        case NT_EXPRESSION: {
            if (node->child == NULL) {
                return harp_new_node(NT_LIST);
            }

            if (node->flags &= FLAG_QUOTED) {
                node->type = NT_LIST;
                return node;
            }

            harp_node_t* first = node->child;
            return eval_function(first, first->next);
        }
        default: { break; }
    }
    return node;
}

harp_node_t* harp_eval_string(char* buff, size_t len) {
    harp_lexer_t lex = harp_create_lexer(buff, len);
    harp_node_t* ast = harp_get_node(&lex);

    assert(ast);
    assert(ast->type == NT_LIST);
    if (ast->child == NULL) return harp_new_node(NT_NONE);

    harp_node_t* next = ast->child;

    if (!next->next) {
        if (next->flags &= FLAG_QUOTED) {
            if (next->type == NT_EXPRESSION)
                next->type = NT_LIST;
            return next;
        }
        return eval_expr(next);
    }

    return ast;
}
