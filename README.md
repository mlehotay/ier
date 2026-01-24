# Informational Experiential Realism (IER)
**Repository Overview — v10.9.x**

---

## Purpose

This repository contains the **Informational Experiential Realism (IER)** project:

* the **canonical theoretical corpus**
* **publication artifacts** derived from that corpus
* **build, verification, and governance infrastructure**

The repository is designed to make **authority, structure, and ordering explicit**.

Nothing is inferred at build time.  
Nothing authoritative is downstream of tooling.

This README is for **contributors and reviewers**.  
It is **not** an introduction to the theory.

---

## Repository Structure

```text
IER/            Canonical IER theory corpus (authoritative)
pub/            Publication-layer selections and scaffolds
scripts/        Build and verification tools
build/          Generated outputs (disposable)
assets/         Static assets
governance/     Build, publishing, deployment, and reader-discipline rules
_work/          Drafts and non-canonical material
````

Each directory defines its own role and constraints.
Read local `README.md` files before interpreting contents.

---

## Canonical Authority

All **theoretical authority** lives exclusively under [`IER/`](IER/).

Key files:

* [`IER/README.md`](IER/README.md) — corpus orientation (non-authoritative)
* [`IER/IER-canon.md`](IER/IER-canon.md) — authority, alignment, and versioning rules
* [`IER/IER-manifest.md`](IER/IER-manifest.md) — official corpus inventory and ordering

Nothing outside [`IER/`](IER/) has theoretical or ethical authority.

---

## Publication Artifacts

Books and papers are **derived views** of the corpus.
They introduce **no independent authority**.

Primary artifacts:

* **Corpus Book** — deployment anchor
* **Paper** — scholarly interface
* **TLDR Book** — reader-facing explanation
* **Foundations Book** — verbatim foundational subset

Build details are defined in
[`governance/IER-build.md`](governance/IER-build.md).

---

## Governance & Verification

Build, publishing, deployment, and reader-discipline rules live under
[`governance/`](governance/).

Verification is **mandatory**.
A build that does not verify is invalid.

---

## Design Philosophy

This repository is intentionally:

* explicit over implicit
* deterministic over convenient
* verified over assumed
* canon-safe over flexible

---

## Status

Active development.

Canonical theory evolves cautiously.
Non-canonical drafts live under [`_work/`](_work/) and never affect builds.
