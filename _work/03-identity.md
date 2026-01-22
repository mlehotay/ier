## Audit: `IER-identity.md` against its Explanatory Orientation

### Overall verdict: **REVISE**

The Orientation is mostly aligned with the file’s intent, but the **implementation violates the interface** in three main ways:

1. **Time / Memory / Possibility leakage** is used as *positive explanation* (not just refused).
2. The Orientation’s **fixed structure** is internally inconsistent with what Identity needs to permit (regime gaps).
3. The article performs **local synthesis** across projections instead of deferring.

---

## 1) Orientation status (contract check)

### A1 Question ownership: **Mostly PASS**

The file’s central claim is identity as **ownership persistence** (“who is bearing this?”). Good.

### A2 Foregrounded aspect: **PASS**

Ownership persistence is stable and dominant.

### A3 Fixed structure: **REVISE (orientation-level issue)**

Orientation invariant includes:

> “the continued existence of a physical system sustaining a Unified Experiential Field”

But the Identity article explicitly depends on **UEF interruption** (sleep/anesthesia) while identity persists. If “sustaining a UEF” is treated as invariant, then regime gaps become illicit.

**Fix (Orientation contract):** make the invariant be **continued existence of the physical system**, not continued existence of a UEF. UEF existence should be allowed to vary (present/absent) while identity persists.

### A4 Allowed variation: **Mostly PASS, minor drift**

“organization of admissible futures” / “narrowing of future-bearing capacity” is identity-relevant, but easily slips into **Possibility** (openness) and **History** (deformation). This is workable if phrased strictly as *ownership persistence under changed continuation structure*, not as forecasting or explanation of why it deformed.

### A5 Non-claims: **Needs tightening**

The list is good, but the body still does many of these “non-claims” in practice (esp. Memory and Time). Add refusal items that match actual leak pressure (see below).

### A6 Deferral clause: **Missing at end**

The clause appears in the supplied Orientation, but **the article itself needs it** at the end, and must not synthesize after it.

---

## 2) Implementation failures (article-body leakage)

### B1 Time leakage: **FAIL**

The article repeatedly *explains* time-direction (“arrow of time,” “time-direction induced by irreversible constraint deformation”) rather than treating it as a presupposed background. That is **Time’s question**.

**Fix:** keep only what Identity needs:

* Identity is *not* a function of experiential continuity.
* If any asymmetry is referenced, it must be stated as **presupposed** (not explained).

### B2 Memory leakage: **FAIL**

Section 7 explains memory as “future accessibility,” felt authority, etc. That is **Memory’s question** (reachability), and it’s doing more than refusal.

**Fix:** Replace §7 with a short identity-only statement:

* “Memory is not an identity criterion.”
* Defer reachability and authority-feel to `IER-memory.md`.

### B6 Possibility leakage: **FAIL**

Identity is defined using “admissible futures” heavily. That can be allowed as *ownership structure*, but the text frequently slides into:

* open/fixed rhetoric,
* continuation-space talk that reads like a Possibility account.

**Fix:** constrain language:

* use “continuations borne by this system” / “owned continuations” rather than “space of admissible futures,” and avoid “open vs closed” framing.

### History leakage: **Borderline-to-fail**

“sedimentation,” “hysteresis,” “deformation” are used to explain *why shaped this way*. Identity only needs: “the bearer persists across deformation,” not the deformation story.

**Fix:** treat deformation as **given**; defer causal shaping to `IER-sedimentation.md`.

### Topology synthesis: **FAIL**

Abstract + multiple sections explicitly reconcile Memory + Time + Identity + continuity gaps. That reconciliation belongs only in `IER-topology.md`.

**Fix:** remove “this also clarifies…” bridges and any multi-projection “puzzle-solving” packaging.

---

## 3) Concrete plan to update `IER-identity.md` to implement the interface

### Phase 0 — Insert and correct Orientation (minimal contract repair)

1. Insert the Explanatory Orientation near the top (after title, before Abstract).
2. Revise **only** the Orientation invariant line:

   * Replace “continued existence of a physical system sustaining a UEF” with something like:
     **“the continued existence of the physical system (whether or not a UEF is presently sustained)”**
3. Freeze the Orientation.

### Phase 1 — Re-scope the Abstract (Identity only)

Rewrite the Abstract to do only:

* identity ≠ memory/narrative/psychological similarity/continuous experience
* identity = ownership persistence of intrinsic constraint across change

Delete from Abstract:

* time-direction explanation
* memory authority explanation
* detailed gap cases as “solved puzzles”

### Phase 2 — Prune or relocate cross-projection explanation blocks

Apply this decision rule per section:

* **Keep (with light edits):** §1, §2, §3, §4, §5, §6 (core identity)
* **Rewrite heavily:** §7 and §8 (currently Memory + Time)
* **Trim/defer:** §9 (copies/uploads, dissociation) to avoid turning into topology/metaphysics adjudication

#### Specific edits

* **§7 “Why Memory Feels Like Identity”** → Replace with a short refusal-only section:

  * “Memory may correlate with identity talk, but is not criterial.”
  * “Reachability and authority-feel are treated in `IER-memory.md`.”
* **§8 “Identity and the Arrow of Time”** → Replace with:

  * “Identity discourse is time-asymmetric because systems undergo irreversible change; this file does not explain time.”
  * Defer to `IER-continuity.md` for the account of time/passage.

### Phase 3 — Language containment pass (stop Possibility drift)

Globally replace or constrain:

* “open/fixed” language → remove
* “space of admissible futures” → “owned continuations / owned trajectories”
* any talk that resembles prediction/branching → remove

### Phase 4 — End-of-article deferral clause

Add verbatim at the very end:

> **Unification with other constraint projections is handled exclusively in `IER-topology.md`.**

Ensure no synthesis appears after it.

---

## 4) One key note about the Orientation’s “admissible futures” phrasing

It *can* remain if treated strictly as **ownership structure** (what is “bound to this bearer”), not as **modal openness** (what could still occur). The implementation needs to enforce that distinction by avoiding branching/probability/forecast language.

---
