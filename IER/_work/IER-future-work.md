# **IER — Future Work and Articulation Notes**

## **Status**

This document is **non-canonical** and **non-authoritative**.

It records **articulation, navigation, clarity, and maintenance tasks** for the IER corpus after **normative closure (v10.8)** and **corpus completion (v10.8.1)**.

It introduces **no ontological, criterial, epistemic, or ethical claims**
and does **not** signal intended changes to the IER framework.

---

## **Purpose**

`IER-future-work.md` exists to track:

* clarity improvements
* reader-alignment hardening
* navigation and indexing tasks
* misuse-containment checks
* consistency and drift audits
* optional explanatory expansions already identified during v10.8.1 planning

It is **not** a roadmap for new theory development.

---

## **Navigation & Orientation**

* [x] `IER-glossary.md`  
  Canon-constrained definitions of recurring terms; explicit rejection of folk and adjacent-theory meanings.

* [x] `IER-index.md`  
  Navigational index mapping concepts → documents → sections; no summaries or reinterpretation.

* [ ] Cross-link glossary terms to first canonical usage.
* [ ] Audit README reading paths for redundancy, dead ends, or circular navigation.
* [ ] Add lightweight “where to go next” pointers for common reader profiles (philosophy, neuroscience, ethics, AI).

---

## **Articulation & Clarity Passes (Non-Normative)**

These items involve **tightening or making explicit what is already implied**, without adding commitments.

* [ ] Add an explicit **anti-isomorphism clarification** in `IER-theory.md`:
  organizational or functional similarity does not imply intrinsic ownership or experience.

* [ ] Add an explicit **no-nested-subjects / no-mini-fields sentence** near the dominance discussion in `IER-dynamics.md`
  (targeting IIT-style intuitions).

* [ ] Add a clarification that **temporal continuity is not a timescale threshold**
  (non-zero duration is regime-formal, not calibratable).

* [ ] Add an explicit denial that **autopoiesis / operational closure** is
  either necessary or sufficient for experience (best placed in `IER-denials.md` or `IER-FAQ.md`).

* [ ] Add a short clarification that **multiple realizability ≠ implementation-independence**
  (anti-computationalist slide) in `IER-theory.md`.

* [ ] Remove or revise metaphors that invite representational, functionalist, or epistemic readings.

---

## **Misuse & Misreading Containment**

* [ ] Scan Tier-2 documents for reintroduced diagnostic, threshold, or proxy language.
* [ ] Ensure all documents using informational language include the required disclaimer.
* [ ] Audit phenomenology-adjacent documents for any implication of graded subjecthood.
* [ ] Re-reinforce **experiential structure ≠ epistemic authority** in:
  meaning, insight, agency, and narrative-adjacent texts.

---

## **History & Lineage Guardrails**

* [ ] Add an explicit **misreading guardrail box** near the top of `IER-history.md` clarifying:
  * not a claim of influence
  * not a refutation project
  * not a criterial mapping
  * not a moral evaluation of prior authors
* [ ] Reaffirm that **only `IER-specification.md` is criterial and normative**.

---

## **Optional Standalone Explanatory Articles**

These were identified during v10.8.1 planning but are **explicitly optional**.
They add **audience management and clarification**, not new theory.

* [ ] **`IER-regimes.md`**  
  Deepens the regime vs. modeling-abstraction distinction already handled in:
  * `IER-theory.md`
  * `IER-dynamics.md`
  * IER-critics (regimes objection)

* [ ] **`IER-and-engineering.md`**  
  Clarifies what IER does and does not imply for:
  * AI research
  * systems engineering
  * “build-and-test” intuitions  
  (Audience-facing governance and expectation management only.)

Neither document is required for corpus completeness.

---

## **Consistency & Drift Audits**

* [ ] Periodic comparison of Tier-2 language against current Tier-1 wording.
* [ ] Ethics language audit for alignment with v10.8 normative closure and the experiential-harm corollary.
* [ ] Reader-alignment tone audit (no adversarial framing, no blame).
* [ ] Verify that all non-canonical documents are clearly labeled and not cited as authority.

---

## **Tier Assignments**

### A) `IER-math.md` is labeled **[T1]** in the manifest

But `IER-math.md` describes itself as a **supportive, non-normative support document** (i.e., not foundational authority). That reads more like **T2** (canon-constrained analysis / support) or possibly **T4** (support/pedagogy), depending on how “math support” is meant to function in your tier scheme.  

**Recommendation:** change manifest tier for `IER-math.md` to **[T2]** (still keep it in Part I if you want it inside the “closed argument” bundle, but don’t mark it T1).

### B) `IER-ethics.md` is labeled **[T1]** in the manifest

But `IER-ethics.md` explicitly says it is **NON-NORMATIVE** and that “all ethical authority resides exclusively in the Specification.” That makes it look like a **T2 elaboration/application** document, even if it’s extremely central.  

**Recommendation:** make `IER-ethics.md` **[T2]** and keep it in Part I only if you still want Part I to include “core consequences elaboration.” If you want Part I to be *only* the definitional/closure spine, you could move ethics to Part II under “Agency, Choice, and Ethics in Practice” (but that’s a structural taste choice; tier-wise it reads T2 either way).

---

## **Tooling / Meta (Optional)**

* [ ] Machine-readable manifest or index for tooling/search.
* [ ] Lightweight linting rules for:
  * agentive language in definitions
  * missing disclaimers
  * forbidden diagnostic phrasing
* [ ] Automated check for accidental Tier violations.

---

## **Non-Goals**

This document does **not** track:

* new theoretical directions
* speculative extensions
* unresolved foundational questions
* potential ontology or identity revisions

Any work of that kind would require a **new version and explicit canon revision**.

---
