# Informational Experiential Realism (IER)  
**Repository Overview — v10.8.3**

---

## Purpose

This repository contains the **Informational Experiential Realism (IER)** project:

- the **canonical theoretical corpus**
- **publication artifacts** derived from that corpus
- the **build, verification, and governance infrastructure** used to assemble releases

The repository is designed to make **structure explicit, deterministic, and verifiable**.

Nothing is implicitly ordered.  
Nothing is inferred at build time.  
Nothing authoritative is downstream of tooling.

This README is intended for **contributors, reviewers, and technically literate readers**
who need to understand the project’s **structure, authority boundaries, and discipline**.
It is **not** an introduction to the theory itself.

---

## Repository Structure

```text
IER/            Canonical IER theory chapters (authoritative)
pub/            Publication-layer inputs (selections, scaffolds, interface drafts)
scripts/        Build and verification tools
build/          Generated outputs (disposable)
assets/         Covers and static assets
governance/     Build, publishing, deployment, legal, and reader-discipline rules
_work/          Drafts, planning notes, non-canonical material
````

Each major directory defines its own role and constraints.
Read the local `README.md` files before modifying or interpreting contents.

---

## Canonical Content

All **theoretical authority** lives exclusively under `IER/`.

* Canonical chapters are named `IER/IER-*.md`
* These files define **all** authoritative claims, criteria, and commitments

See:

* `IER/README.md` — theory orientation and reading paths (non-authoritative)
* `IER/IER-canon.md` — authority, alignment, and versioning rules
* `IER/IER-manifest.md` — official corpus inventory and ordering

Nothing outside `IER/` has theoretical authority.

---

## Publication Artifacts

IER is published in distinct **artifact classes** with different epistemic roles.

All publication artifacts are **derived views** of the canonical corpus and carry
**no independent theoretical authority**.

The primary artifact classes are:

1. **Paper** — condensed scholarly interface
2. **Corpus Book** — full technical monograph (deployment anchor)
3. **TLDR Book** — reader-facing explanatory interface
4. **Foundations Book** — verbatim foundational subset interface

These artifacts differ by **epistemic role**, not merely by length or style.

---

### Paper (Scholarly Interface)

* Source: `pub/IER-paper.md`
* Output: `build/IER-paper.pdf`

Build with:

```bash
make paper
```

The paper is **non-authoritative** and downstream of the corpus book.
It is intended for academic circulation, critique, and citation.

---

### Corpus Book (Anchor Artifact)

* Selection file: `pub/IER-corpus-selection.md`
* Scaffold directory: `pub/corpus-book/`
* Output: `build/IER-corpus-book.pdf`

Build with:

```bash
make book
```

The Corpus Book is the **sole deployment anchor** for a given IER version.
It establishes stable ordering, pagination, and citation reference.

---

### TLDR Book (Gateway Interface)

* Selection file: `pub/IER-tldr-selection.md`
* Scaffold directory: `pub/tldr-book/`
* Output: `build/IER-tldr-book.pdf`

Build with:

```bash
make tldr
```

This artifact is explicitly **non-authoritative** and reader-facing.
It exists to explain, not to define.

---

### Foundations Book (Verbatim Interface)

* Selection file: `pub/IER-foundations-selection.md`
* Output: `build/IER-foundations-book.pdf`

This artifact contains a **verbatim subset** of canonical material corresponding
to IER’s foundations, with minimal framing and no interpretation.

It is **non-authoritative** and does not replace the corpus book.

---

## Build and Verification (High Level)

Book artifacts are produced by a **mechanical, non-inferential build system**:

* Chapter inclusion and ordering are declared explicitly
* Structural pages may be generated, but never theory
* Verification is mandatory for validity

This README intentionally does **not** duplicate build mechanics.

For authoritative details, see:

* `governance/IER-build.md`

---

## Verification

Verification is **not optional linting**.

A book that does not verify is considered **invalid**, regardless of whether
Pandoc succeeds.

Common targets:

```bash
make verify
make verify-tldr
```

Verification scope and invariants are defined in `IER-build.md`.

---

## Governance

All non-canonical constraints governing **build mechanics, publishing,
deployment, readership, and legal context** live under `governance/`.

See `governance/README.md` for:

* authority boundaries
* document roles
* conflict resolution rules
* guidance on when each governance file applies

Key governance documents include:

* `governance/IER-build.md`
  Mechanical assembly rules and verification invariants

* `governance/IER-publishing.md`
  Rendering, typography, and physical format rules

* `governance/IER-deployment.md`
  Release order, anchoring discipline, and immutability rules

* `governance/IER-readers.md`
  Audience analysis and reading patterns

* `governance/IER-legal.md`
  Legal context for AI-assisted authorship (as of 2025)

If documents conflict, authority resolves **upstream** toward the canon.

---

## Design Philosophy

This repository is intentionally:

* **explicit** over implicit
* **deterministic** over convenient
* **verified** over assumed
* **canon-safe** over flexible

If a structure matters, it is encoded.
If a rule matters, it is enforced.
If something changes, verification fails loudly.

---

## Status

Active development.

* Canonical theory evolves cautiously
* Build and verification rules evolve conservatively
* Governance clarifies constraints, not content
* Non-canonical drafts live under `_work/` and never affect builds

---
