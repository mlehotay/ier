# **IER-prerequisites.md**

## **Generated Prerequisite Index (Model / Guide for Generator)**

### **NON-CANONICAL · NON-CORPUS · NOT READER-FACING**

---

## **Status Declaration (Binding for Interpretation)**

This document is **NOT CANONICAL** and **NOT PART OF THE IER CORPUS**.

It is a **derived view** intended to be generated automatically from chapter-local YAML metadata
as described in `IER-dependencies.md`.

Accordingly:

* This document introduces **no ontological, criterial, epistemic, ethical, or diagnostic commitments**
* It carries **no interpretive or adjudicative authority**
* If any entry here conflicts with a corpus chapter’s YAML metadata, **the chapter metadata wins**
* This file should be **regenerated**, not hand-edited (except during early bootstrapping)

This file exists solely to support dependency auditing, generator development, and flat-order derivation.

---

## **0. Generator Output Contract (What This File Models)**

This file models what the Python generator should emit.

### **0.1 Identifier Conventions**

* Chapter IDs are file stems without extension (e.g., `IER-dynamics`, not `IER-dynamics.md`).
* Bundles are all-caps tokens (e.g., `FND`, `REG`, `SLK`, `GRD`).
* Gates are all-caps domain labels (e.g., `TOPOLOGY`, `HISTORY`).

### **0.2 Minimal Per-Chapter Fields**

For each chapter:

* `Tier`
* `Role`
* `Requires (Hard)`
* `Requires (Structural)`
* `Requires (Guardrails)`
* `Provides`
* `Gates`
* `Status`

### **0.3 Notes on Accuracy**

Prerequisites below are **best-effort placeholders** for bootstrapping.
They are not expected to be fully correct yet.

---

## **1. Bundles (Illustrative Shorthand Sets)**

> Bundles are expanded by the generator.
> Bundles listed here are a *model* for the generator’s output format.

* **FND** — `IER-specification`, `IER-theory`
* **REG** — `IER-dynamics`, `IER-participation`, `IER-attention` *(placeholder cluster)*
* **SLK** — `IER-slack`, `IER-saturation`
* **HIS** — `IER-sedimentation`, `IER-hysteresis`, `IER-calibration`
* **TOP** — `IER-choice`, `IER-futures`, `IER-topology`, `IER-choice-topology`
* **GRD** — `IER-disclaimers`, `IER-denials`, `IER-diagnostics`, `IER-correlates`, `IER-rubric`, `IER-shannon`, `IER-falsifiability`

---

## **2. Gate Chapters (Illustrative Gate Openers)**

> These are examples of how the generator might surface “gate openers”.

* **TOPOLOGY** — `IER-topology` *(primary)*, `IER-choice-topology` *(bridge)*
* **HISTORY** — `IER-sedimentation` *(primary)*, `IER-hysteresis` *(support)*
* **TRAVERSAL** — `IER-traversal` *(primary)*, `IER-navigation-control` *(support)*
* **DIAGNOSTIC-LIMITS** — `IER-diagnostics` *(primary)*, `IER-correlates` *(support)*

---

## **3. Foundations (Model Entries)**

### **IER-canon**
* **Tier:** T4 *(repo infrastructure)*
* **Role:** CONSTRAINT
* **Requires (Hard):** —
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** authority / governance rules *(procedural only)*
* **Gates:** —
* **Status:** canonical

