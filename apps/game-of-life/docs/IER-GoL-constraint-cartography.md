# From Structural Example to Constraint Cartography

## A Proposed Game of Life Substrate for Organizational Reconstruction

Status: Non-Canonical Research Note / Design Retrospective\

## Abstract

Conway’s Game of Life supplies a deterministic, inspectable substrate on which
to develop tests of dependence, projection, region predicates and organizational
reconstruction. The current executable supplies a simulator baseline and
ordinary step statistics. Its GoL phase is incomplete, and its dependence,
region and support analysis remains proposed. This note explains how those
future tools could support constraint cartography without treating a descriptive
graph as a physical subject or an unfinished construction as a demonstrated
result.

## Original Role and Retained Question

The original project asked whether a simple deterministic system could isolate
structural questions relevant to IER. At a fixed complete Life configuration,
the next-state set is singleton. Its projections factorize. A separately
defined region predicate can nevertheless be sensitive to the boundary at which
it is evaluated. These are different mathematical questions.

The [predicate specification](IER-GoL-predicate-nondecomposability.md) and
[conditional argument](IER-life-nondecomposability.md) preserve that distinction.
If the whole predicate succeeds while a subregion predicate fails, the filtered
singleton differs from the empty recombination. An explicit computed GoL history
satisfying those premises is still required. The
[regional support proposal](IER-GoL-support-admissibility.md) uses outputs across
perturbed starting configurations, not multiple successors of the actual state.
Its neighborhood, history policy and factorization result remain to be supplied.

The useful negative-control question survives: which forms of apparent unity
can arise in a description without establishing canonical intrinsic closure?
It does not require declaring the physical computer conscious or non-conscious.

## Current Meaning of Constraint Cartography

The earlier retrospective framed cartography mainly as reconstruction of
persistent organization behind transient computation, aimed eventually at
neural networks. That is a possible application, but it is not the complete
current account. *Constraint Cartography* (Paper 02)
develops a model-relative method through slime-mold studies and distinguishes
three explanatory targets:

| Map | Question for a future Life experiment |
| --- | --- |
| Instance | Why did this board follow this trajectory under this topology and initialization? |
| Mechanism | Which specified update sensitivities recur across a declared family of configurations? |
| Organization | How do those relations compose, persist, compete or change over the selected interval? |

A cartographic relation states its bearer, locus, timescale, origin, evidence,
effect, rival explanation and failure condition. In Life, these fields would
need an explicit board, boundary rule, perturbation family, history window and
comparison target. Observed co-activity and intervention-supported sensitivity
must retain different evidence roles. A connected region is one descriptive
unit under that construction, not automatically one autonomous operation.

Cartographic constraint is ordinary and model-relative. A board boundary,
initialization, changed topology or prior configuration can affect later
transitions without establishing intrinsic constraint, owned foreclosure,
welding or subject persistence. Current-state effects of past evolution do not
show that the past is stored as experiential content.

## Proposed Correspondences and Their Limits

| Cartographic question | Proposed Life construction | Limitation |
| --- | --- | --- |
| Present organization with later effects | Current configuration and declared history-window dependence | Window accumulation is not numerical subject continuity or irreversible inheritance. |
| Local and distributed relations | Counterfactual update sensitivity and cross-region edges | Coupling and indispensability do not establish constitutive membership. |
| Descriptive boundaries | Regions and region-relative predicates | No canonical closure or ownership verdict follows from connectivity. |
| Realized trajectories | Successive board configurations | One realized trajectory does not inventory all physically available continuations. |
| Alternative support | Outputs across a fixed perturbation family | Changing starting states is distinct from the actual state's outgoing successor fibre. |
| Reconstruction validation | Comparison with exactly computed relations under a declared construction | Knowing the update rule does not make every proposed organizational boundary a unique ground truth. |

Life has no implemented learning mechanism here. A recurrent pattern or
sliding-window graph is therefore not learned organization in the sense of a
trained neural network. Successful recovery of an example relation would not
establish that the same method explains slime-mold intelligence or scales to
language models. Those comparisons need independent models and evidence.

## Why the Substrate Remains Useful

The update rule is known, individual states can be inspected, and perturbations
can be specified exactly. A future experiment could compare a reconstruction
algorithm against a computed sensitivity relation instead of judging it by
visual resemblance. The comparison must fix the boundary, grain, topology,
window and intervention family before seeing the reconstruction output.

This makes a limited test tractable. It does not establish one complete
organizational map in advance. Different timescales, region rules and
perturbations may produce different legitimate descriptions. An experiment
should state the shared observation, rival maps, divergent predictions and
rejection condition so that disagreement can narrow the proposed method.

## Implementation Status and Sequence

The [README](../README.md) describes the current CLI and controls. The
[design](gol-design.md#implementation-status) targets a complete simulator;
runtime commands and random/pattern/file startup initialization are available. A basic
[world test harness](../tests/README.md) checks block/oscillator/glider evolution;
Gosper gun emission is also verified; the phase-exit audit remains open. Ordinary birth/death/change statistics exist, but there is no
implemented dependence graph, region extraction, closure score, support
enumerator or cartography reconstruction layer.

Current terminal resizing changes the world dimensions with overlap preservation
and dead-fill. It is a world intervention, not a display-only change. Future
analysis must fix geometry during a trial, record resize/topology changes, or
implement a separately validated world/viewport policy.

After the simulator phase is complete, the
[future programme](IER-GoL-future-directions.md) proposes:

1. Fix the example definitions and compute an explicit predicate witness.
2. Add read-only sensitivity/history/region tools incrementally.
3. Specify and enumerate the regional perturbation support, with same-support
   projections kept distinct from independently recomputed regional filters.
4. Evaluate rival reconstructions against the declared comparison target.
5. Develop visualization, continuation variants and cross-rule experiments
   only for concrete analysis questions.

These are research and implementation proposals. They are not newly imposed
repository gates or missing results blocking this document's alignment.

## Canonical Qualification and Consumer Boundary

The [orientation](gol-orientation.md#canonical-boundary) supplies the current
relation order. Intrinsic closure requires same-fibre support and an exhaustive
meaningful partition family at an independently declared physical typing.
Ownership, all five constitutive membership conditions and all five produced
and inherited continuity conditions remain independent. Descriptive boundary
closure is not causal isolation, and external support does not automatically
defeat intrinsic ownership. Ordinary continuation need not have fresh collapse.

No proposed score, graph, recurrence or toy result supplies those relations or
the abductive identity warrant. This note revises application currency; it does
not report a completed empirical/model result, change Paper 02, or authorize
new simulator features. The original methodological purpose remains: use a
small inspectable system to test clearly typed structural questions before
attempting broader explanations.
