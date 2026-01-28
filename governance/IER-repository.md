# **IER Repository Management and Release Workflow**

**Applies to:** All repositories hosting Informational Experiential Realism (IER) artifacts
**Primary audience:** Maintainers, collaborators, and contributors

---

## **0. Status and Scope**

This document defines the **repository-level governance and workflow** for managing Informational Experiential Realism (IER), including:

* versioning discipline
* git tagging and release mechanics
* management of the repository export subtree
* synchronization between private and public repositories
* acceptance and integration of public contributions

This document governs **how IER is managed as a versioned body of work in source control**.

It does **not**:

* define theoretical content, ontology, or commitments
* define canonical authority
* define corpus membership or ordering
* define deployment order or publication sequencing
* define rendering, layout, or production rules

Those concerns are governed upstream or elsewhere by:

1. `IER-canon.md`
2. `IER-manifest.md`
3. `IER-publishing.md`
4. Future documents such as `IER-deployment.md`

In the event of conflict, **authority always resolves upstream** in favor of the canon and manifest.

---

## **1. Authority Model**

### **1.1 Repository as Upstream Authority**

The git repository is the **authoritative substrate** of Informational Experiential Realism.

All public artifacts — including but not limited to:

* the Seed Release
* corpus books
* preprints and papers
* interface or explanatory publications
* digital or print editions

are **derived projections** of specific, tagged repository states.

No publication venue, file format, or distribution mechanism confers authority or supersedes the repository and its tags.

---

### **1.2 Private and Public Repositories**

IER is maintained across two repositories:

* a **private repository**, which is the authoritative development environment
* a **public repository**, which is a curated export of selected material

The public repository is **not a peer authority**. It exists to distribute, discuss, and collaborate on material explicitly designated for public exposure.

All authoritative changes originate upstream in the private repository.

---

## **2. Repository Structure**

### **2.1 Canonical Directories**

The repository may contain multiple top-level directories serving distinct roles. Typical examples include:

* `IER/` — authoritative working corpus (private)
* `pub/` — publishing workspace
* `pub/ier-repo/` — repository export surface
* `governance/` — governance and workflow documents
* `docs/` or similar — non-authoritative supporting material

Directory names are conventional; authority is determined by governance rules, not naming alone.

---

### **2.2 The `pub/ier-repo/` Boundary**

The directory:

```
pub/ier-repo/
```

defines a **hard repository export boundary**.

Rules:

* Only files under `pub/ier-repo/` may be synchronized to the public repository.
* Files outside `pub/ier-repo/` are treated as private or internal by default.
* Accidental inclusion of material under `pub/ier-repo/` is considered a release-blocking error.

This directory is the **sole mechanism** by which material becomes eligible for public repository release.

---

## **3. Public Repository Synchronization**

### **3.1 Subtree Model**

The public repository is synchronized from the private repository using a **subtree workflow** rooted at `pub/ier-repo/`.

Key properties:

* only the contents of `pub/ier-repo/` are exported
* private history and files are not disclosed
* synchronization may occur in either direction

The subtree model preserves a single authoritative history while enabling public collaboration.

---

### **3.2 Synchronization Procedures**

#### **Private → Public**

* Changes under `pub/ier-repo/` are committed in the private repository.
* Updates are pushed to the public repository as a subtree.
* Tags intended for public reference are mirrored.

#### **Public → Private**

* Public pull requests may be merged in the public repository.
* Changes are periodically pulled upstream using squash commits.
* Upstream authority is preserved.

---

## **4. Versioning Discipline**

### **4.1 Global IER Version Namespace**

IER uses a **single, global version namespace**, for example:

```
v10.8.4
v10.9.0
```

Version numbers are:

* unique
* authoritative
* consistent across all repositories and venues

Public and private repositories must never diverge in version numbering.

---

### **4.2 Meaning of an IER Version**

An IER version fixes:

* a determinate set of theoretical commitments
* a stable identity claim
* enforced structural exclusions
* fixed ethical consequences

An IER version does **not** imply:

* publication completeness
* pedagogical readiness
* availability of interface artifacts
* existence of a corpus book

Versioning tracks **commitments**, not presentation.

---

## **5. Tagging Policy**

### **5.1 Annotated Tags as Authority**

Annotated git tags are the **authoritative markers** of IER versions and release events.

Tags:

* must be immutable once published
* must never be moved, deleted, or rewritten
* define the exact repository state being referenced

GitHub Releases, if used, are **derivative** and must reference existing tags.

---

### **5.2 Core Version Tags**

Each IER version is marked by exactly one core tag:

```
ier-vX.Y.Z
```

This tag identifies the authoritative repository state for that version.

Creation of a core version tag constitutes a **version publication event**.

---

### **5.3 Release and Projection Tags**

Additional tags may be created to anchor specific release events derived from a given version, for example:

```
ier-v10.9.0-preprint
ier-v10.9.0-corpus-book
ier-v10.9.0-foundations
```

These tags:

* do not create new IER versions
* reference commits compatible with the associated version
* exist solely to anchor distribution or publication events

---

## **6. Release Classes**

### **6.1 The Seed Release (One-Time)**

The IER Seed Release is a **single, non-recurring publication event**.

Its purpose is to:

* establish the public existence of Informational Experiential Realism
* fix the identity claim, structural exclusions, and ethical consequences
* anchor priority independent of later completeness

The Seed Release:

* occurs exactly once

* is identified by the unique, immutable tag:

  ```
  ier-seed
  ```

* may be associated with a versioned tag (e.g. `ier-v10.9.0-seed`) for reference

* does not establish reference authority or corpus completeness

No subsequent release is a Seed Release.

---

## **7. Graduation of Content to the Repository Export Surface**

Files become publicly visible through **deliberate graduation** into `pub/ier-repo/`.

Rules:

* Graduation is intentional and reviewable.
* Newly graduated files appear only in **future tags**.
* Past tags are not retroactively modified.

Graduation constitutes a public commitment at the next applicable release.

---

## **8. Public Contributions and Pull Requests**

### **8.1 Acceptable Scope**

The public repository may accept pull requests that:

* correct typographical or formatting errors
* improve metadata, tooling, or CI
* improve presentation of existing public material
* propose changes via discussion or issues

Public contributions must not:

* alter canonical commitments
* modify identity claims or exclusions
* advance IER versions
* create or modify tags

---

### **8.2 Upstream Integration**

Accepted public contributions:

* are merged in the public repository
* may be pulled upstream using squash commits
* do not acquire canonical authority by virtue of acceptance

All canonical changes originate upstream.

---

## **9. Immutability and Change Discipline**

Once a tag has been published:

* its contents are treated as immutable
* silent replacement is prohibited

Corrections:

* require new commits
* require new tags if they affect released material

Immutability applies equally to free and commercial distributions.

---

## **10. Tooling and Conventions (Non-Normative)**

The project may adopt:

* git aliases for subtree synchronization and tagging
* CI checks for `pub/ier-repo/` hygiene
* GitHub Releases for distribution convenience

These tools are advisory and do not confer authority.

---

## **11. Governance Evolution**

This document may evolve as IER governance matures.

Revisions:

* must not retroactively alter published tags
* must preserve established authority relationships
* may introduce new workflows or artifact classes

---

## **Final Statement**

> The IER repository is the upstream authority from which all public artifacts derive.
>
> Versions fix commitments.
> Tags fix history.
> Publications project, but do not govern.

---
