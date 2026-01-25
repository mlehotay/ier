# **IER-dependencies.md**

## **Dependency Metadata, Prerequisite Generation, and Flat Ordering Procedure**

### **NON-CANONICAL · NON-CORPUS · NOT READER-FACING**

---

## **Status Declaration (Binding for Interpretation)**

This document is **NOT CANONICAL** and **NOT PART OF THE IER CORPUS**.

Under the rule defined in `IER-manifest.md`:

> **Non-Corpus Material** — Any file not explicitly enumerated in the manifest is **non-corpus**, regardless of repository location or filename.

Accordingly:

* This document introduces **no ontological, criterial, epistemic, ethical, or diagnostic commitments**
* It carries **no interpretive or adjudicative authority**
* It may **not** be cited to interpret, defend, evaluate, or extend Informational Experiential Realism (IER)
* If any statement here conflicts with a corpus document—especially  
  `IER-canon.md`, `IER-specification.md`, `IER-theory.md`, `IER-dynamics.md`, `IER-ethics.md`, or `IER-topology.md`—  
  **the corpus document takes precedence and this document is void at the point of conflict**

This document exists **solely for internal author coordination**, dependency hygiene, and build-layer behavior.

---

## **0. Purpose**

IER is a large, evolving corpus whose internal coherence depends on **explicitly declared dependencies**.

This document defines the **procedure** for:

1. recording dependency metadata in a single authoritative location
2. classifying dependencies by type
3. validating the dependency graph mechanically
4. deriving secondary artifacts, including:
   * a prerequisites index (`IER-prerequisites.md`)
   * one or more flat orderings (repository or book)
   * chapter headers and footers for publication builds

This document does **not** determine which dependencies are correct.  
It defines **how dependencies are declared, checked, and projected**.

---

## **1. Single Source of Truth**

### **1.1 Authoritative Metadata Lives in Chapters**

Each corpus chapter may contain **YAML front matter** used **exclusively** for dependency metadata.

This YAML:

* is **author-facing**
* is **non-reader-facing**
* is the **single authoritative source** of dependency information
* must be **stripped from all publication outputs** (books, PDFs, EPUB, public HTML)

Any other dependency artifact (including `IER-prerequisites.md`) is a **derived view** and is **non-authoritative**.

### **1.2 Relationship to Canon Rule A3**

The use of YAML here is permitted under Canon Rule A3 as clarified:

* the YAML contains **only dependency metadata**
* it introduces **no theoretical, ethical, epistemic, or diagnostic authority**
* it is never visible to readers
* it is removed during all publication builds

YAML is treated as an **authorial coordination mechanism**, not publication metadata.

---

## **2. Minimal YAML Schema (Stable Core)**

All dependency metadata lives under a single top-level key:

