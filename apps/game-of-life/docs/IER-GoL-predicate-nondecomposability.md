---
ier:
  tier: T0
  role: EXAMPLE
  layer: dynamics
  domain:
    - examples
    - dynamics
  category: cellular_automaton
  status: non_canonical
  filename: IER-GoL-predicate-nondecomposability
---

# Predicate Nondecomposability in Game of Life

## Game of Life with Diachronic Dependence and Structural Closure (Descriptive Layer)

NON-CANONICAL | EXAMPLE-SCOPED | NOT READER-FACING


## Status, Scope, and Causal Boundary

This document defines a derived structural layer over standard Conway's Game of Life.

It is:

* NON-CANONICAL
* EXAMPLE-SCOPED
* NOT READER-FACING

> This document demonstrates non-decomposability arising from predicate evaluation over projected continuation in a system with singleton admissible futures.
> It does not introduce non-factorability at the level of admissibility itself.


## Causal Status

The system evolves strictly under standard Game of Life dynamics.

The additional structures defined here:

* do not modify the transition relation
* do not select or reject transitions
* do not introduce causal influence

Formally:

$$
R = T
$$

All additional constructs are descriptive only.


## Base System

Let:

$G$

be a finite lattice.

A configuration is:

$$
s(t) \in S
$$

where:

$$
s(t) = \{x_i(t)\}_{i \in G}, \quad x_i(t) \in \{0,1\}
$$


## Transition

Let:

$$
T \subseteq S \times S
$$

be the standard Game of Life transition relation.

Since deterministic:

$$
s(t+1) = \hat{s} = T(s(t))
$$


## Admissible Futures

$$
A_t = \{ \hat{s} \}
$$


## Invariant (Singleton Structure)

For all regions \( \Sigma \):

$$
A_t^{\mathrm{bd}}(\Sigma) \in \{ \varnothing, \{ \hat{s}|_{\Sigma} \} \}
$$


## Implication

* no multiplicity of futures
* no branching structure
* no probabilistic interpretation

All structural phenomena arise from predicate filtering, not from properties of \(A_t\).


## Relation to Global Temporal Structure (Omniperiodicity)

Game of Life is known to be omniperiodic, i.e.:

> for every \( n \in \mathbb{N} \), there exists a configuration whose global trajectory has period \( n \).

This result concerns the existence of global trajectories over time.

### Non-Interaction with This Construction

The present system operates at a fixed time \(t\), where:

$$
A_t = \{ \hat{s} \}
$$

Thus:

* admissible futures remain singleton
* projection remains trivial
* bounded projection remains empty or singleton

### Consequence

Omniperiodicity does not introduce:

* multiplicity of futures
* branching structure
* non-determinism

Therefore:

> Non-decomposability in this system does not arise from temporal richness or long-horizon behavior.

### Correct Placement

Omniperiodicity concerns global trajectory existence, whereas this construction concerns:

> predicate-filtered projected continuation at a single step

These operate at different levels and do not interact.


## Counterfactual Dependence

For neighboring sites \(i,j\), define:

$s^{(j \leftarrow 0)}, \quad s^{(j \leftarrow 1)}$

Let:

- \(\hat{s}\) = update from \(s\)
- \(\hat{s}^{(j \leftarrow v)}\) = update from perturbed configuration


## Dependence Relation

$$
(i,j) \in D(s)
\iff
\exists v \in \{0,1\}
\text{ such that }
\hat{s}_i \ne \hat{s}^{(j \leftarrow v)}_i
$$


## Interpretation

* captures local update sensitivity
* purely structural
* no probabilistic meaning


## Diachronic Dependence Relation

Let persistence window \(k \ge 1\).

Define:

$$
(i,j) \in C_t
\iff
\exists \tau \in [t-k+1, t] \text{ such that } (i,j) \in D(s(\tau))
$$


## Interpretation

* sliding-window accumulation of dependence
* fully specified
* no recursion ambiguity


## Region Structure

Define live set:

$$
L(t) = \{ i \in G \mid x_i(t)=1 \}
$$


## Region Definition

A region \( \Sigma(t) \subseteq G \) is a maximal subset such that:

1. \( \Sigma(t) \subseteq L(t) \)
2. for all \(i,j \in \Sigma(t)\), there exists a path:

$$
i = i_0, \dots, i_n = j
$$

with:

$$
(i_m, i_{m+1}) \in C_t
$$

Connectivity is evaluated on the undirected interpretation of \(C_t\).


## Internal and External Dependence

For region \( \Sigma \):

$$
C_t^{\mathrm{in}}(\Sigma) = C_t \cap (\Sigma \times \Sigma)
$$

$$
C_t^{\mathrm{out}}(\Sigma) = C_t \cap (\Sigma \times \Sigma^c)
$$


