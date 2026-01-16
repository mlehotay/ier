# Informational Experiential Realism (IER)

This repository contains the **Informational Experiential Realism (IER)** project:

- the **canonical theoretical corpus**
- **publication artifacts** (books and papers)
- and the **build, verification, and governance infrastructure** used to assemble and validate releases

The repository is designed to make **structure explicit, deterministic, and verifiable**.  
Nothing is implicitly ordered; nothing is trusted without verification.

---

## Repository Structure

```text
IER/            Canonical IER theory chapters (authoritative content)
pub/            Publication inputs (selections, scaffolds, papers)
scripts/        Build helpers (extractor, verifier)
build/          Generated outputs (disposable)
assets/         Covers and static assets
governance/     Build, publishing, legal, and deployment rules
_work/          Drafts, plans, and non-canonical working notes
````

### Canonical Content

* `IER/IER-*.md`
  The canonical IER chapters. These files are subject to **strict authoring rules** and are the only files treated as canonical theory.

See:

* [`IER/README.md`](IER/README.md)
* [`IER/IER-canon.md`](IER/IER-canon.md)

---

## Publications

IER is published in three main forms:

1. **Standalone paper**
2. **Corpus book** (full reference work)
3. **TLDR book** (condensed, reader-oriented)

### Paper

* Source: [`pub/IER-paper.md`](pub/IER-paper.md)
* Output: `build/IER-paper.pdf`

Build with:

```bash
make paper
```

---

### Corpus Book

* Selection file: [`pub/IER-corpus-selection.md`](pub/IER-corpus-selection.md)
* Scaffold directory: [`pub/corpus-book/`](pub/corpus-book/)
* Emitted list: `build/corpus-input.txt`
* Output PDF: `build/IER-corpus-book.pdf`

Build with:

```bash
make book
```

---

### TLDR Book

* Selection file: [`pub/IER-tldr-selection.md`](pub/IER-tldr-selection.md)
* Scaffold directory: [`pub/tldr-book/`](pub/tldr-book/)
* Emitted list: `build/tldr-input.txt`
* Output PDF: `build/IER-tldr-book.pdf`

Build with:

```bash
make tldr
```

---

## Build Pipeline (Books)

Book artifacts are assembled in **three explicit stages**:

### 1. Selection

Selection files declare:

* chapter order
* part boundaries (`##`)
* section boundaries (`###`)

Examples:

* [`pub/IER-corpus-selection.md`](pub/IER-corpus-selection.md)
* [`pub/IER-tldr-selection.md`](pub/IER-tldr-selection.md)

Selection files are **structure-only** and contain backticked `.md` paths.

---

### 2. Emission

The extractor:

* [`scripts/extract_book_list.py`](scripts/extract_book_list.py)

takes:

```text
selection.md + scaffold directory
```

and produces:

* a deterministic Pandoc input list (`build/*-input.txt`)
* generated structural pages in `build/`:

  * part dividers
  * section dividers
  * chapter breaks

This step is **mechanical**, not authoritative.

---

### 3. Verification (Authoritative)

The verifier:

* [`scripts/verify_book.py`](scripts/verify_book.py)

is the **authoritative enforcement layer**.

It:

* recomputes the expected emitted list from the selection and scaffolds
* requires **exact equality** with the emitted booklist
* enforces structural invariants (divider placement, scaffold slots)
* enforces scoped authoring rules on canonical content

Verification is **not optional linting**.
A book that does not verify is considered **invalid**.

Run manually:

```bash
make verify
make verify-tldr
```

---

## Scaffold Files

Book scaffolds live under:

* `pub/corpus-book/`
* `pub/tldr-book/`

and must be named:

```text
NN-description.md
```

Where:

* `NN // 10` → part index
* `NN % 10`  → slot within the part

Slot semantics:

| Slot | Meaning        |
| ---: | -------------- |
|  0–5 | Before content |
|  6–9 | After content  |

These rules are **structural**, not cosmetic, and are enforced by the verifier.

---

## Verification Targets

Common targets:

```bash
make verify                 # full corpus verification
make verify-structure       # skip glyph checks
make verify-authoring       # authoring rules only

make verify-tldr
make verify-tldr-structure
make verify-tldr-authoring
```

---

## Authoring Rules

Canonical chapters (`IER/IER-*.md`) are subject to strict rules, including:

* exactly one top-level `#` heading
* no YAML front matter
* no raw LaTeX preamble directives
* no indent-based code blocks
* no HTML tables
* controlled Unicode and math glyph usage

Scaffold files are checked with a **relaxed but stable** subset of rules.

See governance documentation for details.

---

## Governance and Policies

All build, publishing, and legal policies live under `governance/`:

* [`governance/IER-build.md`](governance/IER-build.md)
  Build mechanics and invariants

* [`governance/IER-publishing.md`](governance/IER-publishing.md)
  Release discipline and publication rules

* [`governance/IER-deployment.md`](governance/IER-deployment.md)
  Distribution and deployment guidance

* [`governance/IER-legal.md`](governance/IER-legal.md)
  Legal context for AI-assisted authorship **as of 2025**,

---

## Design Philosophy

This repository is intentionally:

* **explicit** over implicit
* **deterministic** over convenient
* **verified** over assumed

If a structure matters, it is encoded.
If a rule matters, it is enforced.
If something changes, verification will fail loudly.

---

## Status

Active development.
Canonical content evolves carefully; build and verification rules evolve conservatively.

Non-canonical drafts and planning material live under `_work/` and do not affect builds.

---
