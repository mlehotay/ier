# Scripts (IER build tooling)

**Scope:** These scripts are build-layer tools. They do not define theory, canon, or corpus authority.
They implement mechanical parsing, generation, and verification for the IER repository.

**Normative status:** If prose here conflicts with the actual script behavior, the script is the
execution truth. This document exists to make interfaces stable and reviewable.

---

## Common conventions

- **Exit codes**
  - `0` = success (may still emit warnings to stderr, depending on script).
  - nonzero = failure (should fail the build).

- **Paths**
  - All file arguments are filesystem paths.
  - Scripts should be invoked from repo root unless explicitly documented otherwise.

- **Determinism**
  - Where outputs are generated (booklists, deps, audit markdown), runs should be deterministic given
    identical inputs, repo state, and configuration.

---

## `extract_book_list.py`

### Purpose
Extract a realized ordered chapter path list (“booklist”) from a selection markdown file and a scaffold directory.

### CLI
```bash
python3 scripts/extract_book_list.py [-h] selection scaffold_dir out
````

### Inputs

* `selection` (positional): selection markdown input containing backticked `.md` paths.
* `scaffold_dir` (positional): directory containing scaffolding `*.md`.
* (Implicit) filesystem state: the referenced `.md` files and scaffold files must exist for a clean run.

### Output

* `out` (positional): output path list file (booklist), containing the realized ordered `.md` paths.

### Failure / warnings

* Fails (nonzero) on unrecoverable parse/path errors (e.g., malformed selection or missing required files),
  as implemented.
* Any warning policy is implementation-defined unless promoted to a build invariant.

### Determinism contract

* Given identical selection file contents and identical `scaffold_dir` contents, the emitted booklist
  ordering should be stable.

---

## `verify_book.py`

### Purpose

Verify that a realized `booklist` is correct relative to the selection file and build invariants:
integrity, equivalence, invariants, and scoped authoring discipline.

### CLI

```bash
python3 scripts/verify_book.py [-h]
  [--skip-glyphs]
  [--skip-authoring]
  [--skip-structure]
  [--scope {canonical,all}]
  [--bare-urls {error,warn,ignore}]
  [--scaffold-dir SCAFFOLD_DIR]
  [--explain]
  [--diff-full]
  selection booklist
```

### Inputs

* `selection` (positional): selection markdown input (backticked `.md` paths).
* `booklist` (positional): emitted `booklist.txt` produced by `extract_book_list.py`.
* `--scaffold-dir`: explicit scaffold directory (recommended), used for structure checks.

### Output

* Primary output is **pass/fail** via exit code.
* Optional diagnostic output:

  * `--explain`: prints computed summaries (counts).
  * `--diff-full`: writes full numbered expected/actual lists under `build/` on list mismatch.

### Key options (behavioral contract)

* `--skip-glyphs`: skips glyph/maths unicode discipline checks.
* `--skip-authoring`: skips authoring checks (“A-rules”).
* `--skip-structure`: skips structure checks; runs authoring only.
* `--scope {canonical,all}`: scope for authoring checks (default: `canonical`).
* `--bare-urls {error,warn,ignore}`: treatment of bare URLs (default: `error`).

### Failure / warnings

* Nonzero exit indicates verification failure (should fail the build).
* Warnings may be emitted depending on `--bare-urls` and other checks.

---

## `generate_deps.py`

### Purpose

Generate derived dependency artifacts by scanning manifest-enumerated chapters for dependency metadata.

### CLI

```bash
python3 scripts/generate_deps.py [-h]
  --manifest MANIFEST
  --bundles BUNDLES
  [--out-deps OUT_DEPS]
  [--out-prereqs OUT_PREREQS]
  [--strict-metadata]
```

### Inputs

* `--manifest`: path to `IER-manifest.md` (authoritative enumeration of chapters in scope).
* `--bundles`: path to `bundles.yml` (e.g., `assets/bundles.yml`) for bundle expansion.
* (Implicit) chapter files referenced by the manifest must exist on disk for a clean run.

### Outputs

* `--out-deps OUT_DEPS` (optional): writes derived deps YAML (e.g., `build/dependencies.yml`).
* `--out-prereqs OUT_PREREQS` (optional): writes derived audit markdown
  (e.g., `build/IER-prerequisites.md`).

### Failure / warnings

* Nonzero exit indicates generation/validation failure.
* By default:

  * missing YAML front matter or missing `ier:` block should **warn** (implementation: currently configurable).
* `--strict-metadata`:

  * fails if any chapter lacks YAML front matter or lacks an `ier:` block (instead of warning).

### Determinism contract

* Given identical inputs (manifest contents, chapter files, bundles config), generated outputs should be
  stable across runs (ordering and formatting).

---

## `verify_order.py`

### Purpose

Verify that a realized book ordering respects dependency constraints, using a generated deps YAML.

### CLI

```bash
python3 scripts/verify_order.py [-h]
  --deps DEPS
  --selection SELECTION
  --booklist BOOKLIST
```

### Inputs

* `--deps`: path to generated deps YAML (e.g., `build/dependencies.yml`).
* `--selection`: path to selection file (used for context/reporting).
* `--booklist`: path to realized booklist (ordered `.md` paths).

### Output

* Pass/fail via exit code.
* Reports violations with enough context to locate the offending chapter(s), as implemented.

### Failure / warnings

* Nonzero exit indicates an ordering violation or an unreadable/invalid input artifact.

---
