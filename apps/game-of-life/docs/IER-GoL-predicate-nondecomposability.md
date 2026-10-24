---
ier:
  aligned_with:
  - certification: 10.11.6-core-theory-r2
    reviewed_on: '2026-10-17'
    review: b8fada6a831d790d66f1c65b33ed8877577575f1:traceability/reviews/batch-17.md#member-10
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

> This document specifies a predicate construction and a proposed GoL non-decomposability case. The conditional set argument is valid if its premises hold; a fully computed GoL witness remains unprovided.
> It does not introduce non-factorability at the level of admissibility itself.


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

## Causal Status

The system evolves strictly under standard Game of Life dynamics.

The additional structures defined here:

* do not modify the transition relation
* do not select or reject transitions
* do not introduce causal influence

Formally:

$$ R = T $$

All additional constructs are descriptive only.


## Base System

Let:

$G$

be a finite lattice.

A configuration is:

$$ s(t) \in S $$

where:

$$ s(t) = \{x_i(t)\}_{i \in G}, \quad x_i(t) \in \{0,1\} $$


## Transition

Let the standard Game of Life update be:

$$ T:S\rightarrow S $$

The lawful transition relation is the graph of this deterministic map; here $R=T$ abbreviates equality with that graph.


Since deterministic:

$$ s(t+1) = \hat{s} = T(s(t)) $$


## Admissible Futures

$$ A_t = \{ \hat{s} \} $$


## Invariant (Singleton Structure)

For all regions $\Sigma$:

$$ A_t^{\mathrm{bd}}(\Sigma) \in \{ \varnothing, \{ \hat{s}|_{\Sigma} \} \} $$


## Implication

* no multiplicity of futures
* no branching structure
* no probabilistic interpretation

Any mismatch in this filtered-set comparison arises from predicate evaluation, not from branching of the actual $A_t$.


## Relation to Global Temporal Structure (Omniperiodicity)

Life on its unbounded lattice is omniperiodic ([Brown et al. 2023](https://arxiv.org/abs/2312.02799)), i.e.:

> for every positive integer $n$, there exists a configuration whose global trajectory has period $n$.

This external result concerns oscillator existence over time and arbitrary required space. It is not a claim that one fixed finite lattice realizes every period.

### Non-Interaction with This Construction

The present system operates at a fixed time $t$, where:

$$ A_t = \{ \hat{s} \} $$

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

For an updated site $i$ and a site $j$ in its local update stencil (including $i$), define:

$s^{(j \leftarrow 0)}, \quad s^{(j \leftarrow 1)}$

Let:

- $\hat{s}$ = update from $s$
- $\hat{s}^{(j \leftarrow v)}$ = update from perturbed configuration


## Dependence Relation

$$ (i,j) \in D(s) \iff \exists v \in \{0,1\} \text{ such that } \hat{s}_i \ne \hat{s}^{(j \leftarrow v)}_i $$


## Interpretation

* captures local update sensitivity
* purely structural
* no probabilistic meaning


## Diachronic Dependence Relation

Let persistence window $k \ge 1$.

Define:

$$ (i,j) \in C_t \iff \exists \tau \in [t-k+1, t] \text{ such that } (i,j) \in D(s(\tau)) $$


## Interpretation

* sliding-window accumulation of dependence
* fully specified
* no recursion ambiguity


## Region Structure

Define live set:

$$ L(t) = \{ i \in G \mid x_i(t)=1 \} $$


## Region Definition

A region $\Sigma(t) \subseteq G$ is a maximal subset such that:

1. $\Sigma(t) \subseteq L(t)$
2. for all $i,j \in \Sigma(t)$, there exists a path:

$$ i = i_0, \dots, i_n = j $$

with:

$$ (i_m, i_{m+1}) \in C_t \quad\text{or}\quad (i_{m+1},i_m)\in C_t $$

Connectivity is evaluated on the undirected interpretation of $C_t$, with path vertices in the tested live subset.


## Internal and External Dependence

For region $\Sigma$:

$$ C_t^{\mathrm{in}}(\Sigma) = C_t \cap (\Sigma \times \Sigma) $$

$$ C_t^{\mathrm{out}}(\Sigma) = C_t \cap (\Sigma \times \Sigma^c) $$


## Boundedness

A region $\Sigma(t)$ is bounded if:


## Internal Dependence

$C_t^{\mathrm{in}}(\Sigma) \ne \varnothing$


## External Independence

$$ C_t^{\mathrm{out}}(\Sigma) = \varnothing $$


## Interpretation

* an internal dependence edge exists
* no recorded outward-coordinate sensitivity to external cells in the chosen window; this is not operational self-sustainment or canonical ownership


## Projection of Admissible Futures

Since:

$$ A_t = \{ \hat{s} \} $$

projection always yields:

$$ \pi_{\Sigma}(A_t) = \{ \hat{s}|_{\Sigma} \} $$


## Key Property

Projection of admissible futures always factorizes trivially.


## Candidate Future Evaluation

Let:

$$ s' = \hat{s} $$

Compute $D(s')$ from the next update. Here $C(s')$ abbreviates $C_{t+1}$ built from the realized history extended by $s'$. It is not a function of $s'$ alone when $k>1$. The window, available prehistory and startup policy must be supplied for a reproducible example.


## Region Continuation

## Conservative Definition (Default)

$$ \Sigma'(t+1) = \{ i \in \Sigma(t) \mid x_i'(t+1)=1 \} $$

New cells are not included.


## Optional Variant: Recruitment

$$ \Sigma'(t+1) = \text{maximal } C(s')\text{-connected set containing surviving members} $$


