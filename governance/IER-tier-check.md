# IER Tier Consistency Checker

## Status and Purpose

This document specifies a repository tool: the **Tier Consistency Checker**.

It governs **verification of manifest tier assignments against cross-document dependency structure**, with the goal of preventing:

* **tier inversions** (upstream documents depending on downstream documents)
* **silent foundational leakage** (foundational documents relying on lower-tier concepts/documents)
* **manifest drift** (missing files, renamed files, stale entries)

This tool introduces **no theoretical or ethical claims** and does **not** revise any IER commitments.
It is a **governance-layer integrity check** only.

This checker is **not** an “extract and verify” script.
Its logic depends on canon structure and is therefore documented explicitly.

---

## Scope

### In Scope

The tool checks the following:

1. **Manifest integrity**
   * every file enumerated in `IER-manifest.md` exists
   * every referenced `IER-*.md` dependency is in the manifest (or is explicitly whitelisted)

2. **Tier inversion detection**
   * a document may not declare **authority dependence** on a lower-authority (higher-numbered) tier document

3. **Downstream reference warnings**
   * a document referencing lower-tier documents outside authority anchors is flagged as suspicious (warning), not necessarily an error

4. **Canon-aligned constraints**
   * the checker assumes tier authority is defined **only** by `IER-manifest.md`
   * the checker avoids requiring YAML metadata (disallowed by canon for corpus chapters)

### Out of Scope

The checker does **not**:

* validate theoretical correctness
* validate mathematical correctness
* detect experiential “claims” or introduce diagnostics
* parse or enforce philosophical content
* enforce stylistic formatting beyond what is needed for reliable parsing

---

## Definitions

### Tier Order

Treat tiers as ordered authority levels:

* T1 = most foundational authority
* T2 = elaboration and application
* T3 = misuse-blocking constraints
* T4 = orientation and meta

For checking, map to integers:

* T1 → 1
* T2 → 2
* T3 → 3
* T4 → 4

Lower number = stronger authority.

### Dependency

A dependency is a directed edge:

> `A → B` means “A depends on B.”

This tool distinguishes two kinds:

#### Hard Dependency (Error-Enforced)

A dependency is **hard** if it appears inside an explicit *authority anchor* segment
(e.g., “All authority resides exclusively in …”, “If any statement conflicts with …”).

Hard dependencies are treated as **binding**: if they invert tier order, that is a canonical structural error.

#### Soft Dependency (Warning-Enforced)

A dependency is **soft** if it appears as an in-text reference to an IER file outside authority anchors.
Soft dependencies are treated as **suspicious** if they invert tier order, because they may indicate a hidden authority inversion (or sloppy referencing).

---

## Inputs and Outputs

### Inputs

Required:

* `IER-manifest.md` (tier assignments; corpus inventory)
* all corpus documents enumerated by the manifest (e.g., `IER-*.md`)

Optional:

* a tool config file (see “Configuration”)

### Outputs

The tool emits findings grouped by severity:

* **ERROR**
  * tier inversions (hard dependencies to lower-authority tiers)
  * missing manifest files
  * manifest parsing failures
* **WARNING**
  * downstream references (soft dependencies to lower-authority tiers)
  * references to unmanifested files
  * ambiguous parsing of authority anchors
* **INFO**
  * summary statistics (counts by tier, edge counts, etc.)
  * suggested remediations

The tool must exit with:

* non-zero status if any **ERROR** exists
* zero status if only **WARNING/INFO** exists (unless configured to “strict mode”)

---

## Canon Constraints (Design Requirements)

The checker must respect the canon-wide authoring rules:

1. **No YAML front matter requirement**
   * Corpus chapters must not be required to add YAML metadata.
   * The checker must operate on the existing prose structure.

2. **Tier authority lives in the manifest**
   * A document cannot self-assign its tier.
   * Tier resolution must come only from `IER-manifest.md`.

3. **Informational language and theoretical content are out of scope**
   * The checker must not attempt to interpret “meaning” or extract claims.
   * It must rely on explicit cross-file references and authority anchor patterns only.

---

## Extraction Logic

### Step 1 — Parse `IER-manifest.md`

The manifest provides a mapping:

> `file → tier`

Parsing requirement:

* detect entries of the form `[...] \`IER-*.md\`` where `[...]` contains `T1`, `T2`, `T3`, or `T4`

If a file appears multiple times with different tiers, emit **ERROR**.

### Step 2 — Extract IER file references from each document

Extract every occurrence of:

* backticked filenames matching `IER-*.md`, e.g. `` `IER-specification.md` ``

Notes:

* The checker must ignore non-IER filenames.
* The checker must ignore references to itself and to files not intended as corpus content unless configured otherwise.

### Step 3 — Identify authority anchor segments

Authority anchor segments are prose regions that declare authoritative dependence. Typical patterns include:

* “All ontological and normative authority resides exclusively in:”
* “All authority resides exclusively in:”
* “All binding authority remains fixed exclusively in:”
* “If any statement in this document conflicts with … the … takes precedence …”
* “This document is canon-binding in a negative sense … fixed exclusively by …”

Anchor detection is heuristic but must be conservative:

