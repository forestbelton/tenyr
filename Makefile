CC       = gcc
CFLAGS  += -std=c99
CFLAGS  += -g
LDFLAGS += -g
CFLAGS  += -Wall -Wextra $(PEDANTIC)

# 32-bit compilation
#CFLAGS  += -m32
#LDFLAGS += -m32

PEDANTIC = -Werror -pedantic-errors

FLEX  = flex
BISON = bison

CFILES = $(wildcard src/*.c) $(wildcard src/devices/*.c) #parser.c lexer.c
GENDIR = src/gen

VPATH += src src/devices $(GENDIR)
INCLUDES += src $(GENDIR)

BUILD_NAME := $(shell git describe --long --always)
DEFINES += BUILD_NAME='$(BUILD_NAME)'
CPPFLAGS += $(patsubst %,-D%,$(DEFINES)) \
            $(patsubst %,-I%,$(INCLUDES))

DEVICES = ram sparseram debugwrap
DEVOBJS = $(DEVICES:%=%.o)

all: tas tsim
tas: parser.o lexer.o
tas tsim: asm.o obj.o
tsim: $(DEVOBJS) sim.o
testffi: ffi.o sim.o obj.o

.PHONY: parser.h lexer.h
lexer.o: parser.h

# don't complain about unused values that we might use in asserts
asm.o tsim.o sim.o ffi.o $(DEVOBJS): CFLAGS += -Wno-unused-value
# don't complain about unused state
ffi.o asm.o $(DEVOBJS): CFLAGS += -Wno-unused-parameter

# flex-generated code we can't control warnings of as easily
parser.o lexer.o: CFLAGS += -Wno-sign-compare -Wno-unused -Wno-unused-parameter

$(GENDIR)/lexer.h $(GENDIR)/lexer.c: lexer.l $(GENDIR)
	$(FLEX) --header-file=$(GENDIR)/lexer.h -o $(GENDIR)/lexer.c $<

$(GENDIR)/parser.h $(GENDIR)/parser.c: parser.y lexer.h $(GENDIR)
	$(BISON) --defines=$(GENDIR)/parser.h -o $(GENDIR)/parser.c $<

$(GENDIR):
	mkdir -p $@

ifeq ($(filter clean,$(MAKECMDGOALS)),)
-include $(CFILES:.c=.d)
endif

%.d: %.c
	@set -e; rm -f $@; \
	$(CC) -M $(CPPFLAGS) $< > $@.$$$$ 2> /dev/null; \
	sed 's,\($*\)\.o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

clean:
	$(RM) tas tsim *.o *.d

clobber: clean
	$(RM) $(GENDIR)/{parser,lexer}.[ch]
	$(RM) -r *.dSYM

