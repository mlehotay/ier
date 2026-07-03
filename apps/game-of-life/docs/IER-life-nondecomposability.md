---
ier:
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

This document establishes a structural result for the system defined in:

```

IER-GoL-predicate-nondecomposability

```

It proves:

> closure predicates on projected continuations need not factor across partitions,
> even when admissible futures are singleton and projection factorizes trivially.


## Relation to Other Constructions

This result concerns:

> predicate-level non-decomposability

It does not establish:

> non-factorability of admissible futures themselves.

A separate construction (*IER-GoL-support-admissibility*) addresses admissibility-level non-factorability.


## Setup

Let:

- \( G \) be the lattice
- \( s(t) \in S \) the configuration
- \( T \) the Game of Life transition
- \( \hat{s} = T(s(t)) \)

Let:

- \( D(s) \subseteq G \times G \) be counterfactual dependence
- \( C_t \subseteq G \times G \) be the diachronic relation

Let:

- \( \Sigma(t) \subseteq G \) be a region
- \( \Sigma = A \cup B \), \( A \cap B = \varnothing \)


## Deterministic Invariant

Game of Life is deterministic:

$$
A_t = \{ \hat{s} \}
$$

Therefore:

$$
\pi_{\Sigma}(A_t) = \{ \hat{s}|_{\Sigma} \}
$$


## Singleton Structure

For all regions \( \Sigma \):

$\pi_{\Sigma}(A_t) \text{ is singleton}$


## Consequences

- no multiplicity of futures
- no branching structure
- no probabilistic interpretation


## Key Implication

> All structural phenomena arise from predicate evaluation, not from admissible future structure.


## Closure Predicate

Let:

$P(\Sigma, \hat{s}|_{\Sigma})$

denote the closure condition defined via diachronic dependence:

$$
C(s') \quad \text{where } s' = \hat{s}
$$


## Bounded Projection

Define:

$$
A_t^{\mathrm{bd}}(\Sigma)
=
\{ \hat{s}|_{\Sigma} \mid P(\Sigma, \hat{s}|_{\Sigma}) \}
$$


## Structure

$$
A_t^{\mathrm{bd}}(\Sigma)
\in
\{ \varnothing,\ \{ \hat{s}|_{\Sigma} \} \}
$$


## Interpretation

Bounded projection is:

> predicate-filtered projection of a singleton future


## Decomposability Framework

Let:

$$
\Sigma = A \cup B, \quad A \cap B = \varnothing
$$


## Subregion Projections

$A_t^{\mathrm{bd}}(A), \quad A_t^{\mathrm{bd}}(B)$


## Recombination

$$
\mathrm{Recombine}(A,B)
=
\{ a \cup b \mid a \in A_t^{\mathrm{bd}}(A),\ b \in A_t^{\mathrm{bd}}(B) \}
$$


## Decomposability Condition

$$
A_t^{\mathrm{bd}}(\Sigma)
=
\mathrm{Recombine}(A,B)
$$


## Main Result

## Proposition (Predicate Non-Decomposability)

There exist configurations \( s(t) \), regions \( \Sigma \), and partitions \( \Sigma = A \cup B \) such that:

$$
P(\Sigma, \hat{s}|_{\Sigma}) = \text{true}
$$

but:

$$
P(A, \hat{s}|_A) = \text{false}
\quad \text{or} \quad
P(B, \hat{s}|_B) = \text{false}
$$


## Equivalent Form

$A_t^{\mathrm{bd}}(\Sigma) \ne \varnothing$

but:

$$
A_t^{\mathrm{bd}}(A) = \varnothing
\quad \text{or} \quad
A_t^{\mathrm{bd}}(B) = \varnothing
$$


## Conclusion

$A_t^{\mathrm{bd}}(\Sigma) \ne \mathrm{Recombine}(A,B)$


## Mechanism

Non-decomposability arises from:

> restriction-sensitive evaluation of dependence structure


## Dependence Structure

Closure depends on:

$$
C(s') \subseteq G \times G
$$

including cross-boundary relations:

$$
C(s')|_{A \times B}
$$


## Whole-Region Evaluation

For \( \Sigma \):

- full dependence structure is present
- closure may hold


## Subregion Evaluation

For \( A \) or \( B \):

- cross-boundary relations are removed
- closure may fail


## Key Property

> Dependence relations used in closure are not preserved under restriction


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

> Closure predicates on projected continuations are not, in general, compositional under restriction, even when admissible futures are singleton.


## End of Document
