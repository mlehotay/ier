# IER Build System  
**Mechanical Assembly and Verification Rules**

---

## Status and Scope

This document defines **how Informational Experiential Realism (IER) publication
artifacts are mechanically assembled and validated**.

It governs:

- how books are assembled from source files
- how ordering and structure are determined
- how structural pages are generated
- how build outputs are verified for correctness

It does **not**:

- define theory, ontology, or claims
- define canonical authority
- define corpus membership or ordering
- define rendering, typography, or physical format
- define deployment order or release policy

Those concerns live elsewhere.

---

## Authority and Precedence

This document is **non-canonical**.

Authority resolves in the following order:

1. **Canonical theory** (`IER/`)
2. **Corpus manifest and canon rules**
3. **This build document**
4. Rendering rules (`IER-publishing.md`)
5. Tooling implementation details

If this document conflicts with any canonical file,  
**the canon prevails**.

---

## Core Principle

> **Nothing is inferred at build time. Everything is assembled mechanically from explicit declarations.**

The build system:

- never invents content
- never rewrites source files
- never infers structure from prose
- never guesses chapter membership
- never alters canonical ordering

All structure must be **explicitly declared**.

---

## Build Inputs (Per Book)

Each book build is defined by **exactly two content inputs**:

1. a **selection file**
2. a **SCAFFOLD directory**

In addition, the build may supply **publication-layer configuration**
(e.g. Pandoc flags, TeX headers) that affect layout only and introduce no content.

---

## Selection Files

A **selection file** is a Markdown document that explicitly declares:

- chapter inclusion
- chapter order
- Part boundaries
- Section boundaries

Selection files are **non-authoritative** and contain **no theory**.

---

### Chapter Path Extraction (Authoritative)

During a linear scan of the selection file:

- only **backticked tokens** ending in `.md` are extracted
- multiple backticked tokens per line are permitted
- order of appearance defines chapter order
- duplicate paths are ignored after first occurrence

Selection files may reference:

- canonical files under `IER/`
- non-canonical files under `pub/`

---

### Path Normalization (Authoritative)

Extracted tokens are normalized as follows:

- tokens must end in `.md`
- bare canonical filenames beginning with `IER-` are mapped to `IER/IER-*.md`
- relative paths are preserved verbatim

Examples:

- `` `IER-theory.md` `` → `IER/IER-theory.md`
- `` `IER/IER-theory.md` `` → `IER/IER-theory.md`
- `` `pub/IER-paper.md` `` → `pub/IER-paper.md`

Unresolvable paths cause build failure.

---

## Structural Markers in Selection Files

Selection files may declare book structure using Markdown heading levels.

Only the **heading level** matters; formatting and wording are cosmetic.

### Header Levels (Authoritative)

- `# ...`   → Book marker (informational only)
- `## ...`  → **Part marker**
- `### ...` → **Section marker**

No keywords are required.

---

### Part Indexing Rule (Authoritative)

Parts are indexed mechanically during scanning:

- content before the first `##` belongs to **Part 0**
- the first `##` begins **Part 1**
- each subsequent `##` increments the Part index

Parts are ordered strictly by appearance.

---

### Section Rule (Authoritative)

Within a Part:

- each `###` increments the Section index
- a Section divider is generated and inserted **immediately before**
  the next chapter encountered
- multiple `###` markers in sequence produce multiple dividers
- a `###` before any `##` belongs to **Part 0**

---

## Generated Structural Pages

The build system may generate **purely structural Markdown files** under `build/`.

These files:

- contain **no theory**
- exist only to enforce deterministic structure and pagination
- may include raw LaTeX page-break directives
- are consumed by Pandoc like any other input file

---

### 1. Part Divider Pages

**Purpose**  
Ensure deterministic Part boundaries when no authored Part scaffold exists.

**Naming (Authoritative)**

```

build/_part_pPP.md

```

Where `PP` is the two-digit Part index.

**Contents (Authoritative)**

- begins with `\cleardoublepage`
- contains a single H1 with the Part title taken from the `##` marker

**Emission Policy (Authoritative)**

- generated for Parts `p ≥ 1`
- emitted **only if** no SCAFFOLD files exist for that Part in slots `p0–p5`

---

### 2. Section Divider Pages

**Purpose**  
Provide explicit, paginated Section boundaries.

**Naming (Authoritative)**

```

build/_section_pPP_sSS.md

```

**Contents (Authoritative)**

- begins with `\clearpage`
- contains a single H2 with the Section title from the `###` marker

Section dividers do **not** enforce recto alignment.

---

### 3. Chapter Break Pages

**Purpose**  
Ensure each chapter starts on a new page and flushes floats deterministically.

**Naming (Authoritative)**

```

build/_break_ch_NNNN.md

```

**Contents (Authoritative)**

- begins with `\clearpage`

A Chapter Break is inserted **immediately before every chapter path**.

---

## SCAFFOLD Directories

A **SCAFFOLD directory** contains publication-only Markdown files such as:

- front matter
- authored Part headers
- disclaimers
- appendices
- closers

SCAFFOLD files:

- are non-canonical
- contain no theoretical claims
- exist only to frame a specific book

Files are ordered **lexicographically by filename**.

---

### SCAFFOLD Naming and Placement (Authoritative)

Each filename **must** begin with a two-digit numeric prefix:

```

NN-description.md

```

Where:

```

part_index = floor(NN / 10)
slot       = NN % 10

```

---

### Fixed Insertion Rule per Part (Authoritative)

For each Part `p`, the emitted order is:

| Slot range | Emission point                  |
|----------:|---------------------------------|
| `p0–p5`   | Before Part content             |
| —         | Generated structure + chapters  |
| `p6–p9`   | After Part content              |

Interleaving SCAFFOLD files between individual chapters is **not supported**.

---

## Ordering Authority

All ordering and generation is performed by:

```

scripts/extract_book_list.py

```

The script:

1. validates the SCAFFOLD directory
2. scans the selection file linearly
3. tracks Part and Section markers
4. extracts chapter paths
5. generates structural pages
6. applies fixed SCAFFOLD insertion rules
7. de-duplicates paths
8. emits:
   - a Pandoc input list (`build/*-input.txt`)
   - a numbered diagnostic file

Pandoc consumes the emitted list **verbatim**.

---

## Verification

Verification is **authoritative**.

The verifier:

```

scripts/verify_book.py

```

- recomputes the expected emitted list
- requires **exact equality** with the produced list
- enforces all structural invariants
- enforces scoped authoring rules

A build that does not verify is considered **invalid**, even if Pandoc succeeds.

---

## Failure Conditions

The build or verification fails if:

- a referenced `.md` file does not exist
- a SCAFFOLD filename violates naming rules
- the emitted input list is empty
- ordering invariants are violated
- authoring rules are violated in canonical content

Failures are **hard errors**, not warnings.

---

## Summary

The IER build system is:

- deterministic
- explicit
- non-inferential
- content-preserving
- canon-safe

It exists to **assemble**, not to interpret.

---
