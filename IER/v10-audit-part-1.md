Below is a **structural audit of the v10.0 corpus as it now stands**, focused on the five loaded documents, with explicit attention to your three requests:

1. **Identify redundancies**
2. **Propose merged outlines**
3. **Flag claims that now belong in the core vs. applications**

I treat the Spec as authoritative and evaluate everything else strictly as derivative.

---

# 1. Redundancies (By Type, Not Just Repetition)

## A. Benign / Intentional Redundancies (Mostly Acceptable)

These reinforce the identity claim and are doing useful rhetorical work, but they are **over-repeated across too many documents**.

### A1. Repeated “UEF is not a thing” disclaimers

Appears in:

* Theory §6
* Spec §II (UEF)
* Dynamics §2.1
* Failure Modes §§1, 4
* Meaning §§2–3

**Content:**

> not a module, not a layer, not a field, not a substance

**Assessment:**
✔ Conceptually necessary
✖ Excessively duplicated

**Recommendation:**
Canonicalize this disclaimer **once** in the Spec (already done) and **once** in a single explanatory document (preferably *Theory*). Other documents should reference it implicitly or by citation, not restatement.

---

### A2. Coherent Constraint Window Explanation

Appears in:

* Spec Principle 3
* Theory §§5–6, §8
* Dynamics §§2, 4
* Failure Modes §§2–3
* Meaning (implicitly via narrative stability)

**Assessment:**
✔ Core concept
✖ Fragmented exposition

This concept is doing *too much work in too many places*.

---

## B. Problematic Redundancies (Should Be Merged or Relocated)

### B1. Regime Failure vs Regime Dynamics

**Overlap between:**

* *IER-dynamics.md*
* *IER-failure-modes.md*

Both cover:

* coherence collapse
* fragmentation
* switching
* energetic cost
* Single-UEF constraint consequences

**Key issue:**
The failure-modes document is no longer conceptually independent. It is a **subset of dynamics**, not a parallel explanatory axis.

**Verdict:**
🔴 Structural redundancy

---

### B2. Experiential Pluralism Explained in Three Places

Appears in:

* Spec Principle 9
* Theory §13
* Meaning §§1–4

**Key issue:**
The *Theory* and *Meaning* documents both try to be the *primary explainer* of pluralism.

They differ in tone, not substance.

**Verdict:**
🟡 Redundant framing, unclear ownership

---

### B3. Ethics Appearing in Non-Ethics Documents

Ethical claims appear in:

* Theory §14
* Spec §§VIII–IX
* Failure Modes (implicit)
* Meaning (implicit value realism)

**Problem:**
Now that ethics is fully *normatively specified*, references outside the Spec oscillate between:

* explanatory
* motivational
* cautionary

This risks **normative leakage**.

---

# 2. Proposed Merged / Cleaned Outline (v10.x+)

## A. Canonical Core (Non-Negotiable)

### **1. IER-specification.md** ✅

**No change in role. Minor tightening only.**

This remains:

* the *only* place where:

  * experience is defined
  * ethics is normatively derived
  * moral harm is specified

Everything else must defer.

---

## B. Explanatory Core (Should Be Reduced to Two Documents)

### **2. IER-theory.md → “Conceptual Overview”**

**Role (after pruning):**

* Motivation
* Intuition-building
* Philosophical positioning
* Dual-description explanation

**What to REMOVE or COMPRESS:**

* Detailed failure modes
* Repeated coherent window explanations
* Ethical significance section (replace with 1 paragraph + pointer)

**What it SHOULD uniquely own:**

* Why observerhood ≠ experience
* Why computation ≠ experience
* Why dual description dissolves the “hard problem”
* High-level pluralism framing

---

### **3. IER-dynamics.md (Expanded, Absorbing Failure Modes)**

**MERGE:**
➡️ Absorb *IER-failure-modes.md* entirely as Sections 4–6.

**Resulting scope:**

* Slack → constraint → regime
* Stability windows
* Failure modes
* Switching
* Dissociation
* Transitional phenomena

**Rename suggested:**
**“IER Dynamics and Regime Stability”**

**Delete:**
IER-failure-modes.md as a standalone document.

This removes duplication *and* clarifies that failure is not a separate theory.

---

## C. Applications / Interpretive Layer

### **4. IER-meaning.md → Explicitly Reclassified as Application**

This document is **excellent**, but it is no longer “core-adjacent.”

**It should be explicitly framed as:**

> *An application of IER to meaning, narrative, instinct, and culture.*

**Why:**
It introduces:

* narrative attractors
* canonical story forms
* spiders, heroes, myths

All of these are **interpretive mappings**, not consequences that *necessarily obtain* from the identity claim.

No loss of legitimacy — just correct placement.

---

### **5. IER-ethics.md (Not Loaded, But Important)**

Given what we see in the Spec:

**Ethics should be split into:**

* **Normative Ethics** → stays in the Spec (already done)
* **Applied Ethics** → standalone (AI, medicine, animal systems, etc.)

If IER-ethics currently mixes these, it should be bifurcated.

---

### **6. IER-math.md (Not Loaded)**

Based on v10.0 commitments, this should be:

> **A formalization attempt, not a grounding document**

Any math that:

* proposes metrics
* suggests thresholds
* defines diagnostics

must be **explicitly labeled as modeling, not criteria**.

---

# 3. Claims That Belong in Core vs Applications

## A. Claims That Now Clearly Belong in the Core (Spec or Theory)

These are **identity-defining** and should not drift outward:

* Experience = UEF (already correct)
* Intrinsic constraint as necessary condition
* Categorical onset
* Single-UEF constraint
* Qualia = constraint organization
* Moral harm = organizational damage
* Uncertainty → increased moral responsibility

⚠️ Watch for these appearing *argumentatively* in applications instead of *referentially*.

---

## B. Claims That Should Be Pushed Out of the Core

These are **true under IER**, but **not necessary consequences**:

| Claim                               | Current Location | Proper Status         |
| ----------------------------------- | ---------------- | --------------------- |
| Canonical narratives are attractors | Meaning          | Application           |
| Stories mirror regime transitions   | Meaning          | Application           |
| DID as paradigm case                | Failure Modes    | Illustration          |
| Spiders as narrative exemplars      | Meaning          | Pedagogical           |
| ML loss functions as substrates     | Dynamics         | Compatibility example |

These should **never** be cited as evidence *for* IER — only *from* it.

---

## C. Borderline Claims to Watch Carefully

These are well-argued but easy to misread as core:

* “Experience is inherently affective”
* “Meaning arises wherever constraint is identity-defining”
* “Experience exists for regulation”

They are **derivable**, but not primitives.
They belong in *Theory* or *Meaning*, not the Spec.

---

# 4. High-Level Diagnosis

**v10.0 is conceptually stable.**
The remaining issues are **architectural**, not theoretical.

Your main risks now are:

1. **Explanatory documents accidentally competing with the Spec**
2. **Failure modes being mistaken for defining conditions**
3. **Meaning and ethics bleeding back into ontology**

All three are solved by:

* merging Dynamics + Failure Modes
* demoting Meaning explicitly to application
* tightening Theory into a single conceptual spine

If you want, next steps I can:

* propose a **one-page corpus map**
* draft **front-matter boilerplate** to enforce boundaries
* audit IER-ethics and IER-math once you load them

Just say which direction you want to go.