* The checker should prefer **false negatives** over false positives for hard dependencies.
* If anchor parsing is uncertain, downgrade to **WARNING** rather than asserting a hard dependency.

Recommended parsing heuristic:

* Anchor begins on a line that matches known anchor-start patterns.
* Anchor continues through:
  * an immediately following indented block, fenced block, or bullet list, or
  * subsequent lines until a blank line ends the segment.

### Step 4 — Classify dependencies

For each document `A`:

* `hard_deps(A)` = referenced IER filenames inside authority anchors
* `soft_deps(A)` = referenced IER filenames anywhere in the document

(soft includes hard; soft-only is `soft - hard`)

---

## Validation Rules

### Rule R1 — Missing file

If the manifest enumerates a file that does not exist:

* **ERROR**: `Manifest lists missing file: <file>`

### Rule R2 — Unmanifested dependency

If a document references an IER file that is not in the manifest:

* **WARNING** by default
* **ERROR** if configured to require full manifest closure

Rationale: sometimes governance files or publication scaffolding may be referenced intentionally.

### Rule R3 — Hard tier inversion (canonical structural error)

For each hard dependency `A → B`:

If `tier(B) > tier(A)`:

* **ERROR**: `Tier inversion (hard): A (Tx) anchors to B (Ty)`

Examples of forbidden inversions:

* T1 anchors to T2/T3/T4
* T2 anchors to T3/T4
* T3 anchors to T4

Permitted:

* T2 anchors to T1
* T3 anchors to T1/T2
* T4 anchors to anything (but should rarely “anchor” to anything)

### Rule R4 — Soft tier inversion (suspicion)

For each soft-only dependency `A → B` where `tier(B) > tier(A)`:

* **WARNING**: `Downstream reference (soft): A (Tx) references B (Ty)`

Rationale: a reference is not necessarily a dependence.
But repeated downstream references often indicate authority creep or missing citations in the status block.

### Rule R5 — Self-anchoring / degenerate edges

Ignore edges where `A == B`.

---

## Remediation Guidance (What to Do When It Fails)

### If you get a hard inversion error

There are only three legitimate fixes:

1. **Remove or correct the authority anchor reference**
   * If `A` does not truly depend on `B`, it must not claim that it does.

2. **Promote the depended-on file**
   * If `A` is correct to depend on `B` and that dependence is foundational, `B` must be moved to an equal or higher authority tier (numerically lower).

3. **Move the dependent file down**
   * If `A` is not truly foundational, demote `A` to a tier compatible with its dependencies.

### If you get a soft inversion warning

Typical fixes:

* move the reference into a proper “Status and Authority” dependency list (if it is actually load-bearing)
* or rewrite to avoid implying dependence (if it is merely a pointer)
* or replace the reference with a more appropriate upstream citation

---

## Configuration

The checker should support a small config surface, stored outside corpus chapters.

Recommended options:

* `strict_manifest_closure: bool`
  * if true: unmanifested `IER-*.md` references become **ERROR**
* `whitelist_unmanifested: [files]`
  * list of allowed `IER-*.md` references not required to be in the manifest (rare)
* `anchor_patterns: [regex]`
  * extend recognized anchor-start patterns without code changes
* `warn_only: bool`
  * if true: no non-zero exit; used for exploratory runs

Configuration must not be required for normal operation.

---

## Quality Requirements

### Q1 — Determinism

Given the same repository state, the checker output must be identical.

### Q2 — Low false positives for hard dependencies

Hard inversions must be based only on high-confidence anchor segments.
Ambiguous parsing must degrade to warnings.

### Q3 — Useful, actionable messages

Every ERROR/WARNING must include:

* dependent file
* referenced file
* tiers for both
* whether it was hard or soft

### Q4 — Fast enough for CI

Target runtime: linear in corpus size.
No expensive NLP steps.

---

## Test Plan (Minimum)

Include unit tests for:

1. manifest parsing (happy path; duplicate entries; malformed lines)
2. reference extraction (backticks; multiple references per line; ignore non-IER)
3. anchor detection (typical “Status and Authority” blocks from several corpus files)
4. tier inversion cases (hard vs soft)
5. unmanifested file references (warning vs error by config)

Include fixture documents that mimic real corpus status blocks.

---

## Integration Recommendations

### Where it should run

Run in CI and locally as part of governance checks:

* pre-merge / pre-release
* optionally as part of `governance/IER-build.md` verification steps

### When it should fail the build

Fail the build on:

* any **ERROR**

Do not fail the build on:

* **WARNING** (unless strict mode is enabled)

---

## Notes on Evolving the Corpus

The tool assumes the corpus will evolve in patch releases without changing normative commitments.

This checker does not enforce semantic versioning, but it does help prevent an important class of “accidental” normative shifts:

> moving a dependency edge across tiers without acknowledging it.

Tier changes should be rare and should appear explicitly in `IER-changelog.md`.

---

## Summary

The Tier Consistency Checker protects a core governance invariant:

> **Foundational documents must not depend on downstream documents for authority.**

It does so by:

* treating `IER-manifest.md` as the single source of tier truth
* extracting explicit file-to-file dependency edges
* distinguishing hard (authority anchor) dependencies from soft references
* failing only on clear authority inversions and manifest integrity errors
