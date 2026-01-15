# **IER Canon (v10.8.2)**

## **Canonical Authority, Alignment, and Governance Rules**

---

## **Status and Purpose**

This document defines the **canonical authority rules, interpretive constraints, alignment requirements, conflict resolution order, and version discipline** governing the **Informational Experiential Realism (IER)** project as of **version 10.8.2**.

It introduces **no ontological, criterial, epistemic, or ethical claims**.

It governs **how documents relate to one another**, not what the theory claims.

---

## **Corpus vs Canon (Definitions)**

**Corpus** refers to the **set of documents explicitly enumerated in `IER-manifest.md`** for a given version.

* Inclusion in the corpus means:

  * the document is part of “IER v10.8.2”
  * it is subject to the governance rules of this canon
  * it may be cited for orientation, exposition, or structure as appropriate

**Canon** refers to the **binding authority rules and interpretive constraints** that govern how the corpus is read, aligned, versioned, and adjudicated.

* Canon:

  * is **not a document list**
  * does **not expand or modify theory claims**
  * determines what kinds of statements are admissible, authoritative, or disallowed

---

### **Canonical vs Non-Canonical Corpus Material**

Within the corpus:

* **Canonical-authoritative documents** may introduce or fix binding commitments (as authorized by tier).
* **Non-canonical corpus documents** are included for publication, pedagogy, assembly, or orientation, but:

  * introduce **no new authoritative commitments**
  * may not be cited to resolve theoretical disputes
  * must defer to canonical-authoritative material in all cases

**Non-corpus material** consists of files present in the repository but **not enumerated in the manifest**. These are outside the scope of IER v10.8.2.

**Document inventories, tier assignments, and inclusion status are defined exclusively in `IER-manifest.md`.**

---

## **General Canon Rule**

> **No document may introduce ontological, criterial, epistemic, or normative commitments unless explicitly authorized by the canon.**

All documents—canonical and non-canonical alike—must defer to canon-authoritative commitments in cases of ambiguity or tension.

Violations constitute **canonical errors**, not stylistic disagreements.

---

## **Normative Closure and Corpus Completion**

**IER v10.8 is normatively closed.**

Normative closure means:

* experiential identity claims are fixed
* ethical consequences are fixed
* epistemic limits are fixed

**IER v10.8.1 marks corpus completion**, meaning:

* the corpus document set is explicitly enumerated
* indices and glossaries are finalized
* scope and limits are sealed for clarity
* no new authoritative commitments are introduced

Corpus completion does **not** imply that every corpus document is canonical-authoritative.

The repository may continue to evolve under patch versions, provided no commitments are added, removed, or weakened.

---

## **Reader Alignment Principle (Global)**

IER may be:

* institution-hostile
* engineering-restrictive
* ethically severe

IER must **not** be **reader-hostile**.

**Reader-hostility** is defined as:

> framing that treats the reader as an adversary, presumes misuse, assigns blame for confusion, or presents difficulty as culpable rather than structural.

All corpus documents must presume **good-faith readership**.

---

# **Global Alignment Rules (Binding Across the Corpus)**

Violation of any rule below is a **canonical error**, even absent direct contradiction of identity claims.

---

## **G1 — Informational Language Is Always Derivative**

All informational language must be readable **only** as shorthand for:

> physically instantiated state distinctions that modulate a system’s own future dynamics under intrinsic constraint.

No informational primitives, representations, semantics, or epistemic contents may be introduced.

Documents using informational language **must include an explicit disclaimer** to this effect.

---

## **G2 — No Agentive Language in Definitions or Necessity Claims**

Agentive or value-laden language must not appear in:

* definitions
* identity claims
* necessity or exclusion arguments

Agentive phrasing may appear **only** in explanatory gloss and must never function as a primitive.

---

## **G3 — Observer Is Non-Normative**

The term *observer* may appear only as:

* a descriptive convenience, or
* an external modeling stance

It confers **no experiential, epistemic, or ethical authority**.

Any indispensable use must explicitly state its non-normative role.

---

## **G4 — Regime Membership Is Categorical; Parameters Are Continuous**

Documents must preserve the distinction between:

* **categorical regime membership** (experience, subjecthood, standing), and
* **continuous parameters** (intensity, stability, participation, vulnerability)

Any passage that could imply **graded subjecthood** must be revised.

---

## **G5 — Experiential Structure Confers No Epistemic Authority**

Any discussion of:

* meaning
* salience
* urgency
* narrative
* agency
* conviction or belief

must explicitly reaffirm:

> **Experiential structure confers no epistemic authority about mind-independent reality.**

This rule is mandatory misuse containment.

---

## **G6 — Reader Alignment Rules**

All corpus documents must be written as if the reader is acting in good faith.

### **G6.1 — No Presumptive Blame**

Confusion must be treated as a structural consequence of a strong identity claim, not reader failure.

### **G6.2 — Structural Framing Over Prohibitive Framing**

Prefer *structurally impossible* to *forbidden* and *not licensed by the framework* to *invalid*.

### **G6.3 — Orientation Before Enforcement**

Canon-authoritative and misuse-restrictive documents must include an early, non-normative orientation explaining:

* why constraints exist
* what misattributions they prevent

### **G6.4 — Acknowledge Difficulty Without Weakening**

