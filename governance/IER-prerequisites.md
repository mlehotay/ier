# **IER-prerequisites-generator.md**

## **Generator Specification for `IER-prerequisites.md`**

### **NON-CANONICAL · NON-CORPUS · NOT READER-FACING**

---

## **Status Declaration**

This document is **NOT CANONICAL** and **NOT PART OF THE IER CORPUS**.

It introduces **no ontological, criterial, epistemic, ethical, diagnostic, or interpretive commitments**.

It specifies a **mechanical generation process only**, aligned with the current
behavior of the generator implementation.

If any statement here conflicts with:

* `IER-canon.md`
* `IER-manifest.md`

those documents take precedence and this document is void at the point of conflict.

---

## **0. Purpose**

This document specifies how to **generate** the file:

```

IER-prerequisites.md

```

from dependency metadata declared in **chapter-local YAML front matter**.

The generator:

* scans manifest-enumerated chapters
* extracts declared dependency metadata
* performs structural validation
* expands bundles
* derives dependency relations
* emits a **non-authoritative audit artifact**

This document does **not** define dependency correctness.
It defines **how declared dependencies are rendered, checked, and exposed for inspection**.

---

## **1. Input Scope**

### **1.1 Source Files**

The generator scans **all chapters explicitly enumerated in `IER-manifest.md`**.

Only files that:

* are listed in the manifest

are included in the dependency surface.

Files listed in the manifest **without** YAML front matter or without an `ier:` block:

* are still included in output
* are treated as having **empty dependency metadata**
* are flagged via warnings unless strict mode is enabled

Non-manifest files are ignored entirely.

---

### **1.2 Manifest Parsing Rules**

The manifest is parsed mechanically:

* Backticked tokens ending in `.md` are extracted
* Only chapter-like filenames are retained:
  * basename starts with `IER-`
  * excludes `IER-manifest.md` itself
* Tokens are resolved to paths using:
  1. explicit repo-relative paths, if present
  2. `IER/` then `pub/`
  3. repo-wide unique filename match
  4. fallback to `IER/<filename>` (for stable missing detection)

Manifest membership is **authoritative** for dependency validation.

---

### **1.3 YAML Extraction Rules**

For each scanned chapter:

* Extract YAML front matter **only if it appears at the top of the file**
* Parse only the top-level `ier:` key
* Ignore all other YAML keys unconditionally

If YAML front matter exists but **no `ier:` key is present**:

* emit a **warning**
* treat the chapter as having empty dependency metadata

If YAML front matter is missing entirely:

* emit a **warning**
* treat the chapter as having empty dependency metadata

The generator must not fail solely due to missing YAML unless explicitly configured to do so.

---

## **2. Preprocessing**

### **2.1 Normalization**

For each chapter entry, normalize:

* **Chapter ID** → filename stem (e.g., `IER-dynamics`)
* **Tier** → literal string (no validation)
* **Role** → literal string
* **Status** → literal string
* **Dependency lists** → stripped, de-duplicated, sorted lists

Empty lists must be represented explicitly as `[]`.

No implicit defaults may be introduced.

---

### **2.2 Bundle Expansion**

Before graph construction or validation:

1. Identify bundle tokens in:
   * `requires.hard`
   * `requires.structural`
   * `requires.guardrails`
2. Expand each bundle into its configured chapter identifiers
3. Replace bundle tokens with expanded identifiers
4. Record bundle usage for diagnostic reporting

Bundle rules:

* Bundle names are matched exactly
* A token that:
  * matches `[A-Z0-9_-]+`
  * does **not** start with `IER-`
  * and is not a defined bundle
  → is treated as an **error**

**Bundles must not appear in final dependency lists.**

Bundle definitions are supplied by generator configuration only.

---

## **3. Validation Phase**

The generator enforces **structural dependency validation only**, including:

* absence of **hard-dependency cycles**
* identifier resolution against manifest membership
* bundle reference validity
* manifest-listed files must exist on disk

The generator does **not** enforce:

* bridge semantics
* guardrail propagation
* gate discipline rules
* semantic correctness of dependencies

