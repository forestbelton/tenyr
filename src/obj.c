// TODO make an mmap()-based version of this for systems that support it

#include "obj.h"

#include <stdlib.h>
#include <string.h>

#define MAGIC_BYTES "TOV"

#define PUTSIZED(What,Size,Where) \
    do { if (fwrite(&(What), (Size), 1, (Where)) != 1) goto bad; } while (0)
#define PUT(What,Where) \
    PUTSIZED(What, sizeof (What), Where)

#define GETSIZED(What,Size,Where) \
    do { if (fread(&(What), (Size), 1, (Where)) != 1) goto bad; } while (0)
#define GET(What,Where) \
    GETSIZED(What, sizeof (What), Where)

static int obj_v0_write(struct obj_v0 *o, FILE *out)
{
    PUT(MAGIC_BYTES, out);
    PUT(o->base.magic.parsed.version, out);
    PUT(o->length, out);
    PUT(o->flags, out);
    PUT(o->count, out);

    UWord remaining = o->count;
    struct objrec *rec = o->records;
    while (rec && remaining-- > 0) {
        PUT(rec->addr, out);
        PUT(rec->size, out);
        if (fwrite(rec->data, rec->size, 1, out) != 1)
            goto bad;

        rec = rec->next;
    }

    return 0;
bad:
    abort(); // XXX better error reporting
}

int obj_write(struct obj *o, FILE *out)
{
    switch (o->magic.parsed.version) {
        case 0: return obj_v0_write((void*)o, out);
        default:
            goto bad;
    }

bad:
    abort(); // XXX better error reporting
}

static int obj_v0_read(struct obj_v0 *o, size_t *size, FILE *in)
{
    GET(o->length, in);
    GET(o->flags, in);
    GET(o->count, in);

    UWord remaining = o->count;
    struct objrec *last = NULL,
                  *rec  = o->records = calloc(remaining, sizeof *rec);
    while (remaining-- > 0) {
        GET(rec->addr, in);
        GET(rec->size, in);
        rec->data = calloc(rec->size, 1);
        if (fread(rec->data, rec->size, 1, in) != 1)
            goto bad;

        rec->prev = last;
        if (last) last->next = rec;
        last = rec;
        rec++;
    }

    last->next = rec;

    *size = sizeof *o;

    return 0;
bad:
    abort(); // XXX better error reporting
}

int obj_read(struct obj *o, size_t *size, FILE *in)
{
    char buf[3];
    GET(buf, in);

    if (memcmp(buf, MAGIC_BYTES, sizeof buf))
        goto bad;

    GET(o->magic.parsed.version, in);

    switch (o->magic.parsed.version) {
        case 0: return obj_v0_read((void*)o, size, in);
        default:
            goto bad;
    }
bad:
    abort(); // XXX better error reporting
}

static void obj_v0_free(struct obj_v0 *o)
{
    UWord remaining = o->count;
    struct objrec *rec = o->records;
    while (rec && remaining-- > 0) {
        struct objrec *temp = rec->next;
        free(rec->data);
        free(rec);
        rec = temp;
    }

    free(o);
}

void obj_free(struct obj *o)
{
    switch (o->magic.parsed.version) {
        case 0: obj_v0_free((void*)o);
        default:
            goto bad;
    }

bad:
    abort(); // XXX better error reporting
}

