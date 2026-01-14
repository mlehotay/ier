# **IER Deployment and Release Policy (v10.8.2)**

## **Status and Scope**

This document defines the **public deployment order and release discipline** for artifacts derived from the **Informational Experiential Realism (IER)** corpus.

It governs:

* sequencing of public releases
* priority anchoring
* immutability expectations after release
* framing obligations between related artifacts
* high-level access and sale policy

It does **not**:

* define theory or ontology
* define canonical authority
* define corpus membership
* define rendering, layout, or production rules

This document is subordinate to:

1. **`IER-canon.md`**
2. **`IER-manifest.md`**

It is orthogonal to **`IER-publishing.md`**, which governs rendering and production only.

---

## **1. Deployment Principles**

### **1.1 Anchor Before Interface Artifacts**

Exhaustive or reference-grade artifacts must be publicly released **before any interface-layer artifacts**.

Interface-layer artifacts include (but are not limited to):

* gateway books
* summaries
* explanatory companions
* preprint papers

This principle prevents:

* authority inversion
* retroactive reframing
* misclassification of stable work as “draft”
* interface artifacts becoming de facto anchors

---

### **1.2 Public Release Implies Immutability**

Once an artifact is publicly released:

* its contents are treated as fixed
* silent replacement is prohibited
* corrections require a new edition or version

This applies equally to:

* sold artifacts
* freely distributed artifacts
* digital and print formats

---

### **1.3 No Authority Transfer by Venue**

Publication venue does not confer theoretical authority.

Authority is fixed exclusively upstream by the IER canon and specification and is not altered by:

* commercial publication
* preprint hosting
* academic repositories
* indexing or citation practices

---

## **2. Deployment-Relevant Artifact Classes**

IER deployment distinguishes **anchor artifacts** from **interface artifacts**.

---

### **2.1 Anchor Artifact**

There is **exactly one anchor artifact** per IER version.

#### **IER Corpus Book**

* Full technical monograph
* Mechanically assembled from manifest-defined chapters
* Contains Parts I–III as defined by the manifest
* Establishes:

  * public timestamp
  * stable pagination
  * citation reference
* Serves as the **deployment anchor**

No other artifact may precede or substitute for this role.

---

### **2.2 Interface Artifacts**

Interface artifacts are **downstream, non-authoritative** presentations of the corpus.

They exist to:

* support reader onboarding
* provide navigation and motivation
* aid translators and secondary explainers

They do **not**:

* introduce new commitments
* redefine canonical claims
* establish priority
* substitute for the anchor artifact

Current interface artifacts include:

* **IER TLDR Book**
* **IER Paper (Preprint)**

---

## **3. Canonical Deployment Order**

### **Step 1 — Corpus Book Release (Anchor)**

The first public release is:

> **IER Corpus Book, Edition 0.1**

This release:

* is mechanically derived from a specific IER version
* is assembled strictly according to `IER-manifest.md`
* establishes the public reference artifact
* anchors pagination and citation

The corpus book:

* may be sold or distributed freely
* may exist in multiple physical or digital editions
* remains the sole deployment anchor regardless of format

This step **must occur before any interface artifact is released**.

---

### **Step 2 — TLDR Book Release (Gateway Interface)**

After the corpus book is publicly available, the following may be released:

> **IER TLDR Book, Edition 0.1**

The TLDR book:

* is a gateway / interface artifact
* is explicitly **non-authoritative**
* is explanatory and reader-facing
* is intended for:

  * gateway readers
  * translators
  * serious non-anchor audiences

Framing requirements:

* must explicitly defer to the corpus book
* must identify the specific corpus book edition it explains
* must not present itself as a substitute or summary sufficient for citation

The TLDR book has **no priority over the paper** and no authority over any other artifact.

---

### **Step 3 — Paper Release (Condensed Interface)**

After the corpus book is publicly available, a paper may be released on:

* Open Science Framework
* PhilSci Archive
* comparable preprint repositories

The paper must be framed as:

* a condensed exposition
* a non-authoritative interface
* a downstream presentation

The paper:

* may be released before or after the TLDR book
* must reference the corpus book as the primary artifact
* must specify which corpus book edition it reflects

---

## **4. Cross-Artifact Framing Rules**

### **4.1 Corpus Book → Interface Artifacts**

The corpus book may include neutral statements such as:

> *Non-authoritative interface presentations of this material are available.*

No dependency, priority, or endorsement claim is implied.

---

### **4.2 Interface Artifacts → Corpus Book (Mandatory)**

All interface artifacts **must** include clear language equivalent to:

> *This artifact is a non-authoritative interface to Informational Experiential Realism.
> The complete technical monograph is the IER Corpus Book, Edition X.Y.*

This requirement applies to:

* TLDR book
* paper
* future summaries or companions

---

### **4.3 Interface Artifact ↔ Interface Artifact**

Interface artifacts:

* are siblings, not parents or children
* do not claim priority over one another
* may cross-reference, but must not imply dependency

All authority flows through the corpus book.

---

## **5. Access and Sale Policy**

* The corpus book may be sold or freely distributed.
* Interface artifacts may be sold or freely distributed.
* Preprint papers must be freely accessible.

No artifact may imply that purchase is required to understand the core theoretical claims.

Pricing and distribution are **deployment choices**, not authority signals.

---

## **6. Version and Edition Discipline**

Each deployment cycle maps:

* **one IER version** → **one anchor corpus book**
* **zero or more interface artifacts**

Edition numbering reflects:

* physical or digital rendering
* not theoretical maturity

A new IER version resets the deployment cycle.

---

## **7. Relationship to Other Governance Documents**

Deployment order does **not** modify:

* canonical authority (`IER-canon.md`)
* corpus membership or ordering (`IER-manifest.md`)
* rendering rules (`IER-publishing.md`)
* legal context (`IER-legal.md`)
* build mechanics or validation rules

Conflicts always resolve upstream in favor of the canon.

---

## **Final Statement**

> Deployment governs **how the work enters public space**, not **what the work claims**.
> Authority flows from **canon → corpus book → interface artifacts** — never in reverse.

---

*End of `IER-deployment.md`*

---
