# Publication Inputs (`pub/`)

This directory contains **publication-layer inputs** used to assemble
public-facing IER artifacts such as books and papers.

Nothing in this directory is canonical.
Nothing in this directory has independent theoretical authority.

All authoritative claims originate upstream in `IER/`.

---

## What This Directory Is For

Files under `pub/` exist to:

- define **what publication artifacts are**, and what roles they serve
- declare **which canonical material** is included in each artifact
- supply **publication-only framing material**
- provide **interface-layer prose** for specific audiences or venues

They exist to support **rendering, distribution, and access** —
not to define, revise, or extend the theory.

---

## What Lives Here

Content under `pub/` falls into four distinct categories.
These categories must not be conflated.

---

### 1. Artifact Specifications

These documents define the **epistemic role, constraints, and intent**
of each publication artifact.

Examples:

- `IER-corpus-book.md`
- `IER-tldr-book.md`
- `IER-foundations-book.md`
- `IER-paper-spec.md`

Artifact specifications:

- define *what an artifact is*, not how it is built
- impose constraints on tone, scope, and authority
- introduce **no theory**
- are non-canonical by design

They exist to prevent role drift between artifacts.

---

### 2. Selection Files

Selection files declare **which canonical chapters** appear in a given
publication artifact and in what order.

Examples:

- `IER-corpus-selection.md`
- `IER-foundations-selection.md`
- `IER-tldr-selection.md`

Selection files:

- contain no theory
- have no authority
- are consumed mechanically by the build system
- exist only to make inclusion and ordering explicit

Changing a selection file changes *what is included*, not *what is true*.

---

### 3. Scaffold Directories

Scaffold directories contain **publication-only framing material** such as:

- front matter
- authored part headers
- disclaimers
- closers and appendices

Examples:

- `pub/corpus-book/`
- `pub/tldr-book/` (if present)

Scaffold files:

- are non-canonical
- introduce no theoretical claims
- exist solely to support a specific artifact
- are inserted at fixed structural points by the build system

They frame artifacts without contributing content authority.

---

### 4. Interface-Layer Drafts

Some files under `pub/` contain **interface-layer prose** rather than
verbatim canonical material.

Examples:

- `IER-paper.md`

These files:

- paraphrase, summarize, or argue from canonical material
- are audience- or venue-specific
- are explicitly **non-authoritative**
- must defer to the corpus book for reference and ordering

They explain or present IER — they do not define it.

---

## What Does *Not* Live Here

This directory does **not** contain:

- canonical theory
- authoritative definitions
- corpus membership rules
- build logic or verification rules
- deployment or release policy
- legal authority

If you are looking for the theory itself, go to `IER/`.
If you are looking for rules or constraints, go to `governance/`.

---

## Relationship to Build Outputs

Files under `pub/` are **inputs**.

Generated artifacts (PDFs, EPUBs, etc.) live under `build/`
and are considered **disposable outputs**.

- Do not edit files under `build/`
- Do not cite files under `pub/` as authoritative
- Do not infer theoretical authority from formatting or presentation

Authority flows **through** publication artifacts, not **from** them.

---

## Common Category Errors

If you find yourself:

- treating a file in `pub/` as the definitive statement of IER
- resolving a theoretical question by editing an artifact spec
- assuming publication order implies authority
- inferring authority from trim size, layout, or venue
- modifying interface prose to “clarify” canonical claims

You are operating in the **wrong layer**.

---

## Final Reminder

The definitive reference surface for IER is the
**IER Corpus Book**, assembled mechanically from canonical material.

Everything in `pub/` exists to support that surface —
never to replace it.

---