### **IER-specification**
* **Tier:** T1
* **Role:** FOUNDATION
* **Requires (Hard):** —
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** experiential identity criteria; exclusion structure; normative closure *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-theory**
* **Tier:** T1
* **Role:** FOUNDATION
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** experiential identity claim articulation; exclusion logic *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-dynamics**
* **Tier:** T1
* **Role:** FOUNDATION
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`
* **Requires (Guardrails):** `GRD`
* **Provides:** regime behavior vocabulary; participation modulation *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-slack**
* **Tier:** T1
* **Role:** FOUNDATION
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`
* **Requires (Guardrails):** —
* **Provides:** slack as exclusion mechanism *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-saturation**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-slack`
* **Requires (Structural):** `IER-specification`
* **Requires (Guardrails):** —
* **Provides:** saturation vocabulary; local degrees-of-freedom exhaustion *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-ethics**
* **Tier:** T2
* **Role:** FOUNDATION
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`
* **Requires (Guardrails):** `GRD`
* **Provides:** ethical entailments forced by experiential identity *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-math**
* **Tier:** T2
* **Role:** FORMALISM
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`, `IER-dynamics`, `IER-slack`
* **Requires (Guardrails):** `IER-shannon`
* **Provides:** formal structure / notation for constraint and regimes *(placeholder)*
* **Gates:** —
* **Status:** canonical

---

## **4. Guardrails (Tier-3 Style Constraints; Book-Foundational Allowed)**

### **IER-disclaimers**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** global interpretive limits *(placeholder)*
* **Gates:** `DIAGNOSTIC-LIMITS` *(placeholder)*
* **Status:** canonical

### **IER-denials**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** explicit non-claims *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-diagnostics**
* **Tier:** T3 *(repo) / Book Part I allowed*
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`, `IER-dynamics`
* **Requires (Guardrails):** —
* **Provides:** limits on diagnostics / inference *(placeholder)*
* **Gates:** `DIAGNOSTIC-LIMITS`
* **Status:** canonical

### **IER-correlates**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-diagnostics`
* **Requires (Guardrails):** —
* **Provides:** why correlates cannot license experiential inference *(placeholder)*
* **Gates:** `DIAGNOSTIC-LIMITS`
* **Status:** canonical

### **IER-falsifiability**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`
* **Requires (Guardrails):** —
* **Provides:** how IER could fail without diagnostics *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-rubric**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-falsifiability`
* **Requires (Guardrails):** —
* **Provides:** evaluation rubric vs diagnostic convenience *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-shannon**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-specification`
* **Requires (Structural):** `IER-theory`
* **Requires (Guardrails):** —
* **Provides:** prohibition on Shannon information grounding experience *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-a-priori**
* **Tier:** T3
* **Role:** CONSTRAINT
* **Requires (Hard):** `IER-theory`
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** predictable objection patterns *(placeholder)*
* **Gates:** —
* **Status:** canonical

---

## **5. Topology / History / Traversal (Bridge Model Entries)**

### **IER-choice**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-agency`
* **Requires (Structural):** `IER-resolution`, `IER-dynamics`
* **Requires (Guardrails):** `GRD`
* **Provides:** choice space framing *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-futures**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-choice`
* **Requires (Structural):** `IER-dynamics`, `IER-constraint`
* **Requires (Guardrails):** `GRD`
* **Provides:** admissible futures framing *(placeholder)*
* **Gates:** `TOPOLOGY` *(placeholder; may instead be pre-topology)*
* **Status:** canonical

### **IER-topology**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-futures`
* **Requires (Structural):** `IER-sedimentation`, `IER-choice`
* **Requires (Guardrails):** `GRD`
* **Provides:** constraint geometry / admissible possibility structure *(placeholder)*
* **Gates:** `TOPOLOGY`
* **Status:** canonical

### **IER-choice-topology**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-choice`, `IER-topology`
* **Requires (Structural):** `IER-futures`
* **Requires (Guardrails):** `GRD`
* **Provides:** explicit unification of choice and topology *(placeholder)*
* **Gates:** `TOPOLOGY`
* **Status:** canonical

### **IER-traversal**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-choice-topology`
* **Requires (Structural):** `IER-navigation-control`
* **Requires (Guardrails):** `GRD`
* **Provides:** traversal as continuous choice / foreclosure *(placeholder)*
* **Gates:** `TRAVERSAL`
* **Status:** canonical

### **IER-navigation-control**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-choice-topology`
* **Requires (Structural):** `IER-dynamics`
* **Requires (Guardrails):** `GRD`
* **Provides:** navigation as modulation through topology *(placeholder)*
* **Gates:** `TRAVERSAL`
* **Status:** canonical

### **IER-sedimentation**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-dynamics`
* **Requires (Structural):** `IER-slack`, `IER-constraint`
* **Requires (Guardrails):** `GRD`
* **Provides:** structural history without storage *(placeholder)*
* **Gates:** `HISTORY`
* **Status:** canonical

