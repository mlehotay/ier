# **IER Publishing and Rendering Rules**

## **Status and Scope**

This document defines **how IER publication artifacts are rendered and produced**.

It governs:

* pagination
* page geometry
* typography
* print and digital rendering constraints

It does **not**:

* define theory, ontology, or claims
* define canonical authority
* define corpus membership or ordering
* define deployment sequence

This document is **non-canonical** and subordinate to:

1. `IER-canon.md`
2. `IER-manifest.md`

If any conflict exists, **the canon and manifest take precedence**.

---

## **1. Publishing Target and Discipline**

IER is published as a **technical monograph**, not a pedagogical text, popular exposition, or narrative work.

Rendering must prioritize:

* density
* navigability
* citation stability
* long-term reference use

The design goal is **seriousness and restraint**, not approachability, aesthetic signaling, or reader motivation.

The production target is **print-first**.

---

## **2. Physical Format**

### **2.1 Trim Size**

Preferred trim size:

* **7 × 9 inches**

Acceptable alternates (for printer compatibility):

* 6.875 × 9.25 inches
* 7.5 × 9.25 inches

Trim size must support:

* high word density
* stable line length
* conventional academic shelving

All editions of a given IER version must use a single trim size.

---

### **2.2 Paper**

* White paper only
* No cream or tinted stock
* No aesthetic paper signaling

Binding assumptions:

* perfect-bound or equivalent POD-compatible binding
* no reliance on special paper weights or finishes

---

## **3. Typography**

Typography must signal **technical reference**, not instruction, narrative pacing, or pedagogy.

### **3.1 Font System**

The corpus book uses a **minimal, unified font system**:

* **Body text:** Libertinus Serif
* **Mathematics:** Libertinus Math
* **Headings:** Libertinus Serif (same family, heavier weight)
* **Monospace:** Inconsolata (for paths, identifiers, and inline literals only)

Multiple decorative font families are not permitted.

---

### **3.2 Body Text**

* Serif body font required
* High word density
* Tight leading (≈ 1.2×)
* Minimal paragraph spacing
* No excess vertical whitespace

Typography must not imply:

* tutorial pacing
* motivational emphasis
* rhetorical softening

---

### **3.3 Headings**

* Functional hierarchy only
* No decorative styling
* Visual distinction achieved primarily through weight and restrained size changes
* Clear differentiation between:

  * Parts
  * Chapters
  * Sections

Headings exist to support navigation, not reader encouragement.

---

### **3.4 Mathematics and Formal Elements**

* Display equations when non-trivial
* Inline math only for simple expressions
* Equation numbering **only** when cross-referenced
* No decorative math styling

Formal elements must signal **necessity**, not pedagogy.

---

### **3.5 Tables**

* Tables are first-class information carriers
* Reduced font size permitted
* Minimal ruling
* Continuation across page breaks permitted with clear labeling

---

### **3.6 Diagrams**

* Black-and-white only
* Vector-based
* Captioned, not narrated
* Diagrams must convey structure, not intuition

The corpus book should contain **very few diagrams**, and none that are decorative.

---

## **4. Pagination and Page-Break Rules**

Pagination follows **technical-monograph convention**.

### **4.1 Recto and Verso**

* **Recto** = right-hand page = odd-numbered page
* **Verso** = left-hand page = even-numbered page

Blank pages may be inserted to enforce recto alignment.
Blank pages must contain no content.

---

### **4.2 Major Structural Units (Recto Starts)**

The following units **must always begin on a recto page**:

* half-title pages (if present)
* full title page
* table of contents
* preface(s)
* introduction
* chapters
* appendices
* index

If the preceding content ends on a recto page, a blank verso page is inserted.

---

### **4.3 Chapters**

* Every chapter starts on a new page
* Every chapter start is recto-aligned
* Chapter pagination is never inferred from content length

Chapter boundaries are **structural**, not rhetorical.

---

### **4.4 Sections**

* Sections **do not** trigger page breaks
* Sections may appear in the PDF outline
* Sections must never force recto alignment

Sections are navigational aids only.

---

### **4.5 Front Matter and Main Matter**

Front matter includes:

* title pages
* copyright
* table of contents
* prefaces
* introduction

Decisions about:

* Roman vs Arabic numerals
* front-matter page numbering visibility

are **edition-level rendering choices**, not semantic ones.

---

### **4.6 Terminal Blank Pages**

Blank pages added to satisfy printer signature requirements:

* are permitted
* are not semantically meaningful
* are not modeled at the content or build level

Signature padding is a print concern, not a publishing rule.

---

## **5. Page Geometry**

### **5.1 Margins**

* Inner and outer margins must account for binding
* Inner margin must exceed outer margin
* Top and bottom margins must allow:

  * running heads (if used)
  * page numbers

---

### **5.2 Line Length**

Line length must remain within conventional academic bounds.

Typography and margins must jointly target:

* readability at high density
* stable visual rhythm
* minimal eye fatigue

---

### **5.3 Running Heads and Folios**

* Page numbers are required
* Placement must be consistent throughout the book
* Running heads (if used) reflect:

  * chapter title only

Section-level running heads are not permitted.

---

## **6. Density Targets**

Target density:

* **350–450 words per page**

Variance is permitted for:

* equations
* tables
* diagrams

Density is an intentional signal of reference-grade material.

---

## **7. Medium-Specific Constraints**

### **7.1 Print**

* Print edition establishes the canonical physical reference
* Pagination must be stable across printings of the same edition

---

### **7.2 PDF**

* PDF must match print pagination exactly
* PDF is the canonical digital reference
* PDF is used for citation and archival purposes

---

### **7.3 EPUB / HTML**

EPUB and HTML editions:

* must preserve:

  * chapter order
  * Part boundaries
  * authority signaling
* may relax:

  * exact pagination
  * recto/verso semantics

They must not introduce:

* reordering
* summaries
* adaptive reading paths
* pedagogical restructuring

---

## **8. What This Document Does Not Decide**

This document does not decide:

* corpus membership
* chapter order
* theoretical authority
* reader pedagogy
* deployment timing
* edition pricing or access policy

---

## **9. Change Discipline**

Permitted changes:

* layout fixes
* font substitutions of equivalent class
* printer compatibility adjustments

Not permitted:

* re-pagination of released editions
* typography that implies authority changes
* content-driven layout inference

---

## **10. Relationship to Other Documents**

* `IER-canon.md` — authority and governance
* `IER-manifest.md` — corpus membership and order
* `IER-corpus-book.md` — artifact identity
* `IER-build.md` — mechanical assembly
* `IER-deployment.md` — release sequencing
* `IER-legal.md` — legal context

---

### **End of `IER-publishing.md`**

---
