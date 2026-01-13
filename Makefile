# ------------------------------------------------------------
# Repo-root Makefile for IER publication artifacts
# ------------------------------------------------------------

BUILD_DIR   := build

PANDOC      := pandoc
PANDOC_OPTS := \
  -V geometry:margin=1in \
  --pdf-engine=xelatex \
  -V mainfont="DejaVu Serif" \
  -V sansfont="DejaVu Sans" \
  -V monofont="DejaVu Sans Mono" \
  -V mathfont="TeX Gyre DejaVu Math"

IER_DIR     := IER
SRC_DIR     := pub
SCRIPTS_DIR := scripts

.PHONY: all pubs dirs paper book booklist verify tldr tldrlist \
        check-corpus check-tldr clean rebuild

# Default target
all: paper

pubs: paper book tldr

dirs:
	@mkdir -p $(BUILD_DIR)

# -----------------------------
# Paper
# -----------------------------
PAPER_SRC := $(SRC_DIR)/IER-paper.md
PAPER_PDF := $(BUILD_DIR)/IER-paper.pdf

paper: $(PAPER_PDF)

$(PAPER_PDF): $(PAPER_SRC) | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

# -----------------------------
# Corpus Book
#  - frontmatter/scaffold: pub/corpus-book/*.md (sorted)
#  - chapters: extracted from IER/IER-manifest.md
# -----------------------------
MANIFEST_CORPUS   := $(IER_DIR)/IER-manifest.md
FRONT_CORPUS_DIR  := $(SRC_DIR)/corpus-book

CORPUS_BOOKLIST   := $(BUILD_DIR)/corpus-input.txt
CORPUS_PDF        := $(BUILD_DIR)/IER-corpus-book.pdf

book: $(CORPUS_PDF)
booklist: $(CORPUS_BOOKLIST)

# Rebuild the list if manifest changes or any frontmatter file changes
CORPUS_FRONT_FILES := $(wildcard $(FRONT_CORPUS_DIR)/*.md)

$(CORPUS_BOOKLIST): $(MANIFEST_CORPUS) $(CORPUS_FRONT_FILES) | dirs
	@rm -f "$@.tmp"
	@# 1) prepend frontmatter/scaffold (optional)
	@find "$(FRONT_CORPUS_DIR)" -maxdepth 1 -type f -name '*.md' 2>/dev/null | sort > "$@.tmp" || true
	@# 2) append extracted canonical chapters from manifest-like source
	@python3 $(SCRIPTS_DIR)/extract_book_list.py --mode corpus "$(MANIFEST_CORPUS)" "$@.chapters.tmp"
	@cat "$@.chapters.tmp" >> "$@.tmp"
	@rm -f "$@.chapters.tmp"
	@mv "$@.tmp" "$@"
	@echo "Wrote booklist: $@"

# -----------------------------
# Verification (manifest/order/sentinels + glyph discipline)
# -----------------------------
verify: $(CORPUS_BOOKLIST)
	@echo "Verifying corpus book list and chapter content..."
	@python3 $(SCRIPTS_DIR)/verify_book.py "$(MANIFEST_CORPUS)" "$(CORPUS_BOOKLIST)"

verify-structure: $(CORPUS_BOOKLIST)
	@echo "Verifying corpus book structure (glyph checks skipped)..."
	@python3 $(SCRIPTS_DIR)/verify_book.py \
	  "$(MANIFEST_CORPUS)" \
	  "$(CORPUS_BOOKLIST)" \
	  --skip-glyphs

# NOTE: Re-enable `verify` before any public or tagged release.
# $(CORPUS_PDF): verify | dirs
# $(CORPUS_PDF): verify-structure | dirs
$(CORPUS_PDF): $(CORPUS_BOOKLIST) | dirs
	@# Guard: do NOT let pandoc block by reading stdin
	@test -s "$(CORPUS_BOOKLIST)" || (echo "ERROR: empty book list: $(CORPUS_BOOKLIST)" >&2; exit 1)
	$(PANDOC) $$(cat "$(CORPUS_BOOKLIST)") \
	  --toc --toc-depth=1 \
	  -o "$@" $(PANDOC_OPTS)

check-corpus: $(CORPUS_BOOKLIST)
	@echo "Checking corpus-book Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_OPTS); \
	done < "$(CORPUS_BOOKLIST)"; \
	echo "All corpus-book chapters passed individual Pandoc check."

# -----------------------------
# TLDR Book
#  - frontmatter/scaffold: pub/tldr-book/*.md (sorted)
#  - chapters: extracted from pub/tldr-book/IER-tldr.md
# -----------------------------
MANIFEST_TLDR     := $(SRC_DIR)/tldr-book/IER-tldr.md
FRONT_TLDR_DIR    := $(SRC_DIR)/tldr-book

TLDR_BOOKLIST     := $(BUILD_DIR)/tldr-input.txt
TLDR_PDF          := $(BUILD_DIR)/IER-tldr-book.pdf

tldr: $(TLDR_PDF)
tldrlist: $(TLDR_BOOKLIST)

TLDR_FRONT_FILES := $(wildcard $(FRONT_TLDR_DIR)/*.md)

$(TLDR_BOOKLIST): $(MANIFEST_TLDR) $(TLDR_FRONT_FILES) | dirs
	@rm -f "$@.tmp"
	@# 1) prepend frontmatter/scaffold (optional)
	@find "$(FRONT_TLDR_DIR)" -maxdepth 1 -type f -name '*.md' 2>/dev/null | sort > "$@.tmp" || true
	@# 2) append extracted chapters from TLDR manifest-like source
	@python3 $(SCRIPTS_DIR)/extract_book_list.py "$(MANIFEST_TLDR)" "$@.chapters.tmp"
	@cat "$@.chapters.tmp" >> "$@.tmp"
	@rm -f "$@.chapters.tmp"
	@mv "$@.tmp" "$@"
	@echo "Wrote tldr booklist: $@"

$(TLDR_PDF): $(TLDR_BOOKLIST) | dirs
	@test -s "$(TLDR_BOOKLIST)" || (echo "ERROR: empty book list: $(TLDR_BOOKLIST)" >&2; exit 1)
	$(PANDOC) $$(cat "$(TLDR_BOOKLIST)") \
	  --toc --toc-depth=1 \
	  -o "$@" $(PANDOC_OPTS)

check-tldr: $(TLDR_BOOKLIST)
	@echo "Checking tldr-book Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_OPTS); \
	done < "$(TLDR_BOOKLIST)"; \
	echo "All tldr-book chapters passed individual Pandoc check."

# -----------------------------
# Generic: build any single chapter PDF from IER/ or pub/
# -----------------------------
.PRECIOUS: $(BUILD_DIR)/%.pdf

# Prefer IER/ when both exist
$(BUILD_DIR)/%.pdf: $(IER_DIR)/%.md | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

# Fall back to pub/ (top-level)
$(BUILD_DIR)/%.pdf: $(SRC_DIR)/%.md | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

# Fall back to pub/*/ (one level deep: corpus-book/, tldr-book/, etc.)
$(BUILD_DIR)/%.pdf: $(SRC_DIR)/*/%.md | dirs
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
	  $(BUILD_DIR)/*.numbered.txt

rebuild: clean all
