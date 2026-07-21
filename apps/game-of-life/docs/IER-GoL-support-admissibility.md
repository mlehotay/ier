---
ier:
  tier: T0
  role: EXAMPLE_SUPPORT
  layer: dynamics
  domain:
    - examples
    - dynamics
  category: cellular_automaton
  status: non_canonical
  filename: IER-GoL-support-admissibility
---

# Support Admissibility in Game of Life

## Game of Life with Region-Indexed Admissibility and Support Non-Factorability

NON-CANONICAL | EXAMPLE-SCOPED | NOT READER-FACING


## Status, Scope, and Causal Boundary

This document defines a derived admissibility layer over standard Conway — s Game of Life (GoL).

It is:

- NON-CANONICAL
- EXAMPLE-SCOPED
- NOT READER-FACING

> This document defines a region-indexed admissibility structure derived from deterministic dynamics and demonstrates non-factorability at the level of admissible continuation itself.
> It extends, but does not replace, the predicate-level result.


## Causal Status

The system evolves strictly under standard GoL dynamics:

$$
s(t+1) = T(s(t))
$$

The structures introduced here:

- do not modify \(T\)
- do not introduce new transitions
- do not select outcomes
- do not introduce probabilities

They define a descriptive admissibility overlay.


## Base System

Let:

- \( G \) be a finite lattice
- \( S = \{0,1\}^G \)
- \( s(t) \in S \)


## Transition

Let:

$$
T \subseteq S \times S
$$

be the standard GoL transition.

Deterministic:

$$
\hat{s} = T(s)
$$


## Counterfactual Dependence

Define:

$$
(i,j) \in D(s)
\iff
\exists v \in \{0,1\} \text{ such that }
\hat{s}_i \ne \hat{s}^{(j \leftarrow v)}_i
$$


## Diachronic Relation

For window \(k\):

$$
C_t = \bigcup_{\tau=t-k+1}^{t} D(s(\tau))
$$


## Regions

Define live set:

$$
L(t) = \{ i \in G \mid x_i(t)=1 \}
$$

A region \( \Sigma(t) \subseteq L(t) \) is a maximal set connected under:

$\text{undirected}(C_t)$


## From Global Determinism to Regional Admissibility

Globally:

$$
A(s) = \{ \hat{s} \}
$$

This is singleton.


## Limitation of Global Admissibility

Global admissibility contains no multiplicity and always factorizes trivially under projection.

Therefore:

> global admissibility cannot express structural non-factorability.


## Strategy

Introduce region-indexed admissibility derived from:

- counterfactual perturbations
- dependence constraints
- bounded support conditions


## Regional Admissibility Structure

For region \( \Sigma \), define:

$$
A_\Sigma(s,t) \subseteq S_\Sigma
$$

as the set of admissible regional continuations.


## Definition (Support-Based Admissibility)

A regional continuation \( u \in S_\Sigma \) is admissible iff:

$$
u \in A_\Sigma(s,t)
$$

iff there exists a perturbed configuration:

$$
\tilde{s} \in N_t(s,\Sigma)
$$

such that:

### Realizability
$$
T(\tilde{s})|_\Sigma = u
$$

### Bounded Perturbation
$\tilde{s} \text{ differs from } s \text{ only within a bounded neighborhood of } \Sigma$

### Dependence Coherence
$\Sigma \text{ remains connected under } C(\tilde{s}')$

where \( \tilde{s}' = T(\tilde{s}) \)


## Interpretation

\( A_\Sigma(s,t) \) contains:

> all regional futures that can be jointly supported by some compatible completion of the surrounding configuration.

This is:

- structural
- non-probabilistic
- non-causal


## Properties of Regional Admissibility


## Non-Singleton Structure

In general:

$|A_\Sigma(s,t)| \ge 1$

Multiple regional continuations may be admissible.


## Non-Equivalence with Projection

In general:

$A_\Sigma(s,t) \ne \pi_\Sigma(A(s))$


## Dependence on Environment Completion

Admissibility is defined via existence of:

$$
v \in S_{\Sigma^c}
$$

such that:

$u \cup v \text{ is realizable and coherent}$


## Factorization and Non-Factorability

Let:

$$
\Sigma = A \cup B, \quad A \cap B = \varnothing
$$


## Induced Subregion Admissibility

Define:

$A_A(s,t), \quad A_B(s,t)$

by the same construction.


## Factorization Test

Define recombination:

$$
\mathrm{Recombine}(A,B)
=
\{ u_A \cup u_B \mid u_A \in A_A,\ u_B \in A_B \}
$$


## Non-Factorability Condition

$A_\Sigma(s,t) \ne \mathrm{Recombine}(A,B)$


## Interpretation

Some regional continuations:

- are admissible only as joint configurations
- cannot be decomposed into independently admissible subregion continuations


## Mechanism of Non-Factorability

Non-factorability arises from:


## Cross-Boundary Dependence

Relations:

$$
C_t|_{A \times B}
$$

link subregions structurally.


## Joint Support Requirement

Some \( u \in A_\Sigma \) require:

- coordinated structure across \(A\) and \(B\)
- no valid completion exists for isolated subregion variants


## Failure of Independent Completion

There exist:

- \( u \in A_\Sigma \)

such that:

- \( u|_A \notin A_A \) or
- \( u|_B \notin A_B \)


## Relation to Canonical IER Structure

This example instantiates:

> admissibility-level non-factorability


## Mapping

| Example Object | Canonical Analog |
|----------------|-------------------|
| \( A_\Sigma(s,t) \) | \( A(s) \) |
| Non-factorability | Lack of admissibility decomposition |
| Support condition | Intrinsic constraint |


## Important Limitation

This is:

- example-scoped
- derived from perturbation structure

It does not:

- define intrinsic constraint
- define identity criteria


## Replacement of Closure Predicate

Previous formulation:

- used predicate-filtered projection

New formulation:

> closure is replaced by admissibility support structure

Optional predicates may still be defined, but are secondary.


## What Is Demonstrated

This system demonstrates:

> deterministic dynamics can induce region-indexed admissibility structures that fail to factor across partitions.


## What Is Not Claimed

This system does not:

- modify GoL evolution
- introduce multiple global futures
- introduce probabilities
- define agents or observers
- establish canonical intrinsic closure


## One-Line Definition

> The model defines region-indexed admissible continuations via support under bounded perturbation, yielding admissibility structures that need not factor across partitions.


## End of Document
