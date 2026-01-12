# ------------------------------------------------------------
# Repo-root Makefile for IER publication artifacts
#
# Layout assumptions:
#   ./IER/   contains canonical + corpus markdown (including IER-manifest.md, IER-intro.md, etc.)
#   ./src/   contains derived / publication markdown (e.g., TLDR assembly, title pages, etc.)
#   ./scripts/ contains helpers (e.g., extract_book_list.py)
#   ./build/ is where PDFs and intermediate lists go
# ------------------------------------------------------------

# Build artifacts live here (relative to repo root)
BUILD_DIR   := build

PANDOC      := pandoc
PANDOC_OPTS := \
  -V geometry:margin=1in \
  --pdf-engine=xelatex \
  -V mainfont="DejaVu Serif" \
  -V sansfont="DejaVu Sans" \
  -V monofont="DejaVu Sans Mono" \
  -V mathfont="TeX Gyre DejaVu Math"

# Source roots
IER_DIR     := IER
SRC_DIR     := manuscripts
SCRIPTS_DIR := scripts

# -----------------------------
# Paper
# -----------------------------
PAPER_SRC   := $(SRC_DIR)/IER-paper.md
PAPER_PDF   := $(BUILD_DIR)/IER-paper.pdf

# -----------------------------
# Corpus Book (built from IER-manifest.md)
# -----------------------------
MANIFEST    := $(IER_DIR)/IER-manifest.md

BOOKLIST    := $(BUILD_DIR)/corpus-input.txt
BOOK_PDF    := $(BUILD_DIR)/IER-corpus-book.pdf

# -----------------------------
# TLDR Book
# -----------------------------
TLDR_MANIFEST := $(SRC_DIR)/IER-tldr.md

TLDR_BOOKLIST := $(BUILD_DIR)/tldr-input.txt
TLDR_PDF      := $(BUILD_DIR)/IER-tldr-book.pdf

.PHONY: all pubs paper book tldr booklist tldrlist clean rebuild dirs \
        check-chapters check-tldr

# Default target: keep conservative (paper only)
all: paper

# Build all publication PDFs
pubs: paper book tldr

dirs:
	@mkdir -p $(BUILD_DIR)

# ---- Paper targets ----
paper: $(PAPER_PDF)

$(PAPER_PDF): $(PAPER_SRC) | dirs
	$(PANDOC) $(PAPER_SRC) -o $(PAPER_PDF) $(PANDOC_OPTS)

# ---- Corpus Book targets ----
book: $(BOOK_PDF)

booklist: $(BOOKLIST)

$(BOOKLIST): $(MANIFEST) | dirs
	python3 $(SCRIPTS_DIR)/extract_book_list.py $(MANIFEST) $(BOOKLIST)

$(BOOK_PDF): $(BOOKLIST) | dirs
	$(PANDOC) $$(cat $(BOOKLIST)) \
	  --toc --toc-depth=1 \
	  -o $(BOOK_PDF) $(PANDOC_OPTS)

# ---- TLDR Book targets ----
tldr: $(TLDR_PDF)

tldrlist: $(TLDR_BOOKLIST)

$(TLDR_BOOKLIST): $(TLDR_MANIFEST) | dirs
	python3 $(SCRIPTS_DIR)/extract_book_list.py $(TLDR_MANIFEST) $(TLDR_BOOKLIST)

$(TLDR_PDF): $(TLDR_BOOKLIST) | dirs
	$(PANDOC) $$(cat $(TLDR_BOOKLIST)) \
	  --toc --toc-depth=1 \
	  -o $(TLDR_PDF) $(PANDOC_OPTS)

# ----------------------------------------
# Generic: build any single chapter PDF from IER/ or src/
# Usage:
#   make IER-intro      -> build/IER-intro.pdf (from IER/IER-intro.md)
#   make IER-tldr       -> build/IER-tldr.pdf  (from src/IER-tldr.md, if it exists)
# ----------------------------------------

# Never delete chapter PDFs as intermediates
.PRECIOUS: $(BUILD_DIR)/%.pdf

# Prefer IER/ when both exist
$(BUILD_DIR)/%.pdf: $(IER_DIR)/%.md | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

# Fall back to src/
$(BUILD_DIR)/%.pdf: $(SRC_DIR)/%.md | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

%: $(BUILD_DIR)/%.pdf
	@:

# ---- Debugging ----
check-chapters: booklist
	@echo "Checking corpus-book Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_OPTS); \
	done < $(BOOKLIST); \
	echo "All corpus-book chapters passed individual Pandoc check."

check-tldr: tldrlist
	@echo "Checking TLDR-book Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_OPTS); \
	done < $(TLDR_BOOKLIST); \
	echo "All TLDR-book chapters passed individual Pandoc check."

# ---- Housekeeping ----
clean:
	rm -f \
	  $(PAPER_PDF) \
	  $(BOOK_PDF) $(BOOKLIST) \
	  $(TLDR_PDF) $(TLDR_BOOKLIST) \
	  $(BUILD_DIR)/book-input.numbered.txt

rebuild: clean all
