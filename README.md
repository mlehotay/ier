# Informational Experiential Realism (IER)

This repository contains the **Informational Experiential Realism (IER)** project:
the canonical theory, non-canonical publication artifacts, and publication build infrastructure.

Only materials in `IER/` are canonical. All other materials are non-authoritative derivatives.

---

## Repository Structure

```
.
├── IER/                         ← Canon (authoritative)
│   ├── IER-specification.md
│   ├── IER-manifest.md
│   ├── IER-canon.md
│   └── …
│
├── pub/                         ← Publication layer (non-canonical)
│   ├── IER-paper.md
│   ├── IER-corpus-book.md
│   ├── IER-tldr-book.md
│   ├── IER-deployment.md
│   ├── IER-legal.md
│   │
│   ├── corpus-book/             ← Corpus-book assembly scaffolding
│   └── tldr-book/               ← TLDR / gateway book scaffolding
│
├── scripts/                     ← Build helpers
├── assets/                      ← Figures and static assets
├── build/                       ← Generated artifacts (disposable)
├── Makefile                     ← Publication build entry point
└── README.md
```

---

## Authority Model

Authority is fixed exclusively by the canon:

* Normative authority: `IER/IER-specification.md`
* Canon governance: `IER/IER-canon.md`
* Corpus inventory & ordering: `IER/IER-manifest.md`

All materials outside `IER/` defer to the canon.
Conflicts always resolve in favor of `IER/`.

---

## Build Model

Publication artifacts are produced mechanically from existing sources.

**Inputs**

* Canonical content from `IER/`
* Publication-layer materials from `pub/`
* Ordering rules from `IER/IER-manifest.md`
* Build helpers from `scripts/`

**Outputs** (in `build/`)

* `IER-paper.pdf`
* `IER-corpus-book.pdf`
* `IER-tldr-book.pdf`

No theory content is generated or modified at build time.

---

## Makefile Usage

```bash
make paper      # build academic paper
make book       # build corpus book
make tldr       # build TLDR book
make pubs       # build all publications
make clean      # remove generated files
```
