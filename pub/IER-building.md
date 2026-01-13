# Building IER Publications

This document describes **how publication artifacts are assembled** from
canonical IER sources.

It is a **procedural and mechanical guide** only.
It does not define theory, canon, or authority.

---

## Scope and Authority

This file is **non-canonical**.

* Canonical authority lives exclusively in `IER/`
* This document describes how **non-authoritative publication artifacts**
  are generated from canonical sources

If this document conflicts with any file in `IER/`,
**the canon always prevails**.

---

## High-Level Principle

> **Nothing is authored at build time. Everything is assembled.**

The build system:
- never invents content
- never rewrites canonical material
- never infers structure beyond explicit ordering rules

All publication structure emerges from:
1. Canonical ordering (`IER/IER-manifest.md`)
2. Explicit scaffold files in `pub/`
3. Mechanical concatenation via the Makefile

---

## Repository Layers (Build-Relevant)

```

IER/        ← Canonical source of truth (authoritative)
pub/        ← Publication-layer scaffolding & wrappers (non-canonical)
scripts/    ← Deterministic build helpers
build/      ← Generated artifacts (disposable)

```

---

## Publication Types

The repository produces three publication artifacts:

| Artifact | Output | Purpose |
|--------|--------|---------|
| Paper | `IER-paper.pdf` | Academic / article-style presentation |
| Corpus Book | `IER-corpus-book.pdf` | Full ordered corpus with structural parts |
| TLDR Book | `IER-tldr-book.pdf` | Gateway / summary presentation |

Each artifact has a **distinct assembly rule**, but all obey the same
authority model.

---

## Canonical Ordering Source

### `IER/IER-manifest.md`

This file defines:

- **Which canonical chapters exist**
- **Their authoritative order**

The manifest is:
- read mechanically
- never overridden by publication logic
- the sole source of chapter ordering for books

Publication builds may *label* or *frame* this order,
but they never redefine it.

---

## Paper Build

### Input
```

pub/IER-paper.md

````

This file is a **standalone authored composition** that may:
- quote or summarize canon
- reference canonical files
- embed figures

It is **not** mechanically expanded from the manifest.

### Command
```bash
make paper
````

### Output

```
build/IER-paper.pdf
```

---

## Corpus Book Build

The corpus book is a **mechanical assembly** of:

1. Publication-layer scaffold files
2. Canonical chapters (in manifest order)

### Inputs

#### 1. Scaffold / Front-Matter Files

```
pub/corpus-book/*.md
```

These files:

* are included first
* are ordered **lexicographically by filename**
* define presentation structure (front matter, Parts, section dividers)

They must **not** contain canonical theory content.

Typical examples:

```
00-front-matter.md
01-theory.md
02-applications.md
03-meta.md
```

#### 2. Canonical Chapters

```
IER/*.md   (as listed in IER/IER-manifest.md)
```

These files:

* contain the actual theory
* are included strictly in manifest order
* are never reordered or filtered by publication logic

### Assembly Rule (Authoritative)

The final Pandoc input order is:

```
(pub/corpus-book/*.md sorted)
→
(chapters extracted from IER/IER-manifest.md)
```

This ordered list is written to:

```
build/corpus-input.txt
```

That file is the **ground truth** for what Pandoc consumes.

### Command

```bash
make book
```

### Output

```
build/IER-corpus-book.pdf
```

---

## Section / Part Dividers (Corpus Book)

### Where They Live

**All section and Part dividers live in:**

```
pub/corpus-book/
```

They are **header-only scaffold files**.

### Divider File Pattern

A divider file typically contains:

```md
# Part I — Informational Experiential Theory

(optional orientation text)
```

Rules:

* Use a **single top-level header**
* Do not enumerate chapters
* Do not restate the manifest
* Do not include canonical content

### How Placement Works

> A Part applies to all chapters that follow it until the next divider.

Because:

* scaffold files are prepended
* canonical chapters follow in manifest order

The **manifest itself must already be ordered by Part**.
Divider files merely label those transitions.

---

## Heading Levels (Recommended)

To maintain a clean table of contents:

* **Part / Section dividers:** `#`
* **Canonical chapter titles:** `##`

This allows Pandoc TOC nesting to behave predictably.

---

## TLDR Book Build

The TLDR book follows the **same model** with a different source set.

### Inputs

#### Scaffold Files

```
pub/tldr-book/*.md
```

#### Chapter Source

```
pub/tldr-book/IER-tldr.md
```

(This file plays a manifest-like role for the TLDR content.)

### Command

```bash
make tldr
```

### Output

```
build/IER-tldr-book.pdf
```

---

## Makefile as Build Orchestrator

The Makefile is responsible only for:

* discovering inputs
* ordering files
* invoking Pandoc
* writing outputs to `build/`

It does **not**:

* understand theory
* encode semantic structure
* decide chapter membership
* contain publication-specific meaning

If structure seems wrong in a PDF, the debugging order is:

1. `build/*-input.txt`
2. scaffold file names
3. `IER/IER-manifest.md`

Never the Makefile first.

---

## What Not to Do

Do **not**:

* Add section logic to the Makefile
* Encode Part membership in the manifest
* Annotate canonical chapters with publication metadata
* Interleave scaffold files into the manifest
* Treat `pub/` as authoritative

Those actions collapse canon, presentation, and tooling into one layer.

---

## Mental Model (Canonical)

* **IER/** — what exists and in what order
* **pub/** — how it is presented
* **scripts/** — how lists are extracted
* **Makefile** — how things are assembled
* **build/** — disposable results

If you keep these layers separate, the system remains stable,
auditable, and extensible.

---

## Status

This document describes the **current, intended build model**.
Changes to the build system should update this file accordingly.
