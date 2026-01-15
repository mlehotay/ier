# IER Build System (`IER-build.md`)

This document defines **how IER publication artifacts are assembled and built**.

It specifies:

* what inputs a book consists of
* how those inputs are ordered
* how the final render list is produced

It does **not** define theory, canon, authority, or deployment order.

---

## Status and Authority

This file is **non-canonical**.

* Canonical authority lives exclusively in `IER/`
* If this document conflicts with any file in `IER/`, **the canon prevails**

---

## Core Rule

> **Nothing is inferred at build time. Everything is assembled mechanically from explicit markers.**

The build system:

* never invents theory content
* never rewrites existing source files
* never infers structure from prose
* never decides chapter membership implicitly

**Exception (explicit):** the build may generate **purely structural divider pages** (e.g., Section title pages) when a selection file explicitly declares them.

---

## Build Inputs (Per Book)

Each book is defined by **exactly two inputs**, declared in the Makefile:

1. a **selection file**
2. a **SCAFFOLD directory**

No other inputs are consulted.

---

## Selection Files

A **selection file** is a Markdown document that explicitly lists included chapters and (optionally) declares book structure.

### Chapter path extraction (authoritative)

* only backticked tokens ending in `.md` are extracted as chapter candidates
* order of appearance defines chapter order
* duplicate paths are ignored after first occurrence
* selection files are non-authoritative

Selection files may reference:

* canonical files in `IER/`
* non-canonical files in `pub/`

### Path normalization (authoritative)

During extraction:

* any backticked token ending in `.md` is treated as a candidate path
* bare canonical filenames beginning with `IER-` are mapped to `IER/IER-*.md`

Examples:

* `` `IER-theory.md` `` → `IER/IER-theory.md`
* `` `IER/IER-theory.md` `` → `IER/IER-theory.md`
* `` `pub/IER-paper.md` `` → `pub/IER-paper.md`

---

## Structure Markers in Selection Files

Selection files may declare **Book / Part / Section** structure using Markdown header levels.

### Header levels (authoritative)

* `# ...` = **Book** (optional; informational only)
* `## ...` = **Part marker** (authoritative for Part boundaries)
* `### ...` = **Section marker** (authoritative for Section boundaries within a Part)

Header formatting (bold, numbering, roman numerals, em-dashes, etc.) is cosmetic. Only the header **level** matters.

### Part indexing rule (authoritative)

Parts are numbered mechanically as the file is scanned top-to-bottom:

* everything **before the first `##`** is **Part 0**
* the first `##` starts **Part 1**
* each subsequent `##` increments the Part index by **1**

Parts are ordered by appearance.

### Section rule (authoritative)

Within a Part, each `###` starts a new Section.

A Section marker triggers insertion of a **generated section divider page** into the render list:

* the divider page is emitted **after** the `###` marker and **before** the next chapter path that follows it
* multiple `###` markers in a row produce multiple divider pages in that order
* a `###` before any `##` belongs to **Part 0**

---

## Generated Section Divider Pages

Section divider pages are generated at build time and contain **no theory**.

### Naming (authoritative)

Divider pages are written under `build/` with deterministic filenames, e.g.:

* `build/_section_p{PP}_s{SS}.md`

Where:

* `PP` = two-digit Part index (`00`, `01`, `02`, …)
* `SS` = two-digit Section index within that Part (`01`, `02`, …)

(Optionally, implementations may add a slug suffix for readability; the numeric prefix is the ordering key.)

### Contents (guidance)

Divider pages should be minimal and PDF-friendly, typically:

* a page break
* a Section title
* a page break

Exact formatting is a publishing choice; the only requirement is that these pages are **purely structural**.

---

## SCAFFOLD Directories

A **SCAFFOLD directory** contains publication-only Markdown files:
front matter, Part headers, disclaimers, appendices, closers.

SCAFFOLD files:

* are non-canonical
* contain no theoretical content
* exist only to frame a specific book

They are ordered **lexicographically by filename** (numeric prefixes then do the work).

---

## SCAFFOLD Numbering and Placement Rule (Authoritative)

SCAFFOLD filenames **must** begin with a two-digit numeric prefix:

```
NN-description.md
```

### Part assignment (authoritative)

```
part_index = floor(NN / 10)
slot       = NN % 10
```

Examples:

* `00-*.md` → Part 0
* `10-*.md` → Part 1
* `20-*.md` → Part 2

Part names in headings are cosmetic.
Only numeric position matters.

### Fixed insertion rule per Part (authoritative)

For each Part `p`, files are emitted in this order:

| Slot range | Emitted order                                                           |
| ---------- | ----------------------------------------------------------------------- |
| `p0–p5`    | SCAFFOLD files **before** content                                       |
| —          | content stream for that Part (chapters in order, with section dividers) |
| `p6–p9`    | SCAFFOLD files **after** content                                        |

Interleaving SCAFFOLD between individual chapters is not supported.
Section divider pages are the only supported mid-Part insertions.

---

## Ordering Authority

All ordering is performed by:

```
scripts/extract_book_list.py
```

The script:

1. reads the SCAFFOLD directory
2. validates SCAFFOLD filenames (`NN-*.md`)
3. scans the selection file:

   * tracks Part boundaries via `##`
   * tracks Section boundaries via `###` (generating divider pages)
   * extracts chapter paths from backticked `.md` tokens
4. applies the fixed SCAFFOLD insertion rule per Part
5. de-duplicates while preserving first occurrence
6. writes `build/<book>-input.txt`

Pandoc consumes this list verbatim.
