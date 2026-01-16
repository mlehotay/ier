# ------------------------------------------------------------
# Repo-root Makefile for IER publication artifacts
# ------------------------------------------------------------

BUILD_DIR   := build

PANDOC      := pandoc

IER_DIR     := IER
SRC_DIR     := pub
SCRIPTS_DIR := scripts

# TeX includes (publication-layer)
TEX_DIR        := $(SRC_DIR)/tex
IER_BOOK_TEX   := $(TEX_DIR)/ier-book.tex
IER_PAPER_TEX  := $(TEX_DIR)/ier-paper.tex

# -----------------------------
# Shared Pandoc/TeX baselines
# -----------------------------

# Shared font stack (used by both book and article)
PANDOC_FONTS := \
  --pdf-engine=xelatex \
  -V fontsize=11pt \
  -V mainfont="TeX Gyre Termes" \
  -V sansfont="TeX Gyre Heros" \
  -V monofont="Inconsolata" \
  -V mathfont="TeX Gyre Termes Math"

# Book baseline (corpus + TLDR)
PANDOC_BOOK_COMMON := \
  $(PANDOC_FONTS) \
  -V documentclass=book \
  --top-level-division=chapter \
  -H $(IER_BOOK_TEX)

# Paper baseline (article/report-like)
PANDOC_ARTICLE_COMMON := \
  $(PANDOC_FONTS) \
  -V documentclass=article \
  --top-level-division=section \
  -H $(IER_PAPER_TEX)

# -----------------------------
# Artifact-specific Pandoc opts
# -----------------------------

# Paper: standalone authored composition (report/article geometry)
PANDOC_PAPER_OPTS := \
  $(PANDOC_ARTICLE_COMMON) \
  -V geometry:paperwidth=8.5in \
  -V geometry:paperheight=11in \
  -V geometry:margin=1in \
  -V linestretch=1.15

# Corpus book: anchor reference artifact (default 7x10)
PANDOC_CORPUS_OPTS := \
  $(PANDOC_BOOK_COMMON) \
  -V classoption=openright \
  -V geometry:paperwidth=7in \
  -V geometry:paperheight=10in \
  -V geometry:inner=1.0in \
  -V geometry:outer=0.9in \
  -V geometry:top=0.95in \
  -V geometry:bottom=1.0in \
  -V linestretch=1.10

# If you want US-letter reference geometry for the corpus book,
# swap the two lines below into PANDOC_CORPUS_OPTS (and remove 7x10):
#   -V geometry:paperwidth=8.5in \
#   -V geometry:paperheight=11in \

# TLDR book: gateway / reading artifact (7x9)
PANDOC_TLDR_OPTS := \
  $(PANDOC_BOOK_COMMON) \
  -V classoption=openright \
  -V geometry:paperwidth=7in \
  -V geometry:paperheight=9in \
  -V geometry:inner=0.95in \
  -V geometry:outer=0.85in \
  -V geometry:top=0.9in \
  -V geometry:bottom=0.95in \
  -V linestretch=1.20

# Default opts for generic single-chapter builds
PANDOC_OPTS := $(PANDOC_PAPER_OPTS)

.PHONY: all pubs dirs \
        paper book booklist verify verify-structure verify-authoring check-corpus \
        tldr tldrlist verify-tldr verify-tldr-structure verify-tldr-authoring check-tldr \
        clean spotless rebuild

# Default target
all: paper

pubs: paper book tldr

dirs:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(TEX_DIR)

# -----------------------------
# Paper (standalone authored composition)
# -----------------------------
PAPER_SRC := $(SRC_DIR)/IER-paper.md
PAPER_PDF := $(BUILD_DIR)/IER-paper.pdf

paper: $(PAPER_PDF)

$(PAPER_PDF): $(PAPER_SRC) $(IER_PAPER_TEX) | dirs
	$(PANDOC) $< -o $@ $(PANDOC_PAPER_OPTS)

