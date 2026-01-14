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

> **Nothing is authored or inferred at build time. Everything is assembled mechanically.**

The build system:

* never invents content
* never rewrites files
* never infers structure from headings or prose
* never decides chapter membership implicitly

All structure is **explicitly declared**.

---

## Build Inputs (Per Book)

Each book is defined by **exactly two inputs**, declared in the Makefile:

1. a **selection file**
2. a **SCAFFOLD directory**

No other inputs are consulted.

---

## Selection Files

A **selection file** is a Markdown document that explicitly lists included chapters.

Rules:

* only backticked `.md` paths are extracted
* order of appearance defines chapter order **within a Part**
* duplicate paths are ignored after first occurrence
* selection files are non-authoritative

Selection files may reference:

* canonical files in `IER/`
* non-canonical files in `pub/`

### Path normalization

During extraction:

* any backticked token ending in `.md` is treated as a candidate path
* bare canonical filenames beginning with `IER-` are mapped to `IER/IER-*.md`

Examples:

* `` `IER-theory.md` `` → `IER/IER-theory.md`
* `` `IER/IER-theory.md` `` → `IER/IER-theory.md`
* `` `pub/IER-paper.md` `` → `pub/IER-paper.md`

---

## Part Markers in Selection Files

Selection files may be divided into Parts using **Part marker headers**.

### Part marker syntax (authoritative)

A Part marker is:

> any **level-2 Markdown header** line (`## ...`) that contains the word **`part`** (case-insensitive).

Examples (all valid Part markers):

* `## PART I — CONTENT` *(Roman numerals are cosmetic)*
* `## **PART 1 — CONTENT**`
* `## Part 2`
* `## part — appendix`

Anything outside the header line is ignored. Formatting (bold, em-dash, etc.) is cosmetic.

### Part indexing rule for selection files (authoritative)

Parts are numbered mechanically as the selection file is scanned top-to-bottom:

* everything **before the first Part marker** is **Part 0**
* on each Part marker:

  * if the header contains a decimal digit immediately after the word `part` (e.g. `PART 2`), that digit is the new Part index
  * otherwise:

    * the first Part marker sets the Part index to **1**
    * each subsequent Part marker increments the Part index by **1**

This is intentionally simple and deterministic.

### Ordering semantics

* chapter order is the order of backticked paths **within each Part**
* Parts themselves are ordered by their numeric Part index

A selection file may omit Part markers; in that case all chapters are Part 0.

---

## SCAFFOLD Directories

A **SCAFFOLD directory** contains publication-only Markdown files:
front matter, Part headers, disclaimers, appendices, closers.

SCAFFOLD files:

* are non-canonical
* contain no theoretical content
* exist only to frame a specific book

They are ordered **lexicographically by filename** (your numeric prefixes then do the work).

---

## **SCAFFOLD Numbering and Placement Rule (Authoritative)**

SCAFFOLD filenames **must** begin with a two-digit numeric prefix:

```
NN-description.md
```

### Part assignment

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

---

### Fixed insertion rule per Part (authoritative)

For each Part `p`, files are emitted in this order:

| Slot range | Emitted order                            |
| ---------- | ---------------------------------------- |
| `p0–p5`    | SCAFFOLD files **before** chapters       |
| —          | chapters (from selection file, in order) |
| `p6–p9`    | SCAFFOLD files **after** chapters        |

This rule is **universal** and never varies.

Interleaving SCAFFOLD between individual chapters is not supported.

---

## Ordering Authority

All ordering is performed by:

```
scripts/extract_book_list.py
```

The script:

1. reads the SCAFFOLD directory
2. validates SCAFFOLD filenames (`NN-*.md`)
3. extracts chapter paths from the selection file (grouped by Part markers)
4. applies the fixed insertion rule per Part
5. de-duplicates while preserving first occurrence
6. writes `build/<book>-input.txt`

Pandoc consumes this list verbatim.

---

## Ground Truth

```
build/<book>-input.txt
```

This file is the **authoritative record** of what was built and in what order.

Debugging order is always:

1. booklist file
2. selection file
3. SCAFFOLD filenames
4. scripts
5. Makefile

Never Pandoc first.

---

## Rendering

Pandoc is invoked with:

* an explicit list of input files
* no stdin
* stable, declared rendering options

The paper (`pub/IER-paper.md`) is a special case:

* single authored file
* no selection or SCAFFOLD
* rendered directly

This is a rendering exception, not an authority exception.

---

## Validation and Fail-Fast Rules

The build must fail if:

* the selection file is missing
* the SCAFFOLD directory is missing
* any SCAFFOLD filename does not match `NN-*.md`
* no chapters are extracted (after parsing selection file)
* any referenced Markdown file does not exist
* the final booklist is empty

---

## Non-Goals

This system does **not** support:

* content-driven structure inference
* heuristic ordering
* filename-based corpus sorting
* adaptive reading paths
* publication semantics in canonical files

Those are intentionally excluded.

---

## Mental Model

* **IER/** — canon and canonical order
* **selection file** — what is included and how chapters are grouped into Parts
* **SCAFFOLD directory** — how each Part is framed (before/after)
* **scripts** — deterministic assembly
* **build/** — disposable output

---

## Status

This document defines the **current intended build contract**.

Any change to:

* Part marker rules
* SCAFFOLD numbering rules
* selection semantics
* script behavior
* Makefile inputs

must update this file.