Costs (ethical discomfort, epistemic opacity) must be acknowledged as consequences of the identity claim.

### **G6.5 — No Adversarial Tone Toward the Reader**

IER may be uncompromising in substance, never prosecutorial in tone.

---

## **Conflict Resolution Order**

When tension appears:

1. Canon-authoritative commitments override all others
2. Explicit statements override inferred intent
3. Misuse-blocking constraints apply globally
4. Explanatory or pedagogical framing defers to authority
5. Alignment rules override stylistic latitude

Appeals to intuition, example, or application may never override canon-authoritative claims.

---

## **Versioning Discipline**

IER uses semantic versioning to track **normative authority**, not repository activity.

```
vMAJOR.MINOR.PATCH
```

* **MAJOR** — identity or foundational commitment changes
* **MINOR** — changes to binding constraints or canon authority
* **PATCH** — clarification, pedagogy, tone, structure, or organization

Patch changes **must not** add, remove, or weaken commitments.

---

## **Typography and Notation**

IER uses typography to enforce **formal and syntactic distinctions**.

* **Math mode** is used only for formal structure:
  variables (`$a$`, `$t$`), functions (`$I(t)$`), sets (`$S$`, `$\mathcal{R}$`), relations (`$T \subseteq S \times S$`), operators (`$\neq$, $\Rightarrow$, $\Delta$`), tuples (`$(S,T)$`), and derivatives (`$\dot{I}$`, `$\ddot{I}$`, `$\dddot{I}$`).
  Sentence punctuation appears **outside** math mode.

* **Display math** must be enclosed in `$$ ... $$` and is required for multi-line equations and governing relations.

* **Conceptual terms inside math** must be wrapped in `\text{...}`.

* **Subscripts** use `_`; conceptual labels in subscripts must use `\text{...}`.

* **Do not** use `\text{}` for genuine variables, functions, or operators.

* **Unicode math glyphs are forbidden.** Use LaTeX operators only.

* **Negation** uses `\neg` in math mode.

* **Backticks** name files or identifiers only; never mathematics.

* **Capitalization** reflects defined constructs, not emphasis.

These conventions are **project-canonical**. Violations are canonical errors even if rendered output appears visually similar.

---

# **Authoring and Rendering Discipline (Corpus-Wide)**

These rules ensure that **all corpus documents render deterministically** under the IER build system (Pandoc → LaTeX → PDF).

Violation of any rule below is a **canonical error**, even if no theoretical commitments are affected.

---

## **A1 — One H1 Title Per Document**

Every corpus document must begin with exactly one `# ...` H1 title.

* The first non-empty line must be the H1.
* No other H1 headings are permitted.

---

## **A2 — Heading Levels Must Not Skip**

Heading levels must advance or retreat by only one level at a time.

* Allowed: `##` → `###` → `##`
* Disallowed: `##` → `####`

---

## **A3 — No YAML Front Matter in Corpus Chapters**

YAML front matter (`--- ... ---`) is disallowed in canonical corpus chapters.

Edition-level metadata belongs in SCAFFOLD or publication-layer files only.

---

## **A4 — Raw LaTeX Is Restricted**

Raw LaTeX is permitted only for:

* mathematics
* minimal local formatting not expressible in Markdown

It must not include:

* preamble or document structure commands
* page layout directives
* recto/verso enforcement

---

## **A5 — Code Blocks Must Be Fenced**

All multi-line code or pseudo-code must use fenced code blocks (` ``` `).

Indent-based code blocks are disallowed.

---

## **A6 — Lists Must Be Markdown-Native**

Use Markdown list syntax only.

Manual numbering or spacing hacks are prohibited.

---

## **A7 — Tables Must Be Pandoc-Compatible**

Tables must use Pandoc-supported table syntax.

HTML tables are disallowed unless explicitly verified.

---

## **A8 — Links and Paths**

* Use backticks for file paths and identifiers.
* Do not use backticks for mathematics.
* Avoid bare URLs in prose.

---

## **A9 — Glyph and Unicode Discipline (Non-Math)**

Avoid Unicode punctuation or spacing characters that may destabilize LaTeX rendering.

ASCII is preferred for all structural tokens.

---

## **A10 — Rendering Ownership**

Corpus documents must not enforce:

* fonts
* margins
* trim size
* recto/verso rules
* ToC styling

Rendering decisions are governed by `IER-publishing.md`.

---

## **A11 — Determinism Requirement**

Formatting constructs that behave differently when concatenated with other files are disallowed unless explicitly supported by the build system.

---

## **Chapter Intermissions**

Each chapter concludes with a brief **Intermission**.

Intermissions:

* introduce no new claims
* are not summaries or arguments
* provide structural acknowledgment only

Each Intermission contains **exactly one** of the following:

* Structural Facts
* Constraint Aphorisms
* Negative Space Markers
* Orientation Signals
* Silent Achievements

They may be read or ignored without loss of content.

---

## **Summary**

IER is governed by:

* a fixed corpus
* fixed identity and ethical commitments
* explicit epistemic limits
* global misuse containment
* mandatory reader alignment
* disciplined versioning
* deterministic authoring rules

IER does not soften its claims.

But it **does not treat good-faith readers as adversaries**.

> **IER is hard because reality is hard —
> not because the reader is at fault.**

---
