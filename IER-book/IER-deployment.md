# **IER Deployment and Release Policy (v10.8.2)**

## **Status and Scope**

This document defines the **public deployment order and release discipline** for artifacts derived from the Informational Experiential Realism (IER) corpus.

It governs:

* sequencing of public releases
* priority anchoring
* immutability expectations after release
* framing obligations between related artifacts
* access and sale policy at a high level

It does **not**:

* define theory or ontology
* define canonical authority
* define book structure or contents
* define rendering, layout, or production rules

This document is subordinate to:

1. `IER-canon.md`
2. `IER-manifest.md`
3. `IER-book.md`

It is orthogonal to `IER-publishing.md`, which governs **rendering and production only**.

---

## **1. Deployment Principles**

### **1.1 Anchor Before Interface**

Exhaustive or reference-grade artifacts must be publicly released **before** derivative or interface-oriented presentations.

This prevents:

* authority inversion
* retroactive reframing
* “draft” misclassification of stable work

---

### **1.2 Public Release Implies Immutability**

Once an artifact is publicly released:

* its contents are treated as fixed
* silent replacement is prohibited
* corrections require a new edition or version

This applies equally to free and sold artifacts.

---

### **1.3 No Authority Transfer by Venue**

Publication venue does not confer theoretical authority.

Authority is fixed exclusively upstream by the IER canon and specification and is not altered by:

* commercial publication
* preprint hosting
* disciplinary repositories

---

## **2. Canonical Deployment Order (v10.8.2)**

### **Step 1 — Book Release (Edition 0.1)**

The first public release is:

* **IER Book, Edition 0.1**
* mechanically derived from IER v10.8.2
* published via **Amazon KDP**

This release:

* establishes a public timestamp
* anchors pagination and citation
* fixes the book as a reference artifact

Edition numbering reflects **physical or digital rendering**, not theoretical maturity.

The book may be sold. Sale status does not affect its role as a public disclosure.

---

### **Step 2 — Paper Release (Preprint)**

After the book is publicly available, a paper may be released on:

* **Open Science Framework**
* **PhilSci Archive**

The paper must be framed as:

* a **condensed exposition**
* a **summary interface**
* a **non-authoritative presentation**

The paper must not:

* introduce new theoretical commitments
* redefine or revise canonical claims
* claim priority over the book

---

## **3. Cross-Artifact Framing Rules**

### **3.1 Book → Paper**

The book may include a neutral statement such as:

> *A condensed paper presentation of this material is available on public preprint servers.*

No dependency or priority claim is required.

---

### **3.2 Paper → Book**

The paper must include a clear reference such as:

> *This paper presents a condensed exposition of Informational Experiential Realism.
> A complete technical monograph (IER Book, Edition 0.1) is publicly available.*

If multiple editions exist, the paper must specify **which edition** it summarizes.

---

### **3.3 OSF ↔ PhilSci Relationship**

When the paper is hosted on multiple preprint platforms:

* the same PDF must be used
* no substantive divergence is permitted
* metadata must cross-reference the primary preprint

Example PhilSci metadata:

> *This paper is also available as an OSF Preprint (DOI: …).*

---

## **4. Access and Sale Policy**

* The IER book may be sold or distributed freely.
* Preprint papers must be freely accessible.
* No artifact may imply that purchase is required to understand the core claims.

Pricing, distribution channels, and formats are **deployment choices**, not authority signals.

---

## **5. Relationship to Other Governance Documents**

* **Structure and contents** are defined by `IER-book.md`
* **Rendering and production** are governed by `IER-publishing.md`
* **Legal context** is described in `IER-legal.md`
* **Build mechanics and validation** are governed by the Master Build & Governance Plan (v10.8.2)

Deployment order does **not** modify:

* manifest membership
* canonical authority
* build validation rules

---

## **6. Version and Edition Discipline**

Each deployment cycle maps:

* **one IER version** → **one or more editions**
* deployment order resets only when the IER version changes

Edition 0.1 corresponds exclusively to **IER v10.8.2**.

---

## **Final Statement**

> Deployment governs **how the work enters public space**, not **what the work claims**.
> Authority flows from canon to book to paper—never in reverse.

---
