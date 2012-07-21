// Simulated implementation of http://opencores.org/project,spi
//  which belongs to Simon Srot <simons@opencores.org>
// Connects tenyr wishbone (plain local bus now, since wishbone is not
// simulated in any special way) to a seekable FILE*. If param "spi.filename"
// is set, that file is opened with mode "r+b" and that FILE* is used as the
// target. Otherwise, spi_set_store() can be used to set the backing store.
#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "device.h"

#define SPI_BASE    0x200
#define SPI_LEN     (0x7 * 4) /* seven registers at four addresses each) */
#define SPI_END     (SPI_BASE + SPI_LEN - 1)

struct spi_state {
    FILE *store;

    union {
        uint32_t raw[7];
        struct {
            union {
                struct {
                    uint32_t Rx0, Rx1, Rx2, Rx3;
                } Rx;
                struct {
                    uint32_t Tx0, Tx1, Tx2, Tx3;
                } Tx;
            } data;
            struct {
                union {
                    unsigned CTRL:13;
                    // Technically bit ordering isn't defined by C99 here
                    struct {
                        unsigned CHAR_LEN   : 7;    // bits [ 6:0]
                        unsigned /* rsvd */ : 1;    // bit  [ 7]
                        unsigned GO_BSY     : 1;    // bit  [ 8]
                        unsigned Rx_NEG     : 1;    // bit  [ 9]
                        unsigned Tx_NEG     : 1;    // bit  [10]
                        unsigned LSB        : 1;    // bit  [11]
                        unsigned IE         : 1;    // bit  [12]
                        unsigned ASS        : 1;    // bit  [13]
                        unsigned /* rsvd */ :18;    // bits [31:14]
                    } bits;
                } u;
            } ctrl;
            unsigned DIVIDER    :16;
            unsigned            :16;
            unsigned SS         : 8;
            unsigned            :24;
        } fmt;
    } regs;
};

// TODO expose this interface
int spi_set_store(void *cookie, FILE *store)
{
    struct spi_state *spi = cookie;

    if (spi->store)
        fclose(spi->store);
    spi->store = store;

    return 0;
}

static void spi_reset_defaults(struct spi_state *spi)
{
    spi->regs.fmt.data.Rx.Rx0 = 0x00000000;
    spi->regs.fmt.data.Rx.Rx1 = 0x00000000;
    spi->regs.fmt.data.Rx.Rx2 = 0x00000000;
    spi->regs.fmt.data.Rx.Rx3 = 0x00000000;

    spi->regs.fmt.data.Tx.Tx0 = 0x00000000;
    spi->regs.fmt.data.Tx.Tx1 = 0x00000000;
    spi->regs.fmt.data.Tx.Tx2 = 0x00000000;
    spi->regs.fmt.data.Tx.Tx3 = 0x00000000;

    spi->regs.fmt.ctrl.u.CTRL = 0x00000000;

    spi->regs.fmt.DIVIDER     = 0x0000ffff;

    spi->regs.fmt.SS          = 0x00000000;
}

static int spi_init(struct sim_state *s, void *cookie, ...)
{
    struct spi_state *spi = *(void**)cookie = malloc(sizeof *spi);

    spi->store = NULL;
    spi_reset_defaults(spi);

    const char *filename = NULL;
    if (param_get(s, "spi.filename", &filename)) {
        FILE *store = fopen(filename, "r+b");
        if (!store)
            fatal(PRINT_ERRNO, "Failed to open file '%s'", filename);
        spi_set_store(spi, store);
    }

    return 0;
}

static int spi_fini(struct sim_state *s, void *cookie)
{
    struct spi_state *spi = cookie;

    if (spi->store)
        fclose(spi->store);
    free(spi);

    return 0;
}

static int spi_op(struct sim_state *s, void *cookie, int op, uint32_t addr,
        uint32_t *data)
{
    struct spi_state *spi = cookie;
    uint32_t offset = addr - SPI_BASE;

    assert(("Address within address space", !(addr & ~PTR_MASK)));
    assert(("Lower bits of offset are cleared", !(offset & 0x7)));

    spi->regs.raw[offset >> 3] = *data;

    return 0;
}

static int spi_cycle(struct sim_state *s, void *cookie)
{
    // TODO implement SPI emulation state machine here
    return 0;
}

int spi_add_device(struct device **device)
{
    **device = (struct device){
        .bounds = { SPI_BASE, SPI_END },
        .op = spi_op,
        .init = spi_init,
        .fini = spi_fini,
        .cycle = spi_cycle,
    };

    return 0;
}

