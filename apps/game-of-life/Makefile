CC := cc
CFLAGS := -std=c11 -Wall -Wextra -Wpedantic -O2
LDFLAGS :=
.DEFAULT_GOAL := gol

SRC := \
	src/main.c \
	src/app.c \
	src/world.c \
	src/term.c \
	src/ui.c \
	src/stats.c \
	src/command.c \
	src/cli.c \
	src/cells.c \
	src/pattern.c \
	src/rle.c

OBJ := $(SRC:.c=.o)
$(OBJ): $(wildcard src/*.h)
TEST_BIN := build/tests/world-tests
UI_TEST_BIN := build/tests/ui-tests
RESIZE_TEST_BIN := build/tests/resize-tests
CELLS_TEST_BIN := build/tests/cells-tests
PATTERN_TEST_BIN := build/tests/pattern-tests
CLI_TEST_BIN := build/tests/cli-tests
COMMAND_TEST_BIN := build/tests/command-tests
RLE_TEST_BIN := build/tests/rle-tests
TEST_CASE ?= all

gol: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LDFLAGS)

clean:
	rm -f $(OBJ) gol $(TEST_BIN) $(UI_TEST_BIN) $(RESIZE_TEST_BIN) \
		$(CELLS_TEST_BIN) $(PATTERN_TEST_BIN) $(CLI_TEST_BIN) $(COMMAND_TEST_BIN) $(RLE_TEST_BIN) build/tests/resize-world.o build/tests/cells-parser.o build/tests/rle-parser.o

.PHONY: test test-unit test-ui test-integration test-cells test-patterns test-cli test-commands test-rle test-startup

test: test-unit
	$(MAKE) test-ui TEST_CASE=all
	$(MAKE) test-integration TEST_CASE=all
	$(MAKE) test-cells TEST_CASE=all
	$(MAKE) test-patterns TEST_CASE=all
	$(MAKE) test-cli TEST_CASE=all
	$(MAKE) test-commands TEST_CASE=all
	$(MAKE) test-rle TEST_CASE=all
	$(MAKE) test-startup TEST_CASE=all

test-unit: $(TEST_BIN)
	./$(TEST_BIN) "$(TEST_CASE)"

$(TEST_BIN): tests/world_tests.c src/world.c src/world.h src/pattern.c src/pattern.h Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/world_tests.c src/world.c src/pattern.c $(LDFLAGS) $(LDLIBS)

test-ui: $(UI_TEST_BIN)
	./$(UI_TEST_BIN) "$(TEST_CASE)"

$(UI_TEST_BIN): tests/ui_tests.c src/ui.c src/stats.c src/world.c $(wildcard src/*.h) Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/ui_tests.c src/ui.c src/stats.c src/world.c $(LDFLAGS) $(LDLIBS)

test-integration: $(RESIZE_TEST_BIN)
	./$(RESIZE_TEST_BIN) "$(TEST_CASE)"

# Allocation failures are injected only into this test build of the world.
build/tests/resize-world.o: src/world.c src/world.h Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Dcalloc=resize_test_calloc -Isrc -c -o $@ src/world.c

$(RESIZE_TEST_BIN): tests/resize_tests.c src/app.c src/cli.c src/command.c src/ui.c src/stats.c src/pattern.c src/cells.c build/tests/resize-world.o $(wildcard src/*.h) Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/resize_tests.c src/app.c src/cli.c src/command.c src/ui.c src/stats.c src/pattern.c src/cells.c build/tests/resize-world.o $(LDFLAGS) $(LDLIBS)

test-cells: $(CELLS_TEST_BIN)
	./$(CELLS_TEST_BIN) "$(TEST_CASE)"

build/tests/cells-parser.o: src/cells.c src/cells.h src/pattern.h Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Drealloc=cells_test_realloc -Dfclose=cells_test_fclose -Isrc -c -o $@ src/cells.c

$(CELLS_TEST_BIN): tests/cells_tests.c src/cells.h src/pattern.h build/tests/cells-parser.o Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/cells_tests.c build/tests/cells-parser.o $(LDFLAGS) $(LDLIBS)

test-patterns: $(PATTERN_TEST_BIN)
	./$(PATTERN_TEST_BIN) "$(TEST_CASE)"

$(PATTERN_TEST_BIN): tests/pattern_tests.c src/pattern.c src/pattern.h src/cells.c src/cells.h src/world.c src/world.h Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/pattern_tests.c src/pattern.c src/cells.c src/world.c $(LDFLAGS) $(LDLIBS)

test-cli: $(CLI_TEST_BIN)
	./$(CLI_TEST_BIN) "$(TEST_CASE)"

$(CLI_TEST_BIN): tests/cli_tests.c src/cli.c src/pattern.c src/world.c $(wildcard src/*.h) Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/cli_tests.c src/cli.c src/pattern.c src/world.c $(LDFLAGS) $(LDLIBS)

test-commands: $(COMMAND_TEST_BIN)
	./$(COMMAND_TEST_BIN) "$(TEST_CASE)"

$(COMMAND_TEST_BIN): tests/command_tests.c src/command.c src/pattern.c src/cells.c src/world.c $(wildcard src/*.h) Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/command_tests.c src/command.c src/pattern.c src/cells.c src/world.c $(LDFLAGS) $(LDLIBS)

test-rle: $(RLE_TEST_BIN)
	./$(RLE_TEST_BIN) "$(TEST_CASE)"

build/tests/rle-parser.o: src/rle.c $(wildcard src/*.h) Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Drealloc=rle_test_realloc -Dfclose=rle_test_fclose -Dfgetc=rle_test_fgetc -Dferror=rle_test_ferror -Isrc -c -o $@ src/rle.c

$(RLE_TEST_BIN): tests/rle_tests.c build/tests/rle-parser.o src/cells.c src/pattern.c src/world.c $(wildcard src/*.h) Makefile
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CFLAGS) -Isrc -o $@ tests/rle_tests.c build/tests/rle-parser.o src/cells.c src/pattern.c src/world.c $(LDFLAGS) $(LDLIBS)

.PHONY: check-work

test-startup: gol
	python3 tests/startup_errors.py ./gol "$(TEST_CASE)"

check-work:
	python3 _work/check.py
