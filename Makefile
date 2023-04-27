CC := $(PREFIX)gcc
AR := $(CC)-ar

BUILDPATH := build
DOCPATH   := doc
INCPATH   := src
LIBPATH   := lib
SRCPATH   := src

SRCS := $(wildcard $(SRCPATH)/*.c)
INCS := $(wildcard $(SRCPATH)/*.h)
OBJS := $(patsubst $(SRCPATH)/%.c, $(BUILDPATH)/%.o, $(SRCS))
LIB  := $(LIBPATH)/libmescoder.a

CFLAGS  := -O2 -Wall -Wextra -std=gnu99
ARFLAGS := crf

all: start $(LIB)
	$(info Building complete)

start:
	$(info Start building)
	mkdir -p $(LIBPATH)
	mkdir -p $(BUILDPATH)

$(LIB): $(OBJS) $(INCS)
	$(AR) $(ARFLAGS) $(LIB) $(OBJS)

$(BUILDPATH)/%.o:$(SRCPATH)/%.c
	$(CC) $(CFLAGS) -c $< -I$(INCPATH) -o $@

clean:
	rm -rf $(BUILDPATH) $(LIBPATH) $(DOCPATH)

doc:
	mkdir -p $(DOCPATH)
	doxygen Doxyfile
	$(MAKE) -C $(DOCPATH)/latex

.PHONY: all start clean doc