# -----------------------------
# Corpus Book
#   selection: pub/IER-corpus-selection.md
#   scaffold:  pub/corpus-book/*.md (sorted by filename)
#   output:    build/corpus-input.txt
# -----------------------------
CORPUS_SELECTION        := $(SRC_DIR)/IER-corpus-selection.md
CORPUS_SCAFFOLD_DIR     := $(SRC_DIR)/corpus-book
CORPUS_SCAFFOLD_FILES   := $(wildcard $(CORPUS_SCAFFOLD_DIR)/*.md)

CORPUS_BOOKLIST         := $(BUILD_DIR)/corpus-input.txt
CORPUS_PDF              := $(BUILD_DIR)/IER-corpus-book.pdf

book: $(CORPUS_PDF)
booklist: $(CORPUS_BOOKLIST)

$(CORPUS_BOOKLIST): $(CORPUS_SELECTION) $(CORPUS_SCAFFOLD_FILES) | dirs
	@python3 $(SCRIPTS_DIR)/extract_book_list.py \
	  "$(CORPUS_SELECTION)" \
	  "$(CORPUS_SCAFFOLD_DIR)" \
	  "$@"
	@echo "Wrote booklist: $@"

# -----------------------------
# Verification (corpus)
#   verify_book.py interface:
#     python3 scripts/verify_book.py <selection.md> <booklist.txt> [flags]
# -----------------------------
verify: $(CORPUS_BOOKLIST)
	@echo "Verifying corpus book list and chapter content..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(CORPUS_SELECTION)" \
	  "$(CORPUS_BOOKLIST)" \
	  --scaffold-dir "$(CORPUS_SCAFFOLD_DIR)"

verify-structure: $(CORPUS_BOOKLIST)
	@echo "Verifying corpus book structure (glyph checks skipped)..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(CORPUS_SELECTION)" \
	  "$(CORPUS_BOOKLIST)" \
	  --scaffold-dir "$(CORPUS_SCAFFOLD_DIR)" \
	  --skip-glyphs

verify-authoring: $(CORPUS_BOOKLIST)
	@echo "Verifying corpus authoring rules only (skip structure)..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(CORPUS_SELECTION)" \
	  "$(CORPUS_BOOKLIST)" \
	  --scaffold-dir "$(CORPUS_SCAFFOLD_DIR)" \
	  --skip-structure

# NOTE: Re-enable `verify` before any public or tagged release.
# $(CORPUS_PDF): verify | dirs
# $(CORPUS_PDF): verify-structure | dirs
$(CORPUS_PDF): $(CORPUS_BOOKLIST) $(IER_BOOK_TEX) | dirs
	@# Guard: do NOT let pandoc block by reading stdin
	@test -s "$(CORPUS_BOOKLIST)" || (echo "ERROR: empty book list: $(CORPUS_BOOKLIST)" >&2; exit 1)
	$(PANDOC) $$(cat "$(CORPUS_BOOKLIST)") \
	  --toc --toc-depth=1 \
	  -o "$@" $(PANDOC_CORPUS_OPTS)

check-corpus: $(CORPUS_BOOKLIST) $(IER_BOOK_TEX)
	@echo "Checking corpus-book Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_CORPUS_OPTS); \
	done < "$(CORPUS_BOOKLIST)"; \
	echo "All corpus-book chapters passed individual Pandoc check."

# -----------------------------
# TLDR Book
#   selection: pub/IER-tldr-selection.md
#   scaffold:  pub/tldr-book/*.md (sorted by filename)
#   output:    build/tldr-input.txt
# -----------------------------
TLDR_SELECTION          := $(SRC_DIR)/IER-tldr-selection.md
TLDR_SCAFFOLD_DIR       := $(SRC_DIR)/tldr-book
TLDR_SCAFFOLD_FILES     := $(wildcard $(TLDR_SCAFFOLD_DIR)/*.md)

TLDR_BOOKLIST           := $(BUILD_DIR)/tldr-input.txt
TLDR_PDF                := $(BUILD_DIR)/IER-tldr-book.pdf

tldr: $(TLDR_PDF)
tldrlist: $(TLDR_BOOKLIST)

$(TLDR_BOOKLIST): $(TLDR_SELECTION) $(TLDR_SCAFFOLD_FILES) | dirs
	@python3 $(SCRIPTS_DIR)/extract_book_list.py \
	  "$(TLDR_SELECTION)" \
	  "$(TLDR_SCAFFOLD_DIR)" \
	  "$@"
	@echo "Wrote tldr booklist: $@"

# -----------------------------
# Verification (tldr)
# -----------------------------
verify-tldr: $(TLDR_BOOKLIST)
	@echo "Verifying TLDR book list and chapter content..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(TLDR_SELECTION)" \
	  "$(TLDR_BOOKLIST)" \
	  --scaffold-dir "$(TLDR_SCAFFOLD_DIR)"

verify-tldr-structure: $(TLDR_BOOKLIST)
	@echo "Verifying TLDR book structure (glyph checks skipped)..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(TLDR_SELECTION)" \
	  "$(TLDR_BOOKLIST)" \
	  --scaffold-dir "$(TLDR_SCAFFOLD_DIR)" \
	  --skip-glyphs

verify-tldr-authoring: $(TLDR_BOOKLIST)
	@echo "Verifying TLDR authoring rules only (skip structure)..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(TLDR_SELECTION)" \
	  "$(TLDR_BOOKLIST)" \
	  --scaffold-dir "$(TLDR_SCAFFOLD_DIR)" \
	  --skip-structure

$(TLDR_PDF): $(TLDR_BOOKLIST) $(IER_BOOK_TEX) | dirs
	@test -s "$(TLDR_BOOKLIST)" || (echo "ERROR: empty book list: $(TLDR_BOOKLIST)" >&2; exit 1)
	$(PANDOC) $$(cat "$(TLDR_BOOKLIST)") \
	  --toc --toc-depth=1 \
	  -o "$@" $(PANDOC_TLDR_OPTS)

check-tldr: $(TLDR_BOOKLIST) $(IER_BOOK_TEX)
	@echo "Checking tldr-book Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_TLDR_OPTS); \
	done < "$(TLDR_BOOKLIST)"; \
	echo "All tldr-book chapters passed individual Pandoc check."

# -----------------------------
# Generic: build any single chapter PDF from IER/ or pub/
#   Defaults to paper/article rendering discipline (no running heads).
# -----------------------------
.PRECIOUS: $(BUILD_DIR)/%.pdf

# Prefer IER/ when both exist
$(BUILD_DIR)/%.pdf: $(IER_DIR)/%.md $(IER_PAPER_TEX) | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

# Fall back to pub/ (top-level)
$(BUILD_DIR)/%.pdf: $(SRC_DIR)/%.md $(IER_PAPER_TEX) | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

# Fall back to pub/*/ (one level deep: corpus-book/, tldr-book/, etc.)
$(BUILD_DIR)/%.pdf: $(SRC_DIR)/*/%.md $(IER_PAPER_TEX) | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

%: $(BUILD_DIR)/%.pdf
	@:

# -----------------------------
# Housekeeping
# -----------------------------
clean:
	rm -f \
	  $(PAPER_PDF) \
	  $(CORPUS_PDF) $(CORPUS_BOOKLIST) $(BUILD_DIR)/corpus-input.numbered.txt \
	  $(TLDR_PDF)   $(TLDR_BOOKLIST)   $(BUILD_DIR)/tldr-input.numbered.txt \
	  $(BUILD_DIR)/*.numbered.txt \
	  $(BUILD_DIR)/*-verify.expected.numbered.txt \
	  $(BUILD_DIR)/*-verify.actual.numbered.txt

# Remove the entire build directory (fresh checkout cleanliness)
spotless: clean
	rm -rf $(BUILD_DIR)

rebuild: clean all
