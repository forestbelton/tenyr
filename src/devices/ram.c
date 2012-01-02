#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "device.h"

struct ram_state {
    int32_t mem[1 << 24];
};

static int ram_init(struct state *s, void *cookie, ...)
{
    struct ram_state *ram = *(void**)cookie = malloc(sizeof *ram);
    memset(ram->mem, 0, sizeof ram->mem);

    return 0;
}

static int ram_fini(struct state *s, void *cookie)
{
    struct ram_state *ram = cookie;
    free(ram);

    return 0;
}

static int ram_op(struct state *s, void *cookie, int op, uint32_t addr, uint32_t *data)
{
    struct ram_state *ram = cookie;
    assert(("Address within address space", !(addr & ~PTR_MASK)));

    if (op == OP_WRITE)
        ram->mem[addr] = *data;
    else if (op == OP_READ)
        *data = ram->mem[addr];
    else
        return 1;

    return 0;
}

int ram_add_device(struct device **device)
{
    **device = (struct device){
        .bounds = { 0, (1 << 24) - 1 },
        .op = ram_op,
        .init = ram_init,
        .fini = ram_fini,
    };

    return 0;
}

