#ifndef SIM_H_
#define SIM_H_

#include "ops.h"
#include "machine.h"

#include <stdint.h>
#include <stddef.h>

struct sim_state;

typedef int recipe(struct sim_state *s);

enum memory_op { OP_READ=0, OP_WRITE=1 };

struct recipe_book {
    recipe *recipe;
    struct recipe_book *next;
};

struct sim_state {
    struct {
        int abort;
        int nowrap;
        int verbose;
        int run_defaults;   ///< whether to run default recipes
        int debugging;
    } conf;

    int (*dispatch_op)(struct sim_state *s, int op, uint32_t addr, uint32_t *data);

    struct recipe_book *recipes;

    struct machine_state machine;
};

int run_instruction(struct sim_state *s, struct instruction *i);

#endif