Validation is **mechanical only**.

---

### **3.1 Validation Failure Handling**

* **Hard validation failures**
  (e.g., cycles, unresolved identifiers, missing files, undefined bundles)
  → generator **must abort** with a non-zero exit code.

* **Soft omissions**
  (e.g., missing YAML, missing `ier:` block)
  → generator **must emit warnings** and continue unless strict mode is enabled.

The generator must **never infer, auto-correct, or mutate** dependency data.

---

## **4. Output File Properties**

### **4.1 File Status**

`IER-prerequisites.md` must begin with a status block that clearly states:

* the file is **non-canonical**
* the file is **non-corpus**
* the file is **derived**
* the file is **non-authoritative**
* chapter-local YAML metadata is the **sole source of truth**

Language must be consistent with this declaration.

---

### **4.2 Ordering of Entries**

Chapters must be ordered by:

1. a topological sort over `requires.hard`
2. stable alphabetical order within the same topological depth

This ordering is **presentational only** and carries no normative meaning.

---

## **5. Output Structure**

The generated file must contain the following sections **in this order**:

1. Status Declaration
2. Generation Metadata
3. Legend
4. Chapter Prerequisite Table
5. Reverse Dependency Index
6. Gate Index
7. Validation Summary
8. Generation Timestamp

---

## **6. Section Specifications**

### **6.1 Status Declaration**

Must state explicitly:

* non-canonical
* non-corpus
* derived
* non-authoritative
* YAML metadata is authoritative

No interpretation or guidance is permitted.

---

### **6.2 Generation Metadata**

Must include:

* generator name
* git commit hash (if available)
* manifest path
* bundles config path
* total chapters in manifest
* chapters with `ier:` metadata
* chapters missing `ier:` metadata
* bundles expanded during the run

---

### **6.3 Legend**

Must explain:

* hard vs structural vs guardrail prerequisites
* meaning of gates
* meaning of empty lists (`[]`)
* meaning of warnings

This section is explanatory only.

---

### **6.4 Chapter Prerequisite Table**

For **every chapter in the manifest**, include one entry with:

* Chapter ID
* Tier
* Role
* Status
* Hard Prerequisites
* Structural Prerequisites
* Guardrail Prerequisites
* Provides
* Gates Opened
* Gates Required (`[]`, not inferable)

Formatting rules:

* All lists must be explicit
* No bundles may appear
* No inferred or transitive dependencies
* Stable ordering only

---

### **6.5 Reverse Dependency Index**

For each chapter, list:

* hard dependents
* structural dependents
* guardrail dependents

Derived mechanically from normalized data.

---

### **6.6 Gate Index**

For each gate:

* gate name
* chapters that open the gate
* chapters that require the gate (`[]`, not inferable)

---

### **6.7 Validation Summary**

Must summarize:

* total chapters processed
* total hard dependency edges
* total structural dependency edges
* total guardrail dependency edges
* total gates
* validation status (PASS / FAIL)
* count of warnings

Warnings must be listed verbatim if present.

---

### **6.8 Generation Timestamp**

Must include:

* UTC timestamp

Git hash is reported in metadata, not required here.

---

## **7. Prohibited Generator Behavior**

The generator must **not**:

* infer missing dependencies
* collapse dependency categories
* resolve semantic conflicts
* rank importance
* imply correctness or authority
* generate interpretive prose
* rewrite chapter YAML

This file is an **audit surface only**.

---

## **8. Determinism Requirements**

Given identical inputs and configuration:

* output must be byte-for-byte identical
* ordering must be stable
* formatting must be deterministic
* filesystem traversal order must not affect output

---

## **9. Failure Modes**

The generator must fail closed if:

* hard dependency cycles are detected
* dependency identifiers cannot be resolved
* bundle references are undefined
* manifest-listed files are missing on disk

---

## **10. Change Policy**

Permitted changes:

* formatting improvements
* richer audit metadata
* stricter validation

Prohibited changes:

* introducing interpretive authority
* treating output as canonical
* allowing bundle leakage
* using this file to justify dependency correctness
