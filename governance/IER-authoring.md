# **IER Authoring and Dependency Declaration Guide**

**(`IER-authoring.md`)**

**Status:** Governance / process specification
**Audience:** IER authors and maintainers
**Scope:** Authoring workflow, YAML metadata, dependency declaration, and build hygiene
**Non-scope:** Theory, ontology, reader-facing structure, pedagogy

---

## 0. Status and Authority

This document governs **authoring process and metadata discipline** for chapters in the IER corpus.

It:

* introduces **no theoretical claims**
* introduces **no interpretive authority**
* does **not** define corpus membership
* does **not** override the manifest, canon, or build rules

Where conflicts arise, authority resolves upstream in the following order:

1. Canonical theory (`IER/`)
2. Canon and manifest rules
3. Build rules (`IER-build.md`)
4. Dependency semantics (`IER-dependencies.md`)
5. This document

This document constrains **how authors write and annotate**, not **what is true**.

---

## 1. Corpus Chapter Authoring

### 1.1 File Location

Canonical chapters must be stored under:

```
IER/
```

Each chapter must be a standalone Markdown file with a stable filename suitable for manifest enumeration.

---

### 1.2 Prose Constraints

Corpus prose:

* must be written as **self-standing theoretical material**
* must not rely on reader guidance, pedagogy, or narrative scaffolding
* must not reference future chapters as explanatory devices
* must not soften or defer commitments due to missing prerequisites

All explanatory or reader-facing accommodation belongs to **interface artifacts**, not the corpus.

---

## 2. Mandatory YAML Front Matter

### 2.1 Requirement

Every corpus chapter **must** include a YAML front matter block at the top of the file.

This YAML is:

* author-facing only
* non-reader-facing
* stripped from all publication outputs
* the **single authoritative source** of dependency metadata

---

### 2.2 Minimal Required Schema

All chapters must include the following structure, with all lists explicit:

```yaml
---
ier:
  tier: T?
  role: ?
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
```

No fields may be omitted.
Empty lists must be written as `[]`.

---

## 3. Metadata Field Semantics (Binding)

### 3.1 `tier`

* Organizational classification only
* Does not determine ordering, authority, or inclusion
* May be revised freely without structural consequence

---

### 3.2 `role`

Declares the **dependency-management role** of the chapter.

The role expresses *how this chapter participates in the dependency graph*, not its importance.

Roles do not affect build ordering directly but are used for validation expectations.

---

### 3.3 `requires`

Dependency declarations are **non-interpretive constraints**.

#### 3.3.1 `requires.hard`

Hard prerequisites are **semantic necessities**.

If chapter A hard-requires chapter B:

* B must precede A in any valid flat ordering
* removing B renders A’s claims ill-defined

Hard prerequisites define the **irreducible partial order** of the corpus.

Hard cycles are invalid.

---

#### 3.3.2 `requires.structural`

Structural prerequisites are **machinery requirements**.

They exist to prevent:

* implicit imports
* teleporting concepts
* metaphor creep
* silent reliance on undeclared structure

Structural prerequisites may influence grouping and adjacency but do not define the minimal order.

---

#### 3.3.3 `requires.guardrails`

Guardrail prerequisites bind **interpretive and usage constraints**.

They are required when a chapter touches:

* diagnostics or detection
* epistemic authority
* certification or inference
* scope boundaries that must not be crossed

Guardrails constrain misuse; they are not narrative prerequisites.

---

### 3.4 `provides`

Declares what **new conceptual resources** this chapter introduces.

Examples include:

* new terminology
* new constraints
* new distinctions
* new exclusion results
* new structural objects

`provides` is **descriptive only**.
It does not create inferred reverse dependencies.

---

### 3.5 `gates`

Gates are used to control access to **high-risk conceptual domains**.

#### 3.5.1 `gates.opens`

Declares that this chapter explicitly authorizes downstream use of a named domain.

#### 3.5.2 `gates.requires`

Declares that this chapter may not be used unless the named domain has been opened upstream.

Gate requirements must be explicit and are never inferred.

---

### 3.6 `status`

Marks corpus status (e.g. `canonical`, `draft`, `deprecated`).

This field does not affect build mechanics unless enforced by tooling.

---

## 4. Dependency Declaration Discipline

### 4.1 Explicitness Rule

All dependencies must be **explicitly declared**.

Tooling must not:

* infer missing dependencies
* rewrite YAML
* collapse dependency categories
* silently “fix” metadata

---

### 4.2 Incremental Declaration

It is permitted and expected that:

* early metadata is incomplete
* dependency checking initially fails
* hard prerequisites are added over time

Build failures are treated as **diagnostic signals**, not errors in intent.

---

### 4.3 Correction Responsibility

When dependency verification fails, the author must resolve the failure by:

* declaring the appropriate dependency, or
* revising prose to remove illicit reliance

Reordering the manifest solely to satisfy tooling is prohibited unless the theoretical dependency is real.

---

## 5. Bundles

### 5.1 Purpose

Bundles are a **compression mechanism** for repeated dependency sets.

They:

* reduce duplication
* improve readability
* carry no independent authority

---

### 5.2 Usage Rules

* Bundles must expand mechanically to their members
* A bundle token is equivalent to enumerating its contents inline
* Bundles must not be used to obscure dependency structure

Bundle definitions live outside chapters (e.g. `assets/bundles.yml`) and are expanded during dependency processing.

---

## 6. Build Interaction

### 6.1 Separation of Concerns

* YAML defines dependency constraints
* The manifest defines inclusion and ordering intent
* The build system verifies consistency

No layer overrides another.

---

### 6.2 Failure Interpretation

A build that fails dependency verification is considered **structurally invalid**, even if rendering succeeds.

Such failures indicate:

* undeclared prerequisites
* illicit forward use
* missing guardrails
* gate violations

They are resolved by authoring changes, not by weakening verification.

---

## 7. Publication Boundary

YAML metadata:

* must never appear in publication outputs
* must never be exposed to readers
* must never be cited or paraphrased

It is an internal coordination and validation surface only.

---

## 8. Non-Goals

This document does not:

* prescribe prose style
* define pedagogical order
* resolve theoretical disputes
* define reader-facing structure
* relax or reinterpret dependency semantics

---

## 9. Final Constraint

IER authoring discipline exists to ensure that:

* theoretical commitments are explicit
* dependencies are honest
* ordering is justified
* misuse is constrained
* authority does not leak across layers

If dependency metadata is uncomfortable to write,
the discomfort reflects **undeclared structure in the prose**, not an excess of governance.

---
