# Build artifacts live here (relative to IER-book/)
BUILD_DIR   := build

# Paper inputs live here (relative to IER-book/)
PAPER_DIR   := ../IER-paper
PAPER_SRC   := $(PAPER_DIR)/IER-paper.md
PAPER_PDF   := $(BUILD_DIR)/IER-paper.pdf

PANDOC      := pandoc
PANDOC_OPTS := -V geometry:margin=1in

.PHONY: all paper clean rebuild dirs

all: paper

dirs:
	@mkdir -p $(BUILD_DIR)

paper: $(PAPER_PDF)

# Rebuild the PDF when the source markdown changes
$(PAPER_PDF): $(PAPER_SRC) | dirs
	$(PANDOC) $(PAPER_SRC) -o $(PAPER_PDF) $(PANDOC_OPTS)

clean:
	rm -f $(PAPER_PDF)

rebuild: clean all
