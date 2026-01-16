# IER Build System (`IER-build.md`)

This document defines **how IER publication artifacts are assembled and built**.

It specifies:

* what inputs a book consists of
* how those inputs are ordered
* how structural pages are generated
* how the final Pandoc input list is produced

It does **not** define theory, canon, authority, or deployment order.

---

## Status and Authority

This document is **non-canonical**.

* Canonical authority lives exclusively in `IER/`
* If this document conflicts with any file in `IER/`, **the canon prevails**

This document governs **mechanical assembly only**.

---

## Core Principle

> **Nothing is inferred at build time. Everything is assembled mechanically from explicit markers.**

The build system:

* never invents theory content
* never rewrites existing source files
* never infers structure from prose
* never decides chapter membership implicitly
* never alters canonical ordering

**Explicit exception:**  
The build system may generate **purely structural Markdown files**
(Part dividers, Section dividers, Chapter breaks) when explicitly directed
by structure markers in a selection file.

These generated files contain **no theory**.

---

## Build Inputs (Per Book)

Each book build is defined by **exactly two content inputs**, declared in the Makefile:

1. a **selection file**
2. a **SCAFFOLD directory**

In addition, the build may supply **publication-layer configuration inputs**
(e.g. Pandoc options, TeX header includes) that affect layout and typography
but introduce no content.

---

## Selection Files

A **selection file** is a Markdown document that explicitly lists included
chapters and declares book structure.

Selection files are **non-authoritative**.

### Chapter Path Extraction (Authoritative)

During scanning of the selection file:

* only **backticked tokens** ending in `.md` are extracted as chapter candidates
* **multiple backticked tokens per line are permitted**
* order of appearance defines chapter order
* duplicate paths are ignored after first occurrence

Selection files may reference:

* canonical files under `IER/`
* non-canonical files under `pub/`

### Path Normalization (Authoritative)

Each extracted token is normalized as follows:

* tokens must end in `.md`
* bare canonical filenames beginning with `IER-` are mapped to `IER/IER-*.md`
* relative paths are preserved verbatim

Examples:

* `` `IER-theory.md` `` → `IER/IER-theory.md`
* `` `IER/IER-theory.md` `` → `IER/IER-theory.md`
* `` `pub/IER-paper.md` `` → `pub/IER-paper.md`

---

## Structural Markers in Selection Files

Selection files may declare **book structure** using Markdown heading levels.

Only the **heading level** matters. Formatting is cosmetic.

### Header Levels (Authoritative)

* `# ...`   → Book marker (informational only)
* `## ...`  → **Part marker** (authoritative)
* `### ...` → **Section marker** (authoritative)

No keywords are required. Any valid Markdown heading at the specified level
is treated as a structural marker.

### Part Indexing Rule (Authoritative)

Parts are indexed mechanically while scanning top-to-bottom:

* content before the first `##` belongs to **Part 0**
* the first `##` starts **Part 1**
* each subsequent `##` increments the Part index by 1

Parts are ordered strictly by appearance.

### Section Rule (Authoritative)

Within a Part:

* each `###` increments the Section index
* a Section divider is generated and **inserted immediately before**
  the next chapter path encountered
* multiple `###` markers in a row produce multiple Section dividers
* a `###` before any `##` belongs to **Part 0**

---

## Generated Structural Pages

The build system generates **purely structural Markdown files** under `build/`.

These files:

* contain **no theory**
* exist only to enforce deterministic pagination and structure
* may contain raw LaTeX pagebreak commands
* are consumed by Pandoc like any other input file

### 1. Part Divider Pages

**Purpose:**  
Ensure deterministic Part boundaries and recto starts when no authored
Part-header scaffold exists.

**Naming (Authoritative):**

```

build/_part_pPP.md

```

Where `PP` is the two-digit Part index.

**Contents (Authoritative):**

* begins with `\cleardoublepage`
* contains a single H1 with the Part title taken from the `## ...` marker

**Emission Policy (Authoritative):**

* generated for Parts `p >= 1`
* **emitted only if** there are **no** SCAFFOLD files for that Part
  in slots `p0–p5`

This prevents conflict with authored Part-header scaffolds.

---

### 2. Section Divider Pages

**Purpose:**  
Provide explicit, paginated Section boundaries.

**Naming (Authoritative):**

```

build/_section_pPP_sSS.md

```

Where:

* `PP` = two-digit Part index
* `SS` = two-digit Section index within that Part

**Contents (Authoritative):**

* begins with `\clearpage`
* contains a single H2 with the Section title taken from the `### ...` marker

Section dividers **do not enforce recto**.

---

### 3. Chapter Break Pages

**Purpose:**  
Ensure each chapter begins on a new page and flush floats deterministically.

**Naming (Authoritative):**

```

build/_break_ch_NNNN.md

```

Where `NNNN` is a zero-padded chapter counter in order of emission.

**Contents (Authoritative):**

* begins with `\clearpage`

A Chapter break page is inserted **immediately before every chapter path**.

---

## SCAFFOLD Directories

A **SCAFFOLD directory** contains publication-only Markdown files:

* front matter
* authored Part headers
* disclaimers
* appendices
* closers

SCAFFOLD files:

* are non-canonical
* contain no theoretical content
* exist only to frame a specific book

SCAFFOLD files are ordered **lexicographically by filename**.

---

## SCAFFOLD Numbering and Placement Rule (Authoritative)

Each SCAFFOLD filename **must** begin with a two-digit numeric prefix:

```

NN-description.md

```

### Part Assignment (Authoritative)

```

part_index = floor(NN / 10)
slot       = NN % 10

```

Examples:

* `00-*.md` → Part 0
* `10-*.md` → Part 1
* `21-*.md` → Part 2, slot 1

Part names in headings are cosmetic.
Only numeric position matters.

### Fixed Insertion Rule per Part (Authoritative)

For each Part `p`, the emitted order is:

| Slot range | Emitted order                                               |
|-----------:|--------------------------------------------------------------|
| `p0–p5`    | SCAFFOLD files **before** content                            |
| —          | content stream (dividers + chapter breaks + chapters)        |
| `p6–p9`    | SCAFFOLD files **after** content                             |

Interleaving SCAFFOLD files between individual chapters is not supported.

Only **generated structural pages** may appear mid-Part.

---

## Ordering Authority

All ordering and generation is performed by:

```

scripts/extract_book_list.py

```

The script:

1. reads and validates the SCAFFOLD directory
2. scans the selection file linearly
3. tracks Part (`##`) and Section (`###`) markers
4. extracts chapter paths from backticked `.md` tokens
5. generates:
   * Part divider pages (conditionally)
   * Section divider pages
   * Chapter break pages
6. applies the fixed SCAFFOLD insertion rules per Part
7. de-duplicates paths while preserving first occurrence
8. writes:
   * a Pandoc input list (e.g. `build/corpus-input.txt`)
   * a numbered diagnostic file alongside it

Pandoc consumes the emitted list **verbatim**.

---

## Failure Conditions

The build fails if:

* the selection file references a non-existent `.md` file
* a SCAFFOLD filename does not match `NN-*.md`
* the emitted input list is empty
* any referenced file cannot be resolved relative to the repository root

---

## Summary

The IER build system is:

* deterministic
* explicit
* non-inferential
* content-preserving
* canon-safe

It exists to **assemble**, not to interpret.

---
