#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "lexer.h"
#include "ast.h"

#define ARROW ("└──")

bool lexer_buffer_test(char* os) {
    harp_lexer_t lex = harp_create_lexer("Hello World", 11);
    return (*(lex.end - 1) == 'd') && (*(lex.it) == 'H');
}

bool lexer_skip_space_test(char* os) {
    char buff[] = "    \n\t X";
    harp_lexer_t lex = harp_create_lexer(buff, strlen(buff));

    harp_skip_space(&lex);

    char c = (*(lex.it));
    if (c != 'X') {
        sprintf(os+strlen(os), "%s It landed on [%c] - %d\n", ARROW, c, (int)c);
        return false;
    }

    return true;
}

bool lexer_number_test(char* os) {
    char buff[] = "13 3.14 -3 .32 -.32";

    harp_lexer_t lex = harp_create_lexer(buff, strlen(buff));

    double nums[] = {13, 3.14, -3, .32, -.32};

    int n = 0;
    while(1) {
        harp_tok_t tok = harp_get_tok(&lex);
        if (tok.type == TT_EOF) {
            if (n != (int)(sizeof(nums)/sizeof(double))) {
                sprintf(os+strlen(os), "%s Lexer didn't make it through all the numbers\n", ARROW);
                return false;
            } else return true;
        }

        char b[100] = {0};
        sprintf(b, "%.*s", (int)(tok.end-tok.start), tok.start);
        sprintf(os+strlen(os), "%s got lexeme: [%s]\n", ARROW, b);

        if (atof(b) != nums[n++]) return false;
    }

    return true;
}

bool lexer_paren_test(char* os) {
    char buff[] = "(()[)]";
    harp_lexer_t lex = harp_create_lexer(buff, strlen(buff));

    int expected[] = {
        TT_OPEN_PAREN, TT_OPEN_PAREN,
        TT_CLOSE_PAREN, TT_OPEN_BRACKET,
        TT_CLOSE_PAREN, TT_CLOSE_BRACKET
    };

    int n = 0;
    while (1) {
        harp_tok_t tok = harp_get_tok(&lex);
        if (tok.type == TT_EOF) {
            if (n != (int)(sizeof(expected)/sizeof(int))){
                sprintf(os+strlen(os), "%s Lexer didn't make it through all the parens and brackets\n", ARROW);
                return false;
            } else return true;
        }

        if (tok.type != expected[n++]) return false;
        if (tok.type == TT_OPEN_PAREN && (*tok.start) != '(') return false;
        if (tok.type == TT_CLOSE_PAREN && (*tok.start) != ')') return false;
        if (tok.type == TT_OPEN_BRACKET && (*tok.start) != '[') return false;
        if (tok.type == TT_CLOSE_BRACKET && (*tok.start) != ']') return false;
    }
    return true;
}

bool lexer_atom_test(char* os){
    char buff[] = "32a aa aaa Hello32World_ _what_ *ba3ana*";

    harp_lexer_t lex = harp_create_lexer(buff, strlen(buff));
    harp_tok_t n = harp_get_tok(&lex);
    if (n.type != TT_NUMBER || !harp_lexeme_equals_num(&n, 32)) {
        sprintf(os+sizeof(os), "%s Lexer failed to lex the first number", ARROW);
        return false;
    }

#define N (6) 
    const char* expected[N] = {
        "a",
        "aa",
        "aaa",
        "Hello32World_",
        "_what_",
        "*ba3ana*",
    };

    for (int i = 0; i < N; i++) {
        harp_tok_t tok = harp_get_tok(&lex);
        if (tok.type == TT_EOF) {
            sprintf(os+strlen(os), "%s Lexer didn't make it through all of the atoms\n", ARROW);
            return false;
        }
        if (tok.type != TT_ATOM) {
            sprintf(os+strlen(os), "%s Token is not of type TT_ATOM, but %s\n", ARROW, harp_token_type_names[tok.type]);
            return false;
        }

        char buff[128];
        sprintf(buff, "%.*s", (int)(tok.end-tok.start), tok.start);
        if (strcmp(buff, expected[i]) != 0) {
            sprintf(os+strlen(os), "%s Lexeme expected [%s] but got [%s]\n", ARROW, expected[i], buff);
            return false;
        } else {
            sprintf(os+strlen(os), "%s Got lexeme [%s]\n", ARROW, buff);
        }
    }
 
    harp_tok_t tok = harp_get_tok(&lex);
    if (tok.type != TT_EOF) {
        sprintf(os+strlen(os), "%s Lexer didn't move through all of the text", ARROW);
    }

#undef N

    return true;
}

bool ast_basic_value_test(char* os){
    char buff[] = "1 2 3";
    harp_lexer_t lex = harp_create_lexer(buff, strlen(buff));
    harp_node_t* ast = harp_get_node(&lex);

    if (ast == NULL) return false;
    if (ast->child == NULL) return false;
    if (ast->child->next == NULL) return false;
    if (ast->child->next->next == NULL) return false;

    return true;
}

bool ast_basic_s_expr_test(char* os){
    char buff[] = "(1 2 (3 (32 2) 4) 5)";
    harp_lexer_t lex = harp_create_lexer(buff, strlen(buff));
    harp_node_t* ast = harp_get_node(&lex);

    return false;
}

typedef bool (*harp_test_fn)(char* buff);

#ifndef HARP_MACROSTR
#define HARP_MACROSTR(k) #k
#endif//HARP_MACROSTR

void harp_run_all_tests() {
    #define DEF_TEST(test) { .name = HARP_MACROSTR(test), .fn = test }
    struct { const char* name; harp_test_fn fn; } tests[] = {
        DEF_TEST(lexer_buffer_test),
        DEF_TEST(lexer_skip_space_test),
        DEF_TEST(lexer_number_test),
        DEF_TEST(lexer_paren_test),
        DEF_TEST(lexer_atom_test),
        DEF_TEST(ast_basic_value_test),
        DEF_TEST(ast_basic_s_expr_test),
    };
    #undef DEF_TEST

    size_t n_tests = (sizeof(tests) / sizeof(tests[0]));
    size_t succ = 0;

    for (size_t i = 0; i < n_tests; i++) {
        char buffer[512] = { [0] = '\0' };
        bool result = tests[i].fn(buffer);

        const char* status = result ? "Passed" : "Failed";
        if (result) succ++;

        char* color = result
            ? "\033[32;1m"
            : "\033[31;1m";

        printf("[%d] <%s> test: %s[%s]\033[m\n",
               (int)i + 1,
               tests[i].name,
               color,
               status);
        printf("\033[36m%s\033[m", buffer);
    }

    bool passed = (n_tests == succ);

    printf("Harp tests [%s] (%d/%d) (%d%%)\n",
           (passed?"Passed":"Failed"),
           (int)succ,
           (int)n_tests,
           (int)((((float)succ)/(float)n_tests) * 100));
}
