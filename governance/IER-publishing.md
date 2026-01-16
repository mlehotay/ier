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

## Artifact Classes and Intent

IER distinguishes publication artifacts by **epistemic role**, not by aesthetics.

Each artifact class has a **distinct physical intent** that rendering decisions
must reinforce.

### 1. Anchor Artifact — IER Corpus Book

The **IER Corpus Book** is the **sole deployment anchor**.

Its physical form must support:

* dense technical reference
* non-linear consultation
* stable pagination and citation
* long-term archival use

It is **not** optimized for cover-to-cover reading.

---

### 2. Interface Artifacts

Interface artifacts are **explicitly non-authoritative** and downstream of the
corpus book.

They include:

* **IER TLDR Book**
* **IER Paper / Preprint**

Their physical form may prioritize readability and linear flow, but **must not
signal authority or completeness**.

---

## Trim Size and Page Geometry

### Geometry as a Functional Choice

Trim size is a **functional parameter**, not a genre or authority signal.

Changing trim size:

* does not alter epistemic status
* does not alter authority
* does not imply pedagogy

It determines **information density and layout affordances only**.

---

### Corpus Book Geometry (Reference-Optimized)

Default target:

* **7 × 10 inches**

Alternate (explicitly allowed):

* **8.5 × 11 inches**

All editions of a given corpus-book release **must use exactly one trim size**.

---

### TLDR Book Geometry (Reader-Oriented)

Required:

* **7 × 9 inches**

This geometry supports:

* stable line length
* reduced visual fatigue
* linear reading

---

### Paper Geometry

* **8.5 × 11 inches**

Optimized for academic circulation and digital distribution.

---

## Typography System

### Font Discipline

IER uses a **minimal, unified font system**.

Default stack:

* **Body:** TeX Gyre Termes
* **Headings:** TeX Gyre Termes (weight only)
* **Sans:** TeX Gyre Heros (limited use)
* **Monospace:** Inconsolata
* **Math:** TeX Gyre Termes Math

Font substitutions are permitted **only within the same functional class**.

---

### Typography Signals

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

### Corpus Book

* Tight leading (≈ **1.10**)
* Target density: **350–450 words per page**

---

### TLDR Book

* Looser leading (≈ **1.20**)
* Density may be reduced for comfort, **not** pedagogy

---

### Paper

* Intermediate leading (≈ **1.15**)

---

## Pagination and Structural Rules

### Build-Generated Structural Pages

The build system may generate **purely structural Markdown files** under `build/`:

* Part divider pages (`_part_pPP.md`)
* Section divider pages (`_section_pPP_sSS.md`)
* Chapter break pages (`_break_ch_NNNN.md`)

These files:

* contain **no theory**
* exist only to enforce deterministic pagination
* may include raw LaTeX pagebreak directives
  (`\cleardoublepage`, `\clearpage`)
* are consumed by Pandoc like any other input file

They are part of the **publication layer**, not the corpus.

---

### Recto and Verso

* Recto = right-hand page = odd
* Verso = left-hand page = even

Blank pages may be inserted to enforce recto alignment.
Blank pages contain no content.

---

### Recto Starts (Book-Class Artifacts)

The following must begin on a recto page:

* title pages
* tables of contents
* prefaces and introductions
* Part boundaries
* chapters
* appendices
* indices

Recto enforcement may be achieved by:

* class options (e.g. `openright`)
* build-generated `\cleardoublepage`
* authored scaffold files

---

### Chapters vs Sections

* Chapters always start on a new page
* For book-class artifacts, chapters start on a recto page
* Sections never force recto alignment
* Sections are navigational only

Build behavior:

* Part dividers may force recto starts
* Chapter break pages enforce new-page starts
* Section dividers enforce new-page starts without recto enforcement

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

Rendering decisions **never override** canonical authority, corpus membership,
or build ordering rules.

---

## Final Statement

> Rendering governs **how the work is physically encountered**,  
> not **what the work claims**.

---
