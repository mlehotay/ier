# Governance Documents

This directory contains **governance and internal procedure documents** for the
Informational Experiential Realism (IER) project.

These documents define **constraints on process, structure, tooling, and publication**.
They do **not** define theory, ontology, or claims.
They do **not** introduce publication content.
They carry **no epistemic authority**.

Their purpose is to ensure that authoritative material is:

* assembled deterministically
* rendered consistently
* released in a disciplined order
* interpreted without authority leakage
* attributed and disclosed in line with current law
* coordinated internally without corpus drift

---

## What “Governance” Means in IER

In IER, *governance* answers questions of the form:

* How is material mechanically assembled?
* In what order may artifacts be released?
* Which artifacts are authoritative, and which are interfaces?
* What constraints apply to layout, typography, and physical format?
* How are readers expected to use different artifacts?
* What legal assumptions govern authorship and responsibility?
* How are internal dependencies, tooling, and coordination managed?

Governance **never** answers questions of the form:

* What is experience?
* What claims are true?
* What follows from the theory?
* What should be believed?

Those questions are answered **only** by canonical material under `IER/`.

---

## Corpus Status and Authority Boundary (Binding)

This directory contains **two distinct classes of documents**.

### 1. In-Corpus Governance (Canon-Governed Infrastructure)

Some governance documents are **in the IER corpus**, as explicitly enumerated in
`IER-manifest.md` under the *Governance Subdirectory*.

These documents:

* are canon-governed
* introduce **no theoretical or ethical commitments**
* regulate build, publishing, deployment, and reader alignment
* may be cited **only** for process and governance

They are part of IER **by enumeration**, not by location.

---

### 2. Internal Procedures (Non-Corpus)

Other documents in this directory are **explicitly non-corpus**, even if they use the
`IER-` prefix.

These documents:

* are **not enumerated** in `IER-manifest.md`
* are **NON-CANONICAL · NON-CORPUS · NOT READER-FACING**
* exist solely for **author coordination, tooling specification, and internal hygiene**
* must never be included in publication outputs
* must never be cited for interpretation, authority, or dispute resolution

**Manifest rule (controlling):**
If a file is not explicitly listed in `IER-manifest.md`, it is **non-corpus**, regardless of name or location.

---

## Authority and Precedence

Authority resolves strictly **upstream**:

1. **Canonical theory** (`IER/`)
2. **Canon and manifest rules** (`IER-canon.md`, `IER-manifest.md`)
3. **In-corpus governance documents** (this directory, enumerated only)
4. **Internal procedures and tooling specifications**
5. **Build scripts and tooling**

If a governance or internal document conflicts with canonical material,
**the canon always prevails**.

Governance constrains **process**, not **content**.
Internal procedures constrain **coordination**, not interpretation.

---

## What Lives in This Directory

Each file governs a distinct axis.
No document substitutes for another.

### `IER-build.md`

**Mechanical Assembly and Verification** *(in-corpus)*

* how books are assembled from declared inputs
* how ordering and structure are determined
* how structural pages are generated
* how builds are verified for validity

This document defines **mechanical invariants**.
A build that violates them is invalid, even if it renders.

---

### `IER-publishing.md`

**Rendering and Physical Instantiation** *(in-corpus)*

* trim sizes and page geometry
* column models and typography
* density and layout discipline
* print vs digital rendering constraints

This document governs **how artifacts are physically encountered**,
not what they claim or how they are ordered.

---

### `IER-deployment.md`

**Release Order and Authority Anchoring** *(in-corpus)*

* which artifact establishes public reference
* when interface artifacts may be released
* immutability expectations after release
* cross-artifact framing requirements

This document governs **how IER enters public space**.

---

### `IER-readers.md`

**Audience Analysis and Reading Patterns** *(in-corpus)*

* intended reader groups
* common reading trajectories
* predictable misuse cases
* artifact–reader alignment

This document is **advisory**, not prescriptive.
It informs editorial decisions without overriding governance or canon.

---

### `IER-legal.md`

**Legal Context for AI-Assisted Authorship (as of 2025)** *(in-corpus)*

* authorship and ownership assumptions
* disclosure norms
* liability and responsibility
* jurisdictional notes

This document describes **current legal treatment only**.
It makes no claims about consciousness, agency, or moral standing.

---

### `IER-authoring.md`

**Corpus Authoring and Dependency Declaration Discipline** *(in-corpus)*

* how canonical chapters are authored as standalone theoretical units
* mandatory YAML front-matter requirements for corpus chapters
* semantics of dependency metadata (`hard`, `structural`, `guardrails`)
* use of `provides`, gates, and bundles for dependency hygiene
* expectations for resolving dependency and ordering verification failures

This document governs **how authors write and annotate corpus material** so that
theory, dependency structure, and build verification remain aligned.

It introduces **no theory**, defines **no authority**, and carries **no reader-facing obligations**.
Its role is to enforce explicitness, prevent silent dependency drift, and ensure that
mechanical validation reflects real theoretical commitments.

---

### Other Files in This Directory

Files marked
**NON-CANONICAL · NON-CORPUS · NOT READER-FACING**
(e.g. dependency procedures, interface contracts, generator specifications)

* are internal coordination artifacts
* are not part of IER
* carry no authority
* must never appear in publication builds

---

## What This Directory Is *Not*

This directory is **not**:

* a theory supplement
* a place to introduce or revise claims
* a style guide for prose
* a substitute for canonical documentation
* a collection of optional recommendations

Violations of governance rules are treated as
**structural errors**, not editorial disagreements.

---

## Common Category Errors

If you find yourself trying to:

* change theory by editing a governance or internal file
* resolve a philosophical dispute with a publishing rule
* justify a build exception for convenience
* treat an interface artifact as authoritative
* infer authority from layout, venue, or tooling
* cite an internal procedure as if it were part of IER

You are operating in the **wrong layer**.

---

## When to Read Which Document

* Modifying build scripts or structure → **IER-build.md**
* Changing trim size, columns, or typography → **IER-publishing.md**
* Planning or sequencing releases → **IER-deployment.md**
* Writing or revising interface artifacts → **IER-readers.md**
* Concerned about authorship or disclosure → **IER-legal.md**
* Writing or revising canonical corpus chapters → **IER-authoring.md**
* Coordinating dependencies or tooling → **non-corpus internal procedures**

If you are asking *“what does IER claim?”*
You are in the **wrong directory**.

---

## Final Constraint

Governance exists to **prevent drift**, not to enable flexibility.

If a rule matters, it is written down.
If it is written down, it is enforced.
If it conflicts with the canon, it yields.
