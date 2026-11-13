---
ier:
  aligned_with:
  - certification: 10.11.6-core-theory-r2
    reviewed_on: '2026-10-17'
    review: b8fada6a831d790d66f1c65b33ed8877577575f1:traceability/reviews/batch-17.md#member-11
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

This document defines a derived admissibility layer over standard Conway’s Game of Life (GoL).

It is:

- NON-CANONICAL
- EXAMPLE-SCOPED
- NOT READER-FACING

> This document proposes a region-indexed perturbation-support construction. It states a factorization question; a concrete neighborhood/history specification and computed witness are still required. These are alternatives across starting configurations, not multiple successors of the fixed actual state.
> It extends, but does not replace, the predicate-level result.


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

The system evolves strictly under standard GoL dynamics:

$$ s(t+1) = T(s(t)) $$

The structures introduced here:

- do not modify $T$
- do not introduce new transitions
- do not select outcomes
- do not introduce probabilities

They define a descriptive admissibility overlay.


## Base System

Let:

- $G$ be a finite lattice
- $S = \{0,1\}^G$
- $s(t) \in S$


## Transition

Let the standard GoL update be:

$$ T:S\rightarrow S $$

The transition relation is the graph of this deterministic map.


Deterministic:

$$ \hat{s} = T(s) $$


## Counterfactual Dependence

Define:

$$ (i,j) \in D(s) \iff \exists v \in \{0,1\} \text{ such that } \hat{s}_i \ne \hat{s}^{(j \leftarrow v)}_i $$


## Diachronic Relation

For window $k$:

$$ C_t = \bigcup_{\tau=t-k+1}^{t} D(s(\tau)) $$


## Regions

Define live set:

$$ L(t) = \{ i \in G \mid x_i(t)=1 \} $$

A region $\Sigma(t) \subseteq L(t)$ is a maximal set connected under:

$\text{undirected}(C_t)$


## From Global Determinism to Regional Admissibility

Globally:

$$ A(s) = \{ \hat{s} \} $$

This is singleton.


## Limitation of Global Admissibility

Global admissibility contains no multiplicity and always factorizes trivially under projection.

Therefore:

> this fixed-state singleton does not supply support non-factorization; other declared physical supports require their own assessment.


## Strategy

Introduce region-indexed admissibility derived from:

- counterfactual perturbations
- dependence constraints
- bounded support conditions


## Regional Admissibility Structure

For region $\Sigma$, define:

$$ A_\Sigma(s,t) \subseteq S_\Sigma $$

as the set of admissible regional continuations.


## Definition (Support-Based Admissibility)

This is a construction schema. Specify $N_t(s,\Sigma)$ independently: allowed sites, perturbation values/radius, topology, time window, and whether earlier history is held fixed or regenerated. The coherence predicate and startup/prehistory policy also need exact definitions. Boundedness alone does not determine a unique support. Here $C(\tilde{s}')$ suppresses those history inputs; for $k>1$ it is not determined by the successor configuration alone.


A regional continuation $u \in S_\Sigma$ is admissible iff:

$$ u \in A_\Sigma(s,t) $$

iff there exists a perturbed configuration:

$$ \tilde{s} \in N_t(s,\Sigma) $$

such that:

### Realizability
$$ T(\tilde{s})|_\Sigma = u $$

### Bounded Perturbation
$\tilde{s} \text{ differs from } s \text{ only within a bounded neighborhood of } \Sigma$

### Dependence Coherence
$\Sigma \text{ remains connected under } C(\tilde{s}')$

where $\tilde{s}' = T(\tilde{s})$


## Interpretation

$A_\Sigma(s,t)$ contains:

> all regional futures that can be jointly supported by some compatible completion of the surrounding configuration.

This is:

- structural
- non-probabilistic
- non-causal


## Properties of Regional Admissibility


## Support Cardinality

Depending on the declared perturbation family and coherence filter, the set may be empty, singleton or larger. Nonemptiness and multiplicity are not established by this schema.


## Non-Equivalence with Projection

In general:

$A_\Sigma(s,t) \ne \pi_\Sigma(A(s))$


## Dependence on Environment Completion

Admissibility is defined via existence of:

$$ v \in S_{\Sigma^c} $$

such that:

$u \cup v \text{ is realizable and coherent}$


## Factorization and Non-Factorability

Let:

$$ \Sigma = A \cup B, \quad A \cap B = \varnothing $$


## Induced Subregion Admissibility

Define:

$A_A(s,t), \quad A_B(s,t)$

by separately applying the regional construction. These independently filtered sets need not be the projections of the one whole-region support.


## Factorization Test

Define recombination:

$$ \mathrm{Recombine}(A,B) = \{ u_A \cup u_B \mid u_A \in A_A,\ u_B \in A_B \} $$


## Non-Factorability Condition

$A_\Sigma(s,t) \ne \mathrm{Recombine}(A,B)$


## Interpretation

A failed comparison with independently constructed subregion sets may reflect changed neighborhoods or filters. To test product structure of one overlay support, instead compare $A_\Sigma$ with recombinations of $\pi_A(A_\Sigma)$ and $\pi_B(A_\Sigma)$ from that same support. Both comparisons require computed sets; neither is established by cross-boundary dependence alone.


## Mechanism of Non-Factorability

Candidate sources of a regional-filter mismatch include:


## Cross-Boundary Dependence

Relations:

$$ C_t|_{A \times B} $$

link subregions structurally.


## Joint Support Requirement

A proposed joint-support case would need to demonstrate:

- coordinated structure across $A$ and $B$
- no valid completion exists for isolated subregion variants


## Failure of Independent Completion

A possible mismatch with independently constructed regional sets would have:

- $u \in A_\Sigma$

such that:

- $u|_A \notin A_A$ or
- $u|_B \notin A_B$


## Relation to Canonical IER Structure

This proposed example investigates:

> non-factorability of a declared perturbation support, separately from actual physical frontier qualification


## Mapping

| Example Object | Typing and Limit |
|----------------|-------------------|
| $A_\Sigma(s,t)$ | A proposed overlay support across perturbed starting states, distinct from canonical $A(s)$ |
| Same-support projection mismatch | A bounded mathematical factorization comparison, if demonstrated |
| Support/coherence filter | Example-defined eligibility, not intrinsic constraint or ownership |


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

> the separate support experiment uses a different object; it does not replace the predicate construction or canonical closure

Optional predicates may still be defined, but are secondary.


## Proposed Test and Required Evidence

This proposal asks whether a specified perturbation family yields non-product support. A complete definition and computed witness must precede an existence or result claim.


## What Is Not Claimed

This system does not:

- modify GoL evolution
- introduce multiple global futures
- introduce probabilities
- define agents or observers
- establish canonical intrinsic closure


## One-Line Definition

> The model defines region-indexed admissible continuations via support under bounded perturbation, with their product structure still to be tested.


## End of Document
