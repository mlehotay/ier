# Build artifacts live here (relative to IER-book/)
BUILD_DIR   := build

PANDOC      := pandoc
PANDOC_OPTS := -V geometry:margin=1in

# -----------------------------
# Paper (from IER-book/)
# -----------------------------
PAPER_DIR   := ../IER-paper
PAPER_SRC   := $(PAPER_DIR)/IER-paper.md
PAPER_PDF   := $(BUILD_DIR)/IER-paper.pdf

# -----------------------------
# Book (from IER-book/)
# -----------------------------
IER_DIR     := ../IER
MANIFEST    := $(IER_DIR)/IER-manifest.md

BOOKLIST    := $(BUILD_DIR)/book-input.txt
BOOK_PDF    := $(BUILD_DIR)/IER-book.pdf

.PHONY: all paper book booklist clean rebuild dirs

# Default: keep your current behavior
all: paper

dirs:
	@mkdir -p $(BUILD_DIR)

# ---- Paper targets ----
paper: $(PAPER_PDF)

$(PAPER_PDF): $(PAPER_SRC) | dirs
	$(PANDOC) $(PAPER_SRC) -o $(PAPER_PDF) $(PANDOC_OPTS)

# ---- Book targets ----
book: $(BOOK_PDF)

booklist: $(BOOKLIST)

$(BOOKLIST): $(MANIFEST) | dirs
	python3 scripts/extract_book_list.py $(MANIFEST) $(BOOKLIST)

$(BOOK_PDF): $(BOOKLIST) | dirs
	$(PANDOC) $$(cat $(BOOKLIST)) \
	  --toc --toc-depth=2 \
	  --pdf-engine=lualatex \
	  -o $(BOOK_PDF) $(PANDOC_OPTS)

# ---- Housekeeping ----
clean:
	rm -f $(PAPER_PDF) $(BOOK_PDF) $(BOOKLIST) $(BUILD_DIR)/book-input.numbered.txt

rebuild: clean all
