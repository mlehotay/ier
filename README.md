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
  → [`IER/README.md`](IER/README.md)

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

Overview:

- **Governance overview:**  
  → [`governance/README.md`](governance/README.md)

Key documents:

- **Build and assembly rules:**  
  `governance/IER-build.md`

- **Deployment and release discipline:**  
  `governance/IER-deployment.md`

- **Legal and IP context:**  
  `governance/IER-legal.md`

---

## Publication artifacts (`pub/`)

The directory **`pub/`** contains non-canonical artifacts used to assemble
reader-facing publications derived from the corpus.

These files are **downstream presentations** only and introduce no authority.

Key artifacts include:

- `pub/IER-corpus-book.md` — corpus book assembly plan
- `pub/IER-corpus-selection.md` — corpus book chapter selection
- `pub/IER-tldr-book.md` — TLDR / gateway book assembly plan
- `pub/IER-tldr-selection.md` — TLDR chapter selection
- `pub/IER-paper.md` — paper / preprint form

Book-specific scaffolding (front matter, part headers, etc.) lives under:

- `pub/corpus-book/`
- `pub/tldr-book/`

---

## Build outputs

Generated artifacts are written to **`build/`** and are disposable.

Typical outputs include:

- `build/IER-paper.pdf`
- `build/IER-corpus-book.pdf`
- `build/IER-tldr-book.pdf`

---

## Build commands

```bash
make paper      # build paper/preprint PDF
make book       # build corpus book PDF
make tldr       # build TLDR book PDF
make pubs       # build all publication artifacts
make clean      # remove generated artifacts
````

---

## Project-wide constraint

> Nothing is inferred at build time.
> Artifacts are assembled mechanically from explicit manifests,
> selections, and scaffolding.
> No theory content is generated, reordered, or rewritten during the build.
