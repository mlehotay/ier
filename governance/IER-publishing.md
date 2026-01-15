# **IER Publishing and Rendering Rules**

## **Status and Scope**

This document defines **how IER publication artifacts are rendered, formatted, and physically instantiated**.

It governs:

* artifact-specific trim size and geometry
* typography and density targets
* pagination and structural layout
* print and digital rendering discipline

It does **not**:

* define theory, ontology, or claims
* define canonical authority
* define corpus membership or ordering
* define deployment sequence
* define reader pedagogy or motivation

This document is **non-canonical** and subordinate to:

1. `IER-canon.md`
2. `IER-manifest.md`

If any conflict exists, **the canon and manifest take precedence**.

---

## **1. Artifact Classes and Physical Intent**

IER distinguishes **publication artifacts by epistemic role**, not by aesthetics.

Each artifact class has a **distinct physical intent**, and rendering decisions must reinforce that intent.

### **1.1 Anchor Artifact — IER Corpus Book**

The **IER Corpus Book** is the **sole deployment anchor**.

Its physical form must support:

* dense technical reference
* non-linear consultation
* stable pagination and citation
* long-term archival use

It is **not** optimized for cover-to-cover reading.

---

### **1.2 Interface Artifacts**

Interface artifacts are **explicitly non-authoritative** and downstream of the corpus book.

They include:

* **IER TLDR Book**
* **IER Paper**

Their physical form may prioritize readability and linear flow, but **must not signal authority**.

---

## **2. Trim Size and Page Geometry**

### **2.1 Geometry as a Functional Choice**

Trim size is a **functional parameter**, not a genre or authority signal.

Changing trim size:

* does not alter epistemic status
* does not alter authority
* does not imply pedagogy

It determines **information density and layout affordances only**.

---

### **2.2 Corpus Book Geometry (Reference-Optimized)**

Permitted trim sizes:

* **Preferred:** **7 × 10 inches**
* **Alternate:** **8.5 × 11 inches**

All editions of a given corpus-book release **must use exactly one trim size**.

---

### **2.3 TLDR Book Geometry (Reader-Optimized)**

* **Required trim size:** **7 × 9 inches**

This geometry supports:

* stable line length
* reduced visual fatigue
* linear reading

---

### **2.4 Paper Geometry**

* **8.5 × 11 inches**

Optimized for academic circulation and digital distribution.

---

## **3. Typography System**

### **3.1 Font Discipline**

IER uses a **minimal, unified font system**.

Default system:

* **Body:** TeX Gyre Termes
* **Headings:** TeX Gyre Termes (weight only)
* **Sans:** TeX Gyre Heros (rare use)
* **Monospace:** Inconsolata
* **Math:** TeX Gyre Termes Math

Font substitutions are permitted **only within the same functional class**.

---

### **3.2 Typography Signals**

Typography must signal:

* technical seriousness
* reference-grade density
* constraint-first exposition

Typography must **not** signal:

* pedagogy
* motivation
* narrative pacing
* authority inflation or softening

---

## **4. Density and Leading Targets**

### **4.1 Corpus Book**

* Tight leading (≈ 1.10)
* Target density: **350–450 words per page**

---

### **4.2 TLDR Book**

* Looser leading (≈ 1.20)
* Density may be reduced for comfort, not pedagogy

---

### **4.3 Paper**

* Intermediate leading (≈ 1.15)

---

## **5. Pagination and Structural Rules**

### **5.1 Recto and Verso**

* Recto = right-hand page = odd
* Verso = left-hand page = even

Blank pages may be inserted to enforce recto alignment.
Blank pages contain no content.

---

### **5.2 Recto Starts (Book-Class Artifacts)**

The following must begin on a recto page:

* title pages
* table of contents
* prefaces
* introductions
* chapters
* appendices
* index

---

### **5.3 Chapters vs Sections**

* Chapters always start on a new recto page
* Sections never force page breaks
* Sections are navigational only

---

## **6. Tables, Mathematics, and Diagrams**

### **Mathematics**

* Display equations when non-trivial
* Number only when cross-referenced
* No decorative styling

### **Tables**

* First-class information carriers
* Minimal ruling
* Page breaks permitted with continuity labels

### **Diagrams**

* Black-and-white
* Vector-based
* Extremely limited
* Structural, not intuitive

---

## **7. Digital vs Print Discipline**

### **7.1 Canonical PDF**

* Matches print pagination exactly
* Serves as the citation reference

---

### **7.2 EPUB / HTML**

Must preserve:

* chapter order
* Part boundaries
* authority signaling

May relax pagination and recto semantics.

Must not introduce summaries or adaptive restructuring.

---

## **8. Change Discipline**

Permitted:

* layout fixes
* equivalent font substitutions
* geometry changes **between editions**

Not permitted:

* post-release re-pagination
* typography that implies authority change
* density manipulation to disguise scope

---

## **9. Relationship to Other Documents**

This document is orthogonal to:

* `IER-canon.md`
* `IER-manifest.md`
* `IER-build.md`
* `IER-deployment.md`
* `IER-legal.md`

Rendering decisions **never override** canonical or manifest-defined structure.

---

## **Final Statement**

> Rendering governs **how the work is physically encountered**,
> not **what the work claims**.

---
