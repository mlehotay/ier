# Build artifacts live here (relative to IER-book/)
BUILD_DIR   := build

PANDOC      := pandoc
PANDOC_OPTS := \
  -V geometry:margin=1in \
  --pdf-engine=xelatex \
  -V mainfont="DejaVu Serif" \
  -V sansfont="DejaVu Sans" \
  -V monofont="DejaVu Sans Mono" \
  -V mathfont="TeX Gyre DejaVu Math"

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

.PHONY: all paper book booklist clean rebuild dirs check-chapters

# ----------------------------------------
# Debug: build each chapter individually
# ----------------------------------------

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
  	  --toc --toc-depth=1 \
  	  -o $(BOOK_PDF) $(PANDOC_OPTS)

# ----------------------------------------
# Generic: build any single chapter PDF
# Usage: make IER-intro   -> build/IER-intro.pdf
#        make IER-math    -> build/IER-math.pdf
# ----------------------------------------

# Never delete chapter PDFs as intermediates
.PRECIOUS: $(BUILD_DIR)/%.pdf

$(BUILD_DIR)/%.pdf: $(IER_DIR)/%.md | dirs
	$(PANDOC) $< -o $@ $(PANDOC_OPTS)

%: $(BUILD_DIR)/%.pdf
	@:

# ---- Debugging ----
check-chapters: booklist
	@echo "Checking individual Markdown files..."
	@set -e; \
	n=0; \
	while read f; do \
	  n=$$((n+1)); \
	  echo "[$$n] Testing $$f"; \
	  $(PANDOC) $$f -t pdf -o /dev/null $(PANDOC_OPTS); \
	done < $(BOOKLIST); \
	echo "All chapters passed individual Pandoc check."

# ---- Housekeeping ----
clean:
	rm -f $(PAPER_PDF) $(BOOK_PDF) $(BOOKLIST) $(BUILD_DIR)/book-input.numbered.txt

rebuild: clean all
