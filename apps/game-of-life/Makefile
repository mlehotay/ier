CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -O2
LDFLAGS :=

SRC := \
	src/main.c \
	src/app.c \
	src/world.c \
	src/term.c \
	src/ui.c \
	src/stats.c \
	src/command.c \
	src/cli.c

OBJ := $(SRC:.c=.o)

gol: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) gol
