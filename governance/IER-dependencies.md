# **IER-dependencies.md**

## **Dependency Metadata Semantics and Validation Expectations**

### **NON-CANONICAL · NON-CORPUS · NOT READER-FACING**

---

## **Status Declaration (Binding for Interpretation)**

This document is **NOT CANONICAL** and **NOT PART OF THE IER CORPUS**.

It introduces **no ontological, criterial, epistemic, ethical, diagnostic, or interpretive commitments**.

It carries **no interpretive or adjudicative authority** and may not be cited to interpret or extend IER.

If any statement here conflicts with a corpus document (especially `IER-canon.md` or `IER-manifest.md`),
the corpus document takes precedence and this document is void at the point of conflict.

This document exists solely to define **how dependency metadata is meant to be declared and understood**
for internal coordination and build hygiene.

---

## **0. Purpose**

IER is a large, evolving corpus whose internal coherence benefits from **explicitly declared dependencies**.

This document defines:

1. the **meaning** of dependency metadata fields
2. the **minimal stable schema** for declaring dependency metadata in chapters
3. **validation expectations** (what “well-formed dependency metadata” means)
4. the status of **derived artifacts** produced from the metadata

This document does **not** define the build system, and it does **not** define generator interfaces.

- Build assembly/verification rules live in `IER-build.md`.
- Script interfaces live in `scripts/README.md`.

---

## **1. Single Source of Truth**

### **1.1 Authoritative Metadata Lives in Chapters**

Each corpus chapter may contain **YAML front matter** used **exclusively** for dependency metadata.

This YAML:

- is **author-facing**
- is **non-reader-facing**
- is the **single authoritative source** of dependency information
- must be **stripped from all publication outputs** (books, PDFs, EPUB, public HTML)

Any other dependency artifact (including generated YAML summaries or prerequisite tables)
is a **derived view** and is **non-authoritative**.

### **1.2 Non-Interpretive Constraint**

Dependency metadata:

- does **not** establish correctness of any claim
- does **not** introduce new theory
- does **not** alter corpus membership
- does **not** override manifest ordering or build policy

It is an internal coordination mechanism and a mechanical validation surface only.

---

## **2. Minimal YAML Schema (Stable Core)**

All dependency metadata lives under a single top-level key: `ier:`.

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
  gates:
    opens: []
    requires: []
  status: canonical
---
````

### **2.1 Field Meanings (Non-Interpretive)**

* `tier`

  * Repository tier label (e.g., `T1`–`T4`). Organizational only.
  * Does not determine book placement by itself.

* `role`

  * Dependency-management role used for expectations and hygiene.
  * Typical values (illustrative):
    `FOUNDATION`, `BRIDGE`, `ELABORATION`, `CONSTRAINT`, `FORMALISM`,
    `CASE`, `ORIENTATION`, `COMPARISON`.

* `requires.hard`

  * **Semantic prerequisites.**
  * If removed, the chapter’s claims become ill-defined.
  * Hard prerequisites define the irreducible partial order: if A hard-requires B, B must precede A
    in any valid flat ordering.

* `requires.structural`

  * **Machinery prerequisites** required for precision.
  * Prevents “teleporting concepts,” implicit imports, and metaphor creep.
  * Structural prerequisites may influence adjacency and grouping but are not the minimal order constraint.

* `requires.guardrails`

  * **Interpretive/usage constraints** that prevent misuse.
  * These are not “read-first narrative inputs.”
  * They are binding constraints on scope, inference, certification, or diagnostic misuse.

* `provides`

  * New vocabulary, structural objects, constraints, or conceptual moves introduced by the chapter.
  * “Provides” is descriptive; it does not automatically create reverse links or inferred dependency edges.

* `gates.opens`

  * Named conceptual domains this chapter explicitly opens for legitimate downstream use.

* `gates.requires`

  * Named domains this chapter requires to be opened upstream.
  * **Must be explicitly declared**. It is not inferable from `requires.*`.

* `status`

  * Corpus status marker (illustrative): `canonical`, `in-corpus-non-canonical`, `deprecated`, `draft`.

### **2.2 No Implicit Defaults Rule**

All lists must be explicit (`[]` if empty). No silent defaults should be inferred downstream.

---

## **3. Dependency Types (Do Not Conflate)**

### **3.1 Hard Prerequisites**

Hard prerequisites are **semantic necessities**.

