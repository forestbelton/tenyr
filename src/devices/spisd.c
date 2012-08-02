#include "spi.h"
#include "plugin.h"

#include <stdlib.h>
#include <stdint.h>

enum spisd_command_type {
    GO_IDLE_STATE           =  0,
    SEND_OP_COND            =  1,

    SEND_CSD                =  9,
    SEND_CID                = 10,

    STOP_TRANSMISSION       = 12,
    SEND_STATUS             = 13,

    SET_BLOCKLEN            = 16,
    READ_SINGLE_BLOCK       = 17,
    READ_MULTIPLE_BLOCK     = 18,


    WRITE_BLOCK             = 24,
    WRITE_MULTIPLE_BLOCK    = 25,

    PROGRAM_CSD             = 27,
  //SET_WRITE_PROT          = 28,
  //CLR_WRITE_PROT          = 29,
  //SEND_WRITE_PROT         = 30,

    ERASE_WR_BLK_START_ADDR = 32,
    ERASE_WR_BLK_END_ADDR   = 33,

    ERASE                   = 38,

    APP_CMD                 = 55,
    GEN_CMD                 = 56,

    READ_OCR                = 58,
    CRC_ON_OFF              = 59,
};

#define spisd_rsp_R1_minbytes 1
#define spisd_rsp_R1_maxbytes 1
struct spisd_rsp_R1 {
    unsigned idle           :1;
    unsigned erase_reset    :1;
    unsigned illegal        :1;
    unsigned crc_error      :1;
    unsigned erase_seq_error:1;
    unsigned address_error  :1;
    unsigned parameter_error:1;

    unsigned must_be_zero   :1;
};

#define spisd_rsp_R1b_minbytes 1
#define spisd_rsp_R1b_maxbytes -1
struct spisd_rsp_R1b {
    // this is an embedded R1
    // TODO replace with an actual struct spisd_rsp_R1 ?
    unsigned idle           :1;
    unsigned erase_reset    :1;
    unsigned illegal        :1;
    unsigned crc_error      :1;
    unsigned erase_seq_error:1;
    unsigned address_error  :1;
    unsigned parameter_error:1;

    unsigned must_be_zero   :1;

    uint8_t busy[];
};

#define spisd_rsp_R2_minbytes 2
#define spisd_rsp_R2_maxbytes 2
struct spisd_rsp_R2 {
    unsigned locked         :1;
    unsigned wp_erase_skip  :1;
    unsigned error          :1;
    unsigned cc_error       :1;
    unsigned card_ecc_fail  :1;
    unsigned wp_violation   :1;
    unsigned erase_param    :1;
    unsigned out_of_range   :1;

    // this is an embedded R1
    // TODO replace with an actual struct spisd_rsp_R1 ?
    unsigned idle           :1;
    unsigned erase_reset    :1;
    unsigned illegal        :1;
    unsigned crc_error      :1;
    unsigned erase_seq_error:1;
    unsigned address_error  :1;
    unsigned parameter_error:1;

    unsigned must_be_zero   :1;
};

#define SPISD_COMMANDS(_) \
    _(GO_IDLE_STATE, R1 )
    //

#define SPISD_ARRAY_ENTRY(Type,Resp) \
    [Type] = { Type, spisd_rsp_##Resp##_minbytes, spisd_rsp_##Resp##_maxbytes },

static const struct spisd_command {
    enum spisd_command_type type;
    int rsp_minbytes, rsp_maxbytes;
} spisd_commands[] = {
    SPISD_COMMANDS(SPISD_ARRAY_ENTRY)
};

struct spisd_state {
    int dummy;
};

int EXPORT spisd_spi_init(void *pcookie)
{
    struct spisd_state *s = malloc(sizeof *s);
    *(void**)pcookie = s;
    return 0;
}

int EXPORT spisd_spi_select(void *cookie, int _ss)
{
    return 0;
}

int EXPORT spisd_spi_clock(void *cookie, int _ss, int in, int *out)
{
    *out = 0; // say nobody is home
    return 0;
}

int EXPORT spisd_spi_fini(void *cookie)
{
    struct spisd_state *s = *(void**)cookie;
    free(s);
    *(void**)cookie = NULL;

    return 0;
}