### **IER-hysteresis**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-sedimentation`
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** path-dependent response machinery *(placeholder)*
* **Gates:** `HISTORY`
* **Status:** canonical

### **IER-calibration**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-hysteresis`
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** calibration drift framing *(placeholder)*
* **Gates:** `HISTORY`
* **Status:** canonical

---

## **6. Agency / Ethics-in-Practice (Model Entries)**

### **IER-resolution**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-dynamics`
* **Requires (Structural):** `IER-constraint`
* **Requires (Guardrails):** `GRD`
* **Provides:** resolution / admissible control vocabulary *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-ownership**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-resolution`
* **Requires (Structural):** `IER-theory`
* **Requires (Guardrails):** `GRD`
* **Provides:** ownership as non-externalizable intrinsic constraint *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-agency**
* **Tier:** T2
* **Role:** BRIDGE
* **Requires (Hard):** `IER-resolution`
* **Requires (Structural):** `IER-ownership`
* **Requires (Guardrails):** `GRD`
* **Provides:** agency as owned resolution *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-free-will**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-agency`
* **Requires (Structural):** `IER-choice-topology`
* **Requires (Guardrails):** `GRD`
* **Provides:** free will without indeterminism *(placeholder)*
* **Gates:** `TRAVERSAL`
* **Status:** canonical

### **IER-harm**
* **Tier:** T2
* **Role:** ELABORATION
* **Requires (Hard):** `IER-ethics`
* **Requires (Structural):** `IER-resolution`
* **Requires (Guardrails):** `GRD`
* **Provides:** moral harm as structural damage *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-gaslighting**
* **Tier:** T2
* **Role:** CASE
* **Requires (Hard):** `IER-harm`
* **Requires (Structural):** `IER-disagreement`, `IER-access`
* **Requires (Guardrails):** `GRD`
* **Provides:** analysis of imposed miscoordination *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-masochism**
* **Tier:** T2
* **Role:** CASE
* **Requires (Hard):** `IER-harm`
* **Requires (Structural):** `IER-pain`
* **Requires (Guardrails):** `GRD`
* **Provides:** chosen pain without harm under coherence *(placeholder)*
* **Gates:** —
* **Status:** canonical

---

## **7. Orientation / Meta (Model Entries)**

> These are included only to model generator output.
> They are not dependency-critical.

### **IER-intro**
* **Tier:** T4
* **Role:** ORIENTATION
* **Requires (Hard):** —
* **Requires (Structural):** `FND`
* **Requires (Guardrails):** —
* **Provides:** reader orientation *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-precis**
* **Tier:** T4
* **Role:** ORIENTATION
* **Requires (Hard):** —
* **Requires (Structural):** `FND`
* **Requires (Guardrails):** —
* **Provides:** compressed summary *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-glossary**
* **Tier:** T4
* **Role:** ORIENTATION
* **Requires (Hard):** —
* **Requires (Structural):** `FND`
* **Requires (Guardrails):** —
* **Provides:** canon-constrained terminology *(placeholder)*
* **Gates:** —
* **Status:** canonical

### **IER-index**
* **Tier:** T4
* **Role:** ORIENTATION
* **Requires (Hard):** —
* **Requires (Structural):** —
* **Requires (Guardrails):** —
* **Provides:** navigation index *(placeholder)*
* **Gates:** —
* **Status:** canonical

---

## **8. Model “Flat Order” Projection Snippet (Illustrative)**

> This section models how the generator might output a derived flat order.
> It is illustrative only.

1. `IER-canon`
2. `IER-specification`
3. `IER-theory`
4. `IER-disclaimers`
5. `IER-denials`
6. `IER-diagnostics` *(book-foundational allowed)*
7. `IER-correlates`
8. `IER-dynamics`
9. `IER-slack`
10. `IER-saturation`
11. `IER-ethics`
12. `IER-math`
13. `IER-resolution`
14. `IER-ownership`
15. `IER-agency`
16. `IER-choice`
17. `IER-futures`
18. `IER-sedimentation`
19. `IER-topology`
20. `IER-choice-topology`
21. `IER-navigation-control`
22. `IER-traversal`
23. `IER-free-will`
24. *(…)*

---

*End of document (model output).*
