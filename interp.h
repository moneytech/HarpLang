#ifndef HARP_INTERP_H
#define HARP_INTERP_H

#include "lexer.h"
#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

harp_node_t* harp_eval_expr(harp_node_t* node);
harp_node_t* harp_car(harp_node_t* list);
harp_node_t* harp_cdr(harp_node_t* list);
harp_node_t* harp_eval_string(char* buff, size_t len);

#endif//HARP_INTERP_H