## Boundedness

A region \( \Sigma(t) \) is bounded if:


## Internal Dependence

$C_t^{\mathrm{in}}(\Sigma) \ne \varnothing$


## External Independence

$$
C_t^{\mathrm{out}}(\Sigma) = \varnothing
$$


## Interpretation

* internally sustained
* no recent dependence on external cells


## Projection of Admissible Futures

Since:

$$
A_t = \{ \hat{s} \}
$$

projection always yields:

$$
\pi_{\Sigma}(A_t) = \{ \hat{s}|_{\Sigma} \}
$$


## Key Property

Projection of admissible futures always factorizes trivially.


## Candidate Future Evaluation

Let:

$$
s' = \hat{s}
$$

Compute:

$D(s'), \quad C(s')$


## Region Continuation

## Conservative Definition (Default)

$$
\Sigma'(t+1) = \{ i \in \Sigma(t) \mid x_i'(t+1)=1 \}
$$

New cells are not included.


## Optional Variant: Recruitment

$$
\Sigma'(t+1)
=
\text{maximal } C(s')\text{-connected set containing surviving members}
$$


## Status

The default model uses conservative continuation.


## Structural Closure Condition

Closure is defined as a predicate on projected continuation.


## Internal Connectivity

$\Sigma'(t+1) \text{ is connected under } C(s')$


## External Independence

$$
C(s') \cap (\Sigma'(t+1) \times \Sigma'^c(t+1)) = \varnothing
$$


## Evaluation Rule (Important)

Closure is evaluated relative to the region under consideration.

That is:

* evaluating \( \Sigma \) uses full dependence structure over \( \Sigma \)
* evaluating subregions uses restricted dependence structure


## Implication

> Closure is not preserved under restriction.


## Bounded Projection

Define:

$$
A_t^{\mathrm{bd}}(\Sigma)
=
\{ \hat{s}|_{\Sigma} \mid \text{closure holds under } s' \}
$$


## Key Property

This set is:

* either singleton
* or empty


## Interpretation

> Bounded projection is predicate-filtered projection, not a new future structure.


## Source of Non-Decomposability

Non-decomposability does not arise from:

* transition relation \(T\)
* admissible futures \(A_t\)
* projection \( \pi_{\Sigma}(A_t) \)

These all factorize trivially.


## Correct Source

Non-decomposability arises from:

$$
A_t^{\mathrm{bd}}(\Sigma)
\subseteq
\pi_{\Sigma}(A_t)
$$

i.e.:

> the closure predicate applied after projection


## Typed Factorization

Let:

$$
\Sigma = A \cup B, \quad A \cap B = \varnothing
$$

Define recombination:

$$
\mathrm{Recombine}(A,B)
=
\{ a \cup b \mid a \in A_t^{\mathrm{bd}}(A),\ b \in A_t^{\mathrm{bd}}(B) \}
$$


## Factorization Test

$$
A_t^{\mathrm{bd}}(\Sigma)
\stackrel{?}{=}
\mathrm{Recombine}(A,B)
$$


## Actual Result (Correct Interpretation)

There exist regions \( \Sigma \) such that:

* \( A_t^{\mathrm{bd}}(\Sigma) \ne \varnothing \)
* but:
  $$
  A_t^{\mathrm{bd}}(A) = \varnothing
  \quad \text{or} \quad
  A_t^{\mathrm{bd}}(B) = \varnothing
  $$


## Interpretation

> Closure predicates on projected continuations do not factor across subregions.


## Mechanism

This occurs because:

* closure depends on diachronic dependence relations
* these relations include cross-boundary edges
* restriction removes those edges


## Conclusion

> Non-decomposability arises from failure of closure predicates to factor under restriction, not from properties of admissible futures.


## System Structure

```text
GoL dynamics
-> counterfactual dependence
-> diachronic relation
-> region extraction
-> projection (trivial)
-> closure predicate (non-trivial)
-> decomposition analysis
````


## What Is Not Claimed

This system does not:

* introduce new causal forces
* modify transition dynamics
* produce multiple admissible futures
* imply non-factorization of (A_t)
* introduce probabilistic structure


## What Is Demonstrated

> Deterministic dynamics can produce regions whose closure predicates - defined via diachronic dependence - do not decompose under restriction.


## Final Summary

* dynamics: deterministic GoL
* admissible futures: singleton
* projection: always factorizes
* bounded projection: predicate-filtered
* closure: depends on diachronic dependence
* non-decomposability: failure of predicate factorization


## One-Line Definition

> This model is standard Game of Life equipped with a diachronic dependence structure in which closure predicates on projected continuations fail to factor under restriction.


## End of Document
