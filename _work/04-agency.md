## Audit: `IER-agency.md` against its Explanatory Orientation

### Overall verdict: **REVISE (moderate)**

The core agency story matches the Orientation well (owned collapse under intrinsic constraint), but the implementation **leaks Ethics/Standing**, **slides into Possibility/Time metaphors**, and has a couple of **interface-level mismatches** that should be corrected to keep the projection clean.

The Orientation itself is mostly fine; the article body is what needs containment edits.

---

## 1) Orientation contract check

### A1 Question ownership: **PASS**

The article is centrally answering: **“Is this resolution owned?”**

### A2 Foregrounded aspect: **PASS**

“Collapse under ownership” is stable through §§4–8 and §10.

### A3 Fixed structure: **⚠️ Minor contract risk**

Orientation lists as fixed:

* “presence of multiple admissible futures prior to resolution”

That’s okay if “multiple” is treated as **structural multiplicity of admissible continuations**, not “represented options” (you already refuse representation). However, some real-world cases of agency-like ownership might involve **degenerate** choice spaces (only one admissible continuation but still “owned” as internal resolution). Your Orientation currently makes “multiple” sound definitional.

**Suggested micro-tightening (optional, orientation-level):**

* change “presence of multiple admissible futures” → **“the presence of an admissible continuation structure (possibly branching) prior to resolution.”**
  This preserves the intent without building in a brittle requirement.

### A6 Deferral clause: **Not present at end of article**

The deferral sentence must appear in the document body, not just in the supplied Orientation block. Add it at the end.

---

## 2) Implementation leakage and scope violations

### Ethics / Standing leakage: **FAIL (major)**

Even though “ethics” isn’t one of the six projections, the interface prohibits normative work inside projections. This file repeatedly makes standing claims:

* Abstract: “loss of agency does not imply loss of experience or standing”
* §7: “Reduced agency does not imply reduced standing”
* §9: “Standing is categorical… does not justify harm…”

This is **Ethics projection content**, not Agency. It also creates local synthesis with `IER-ethics.md`.

**Fix:** Replace standing language with scope-safe refusals:

* Agency can say: **“agency is graded; experience is not inferred here.”**
* Agency can refuse: **“this article does not determine standing or moral permissibility.”**
  But it must not assert standing structure (“categorical”) or normative constraints (“does not justify harm”).

### Possibility leakage: **⚠️ Moderate**

The file uses “choice space,” “counterfactual alternatives,” and graph metaphors. That can be fine as descriptive shorthand (you already disclaim semantics), but some lines drift into Possibility’s territory:

* “supports counterfactual alternatives”
* “accumulates history and narrowing”
* “future possibility”

If left as-is, this reads like a modal account of possibility, not an agency account of *owned collapse*.

**Fix:** constrain the language to agency-owned collapse:

* Replace “counterfactual alternatives” → “non-realized admissible continuations”
* Replace “future possibility” → “admissible continuation”
* Remove “accumulates history” (History) unless explicitly deferred.

### History leakage: **⚠️ Present**

“accumulates history and narrowing” (in §5) and scattered “narrowing” talk risks importing `IER-sedimentation.md`.

**Fix:** Either delete that phrase, or explicitly mark:

* “how the space becomes narrowed is History (sedimentation); here we only characterize ownership at collapse.”

### Time leakage: **⚠️ Minor**

“traversal,” “irreversibly narrows,” “collapse becomes unavoidable” are okay (agency needs irreversibility), but don’t let the file turn into an account of time’s asymmetry. Right now it’s not doing full Time, but the phrasing in §10 (“Lawfulness governs how transitions occur…”) is fine; avoid any extra arrow-of-time explanation.

---

## 3) Additional internal inconsistencies to clean up

### “Experiential agency” labeled “Normative” in §3 table

The table says:

* “Experiential agency — Normative”

But earlier the file insists it is **non-normative**. This is an internal terminology mismatch.

**Fix:** change the table row to something like:

* “Experiential agency — regime-bound structural feature (non-normative)”
  and keep “normative” out of the agency file unless you mean “ethically loaded,” which you should avoid here.

### “Agency exists iff experience exists” (strong claim)

This appears multiple times. It may be correct under your corpus, but it is also a potential **criterion-like statement** (“iff”) that can sound like a diagnostic. If it’s already canonical in Specification, fine; but in this explanatory file it should be phrased as a **presupposition** and not used to infer detection conditions.

**Fix (phrasing hygiene):**

* “On IER, experiential agency is defined only for systems sustaining a UEF.”
  Avoid “iff” framing unless you’re quoting/specifying.

---

## 4) Minimal edit plan for interface compliance (Agency)

### Step 1 — Insert Orientation

Insert the Orientation near the top (after title), freeze it.

### Step 2 — Remove standing/responsibility content (relocate)

* Delete §9 entirely, or replace it with a one-paragraph **refusal-only** section:

  * “This document does not address responsibility, standing, blame, or permissibility. See `IER-ethics.md`.”
* Remove standing claims from Abstract and §7.

### Step 3 — Tighten choice-space language to avoid Possibility/History

* In §5, remove:

  * “supports counterfactual alternatives”
  * “accumulates history and narrowing”
* Replace with agency-safe descriptions:

  * “multiple admissible continuations exist prior to resolution (as physical admissibility, not representation).”

### Step 4 — Fix the “Normative” label in §3 table

Make terminology consistent with “NON-NORMATIVE” status.

### Step 5 — Add the mandatory deferral clause at the end

> **Unification with other constraint projections is handled exclusively in `IER-topology.md`.**

No synthesis after it.

---

## 5) Quick pass/fail summary

**PASS:** core definition of agency as owned collapse; refusals of indeterminism/homunculus/deliberation; degrees of agency framing (once de-ethicized).
**REVISE:** standing/responsibility content; choice-space/counterfactual language; history phrasing; minor internal terminology (“normative”).
