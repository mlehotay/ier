# **IER Deployment and Release Policy (v10.8.3)**

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

Exhaustive or reference-grade artifacts must be publicly released **before any interface-layer artifacts that reinterpret or explain the theory**.

This principle prevents:

* authority inversion
* retroactive reframing
* interface artifacts becoming de facto anchors
* ambiguity about what constitutes the reference work

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

The corpus book is the **sole deployment anchor**.

No other artifact may precede, substitute for, or compete with this role.

---

### **2.2 Interface Artifacts**

Interface artifacts are **downstream, non-authoritative** artifacts derived from the IER corpus.

They exist to:

* support access, onboarding, and orientation
* expose or explain canonical material
* reduce barriers to engagement

They do **not**:

* introduce new commitments
* modify or restate canonical claims
* establish priority
* substitute for the anchor artifact

Interface artifacts fall into two sub-classes.

---

#### **2.2.1 Verbatim Interface Artifacts (Scope-Limited)**

These artifacts consist of **verbatim canonical text**, mechanically assembled
into a **partial compilation**.

They:

* include only a strict subset of the corpus
* introduce no new prose except framing and disclaimers
* preserve canonical ordering within their scope
* introduce no interpretation, pedagogy, or exemplars

A verbatim interface artifact:

* is **non-authoritative**
* does **not** replace the corpus book
* does **not** establish citation priority
* exists solely to improve access to foundational material

Example:

* **IER Foundations Compilation**

---

#### **2.2.2 Expository Interface Artifacts**

These artifacts **reinterpret or explain** canonical material for readers.

They may:

* paraphrase
* motivate
* illustrate
* condense
* reorganize for reader flow

They must explicitly defer to the corpus book.

Planned or potential examples include:

* **IER TLDR Book**
* **IER Paper / Preprint**

Readiness or existence of these artifacts is **not assumed** by this document.

---

## **3. Canonical Deployment Order**

### **Step 1 — Corpus Book Release (Anchor)**

The first authoritative public release for any IER version is:

> **IER Corpus Book, Edition X.Y**

This release:

* is mechanically derived from a specific IER version
* is assembled strictly according to `IER-manifest.md`
* establishes the public reference artifact
* anchors pagination and citation

This step establishes deployment completeness for that version.

---

### **Step 2 — Interface Artifact Releases (Optional, Downstream)**

After the corpus book is publicly available, interface artifacts may be released.

These may include:

* verbatim interface artifacts
* expository interface artifacts
* preprints or explanatory companions

Each must:

* explicitly defer to the corpus book
* identify the corpus book edition it reflects
* state its non-authoritative status

---

### **Exception: Early Verbatim Interface Release**

A verbatim interface artifact consisting exclusively of canonical text
may be released **prior to completion of the full corpus book**, provided that:

* it is explicitly labeled as **partial**
* it establishes no citation anchor
* it introduces no interpretation or explanatory material
* it defers explicitly to a future corpus book edition

This exception exists solely to improve access to foundational material and
does **not** alter anchor primacy.

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

---

### **4.3 Additional Requirement for Verbatim Interface Artifacts**

Verbatim interface artifacts must additionally state:

> *This artifact contains selected canonical chapters for access purposes.
> The complete authoritative reference is the IER Corpus Book, which
> supersedes this compilation in full.*

---

### **4.4 Interface Artifact ↔ Interface Artifact**

Interface artifacts:

* are siblings, not parents or children
* do not claim priority over one another
* may cross-reference without implying dependency

All authority flows through the corpus book.

---

## **5. Access and Sale Policy**

* The corpus book may be sold or freely distributed.
* Interface artifacts may be sold or freely distributed.
* Preprint papers, if released, must be freely accessible.

No artifact may imply that purchase is required to understand the core
theoretical claims.

Pricing and distribution are deployment choices, not authority signals.

---

## **6. Version and Edition Discipline**

Each deployment cycle maps:

* **one IER version** → **one anchor corpus book**
* **zero or more interface artifacts**

Edition numbering reflects physical or digital rendering,
not theoretical maturity.

A new IER version resets the deployment cycle.

---

## **7. Relationship to Other Governance Documents**

Deployment order does **not** modify:

* canonical authority (`IER-canon.md`)
* corpus membership or ordering (`IER-manifest.md`)
* rendering rules (`IER-publishing.md`)
* legal context (`IER-legal.md`)
* build mechanics or validation rules (`IER-build.md`)

Conflicts always resolve upstream in favor of the canon.

---

## **Final Statement**

> Deployment governs **how the work enters public space**,
> not **what the work claims**.

Authority flows from **canon → corpus book → interface artifacts** — never in reverse.
