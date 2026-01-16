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

---

## Repository Structure

```text
IER/            Canonical IER theory chapters (authoritative)
pub/            Publication inputs (selections, scaffolds, papers)
scripts/        Build and verification tools
build/          Generated outputs (disposable)
assets/         Covers and static assets
governance/     Build, publishing, deployment, and legal rules
_work/          Drafts, planning notes, non-canonical material
````

---

## Canonical Content

All **theoretical authority** lives exclusively under `IER/`.

* Canonical chapters are named `IER/IER-*.md`
* These files define *all* authoritative claims, criteria, and commitments

See:

* `IER/README.md`
* `IER/IER-canon.md`

Nothing outside `IER/` has theoretical authority.

---

## Publication Artifacts

IER is published in three distinct artifact classes:

1. **Paper** (condensed, interface-layer)
2. **Corpus Book** (full technical monograph; deployment anchor)
3. **TLDR Book** (gateway / explanatory interface)

These artifacts differ in **epistemic role**, not just length or style.

---

### Paper

* Source: `pub/IER-paper.md`
* Output: `build/IER-paper.pdf`

Build with:

```bash
make paper
```

The paper is **non-authoritative** and downstream of the corpus book.

---

### Corpus Book (Anchor Artifact)

* Selection file: `pub/IER-corpus-selection.md`
* Scaffold directory: `pub/corpus-book/`
* Output: `build/IER-corpus-book.pdf`

Build with:

```bash
make book
```

This is the **sole deployment anchor** for a given IER version.

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

---

## Build and Verification (High Level)

Book artifacts are produced by a **mechanical, non-inferential build system**:

* Chapter inclusion and ordering are declared explicitly
* Structural pages may be generated, but never theory
* Verification is mandatory for validity

The README intentionally does **not** duplicate build mechanics.

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

## Governance Documents

All policy, discipline, and constraint documents live under `governance/`:

* `governance/IER-build.md`
  Mechanical assembly rules and invariants

* `governance/IER-publishing.md`
  Rendering, typography, and physical format rules

* `governance/IER-deployment.md`
  Release order, anchoring, and public framing

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
* Non-canonical drafts live under `_work/` and never affect builds

---
