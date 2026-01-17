# Governance Documents

This directory contains **non-canonical governance documents** for the
Informational Experiential Realism (IER) project.

These documents define **constraints on process, structure, and publication**.
They do **not** define theory, ontology, or claims.
They do **not** introduce publication content.
They carry **no epistemic authority**.

Their purpose is to ensure that authoritative material is:

- assembled deterministically
- rendered consistently
- released in a disciplined order
- interpreted without authority leakage
- attributed and disclosed in line with current law

---

## What “Governance” Means in IER

In IER, *governance* answers questions of the form:

- How is material mechanically assembled?
- In what order may artifacts be released?
- Which artifacts are authoritative, and which are interfaces?
- What constraints apply to layout, typography, and physical format?
- How are readers expected to use different artifacts?
- What legal assumptions govern authorship and responsibility?

Governance **never** answers questions of the form:

- What is experience?
- What claims are true?
- What follows from the theory?
- What should be believed?

Those questions are answered **only** by canonical material under `IER/`.

---

## Authority and Precedence

Authority resolves strictly **upstream**:

1. **Canonical theory** (`IER/`)
2. **Canon and manifest rules** (`IER-canon.md`, `IER-manifest.md`)
3. **Governance documents** (this directory)
4. **Build scripts and tooling**

If a governance document conflicts with canonical material,
**the canon always prevails**.

Governance constrains **process**, not **content**.

---

## What Lives in This Directory

Each file governs a distinct axis.
No document substitutes for another.

### `IER-build.md`  
**Mechanical Assembly and Verification**

- how books are assembled from declared inputs
- how ordering and structure are determined
- how structural pages are generated
- how builds are verified for validity

This document defines **mechanical invariants**.
A build that violates them is invalid, even if it renders.

---

### `IER-publishing.md`  
**Rendering and Physical Instantiation**

- trim sizes and page geometry
- column models and typography
- density and layout discipline
- print vs digital rendering constraints

This document governs **how artifacts are physically encountered**,
not what they claim or how they are ordered.

---

### `IER-deployment.md`  
**Release Order and Authority Anchoring**

- which artifact establishes public reference
- when interface artifacts may be released
- immutability expectations after release
- cross-artifact framing requirements

This document governs **how IER enters public space**.

---

### `IER-readers.md`  
**Audience Analysis and Reading Patterns**

- intended reader groups
- common reading trajectories
- predictable misuse cases
- artifact–reader alignment

This document is **advisory**, not prescriptive.
It informs editorial decisions without overriding governance or canon.

---

### `IER-legal.md`  
**Legal Context for AI-Assisted Authorship (as of 2025)**

- authorship and ownership assumptions
- disclosure norms
- liability and responsibility
- jurisdictional notes

This document describes **current legal treatment** only.
It makes no claims about consciousness, agency, or moral standing.

---

## What This Directory Is *Not*

This directory is **not**:

- a theory supplement
- a style guide for prose
- a place for interpretation or motivation
- a substitute for canonical documentation
- a collection of optional recommendations

Violations of governance rules are treated as
**structural errors**, not editorial disagreements.

---

## Common Category Errors

If you find yourself trying to:

- change theory by editing a governance file
- resolve a philosophical dispute with a publishing rule
- justify a build exception for convenience
- treat an interface artifact as authoritative
- infer authority from layout or venue

You are operating in the **wrong layer**.

---

## When to Read Which Document

- Modifying build scripts or structure → **IER-build.md**
- Changing trim size, columns, or typography → **IER-publishing.md**
- Planning or sequencing releases → **IER-deployment.md**
- Writing or revising interface artifacts → **IER-readers.md**
- Concerned about authorship or disclosure → **IER-legal.md**

If you are asking *“what does IER claim?”*  
You are in the **wrong directory**.

---

## Final Constraint

Governance exists to **prevent drift**, not to enable flexibility.

If a rule matters, it is written down.  
If it is written down, it is enforced.  
If it conflicts with the canon, it yields.

---
