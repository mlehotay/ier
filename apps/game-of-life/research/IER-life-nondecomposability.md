---
ier:
  aligned_with:
  - certification: 10.11.6-core-theory-r2
    reviewed_on: '2026-10-17'
    review: b8fada6a831d790d66f1c65b33ed8877577575f1:traceability/reviews/batch-17.md#member-12
  tier: T1
  role: EXAMPLE_RESULT
  layer: projection
  domain:
    - examples
    - projection
  category: structural_result
  status: non_canonical
  filename: IER-life-nondecomposability
---

# Life Nondecomposability

## Predicate Non-Decomposability in Game of Life Under Diachronic Dependence

NON-CANONICAL | EXAMPLE-SCOPED | NOT READER-FACING


## Status and Scope

This document gives the conditional set argument for the [predicate construction](IER-GoL-predicate-nondecomposability.md). A computed configuration/history witnessing its GoL premises is still required. The argument establishes:

> closure predicates on projected continuations need not factor across partitions,
> even when admissible futures are singleton and projection factorizes trivially.


## Relation to Other Constructions

This result concerns:

> predicate-level non-decomposability

It does not establish:

> non-factorability of admissible futures themselves.

The separate [support proposal](IER-GoL-support-admissibility.md) asks about regional perturbation supports; it does not establish non-factorization of the actual GoL successor fibre.


## Canonical Boundary

All constructs here are example-scoped descriptions. A singleton next-state
set at a fixed complete configuration factorizes under same-fibre projection.
Predicate filtering and regional support formed from alternative starting
configurations are different objects, not evidence that the actual GoL
successor fibre has changed.

[Intrinsic Closure](../../../IER/IER-intrinsic-closure.md)
requires an independently declared physical candidate, variables, grain,
interval, interventions and exhaustive meaningful partition family. Even a
properly typed support failure establishes only the tested synchronic relation.
[Ownership](../../../IER/IER-ownership.md),
[all-five constitutive membership](../../../IER/IER-processes.md)
(contribution, global bearing, joint resolution, uptake and common typing), and
[all-five continuity](../../../IER/IER-continuity.md)
(continuous owned closure, successor production, typed effective inheritance,
mediated change and unique successor/predecessor) remain separate burdens.
These documents establish none of them for a physical computer or organism.
The identity proposal is abductive; the simulation is no experience detector.

## Setup

Let:

- $G$ be the lattice
- $s(t) \in S$ the configuration
- $T$ the Game of Life transition
- $\hat{s} = T(s(t))$

Let:

- $D(s) \subseteq G \times G$ be counterfactual dependence
- $C_t \subseteq G \times G$ be the diachronic relation

Let:

- $\Sigma(t) \subseteq G$ be a region
- $\Sigma = A \cup B$, $A \cap B = \varnothing$


## Deterministic Invariant

Game of Life is deterministic:

$$ A_t = \{ \hat{s} \} $$

Therefore:

$$ \pi_{\Sigma}(A_t) = \{ \hat{s}|_{\Sigma} \} $$


## Singleton Structure

For all regions $\Sigma$:

$\pi_{\Sigma}(A_t) \text{ is singleton}$


## Consequences

- no multiplicity of futures
- no branching structure
- no probabilistic interpretation


## Key Implication

> In this filtered-set comparison, any factorization failure arises from predicate evaluation; the fixed-state successor remains singleton. Life’s broader dynamics are not thereby trivial.


## Closure Predicate

Let:

$P(\Sigma, \hat{s}|_{\Sigma})$

denote the closure condition defined via diachronic dependence:

$$ C(s') \quad \text{where } s' = \hat{s} $$


## Bounded Projection

Define:

$$ A_t^{\mathrm{bd}}(\Sigma) = \{ \hat{s}|_{\Sigma} \mid P(\Sigma, \hat{s}|_{\Sigma}) \} $$


## Structure

$$ A_t^{\mathrm{bd}}(\Sigma) \in \{ \varnothing,\ \{ \hat{s}|_{\Sigma} \} \} $$


## Interpretation

Bounded projection is:

> predicate-filtered projection of a singleton future


## Decomposability Framework

Let:

$$ \Sigma = A \cup B, \quad A \cap B = \varnothing $$


## Subregion Projections

$A_t^{\mathrm{bd}}(A), \quad A_t^{\mathrm{bd}}(B)$


## Recombination

$$ \mathrm{Recombine}(A,B) = \{ a \cup b \mid a \in A_t^{\mathrm{bd}}(A),\ b \in A_t^{\mathrm{bd}}(B) \} $$


## Decomposability Condition

$$ A_t^{\mathrm{bd}}(\Sigma) = \mathrm{Recombine}(A,B) $$


## Main Result

## Proposition (Predicate Non-Decomposability)

Assume a configuration/history, region $\Sigma$, and nonempty disjoint partition $\Sigma = A \cup B$ satisfying:

$$ P(\Sigma, \hat{s}|_{\Sigma}) = \text{true} $$

but:

$$ P(A, \hat{s}|_A) = \text{false} \quad \text{or} \quad P(B, \hat{s}|_B) = \text{false} $$


## Equivalent Form

$A_t^{\mathrm{bd}}(\Sigma) \ne \varnothing$

but:

$$ A_t^{\mathrm{bd}}(A) = \varnothing \quad \text{or} \quad A_t^{\mathrm{bd}}(B) = \varnothing $$


## Conclusion

$A_t^{\mathrm{bd}}(\Sigma) \ne \mathrm{Recombine}(A,B)$

Under the assumptions, the left side contains its one projected successor. At least one factor on the right is empty, so recombination is empty. This proves the implication, not existence of a GoL history meeting its premises.


## Mechanism

Non-decomposability arises from:

> restriction-sensitive evaluation of dependence structure


## Dependence Structure

Closure depends on:

$$ C(s') \subseteq G \times G $$

including cross-boundary relations:

$$ C(s')|_{A \times B} $$


## Whole-Region Evaluation

For $\Sigma$:

- full dependence structure is present
- closure may hold


## Subregion Evaluation

For $A$ or $B$:

- formerly internal cross-cut relations become external to each subregion; external independence must be checked against the full relation
- closure may fail


## Key Property

> Internal connectivity and absence of external dependence need not survive a changed evaluation boundary.


## Nature of the Result

This is a failure of:

> predicate factorization under projection


## Not a Failure Of

- admissible future factorization
- transition structure
- determinism


## Precise Statement

> Projection factorizes, but predicate evaluation does not.


## Conceptual Interpretation

This result shows:

> Deterministic systems with trivial future structure can exhibit non-decomposable structural predicates.


## Important Distinction

- admissible futures: trivial and factorable
- structural validity (closure): non-compositional


## Relation to IER

This example instantiates:

> structural constraint appearing as non-compositional evaluation over projected continuation


## IER Mapping

- flow: unchanged (GoL dynamics)
- admissible futures: singleton
- constraint appearance: predicate filtering
- non-decomposability: predicate-level


## One-Line Result

> Whole-region success and subregion failure imply predicate non-decomposability even for a singleton successor. A concrete GoL witness is an open example-development task.


## End of Document
