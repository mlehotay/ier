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
2. a **scaffolding directory**

No other inputs are consulted.

---

## Selection Files

A **selection file** is a Markdown document that explicitly lists included chapters.

Rules:

* only backticked `.md` paths are extracted
* order of appearance defines chapter order
* duplicate paths are ignored after first occurrence
* selection files are non-authoritative

Selection files may reference:

* canonical files in `IER/`
* non-canonical files in `pub/`

### Corpus selection (special case)

For the corpus book:

* the selection file is `IER/IER-manifest.md`
* only **Parts I–III** are included
* only canonical chapter files (`IER/IER-*.md`) are extracted
* chapter order is defined by the manifest itself

---

## Scaffolding Directories

A **scaffolding directory** contains publication-only Markdown files:
front matter, Part headers, disclaimers, appendices, closers.

Scaffolding files:

* are non-canonical
* contain no theoretical content
* exist only to frame a specific book

They are ordered **lexicographically by filename**.

---

## **Scaffold Numbering and Placement Rule (Authoritative)**

Scaffolding filenames **must** begin with a two-digit numeric prefix:

```
NN-description.md
```

### Part assignment

```
part_index = floor(NN / 10)
```

Examples:

* `00-*.md` → Part 0
* `10-*.md` → Part 1
* `20-*.md` → Part 2

Part names in headings are cosmetic.
Only numeric position matters.

---

### Fixed insertion rule per Part

For each Part `p`, files are emitted in this order:

| Filename range | Emitted order                            |
| -------------- | ---------------------------------------- |
| `p0–p5`        | scaffolding **before** chapters          |
| —              | chapters (from selection file, in order) |
| `p6–p9`        | scaffolding **after** chapters           |

This rule is **universal** and never varies.

Interleaving scaffolding between individual chapters is not supported.

---

## Ordering Authority

All ordering is performed by:

```
scripts/extract_book_list.py
```

The script:

1. reads the scaffolding directory
2. sorts scaffolding files lexicographically
3. extracts chapter paths from the selection file
4. applies the fixed insertion rule above
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
3. scaffolding filenames
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
* no selection or scaffolding
* rendered directly

This is a rendering exception, not an authority exception.

---

## Validation and Fail-Fast Rules

The build must fail if:

* the selection file is missing
* the scaffolding directory is missing
* no chapters are extracted
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
* **selection file** — what is included
* **scaffolding directory** — how it is framed
* **scripts** — deterministic assembly
* **build/** — disposable output

---

## Status

This document defines the **current intended build contract**.

Any change to:

* scaffold numbering rules
* selection semantics
* script behavior
* Makefile inputs

must update this file.

---
