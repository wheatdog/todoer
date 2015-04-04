CC = gcc
CFLAGS = -std=c99 
BUILDDIR = ../build

src = todoer.c random.c
target = $(patsubst %.c,$(BUILDDIR)/%,$(src))

all: $(target)

%: %.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/todoer: todoer.c
	$(CC) $(CFLAGS) $< -o $@

$(BUILDDIR)/random: random.c
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm ../build/*

rebuild: clean all

run:
	@$(BUILDDIR)/todoer /home/wheatdog/.todo ${ARGS}