```yaml
---
ier:
  tier: T2
  role: ELABORATION
  requires:
    hard: []
    structural: []
    guardrails: []
  provides: []
  gates: []
  status: canonical
---
````

### **2.1 Field Semantics (Non-Interpretive)**

* `tier`
  Repository tier label (T1–T4). Organizational only. Does not determine book placement.

* `role`
  Dependency-management role, used for validation expectations only.
  Typical values:
  `FOUNDATION`, `BRIDGE`, `ELABORATION`, `CONSTRAINT`, `FORMALISM`,
  `CASE`, `ORIENTATION`, `COMPARISON`.

* `requires.hard`
  Semantic prerequisites. If removed, the chapter’s claims become ill-defined.

* `requires.structural`
  Machinery prerequisites required for precision (prevents metaphor and implicit imports).

* `requires.guardrails`
  Binding misuse and interpretation constraints (e.g., diagnostics, correlates, scope limits).
  These are **not** “read-first” requirements.

* `provides`
  New vocabulary, structural objects, or conceptual moves introduced by this chapter.

* `gates`
  Conceptual domains this chapter introduces or is permitted to use (see §5).

* `status`
  Corpus status marker (`canonical`, `in-corpus-non-canonical`, `deprecated`, `draft`).

---

## **3. Dependency Types (Do Not Conflate)**

### **3.1 Hard Prerequisites**

Hard prerequisites are **semantic necessities**.

If chapter A hard-depends on chapter B, then B must precede A in any valid flat ordering.

Hard prerequisites define the **irreducible partial order** of the corpus.

---

### **3.2 Structural Prerequisites**

Structural prerequisites provide machinery or vocabulary required to keep claims precise.

They exist to prevent:

* metaphor creep
* silent use of later-developed structures
* “teleporting concepts”

Structural prerequisites may influence adjacency in flat orderings but do not define minimal order.

---

### **3.3 Guardrail Prerequisites**

Guardrail prerequisites bind interpretation and prohibit misuse.

They apply especially to chapters touching:

* diagnostics
* correlates
* epistemic authority
* inference or certification limits

Guardrails are **interpretive constraints**, not narrative inputs.

---

## **4. Bundles (Optional Compression Mechanism)**

Bundles are named sets of dependency identifiers used as shorthand.

Examples (illustrative only):

* `FND` — core identity foundations
* `REG` — regime and participation machinery
* `SLK` — slack and saturation machinery
* `HIS` — sedimentation and history machinery
* `TOP` — topology and admissible futures
* `GRD` — misuse-blocking constraints

Bundles are expanded during generation and carry no independent authority.

---

## **5. Gate Discipline**

Some conceptual domains are treated as **gated**.

A gate enforces the rule:

> A chapter may not use domain X unless it depends (hard or structural) on a chapter that opens gate X.

Typical gated domains include (illustrative):

* TOPOLOGY
* HISTORY
* TRAVERSAL / AGENCY-II
* DIAGNOSTIC-LIMITS

Gate enforcement is mechanical, not interpretive.

---

## **6. Validation Rules**

All dependency metadata must satisfy the following checks.

### **6.1 No Hard Cycles**

No chapter may directly or indirectly hard-depend on itself.

---

### **6.2 Bridge Exposure Rule**

Any chapter with `role: BRIDGE` must:

* declare at least two structural prerequisites
* declare a non-empty `provides` list (unless explicitly a pure index)
* declare at least one gate or translation role

---

### **6.3 Guardrail Propagation**

Any chapter that discusses diagnostics, correlates, epistemic limits, or certification must include the relevant guardrail prerequisites.

---

### **6.4 Identifier Validity**

All identifiers in `requires.*` must resolve to:

* an existing chapter stem (`IER-…`), or
* a defined bundle token

---

## **7. Generating `IER-prerequisites.md`**

`IER-prerequisites.md` is generated by parsing YAML metadata across chapters.

It is:

* non-authoritative
* regenerated automatically
* intended for auditing, navigation, and review

Conflicts are resolved in favor of chapter YAML.

---

## **8. Deriving Flat Orderings**

### **8.1 Flat Ordering as Projection**

A flat ordering is derived by:

1. expanding bundles
2. computing a topological sort over `requires.hard`
3. applying adjacency heuristics using `requires.structural`
4. applying presentation rules (e.g., meta material last)

Multiple valid flat orderings may exist.

---

### **8.2 Repository Tiers vs Book Parts**

Repository tiers (T1–T4) are organizational and need not correspond to book parts.

Book parts are **presentation projections**.

#### **Policy Note — Diagnostics in Book Foundations**

`IER-diagnostics.md` may appear in **Book Part I (Foundations)** even if it remains in a **T3 section** in the repository.

This reflects its role as a foundational interpretive constraint for readers, not a change in authority.

---

## **9. Chapter Headers and Footers (Publication Builds)**

### **9.1 Headers**

Headers may be generated from YAML metadata and may include:

* Tier
* Role
* Declared prerequisites
* Applicable guardrails

Headers are reader-facing projections and carry no independent authority.

---

### **9.2 Footers**

Footers may be generated from:

* dependency graph adjacency
* downstream dependents
* editorial reading paths

Footers are intentionally flexible and may include intermissions or transitions.

---

## **10. Build Requirement: Strip YAML**

All publication outputs must strip YAML front matter.

This requirement applies to:

* PDF
* EPUB
* HTML
* any public derivative

The build system (Make + Pandoc) must enforce this step.

---

## **11. Change Policy**

This procedure may evolve.

Permitted changes:

* adding optional metadata fields
* refining roles or gates
* refining validation rules
* adjusting projection policies

Prohibited changes:

* introducing theoretical claims here
* using this document to resolve corpus disputes
* treating derived artifacts as authoritative

---
