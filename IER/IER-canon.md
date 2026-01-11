# **IER Canon (v10.8.1)**

## **Canonical Authority, Alignment, and Governance Rules**

---

## **Status and Purpose**

This document defines the **canonical authority rules, interpretive constraints, alignment requirements, conflict resolution order, and version discipline** governing the **Informational Experiential Realism (IER)** project as of **version 10.8.1**.

It introduces **no ontological, criterial, epistemic, or ethical claims**.

It governs **how documents relate to one another**, not what the theory claims.

---

## **Corpus vs Canon (Definitions)**

**Corpus** refers to the **set of documents explicitly enumerated in `IER-manifest.md`** for a given version.

* Inclusion in the corpus means:

  * the document is part of “IER v10.8.1”
  * it is subject to the governance rules of this canon
  * it may be cited for orientation, exposition, or structure as appropriate

**Canon** refers to the **binding authority rules and interpretive constraints** that govern how the corpus is read, aligned, versioned, and adjudicated.

* Canon:

  * is **not a document list**
  * does **not expand or modify theory claims**
  * determines what kinds of statements are admissible, authoritative, or disallowed

**Canonical vs Non-Canonical Corpus Material**

Within the corpus:

* **Canonical-authoritative documents** may introduce or fix binding commitments (as authorized by tier).
* **Non-canonical corpus documents** are included for publication, pedagogy, assembly, or orientation, but:

  * introduce **no new authoritative commitments**
  * may not be cited to resolve theoretical disputes
  * must defer to canonical-authoritative material in all cases

**Non-corpus material** consists of files present in the repository but **not enumerated in the manifest**. These are outside the scope of IER v10.8.1.

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

Here is a **maximally condensed**, still **normative and unambiguous**, version of the Typography section. This is about as short as it can get without losing enforceability.

---

## **Typography and Notation**

IER uses typography to mark **ontological and formal distinctions**.

* **Math mode (`$...$`)** is used only for formal structure:
  variables (`$a$`), sets (`$S$`), relations (`$T \subseteq S \times S$`), operators (`$\neq$, $\Rightarrow$, $\Delta$`), and tuples (`$(S,T)$`).
  Sentence punctuation appears **outside** math mode.

* **Conceptual terms inside math** must be wrapped in `\text{...}`:

  ```latex
  $\text{participation} \neq \text{subjecthood}$
  ```

* **Do not** use `\text{}` for genuine variables or symbols.

* **Unicode math glyphs are forbidden.** Use LaTeX operators (`\neq`, `\Rightarrow`, `\subseteq`, `\in`, `\equiv`, `\Delta`).

* **Negation** uses `\neg` in math mode.

* **Backticks** name files or identifiers only; never mathematics.

* **Capitalization** reflects defined constructs, not emphasis.

These rules are **project-canonical** and apply to all IER documents.

---

## **Summary**

IER is governed by:

* a fixed corpus, explicitly enumerated
* fixed identity and ethical commitments
* explicit epistemic limits
* global misuse containment
* mandatory reader alignment
* disciplined versioning

IER does not soften its claims.

But it **does not treat good-faith readers as adversaries**.

> **IER is hard because reality is hard —
> not because the reader is at fault.**

---
