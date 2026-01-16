# Informational Experiential Realism (IER)

This repository contains the **Informational Experiential Realism (IER)** project:
the canonical theoretical corpus, non-canonical publication artifacts, and the
governance and build infrastructure used to assemble public releases.

**Authority rule:**  
Only files under **`IER/`** are canonical (authoritative).  
Everything outside `IER/` is non-canonical, non-authoritative, and downstream.

---

## Repository layout

```

IER/           Canonical corpus (authoritative)
governance/    Build, deployment, and legal governance (non-canonical)
pub/           Publication artifacts + assembly inputs (non-canonical)
scripts/       Build helpers
assets/        Static assets (covers, figures)
build/         Generated outputs (disposable)
Makefile       Main build entry point

````

---

## Canonical corpus (`IER/`)

The directory **`IER/`** contains the authoritative theory: definitions, constraints,
arguments, and corpus structure.

Key entry points:

- **Corpus overview:**  
  `IER/README.md`

- **Normative specification:**  
  `IER/IER-specification.md`

- **Canon governance:**  
  `IER/IER-canon.md`

- **Corpus membership and ordering:**  
  `IER/IER-manifest.md`

If any document outside `IER/` conflicts with a document inside `IER/`,
the material inside `IER/` takes precedence by definition.

---

## Governance documents (`governance/`)

The directory **`governance/`** contains non-canonical documents that govern
**process**, not theory.

These documents define how the project is built, released, and legally framed,
but introduce no theoretical claims and carry no authority over the corpus itself.

Key documents:

- `governance/IER-build.md` — build and assembly rules
- `governance/IER-publishing.md` — rendering and physical instantiation rules
- `governance/IER-deployment.md` — public release order and immutability discipline
- `governance/IER-legal.md` — legal and IP context for AI-assisted authorship

---

## Publication artifacts (`pub/`)

The directory **`pub/`** contains non-canonical artifacts used to assemble
reader-facing publications derived from the corpus.

These files are **downstream presentations only** and introduce no authority.

Key artifacts:

- `pub/IER-corpus-selection.md` — corpus book selection and structure
- `pub/IER-tldr-selection.md` — TLDR / gateway book selection and structure
- `pub/IER-paper.md` — standalone paper / preprint source

Book-specific scaffolding (front matter, part headers, disclaimers, appendices)
lives under:

- `pub/corpus-book/`
- `pub/tldr-book/`

### TeX header includes (publication layer)

Publication-layer TeX configuration lives under:

- `pub/tex/ier-book.tex`  
  Header for book-class artifacts (corpus book and TLDR book)

- `pub/tex/ier-paper.tex`  
  Header for article-class artifacts (paper and single-chapter PDFs)

These files control pagination, headers/footers, and typography, but contain
no theory and introduce no corpus content.

---

## Build outputs

Generated artifacts are written to **`build/`** and are disposable.

Typical outputs include:

- `build/IER-paper.pdf`
- `build/IER-corpus-book.pdf`
- `build/IER-tldr-book.pdf`

Intermediate build products include:

- `build/corpus-input.txt` — Pandoc input list for the corpus book
- `build/tldr-input.txt` — Pandoc input list for the TLDR book
- `build/*.numbered.txt` — numbered diagnostics for input order

---

## Build system overview

IER books are assembled mechanically using:

- a **selection file** (explicit chapter paths and structure markers)
- a **SCAFFOLD directory** (front matter and framing files)

The build system:

- extracts backticked `.md` paths from the selection file
- tracks Part (`##`) and Section (`###`) structure markers
- generates purely structural Markdown files:
  - Part divider pages
  - Section divider pages
  - Chapter break pages
- merges these with scaffold files according to fixed numeric rules
- emits a final Pandoc input list consumed verbatim

No theory is inferred, rewritten, reordered, or synthesized at build time.

---

## Build commands

```bash
make paper        # build paper / preprint PDF
make book         # build corpus book PDF
make tldr         # build TLDR / gateway book PDF
make pubs         # build all publication artifacts

make verify       # verify corpus book list + content discipline checks
make verify-structure  # verify corpus book structure only (glyph checks skipped)

make check-corpus # pandoc-check each corpus-book chapter individually
make check-tldr   # pandoc-check each tldr-book chapter individually

make clean        # remove generated artifacts
make spotless     # remove entire build/ directory (fresh checkout cleanliness)
````

---

## Project-wide constraint

> **Nothing is inferred at build time.**

Artifacts are assembled mechanically from explicit manifests, selections,
and scaffolding.
No theory content is generated, reordered, or rewritten during the build.

---
