#include "lexer.h"

harp_lexer_t harp_create_lexer(char* buff, size_t len) {
    return (harp_lexer_t) {
            .start = buff,
            .end = buff + len,
            .it = buff,
            .len = len,
    };       
}

void harp_lexer_set_buff(harp_lexer_t* lex, char* buff, size_t len) {
    lex->it = lex->start = buff;
    lex->end = buff + len;
    lex->len = len;
}

bool harp_lexeme_equals_num(harp_tok_t* tok, double n) {
    char b[256] = {0};
    sprintf(b, "%.*s", (int)(tok->end-tok->start), tok->start);
    return atof(b) == n;
}

void harp_skip_space(harp_lexer_t* lex) {
    while (lex->it != lex->end && isspace((*lex->it)))
        ++lex->it;
}

harp_tok_t harp_get_tok(harp_lexer_t* lex) {
    harp_skip_space(lex);
    if (lex->it == lex->end) return (harp_tok_t){.type = TT_EOF};

    switch(*lex->it){
        case '(': return (harp_tok_t){.type=TT_OPEN_PAREN, .start=lex->it++, .flags = 0};
        case ')': return (harp_tok_t){.type=TT_CLOSE_PAREN, .start=lex->it++, .flags = 0};
        case '[': return (harp_tok_t){.type=TT_OPEN_BRACKET, .start=lex->it++, .flags = 0};
        case ']': return (harp_tok_t){.type=TT_CLOSE_BRACKET, .start=lex->it++, .flags = 0};
        default: break;
    }

    {
        char *begin = lex->it, *walk = lex->it;
        bool has_decimal = false, negative = false;

        if (*walk == '-') { ++walk; negative = true; }
        if (walk != lex->end && *walk == '.') { ++walk; has_decimal = true; }

        if (walk != lex->end && isdigit(*walk)) {
            // Parse the number
            while (walk != lex->end) {
                if ((*walk) == '.') {
                    //TODO(Dustin): @ThrowError
                    assert(!has_decimal);
                    has_decimal = true;
                } else if (!isdigit((*walk))) {
                    lex->it = walk;
                    int flags = 0;
                    if (negative) flags |= TF_IS_NEGATIVE;
                    if (has_decimal) flags |= TF_IS_REAL;
                    return (harp_tok_t) {
                        .type = TT_NUMBER,
                        .flags = flags,
                        .start = begin,
                        .end = walk,
                    };
                }
                walk++;
            }

            if (walk == lex->end) {
                int flags = 0;
                if (negative) flags |= TF_IS_NEGATIVE;
                if (has_decimal) flags |= TF_IS_REAL;
                lex->it = walk;
                return (harp_tok_t) {
                    .type = TT_NUMBER,
                    .flags = flags,
                    .start = begin,
                    .end = walk,
                };
            }
        }
    }

    {
        char* start = lex->it;
        while (lex->it != lex->end) {
            if (isspace(*lex->it)
                || *lex->it == '('
                || *lex->it == ')'
                || *lex->it == '['
                || *lex->it == ']') {
                return (harp_tok_t) {
                    .type = TT_ATOM,
                    .flags = 0,
                    .start = start,
                    .end = lex->it,
                };
            }
            lex->it++;
        }

        if (lex->it == lex->end && lex->start != lex->it) {
            return (harp_tok_t) {
                .type = TT_ATOM,
                .flags = 0,
                .start = start,
                .end = lex->it,
            };
        }
    }

    return (harp_tok_t) { .type = TT_EOF, .flags = 0 };
}
