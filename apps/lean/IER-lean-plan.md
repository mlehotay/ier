# Lean plan for core theory relations and physical examples

Date: 22 November 2026\
Status: proposed implementation sequence; planning only\
Authority: non-canonical application plan

## Goal and starting point

Encode the relations in the `core-theory` file set in Lean, then use those
definitions and their proved consequences to check bounded models of physical
examples, beginning with Rab transport and the Stapled Net. The intended result
is a reusable relation library with explicit premises and source coverage.

Preserve ChatGPT's [IER-unity.lean](IER-unity.lean) proof of concept and its
[explanation](IER-lean-explained.md) unchanged as the starting artifact. It
contains membership and adjacency lemmas plus a local Game of Life calculation;
it does not yet implement the physical predicates or full token continuity.
Its recorded Lean version is 4.19.0 and it needs no Mathlib. Reproduce that
baseline before choosing a toolchain for new modules.

This plan authorizes no canonical revision or empirical attribution. Checking
an example means establishing what follows in its declared model and identifying
which physical premises have evidence. It does not supply a diagnostic of
experience, identity or unity.

## Scope and source baseline

The development repository's `data/file-sets.yml` registry and
`data/paper-01-core-scope.txt` manifest own membership. These files, the `_work/`
records and the example sources named below are development references, not
included in this public Lean export. Resolve membership in the development
repository with:

```sh
make file-set FILE_SET=core-theory
```

On 22 November this resolves to 33 articles, but the author identifies the
selection as out of date. The
core membership objective in `_work/v10.12.3-objectives.md`, section 10,
must be completed before freezing formalization coverage. Preliminary work on
the preserved proof and individual relations can proceed meanwhile.

Record the revised member list, repository revision and source hashes when
implementation begins. Treat `IER-unity.md` as a companion to the proof of
concept until its membership is explicitly decided. Record any other outside
definition owners as explicit imports rather than silently expanding the set.

Assess every selected article for formalizable relations and argument roles.
A philosophical or interpretive passage may require a reviewed scope note
rather than a Lean theorem. File coverage alone does not establish claim coverage.

## Representation and evidence

Start with a shared vocabulary for physical states, lawful transitions,
occurring processes, operation contexts, intervals and interventions. Keep
states and descriptions distinct from the processes they describe. Represent
common operation, restriction, grain, scale, regime and interval explicitly
where possible, so evidence from incompatible contexts cannot be combined
merely by assigning it the same label.

For each relation or argument, distinguish:

- **Definition:** terminology and mathematical structure fixed by a source.
- **Commitment:** an explicit theory premise used in conditional proofs.
- **Theorem:** a consequence derived from separately identified premises.
- **Open obligation:** an incomplete physical formulation or unresolved argument.
- **Application evidence:** support for a premise in one declared preparation.

Use explicit parameter structures for physical relations and theory commitments.
Do not make an unfinished premise disappear behind a global axiom or `sorry`.
An experience/UEF biconditional can express existence correspondence, but does
not alone capture the stronger identity of experience, subject and operation.
Review that representation separately before claiming identity coverage.

Maintain a small coverage table alongside later modules: canonical path and
section, applicable claim/invariant IDs, Lean declaration, status, premises,
source baseline and remaining obligation. Reuse the existing
claims (`data/claims.csv`), source mappings (`data/claim-sources.csv`),
claim relations (`data/claim-relations.csv`) and
invariants (`data/invariants.csv`) for navigation; canonical prose controls
meaning. Record uncovered passages as well as translated registry entries.

## Implementation sequence

1. **Reproduce the starting proof.** Install one pinned toolchain, run the
   preserved file and inspect its four axiom reports. Add a narrow Make target
   and a separate entry point for new work. Keep the initial dependency set
   small; add Mathlib only when a concrete relation needs it.
2. **Encode the first physical slice.** Introduce explicit context and transition
   types, then implement typed membership and local stage adjacency in new
   modules. Prove each missing membership clause blocks membership and retain
   the fork/convergence results. Distinguish unestablished from refuted premises.
