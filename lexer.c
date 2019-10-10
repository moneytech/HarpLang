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
