#ifndef HARP_LEXER_H
#define HARP_LEXER_H

#include <stdlib.h>

typedef struct {
    char* start;
    char* end;

    char* it;

    size_t len;
    
} harp_lexer_t;

harp_lexer_t harp_create_lexer(char* buff, size_t len);
void harp_lexer_set_buff(harp_lexer_t* lex, char* buff, size_t len);

#endif//HARP_lEXER_H
