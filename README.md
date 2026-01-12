# Informational Experiential Realism (IER)

This repository contains the complete **Informational Experiential Realism (IER)** project:
the canonical theory, non-authoritative manuscripts, and publication build infrastructure.

---

## Repository Structure

```

.
├── IER/                    ← Canonical IER corpus (authoritative)
│   ├── IER-specification.md
│   ├── IER-theory.md
│   ├── IER-dynamics.md
│   ├── IER-manifest.md
│   ├── IER-canon.md
│   └── …
│
├── manuscripts/            ← Non-authoritative publication manuscripts
│   ├── IER-paper.md
│   ├── IER-tldr.md
│   ├── 00-front-matter.md
│   └── …
│
├── scripts/                ← Build helpers (e.g. extract_book_list.py)
│
├── assets/                 ← Covers, figures, static assets
│
├── build/                  ← Generated artifacts (disposable)
│
├── Makefile                ← Publication build entry point
└── README.md

````

---

## Authority Model (Fixed)

Authority in IER is fixed exclusively by canon rules.

* **Normative authority:** `IER/IER-specification.md`
* **Canonical governance:** `IER/IER-canon.md`
* **Corpus inventory & ordering:** `IER/IER-manifest.md`

All other materials are **non-authoritative derivatives**.

If any conflict appears, it resolves to the canon in `IER/`.

---

## Document Classes

### 1. Canonical Theory (Authoritative)
* Lives in `IER/`
* Includes Tier 1–4 corpus documents
* Fixes identity claims, constraints, exclusions, and scope

### 2. Manuscripts (Non-Authoritative)
* Lives in `manuscripts/`
* Includes:
  * Academic paper manuscript (`IER-paper.md`)
  * TLDR / gateway book assembly materials
  * Front matter and exemplar content
* Introduce **no new commitments**
* May **not** be cited to establish IER claims

### 3. Publication Infrastructure
* `Makefile`
* `scripts/`
* Build configuration and process logic

---

## Build Model

All publication artifacts are produced mechanically from existing sources.

### Inputs
* Canonical chapters from `IER/`
* Manuscripts and front matter from `manuscripts/`
* Ordering rules from `IER/IER-manifest.md`
* Build helpers from `scripts/`

### Outputs (in `build/`)
* `IER-paper.pdf`
* `IER-corpus-book.pdf`
* `IER-tldr-book.pdf`
* Intermediate ordered input lists (e.g. `corpus-input.txt`, `tldr-input.txt`)

No theory content is generated, rewritten, or modified at build time.

---

## Makefile Usage

From the repository root:

```bash
make paper      # build academic paper PDF
make book       # build full corpus book PDF
make tldr       # build TLDR / gateway book PDF
make pubs       # build all publication artifacts
make clean      # remove generated files
````

The Makefile:

* reads Markdown from `IER/` and `manuscripts/`
* writes all artifacts to `build/`
* treats `build/` as disposable