* If A hard-depends on B, B must precede A in any valid flat ordering.
* Hard prerequisites define the core partial order of the corpus.

### **3.2 Structural Prerequisites**

Structural prerequisites provide machinery required to keep claims precise.

They exist to prevent:

* metaphor creep
* silent use of later-developed structures
* “teleporting concepts” (using tools that have not yet been introduced)

Structural prerequisites may influence recommended reading flow, but do not define minimal order.

### **3.3 Guardrail Prerequisites**

Guardrail prerequisites bind interpretation and prohibit misuse.

They apply especially to chapters involving:

* diagnostics
* correlates
* epistemic authority
* certification or inference limits
* scope boundaries that must not be crossed

Guardrails are constraints, not narrative dependencies.

---

## **4. Bundles (Optional Compression Mechanism)**

Bundles are named sets of dependency identifiers used as shorthand to reduce repetition.

* Bundles are expanded during dependency processing.
* Bundles carry no independent authority.
* Bundles are a convenience layer only.

Bundle definition sources and expansion behavior are implementation concerns
(see `scripts/README.md` and the generator script), but the semantic rule here is:

> A bundle token is equivalent to enumerating its members in the same dependency category.

---

## **5. Gate Discipline**

Some conceptual domains are treated as **gated**.

A gate enforces the rule:

> A chapter may not use domain X unless it depends (hard or structural) on a chapter that opens gate X.

Key points:

* Gates are mechanical discipline, not interpretation.
* `gates.requires` must be explicit; it is not inferred from dependencies.
* Gate checking is a hygiene mechanism: it prevents premature domain use.

---

## **6. Validation Expectations**

This section defines what *well-formed* dependency metadata means.
Specific tooling may enforce a subset, but these are the intended checks.

### **6.1 Identifier Validity**

All identifiers appearing in:

* `requires.hard`
* `requires.structural`
* `requires.guardrails`

must resolve to:

* a real chapter identifier (e.g., `IER-…` stems), or
* a defined bundle token (if bundles are enabled)

### **6.2 No Hard Cycles**

No chapter may directly or indirectly hard-depend on itself.

Hard cycles are always invalid.

### **6.3 Manifest Scope Consistency**

Dependency checking is performed over a declared scope (typically manifest-enumerated chapters).

Within the checked scope:

* references must resolve
* required files must exist on disk (for build hygiene)

### **6.4 Bridge Hygiene (Role-Dependent Expectations)**

If `role: BRIDGE`, the chapter is expected to:

* declare enough structural prerequisites to make the bridge real
* declare non-empty `provides` (unless explicitly a pure index)
* open or translate at least one conceptual domain when appropriate

This rule is about preventing “mystery bridges” (bridges that do not declare what they connect).

### **6.5 Guardrail Presence for High-Risk Domains**

Chapters that discuss diagnostics, correlates, certification, epistemic limits, or inference boundaries
are expected to include relevant guardrail prerequisites.

This is a discipline rule: it prevents authors from accidentally publishing “power without constraints.”

### **6.6 No Inference Rule (Downstream)**

Validation and projection tools must not:

* infer missing dependencies
* collapse categories (hard/structural/guardrails)
* rewrite chapter YAML
* “fix” metadata silently
* generate interpretive prose

---

## **7. Derived Artifacts**

Tools may produce derived artifacts, such as:

* a consolidated dependency YAML file (e.g., `build/dependencies.yml`)
* an audit-facing prerequisites report (e.g., `build/IER-prerequisites.md`)
* ordering checks and reports

All such artifacts are:

* **derived**
* **non-authoritative**
* used for inspection, validation, and build hygiene only

They must never be treated as a new source of truth.

---

## **8. Relationship to Build System**

* `IER-build.md` defines mechanical assembly and authoritative verification of booklists.
* Dependency tooling supports hygiene (generation + validation + order checking) but does not
  define corpus membership or canonical ordering.

Script invocation details, CLI flags, and exact output formats are specified in:

* `scripts/README.md`

---

## **9. Non-Goals**

This document does not:

* determine which dependencies are “correct” in a philosophical or interpretive sense
* establish reading order as pedagogy
* create authority beyond chapter text + canon + manifest
* define publication rendering, layout, or typography
* define deployment or release policy

It defines only a disciplined, explicit, mechanically checkable dependency metadata layer.

---