3. **Extend the relation library.** Follow the revised core and its dependencies:
   constraint and admissibility; closure, ownership and participation; produced
   and inherited continuity; trajectories and maximal token chains; regime and
   multiscale relations; other relations identified by the coverage review.
   State any ordering, time-domain or maximality assumptions explicitly.
4. **Encode conditional IER consequences.** Connect structural premises to
   specification commitments and unity consequences without supplying an
   invented qualification test. Keep incomplete present-organization conditions
   visible. Support factorization is not a necessary or sufficient unity test.
5. **Check bounded examples.** Give each example its own state space, transition
   rules, interventions, evidence mapping and unresolved premises. Reuse library
   definitions without weakening them to obtain a desired result.
6. **Review coverage and adequacy.** Review translated statements against prose,
   prove substantive examples satisfy their stated premises, and construct
   countermodels where appropriate. Report checked consequences, assumptions
   and remaining obligations separately. Compilation alone does not establish
   consistency, faithful translation or empirical adequacy.

The first implementation milestone is steps 1–2 plus one small example result;
it does not require translating the entire core first. Each later batch should
be reviewable and checked before extending coverage.

## Rab transport checks

Start from the development repository's physical correspondence ledger,
`_work/mechanism/phase-3-correspondence-ledger-2026-11-17.md`, and its assessment,
`_work/mechanism/phase-4-assessment-2026-11-17.md`, reading their current
dispositions before historical proposals. Follow their primary source locators
for retained empirical premises. The public
[biological transport companion](../neuroscience/biological-transport.md)
provides an accessible evidence route and its limits.

Choose one preparation, interval and transition question first, such as the
plant ARA7/MON1–CCZ1/RABG3f recruitment and activation account. Represent cargo,
regulator state, membrane association and downstream readouts separately. Label
which model transitions are experimentally supported and which are assumptions.
Do not concatenate animal, plant, purified, protoplast and seedling results into
one observed continuous operation.

Check stated dependencies, enabling conditions, intervention compatibility and
production/uptake consequences where the evidence supports them. Track missing
membership, ownership, maintenance and full-continuity premises explicitly.
General relation lemmas and Life calculations do not establish Rab mechanisms.

## Stapled Net checks

Use the development sources `IER-stapled-net-model.md`,
`IER-stapled-net-apparatus.md` and `IER-stapled-net-discussion.md` under
`examples/collections/stapled-net/` as distinct mechanical and interpretive
sources. Review their correspondences
against current canonical owners before translating them. In particular, the
Discussion's support-factorization wording must not be imported as a current
unity criterion.

Begin with a finite grasper-state model: OPEN, CLOSED and LOCKED; CLOSED may
reopen, while LOCKED may not reopen within a run. Prove the difference in
permitted immediate successors even when visible seating geometry is equal.
Keep capture-and-seat conditions explicit rather than inventing mechanics.

Extend to permitted sequences only when the required geometry and transition
rules are declared. Distinguish immediate capture availability from multi-step
mechanical attainability and from canonical UEF reachability. Removal of B2's
reopening transition does not establish B1's permanent uncapturability or
canonical collapse. Treat reset as ending the run, not as a within-run reversal.

## Checks and completion records

For each implementation batch, run the owning Lean Make target, reject unfinished
proofs, inspect axiom dependencies and review both explicit hypotheses and
commitment structures. Use concrete satisfying models to detect vacuous results.
For a claim of non-entailment, supply a countermodel at the stated scope where
possible; absence of a proof is not a proof of negation.

Record toolchain, command, result, source snapshot and limits. Repository checks
validate integration; Lean validates the encoded proofs; source review validates
the correspondence. Each example report should state exactly which relations
were checked and which physical premises remain unestablished. Full core coverage
is complete only when every in-scope relation or argument has an explicit
formalization or reviewed disposition, with open burdens retained.
