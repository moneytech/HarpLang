#ifndef HARP_LEXER_H
#define HARP_LEXER_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <assert.h>

typedef struct {
    char* start;
    char* end;
    char* it;
    size_t len;
} harp_lexer_t;

#ifndef HARP_MACROSTR
#define HARP_MACROSTR(k) #k
#endif//HARP_MACROSTR

#define X_HARP_TOKEN_TYPES                      \
        X(TT_ATOM)                              \
        X(TT_OPEN_PAREN)                        \
        X(TT_CLOSE_PAREN)                       \
        X(TT_OPEN_BRACKET)                      \
        X(TT_CLOSE_BRACKET)                     \
        X(TT_NUMBER)                            \
        X(TT_STRING)                            \
        X(TT_QUOTE)                             \
        X(TT_EOF)

enum harp_token_types {
#define X(Enum) Enum,
    X_HARP_TOKEN_TYPES
#undef X
};

static const char* harp_token_type_names[] = {
#define X(String) HARP_MACROSTR(String),
    X_HARP_TOKEN_TYPES
#undef X
};

enum TokenFlags {
    TF_IS_NEGATIVE = 1 << 0,
    TF_IS_REAL     = 1 << 1,
};

typedef struct {
    int type;
    int flags;
    char* start;
    char* end;
} harp_tok_t;

bool harp_lexeme_equals_num(harp_tok_t* tok, double n);

harp_lexer_t harp_create_lexer(char* buff, size_t len);
void harp_lexer_set_buff(harp_lexer_t* lex, char* buff, size_t len);

void harp_skip_space(harp_lexer_t* lex);
harp_tok_t harp_get_tok(harp_lexer_t* lex);

#endif//HARP_lEXER_H
