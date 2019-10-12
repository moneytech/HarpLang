#ifndef HARP_AST_H
#define HARP_AST_H

#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"

#ifndef HARP_MACROSTR
#define HARP_MACROSTR(k) #k
#endif

#define X_AST_NODE_TYPES\
    X(NT_EXPRESSION)\
    X(NT_LIST)\
    X(NT_REAL_LITERAL)\
    X(NT_ATOM)\
    X(NT_C_FUNCTION)\
    X(NT_LAMBDA)\
    X(NT_PROGN)

enum harp_node_types {
#define X(Enum) Enum,
    X_AST_NODE_TYPES
#undef X
};

static const char* harp_node_type_names[] = {
#define X(String) HARP_MACROSTR(String),
    X_AST_NODE_TYPES
#undef X
};

typedef struct {
    char* data;
    size_t len;
} harp_str_t;

typedef union {
    harp_str_t atom;
    harp_str_t string;
    double number;
    int boolean;
} harp_value_u;

typedef struct harp_node_t {
    int type;
    harp_value_u value;
    struct harp_node_t *child, *next;
} harp_node_t;

harp_node_t* harp_get_node(harp_lexer_t* lex);

#endif//HARP_AST_H