## Status

The documented default uses conservative continuation. Recruitment is a proposed variant: it requires a rule for no survivors and for survivors in multiple connected components; it is not an implemented identity relation.


## Structural Closure Condition

Closure is defined as a predicate on projected continuation.


## Internal Connectivity

$\Sigma'(t+1) \text{ is connected under } C(s')$


## External Independence

$$ C(s') \cap (\Sigma'(t+1) \times \Sigma'^c(t+1)) = \varnothing $$


## Evaluation Rule (Important)

Closure is evaluated relative to the region under consideration.

That is:

* internal connectivity uses the relation induced on the surviving cells of the tested region;
* external independence uses the full $C_{t+1}$ and that region’s complement in $G$. Deleting boundary edges before testing external independence would make that test vacuous;
* empty continuation is not a connected continuing region for this comparison;
* $P$ depends on full successor/history context, suppressed in the compact $P(\Sigma,\hat{s}|_\Sigma)$ notation. It cannot in general be evaluated from the projected cell values alone.


## Implication

> Closure is not preserved under restriction.


## Bounded Projection

Define:

$$ A_t^{\mathrm{bd}}(\Sigma) = \{ \hat{s}|_{\Sigma} \mid \text{closure holds under } s' \} $$


## Key Property

This set is:

* either singleton
* or empty


## Interpretation

> Bounded projection is predicate-filtered projection, not a new future structure.


## Source of Non-Decomposability

Non-decomposability does not arise from:

* multiple transitions from the same complete input under $T$
* admissible futures $A_t$
* projection $\pi_{\Sigma}(A_t)$

The singleton successor projections factorize trivially; the update function can still couple neighboring cells.


## Correct Source

Non-decomposability arises from:

$$ A_t^{\mathrm{bd}}(\Sigma) \subseteq \pi_{\Sigma}(A_t) $$

i.e.:

> the closure predicate applied after projection


## Typed Factorization

Let:

$$ \Sigma = A \cup B, \quad A \cap B = \varnothing $$

Define recombination:

$$ \mathrm{Recombine}(A,B) = \{ a \cup b \mid a \in A_t^{\mathrm{bd}}(A),\ b \in A_t^{\mathrm{bd}}(B) \} $$


## Factorization Test

$$ A_t^{\mathrm{bd}}(\Sigma) \stackrel{?}{=} \mathrm{Recombine}(A,B) $$


## Proposed GoL Witness

The proposed GoL witness must supply a region $\Sigma$ such that:

* $A_t^{\mathrm{bd}}(\Sigma) \ne \varnothing$
* but:
  $$ A_t^{\mathrm{bd}}(A) = \varnothing \quad \text{or} \quad A_t^{\mathrm{bd}}(B) = \varnothing $$


## Interpretation

> Under the proposed whole-success/subregion-failure premises, the filtered sets do not factor; those premises still need a computed Life witness.


## Mechanism

This occurs because:

* closure depends on diachronic dependence relations
* these relations include cross-boundary edges
* restriction changes which edges are internal; an edge across a subregion cut remains external when testing independence


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
```


## What Is Not Claimed

This system does not:

* introduce new causal forces
* modify transition dynamics
* produce multiple admissible futures
* imply non-factorization of $A_t$
* introduce probabilistic structure


## Conditional Result and Open Witness

> If a computed Life history satisfies the stated whole/subregion predicates, the filtered sets fail to decompose. Cross-boundary edges suggest a candidate but do not prove a GoL realization. The explicit witness is still required.


## Final Summary

* dynamics: deterministic GoL
* admissible futures: singleton
* projection: always factorizes
* bounded projection: predicate-filtered
* closure: depends on diachronic dependence
* non-decomposability: failure of predicate factorization


## One-Line Definition

> This model is standard Game of Life equipped with a diachronic dependence structure in which closure predicates on projected continuations are tested for failure to factor under restriction.


## End of Document
