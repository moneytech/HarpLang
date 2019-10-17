#include "ast.h"

/*
<s_expression>  ::= <atomic_symbol>
                  | "(" <s_expression> "." <s_expression> ")"
                  | <list> .
<_list>         ::= <s_expression> <_list>
                  | <s_expression> .
<list>          ::= "(" <s_expression> <_list> ")" .
<atomic_symbol> ::= <letter> <atom_part> .
<atom_part>     ::= <empty> | <letter> <atom_part> | <number> <atom_part> .
<letter>        ::= "a" | "b" | "c" | "d" | "e" | "f" | "g" | "h" | "i" | "j"
                  | "k" | "l" | "m" | "n" | "o" | "p" | "q" | "r" | "s" | "t"
                  | "u" | "v" | "w" | "x" | "y" | "z" .
<number>        ::= "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" | "0" .
<empty>         ::= " ".
*/

void harp_quote(harp_node_t* node) {
    // Recursively quote

    node->flags |= FLAG_QUOTED;
    if (node->type != NT_REAL_LITERAL &&
        node->type != NT_ATOM) {
        harp_node_t* it = node->child;
        while (it) {
            harp_quote(it);
            it = it->next;
        }
    }
}

harp_node_t* harp_new_node(int type) {
    harp_node_t* node = malloc(sizeof(harp_node_t));
    if (!node) return NULL;
    node->flags = 0;
    node->next = node->child = NULL;
    node->type = type;
    return node;
}

harp_str_t get_str_from_tok(harp_tok_t* tok) {
    harp_str_t result;
    result.data = malloc(tok->end-tok->start+1);
    result.len = tok->end-tok->start;
    strncpy(result.data, tok->start, tok->end-tok->start);
    result.data[result.len] = '\0';
    return result;
}

harp_node_t* parse_s_expr(harp_lexer_t* lex) {
    harp_tok_t tok = harp_get_tok(lex);

    bool quoted = false;
    if (tok.type == TT_QUOTE) {
        quoted = true;
        tok = harp_get_tok(lex);
    }

    if (tok.type != TT_EOF) {
        switch(tok.type) {
            case TT_ATOM: {
                harp_node_t* result = harp_new_node(NT_ATOM);
                result->value.atom = get_str_from_tok(&tok);
                if (quoted) harp_quote(result);
                return result;
            }
            case TT_NUMBER: {
                harp_node_t* result = harp_new_node(NT_REAL_LITERAL);
                char buff[256] = {'\0'};
                sprintf(buff, "%.*s", (int)(tok.end-tok.start), tok.start);
                result->value.number = atof(buff);
                if (quoted) harp_quote(result);
                return result;
            }
            case TT_OPEN_BRACKET: {}
            case TT_CLOSE_BRACKET: {}
            case TT_OPEN_PAREN: {
                harp_node_t* result = harp_new_node(NT_EXPRESSION);
                harp_node_t *lst = NULL, *fst = NULL;
                while (1) {
                    harp_node_t* v = parse_s_expr(lex);
                    if (!v) break;
                    if (!lst) { lst = v; fst = lst; }
                    else {
                        lst->next = v;
                        lst = v;
                    }
                }
                result->child = fst;
                if (quoted) harp_quote(result);
                return result;
            }
            case TT_CLOSE_PAREN: {
                return NULL;
            }
            default:
                printf("Unhandled token type %s\n", harp_token_type_names[tok.type]);
                break;
        }
    }
    return NULL;
}

harp_node_t* harp_get_node(harp_lexer_t* lex) {
    harp_node_t* result = harp_new_node(NT_LIST);
    harp_node_t* next = result;
    next->child = parse_s_expr(lex);
    next = next->child;

    while (1) {
        harp_node_t* val = parse_s_expr(lex);
        if (!val) break;
        next->next = val;
        next = next->next;
        if (lex->it == lex->end) break;
    }

    return result;
}
