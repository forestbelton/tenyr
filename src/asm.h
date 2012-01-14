#ifndef ASM_H_
#define ASM_H_

enum { ASM_ASSEMBLE, ASM_DISASSEMBLE };

struct format {
    const char *name;
    int (*init)(FILE *, int flags, void *ud);
    int (*in  )(FILE *, struct instruction *, void *ud);
    int (*out )(FILE *, struct instruction *, void *ud);
    int (*fini)(FILE *, void *ud);
};

int find_format_by_name(const void *_a, const void *_b);

int print_disassembly(FILE *out, struct instruction *i);
int print_registers(FILE *out, int32_t regs[16]);

extern const struct format formats[];
extern const size_t formats_count;
#endif

