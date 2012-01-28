#ifndef PARSER_GLOBAL_H_
#define PARSER_GLOBAL_H_

#include "ops.h"

#define SMALL_IMMEDIATE_BITWIDTH    12
#define LARGE_IMMEDIATE_BITWIDTH    24
#define WORD_BITWIDTH               32

struct parse_data {
    void *scanner;
    struct {
        unsigned savecol;
        char saveline[LINE_LEN];
    } lexstate;
    struct instruction_list *top;
    struct relocation_list {
        struct const_expr *ce;
        uint32_t *dest;     ///< destination word to be updated
        int width;          ///< width in bits of the right-justified immediate
        int mult;           ///< multiplier (1 or -1, according to sign)
        struct relocation_list *next;
    } *relocs;
    struct label_list {
        struct label *label;
        struct label_list *next;
    } *labels;
};

int tenyr_parse(struct parse_data *);

struct const_expr {
    enum { OP2, LAB, IMM, ICI } type; // TODO namespace
    int32_t i;
    char labelname[LABEL_LEN];
    int op;
    struct instruction *insn; // for '.'-resolving
    struct const_expr *left, *right;
};

struct const_expr_list {
    struct const_expr *ce;
    struct const_expr_list *right;
};

struct expr {
    int deref;
    int x;
    int op;
    int y;
    int32_t i;
    int width;  ///< width of relocation XXX cleanup
    int mult;   ///< multiplier from addsub
    struct const_expr *ce;
};

struct cstr {
    int len;
    char *str;
    struct cstr *right;
};

struct directive {
    enum directive_type { D_GLOBAL } type;
    void *data;
};

#endif

