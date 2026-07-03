---
ier:
  tier: T0
  role: EXAMPLE_SUPPORT
  layer: math
  domain:
    - math
    - examples
  category: register_extension
  status: non_canonical
  filename: IER-math-register-expansion
---

# Game of Life Math Register Expansion

## Localized Projection, Predicate-Filtered Continuation, and Diachronic Relations (Example-Scoped Extension)

NON-CANONICAL | EXAMPLE-SCOPED | NOT READER-FACING


## Status, Scope, and Authority Boundary

This document defines a minimal extension of the IER mathematical register for use in example systems such as cellular automata.

This document is:

* NON-CANONICAL
* EXAMPLE-SCOPED
* NOT READER-FACING

It:

* introduces no ontological primitives
* introduces no causal powers
* introduces no epistemic or measurement structure
* does not modify canonical admissibility or constraint definitions
* does not extend IER identity claims

All canonical authority remains in:

```text
IER-math
IER-futures
IER-constraint
````


## Purpose

The canonical IER register already supports:

* configuration sets (S)
* transition relations (T)
* admissible regimes (R)
* admissible futures (A(s), A_t)
* trajectories \(\gamma\)
* partitions and projections

Example systems require additional ability to express:

* restriction of configurations to subregions
* projection of admissible futures onto subregions
* deterministic singleton projection cases
* post-projection predicate filtering
* counterfactual perturbations of configurations
* diachronic dependence between components
* restriction-sensitive closure evaluation
* structural comparison of projected continuations

This document introduces only the minimal notation required.


## Design Constraints

All additions must remain:

* set-theoretic
* relational
* deterministic
* non-probabilistic
* non-optimizing
* structurally interpretable

No heavy formalism is introduced.


## Additional Allowed Forms

## Region Symbols

Use:

$\Sigma$

to denote a subset of the configuration domain.

Use:

$\Sigma^c$

for its complement.

For partitions, use:

$$
\Sigma = A \cup B, \quad A \cap B = \varnothing
$$


## Restriction of Configurations

Use:

$s|_{\Sigma}$

to denote restriction of configuration (s) to region \(\Sigma\).

This is a structural restriction only.

It does not imply subsystem autonomy, enclosure, or informational isolation.


## Projection of Admissible Futures

The canonical admissible future set is:

$$
A_t = { s' \mid (s(t), s') \in T }
$$

Define projection:

$$
\pi_{\Sigma}(A_t)
=================

{ s'|_{\Sigma} \mid s' \in A_t }
$$


### Notation Convention

The shorthand:

$A_t(\Sigma)$

may be used only as notation for \(\pi_{\Sigma}(A_t)\).

It does not introduce an independent subsystem-level admissibility relation.


## Deterministic Singleton Case

In deterministic examples where:

$$
A_t = { \hat{s} }
$$

projection reduces to:

$$
\pi_{\Sigma}(A_t) = { \hat{s}|_{\Sigma} }
$$

This may be treated as a named special case when useful.


## Predicate-Filtered Projection

Where explicitly defined in an example:

$$
A_t^{\mathrm{bd}}(\Sigma)
\subseteq
\pi_{\Sigma}(A_t)
$$

denotes a subset obtained by applying a structural predicate after projection.

Typical form:

$$
A_t^{\mathrm{bd}}(\Sigma)
=========================

{ \hat{s}|*{\Sigma} \mid P(\Sigma,\hat{s}|*{\Sigma}) }
$$

where (P) is defined locally in the example.

This notation does not introduce a new transition relation or subsystem dynamics.


## Predicate Form

Example systems may define structural predicates of the form:

$P(\Sigma, s|_{\Sigma})$

to represent region-relative conditions on restricted configurations or projected continuations.

Typical uses include:

* closure
* boundedness
* internal connectivity
* external independence

These predicates remain:

* structural
* local to the example
* non-causal
* non-probabilistic


## Counterfactual Replacement

Use:

$s^{(j \leftarrow 0)}, \quad s^{(j \leftarrow 1)}$

or more generally:

$s^{(j \leftarrow v)}$

to denote replacement of component (j) by value (v).

This is a structural operation only.

It must not be interpreted as:

* probabilistic branching
* modal semantics
* epistemic uncertainty


## Diachronic Relations

Use:

$$
C_t \subseteq G \times G
$$

to denote a time-indexed relation over components.

Typical uses include:

* sustained dependence
* interaction persistence
* historical coupling

This remains a standard relation.


## Structural Difference

Use:

$A \triangle B$

for symmetric difference between sets.

This may be used to express structural change under perturbation, but is optional and example-dependent.


## Derived Forms

## Relation Restriction

For region \(\Sigma\):

$$
C_t|_{\Sigma \times \Sigma}
$$

denotes internal relations.

$$
C_t|_{\Sigma \times \Sigma^c}
$$

denotes boundary-crossing relations.

Equivalent intersection forms are also allowed:

$$
C_t \cap (\Sigma \times \Sigma)
$$

$$
C_t \cap (\Sigma \times \Sigma^c)
$$


## Empty-or-Singleton Predicate-Filtered Case

In deterministic examples with projected singleton futures, predicate-filtered projection may satisfy:

$$
A_t^{\mathrm{bd}}(\Sigma) \in { \varnothing, { \hat{s}|_{\Sigma} } }
$$

This expresses:

* no multiplicity of futures
* no branching structure
* predicate filtering only


## Typed Regional Recombination

Let:

$$
\Sigma = A \cup B, \quad A \cap B = \varnothing
$$

Define recombination:

$$
\mathrm{Recombine}(A,B)
=======================

{ a \cup b \mid a \in X,\ b \in Y }
$$

for sets (X,Y) of restricted configurations.


### Projection Factorization Form

$$
\pi_{\Sigma}(A_t)
=================

{ a \cup b \mid a \in \pi_A(A_t),\ b \in \pi_B(A_t) }
$$

This is a statement about projected admissible futures only.


### Predicate-Filtered Factorization Form

$$
A_t^{\mathrm{bd}}(\Sigma)
\stackrel{?}{=}
{ a \cup b \mid a \in A_t^{\mathrm{bd}}(A),\ b \in A_t^{\mathrm{bd}}(B) }
$$

This is a statement about predicate-filtered projected continuation, not about future multiplicity.


### Interpretation

Failure indicates:

> non-decomposability of a structural predicate on projected continuation under the locally defined example conditions.


## Componentwise Perturbation Sensitivity

Sensitivity may be expressed directly by comparison of restricted or component values under perturbed updates.

Typical form:

$\hat{s}_i \ne \hat{s}^{(j \leftarrow v)}_i$

This is the preferred form when the example defines dependence by local update sensitivity.


## Setwise Perturbation Sensitivity

Where useful, structural sensitivity may also be expressed as:

$\pi_{\Sigma}(A_t) \triangle \pi_{\Sigma}(A_t^{(j \leftarrow 0)})$

This remains purely set-theoretic.

It should not be treated as primary unless the example actually uses setwise difference.


## Restriction-Sensitive Evaluation

Some example predicates are evaluated relative to the region under consideration.

In such cases:

* evaluating \(P(\Sigma,\cdot)\) may use one relation structure
* evaluating \(P(A,\cdot)\) or \(P(B,\cdot)\) may use restricted relation structure

Therefore:

> structural predicates need not be preserved under restriction.

This rule is especially important when a predicate depends on internal connectivity or boundary-crossing relations.


## Interpretation Discipline

All added notation is strictly structural.

In particular:

* \(\Sigma\) does not imply ontological enclosure
* \(s|_{\Sigma}\) does not imply subsystem isolation
* \(\pi_{\Sigma}(A_t)\) does not imply independent subsystem dynamics
* \(A_t^{\mathrm{bd}}(\Sigma)\) does not imply causal restriction unless explicitly defined in the example
* \(P(\Sigma,\cdot)\) does not introduce a new law of motion
* (C_t) does not imply force, signal, or mechanism
* counterfactual notation does not introduce modality
* \(\triangle\) does not imply metric structure


## Disallowed Extensions

This document does not permit introduction of:

* probability or entropy
* optimization operators
* weighted graph structures without explicit example need
* dynamical fields or potentials
* measurement procedures
* observer-dependent quantities


## Minimal Summary

This extension adds only:

* region indexing: \(\Sigma\)
* restriction: \(s|_{\Sigma}\)
* projection: \(\pi_{\Sigma}(A_t)\)
* shorthand: \(A_t(\Sigma)\)
* deterministic singleton projection case
* predicate-filtered projection: \(A_t^{\mathrm{bd}}(\Sigma)\)
* predicate form: \(P(\Sigma, s|_{\Sigma})\)
* counterfactual replacement: \(s^{(j \leftarrow \cdot)}\)
* diachronic relations: (C_t)
* relation restriction
* typed recombination for factorization
* optional structural difference: \(\triangle\)


## Role Within Examples

This extension supports example systems such as:

* cellular automata
* lattice interaction systems
* discrete dynamical models with localized structure

It is especially suited to examples where:

* global dynamics remain deterministic
* projection is trivial
* non-decomposability arises from region-relative predicates rather than from multiplicity of futures

It does not modify or generalize canonical IER mathematics.


## End of Document
