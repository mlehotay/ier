# IER Publishing and Rendering Rules (`IER-publishing.md`)

This document defines **how IER publication artifacts are rendered, formatted,
and physically instantiated**.

It governs:

* trim size and page geometry
* typography and density targets
* pagination and structural layout
* print and digital rendering discipline

It does **not**:

* define theory, ontology, or claims
* define canonical authority
* define corpus membership or ordering
* define build mechanics or selection logic
* define deployment order or release policy

---

## Status and Authority

This document is **non-canonical**.

* Canonical authority lives exclusively in `IER/`
* If this document conflicts with any canonical file, **the canon prevails**
* If this document conflicts with `IER-build.md` on mechanics,
  **`IER-build.md` prevails**

This document governs **rendering choices only**.

---

## Core Principle

> Rendering governs **how the work is physically encountered**,  
> not **what the work claims**.

Layout, typography, and geometry may reinforce epistemic role,
but they **never confer authority**.

---

## Artifact Classes and Rendering Intent

IER distinguishes publication artifacts by **epistemic role**.
Rendering rules exist to make those roles *visibly legible*.

---

### 1. Anchor Artifact — IER Corpus Book

The **IER Corpus Book** is the **sole deployment anchor**.

Rendering must support:

* dense technical reference
* non-linear consultation
* stable pagination and citation
* long-term archival use

Rendering characteristics:

* high information density
* minimal reader affordances
* reference-oriented typography
* constraint-first presentation

This artifact is **not optimized for cover-to-cover reading**.

---

### 2. Verbatim Interface Artifact — IER Foundations Compilation

The **IER Foundations Compilation** is a **verbatim interface artifact**
consisting exclusively of a strict subset of canonical material.

Rendering intent:

* technical monograph
* reference-grade density
* linear readability without pedagogy

Constraints:

* canonical text must appear **verbatim**
* ordering must match canonical ordering within scope
* no interpretive or explanatory material
* only a foreword and mandatory disclaimers may be added

This artifact:

* is **non-authoritative**
* does **not** establish citation priority
* does **not** replace the corpus book

---

### 3. Expository Interface Artifact — IER TLDR Book

The **IER TLDR Book** is a **reader-facing, expository interface artifact**.

Rendering may prioritize:

* linear reading flow
* reduced visual fatigue
* increased paragraph separation
* looser leading

Rendering must **not**:

* imply completeness
* mimic corpus-book density
* signal authority or exhaustiveness

---

### 4. Paper / Preprint Artifact

The **IER Paper** is a condensed interface artifact optimized for:

* academic circulation
* repository hosting
* indexing and citation
* rapid dissemination

Rendering must conform to:

* standard academic paper conventions
* single-column layout
* conservative typography

This artifact carries **no authority** independent of the corpus book.

---

## Trim Size and Page Geometry

Geometry is a **functional parameter**, not an authority signal.

---

### Corpus Book Geometry (Reference-Optimized)

Required:

* **8.5 × 11 inches**

This geometry supports:

* high information density
* two-column layouts
* wide tables and formal structures
* handbook-style reference use

Additional constraints:

* single- or two-column layout permitted
* column balance prioritized over narrative flow
* tables and formal elements may span columns
* all editions of a given version must share geometry

---

### Foundations Compilation Geometry

Required:

* **7 × 9 inches**

The Foundations Compilation is rendered as a **technical monograph**
optimized for readability without pedagogical signaling.

Layout requirements:

* single-column layout
* typography class shared with the corpus book
* high information density
* minimal reader affordances

This artifact must **not** visually resemble the TLDR book beyond trim size.

---

### TLDR Book Geometry (Reader-Oriented)

Required:

* **7 × 9 inches**

This geometry supports:

* stable line length
* reduced visual fatigue
* cover-to-cover reading

Rendering may prioritize:

* looser leading
* increased paragraph separation
* linear narrative flow

---

### Paper / Preprint Geometry

Required:

* **8.5 × 11 inches**

Optimized for:

* academic circulation
* digital distribution
* repository compatibility

---

## Typography System

IER uses a **minimal, unified font system**.

Default stack:

* **Body:** TeX Gyre Termes
* **Headings:** TeX Gyre Termes (weight only)
* **Sans:** TeX Gyre Heros (limited use)
* **Monospace:** Inconsolata
* **Math:** TeX Gyre Termes Math

Font substitutions are permitted **only within the same functional class**.

Typography must signal:

* technical seriousness
* reference-grade density
* constraint-first exposition

Typography must **not** signal:

* pedagogy
* narrative pacing
* motivational framing
* authority inflation or softening

---

## Density and Leading Targets

| Artifact                | Leading | Target Density (words/page) |
|-------------------------|---------|-----------------------------|
| Corpus Book             | ≈ 1.10  | 350–450                     |
| Foundations Compilation | ≈ 1.10  | 350–450                     |
| TLDR Book               | ≈ 1.20  | Reduced for comfort         |
| Paper / Preprint        | ≈ 1.15  | Academic norms              |

Variance is permitted for tables, equations, and diagrams.

---

## Pagination and Structural Rules

### Build-Generated Structural Pages

The build system may generate **purely structural Markdown files** under `build/`:

* Part divider pages
* Section divider pages
* Chapter break pages

These files:

* contain **no theory**
* exist only to enforce deterministic pagination
* may include raw LaTeX pagebreak directives
* are consumed by Pandoc like any other input file

They are part of the **publication layer**, not the corpus.

---

### Recto and Verso Discipline

* Recto = right-hand page = odd
* Verso = left-hand page = even

Recto starts are required for:

* title pages
* tables of contents
* prefaces and forewords
* Part boundaries
* chapters
* appendices
* indices

---

## Mathematics, Tables, and Diagrams

### Mathematics

* Display equations when non-trivial
* Number equations only when cross-referenced
* No decorative or pedagogical math styling

---

### Tables

* Tables are first-class information carriers
* Reduced font size permitted
* Minimal ruling preferred
* Page breaks permitted with continuity labels

---

### Diagrams

* Black-and-white only
* Vector-based
* Extremely limited
* Structural, not illustrative
* Captioned, not narrated

---

## Digital vs Print Discipline

### Canonical PDF

* Matches print pagination exactly
* Serves as the citation reference
* Uses a single, continuous pagination stream

---

### EPUB / HTML (Secondary)

Must preserve:

* chapter order
* Part boundaries
* non-authoritative status of interface artifacts

Must not introduce:

* reordering
* summaries
* adaptive reading paths
* authority cues

Pagination semantics may be relaxed.

---

## Change Discipline

Permitted between editions:

* layout fixes
* equivalent font substitutions
* geometry changes

Not permitted:

* post-release re-pagination within an edition
* typography that implies authority change
* density manipulation to disguise scope

---

## Relationship to Other Governance Documents

This document is orthogonal to:

* `IER-canon.md`
* `IER-manifest.md`
* `IER-build.md`
* `IER-deployment.md`
* `IER-legal.md`

Rendering decisions **never override** canonical authority,
corpus membership, build ordering, or deployment rules.

---

## Final Statement

> Rendering governs **how the work is physically encountered**,  
> not **what the work claims**.
