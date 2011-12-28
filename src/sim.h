#ifndef SIM_H_
#define SIM_H_

struct state;

typedef int recipe(struct state *s);

enum memory_op { OP_READ=0, OP_WRITE=1 };

struct recipe_book {
    recipe *recipe;
    struct recipe_book *next;
};

struct state {
    struct {
        int abort;
        int nowrap;
        int verbose;
        int run_defaults;   ///< whether to run default recipes
    } conf;

    size_t devices_count;   ///< how many device slots are used
    size_t devices_max;     ///< how many device slots are allocated
    struct device **devices;
    int (*dispatch_op)(struct state *s, int op, uint32_t addr, uint32_t *data);

    struct recipe_book *recipes;

    int32_t regs[16];
};

#endif

