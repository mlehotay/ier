# Lean formalization

## Status

Status: experimental\
Authority: non-canonical

This project starts with a small Lean proof of concept for IER relations. The
planned expansion will encode relations from the revised core-theory file set
and use them to check bounded models of physical examples, including Rab
transport and the Stapled Net.

## Files

- [Proof source](IER-unity.lean): membership and stage-adjacency lemmas, plus a
  local Game of Life calculation.
- [Slack relations](IER-slack.lean): available independent resolution pathways,
  saturation, conditional exclusion results, and finite countermodels.
- [Vector pickup](vector-pickup-explained.md): two hypothetical histories with
  the same cradle state and different continuity, using
  [vector-pickup.lean](vector-pickup.lean).
- [Epistemic Kettle](epistemic-kettle-explained.md): rival predictions,
  discriminating trials, and remaining observational equivalence, using
  [epistemic-kettle.lean](epistemic-kettle.lean).
- [Explanation](IER-lean-explained.md): the definitions, four results, and their
  application limits.
- [Formalization plan](IER-lean-plan.md): scope, implementation sequence,
  evidence requirements, and proposed physical-example checks.

## Run the proof of concept

For a minimal command-line setup on Ubuntu in WSL, follow the
[WSL installation guide](install-wsl.md).

The examples use Lean 4.19.0 and require no Mathlib. From this
directory, run:

```sh
lean IER-unity.lean
lean IER-slack.lean
lean epistemic-kettle.lean
lean -o IER-unity.olean IER-unity.lean
LEAN_PATH=. lean vector-pickup.lean
```

The unity file includes four `#print axioms` commands for inspecting proof
dependencies. Its physical membership and continuity predicates are parameters;
applying the lemmas requires evidence for the stated premises.

From the development repository root, `make check-lean` checks all four files
and puts the unity import artifact in `build/lean/`.
Use `make check-lean LEAN=/path/to/lean` to select a toolchain explicitly.
The standalone commands above also work in the public export.

## Slack formalization

The slack file uses one context for the candidate, restriction, grain, interval,
and interventions. An available pathway supplies slack only when it preserves
independence and completes the relevant burden through localization, indefinite
deferral, or external resolution. Its physical predicates remain parameters;
finite delay and external supply do not automatically satisfy them.

The code proves that saturation is absence of those resolution pathways. Under
explicit IER commitments about the necessary features of UEF operation and
experience requiring a UEF, it proves that slack excludes both at the same
scope. Those commitments are visible hypotheses, not independently established
physical tests or a proof of experiential identity.

Finite stipulated models have the same two successors with and without slack.
A further model satisfies the commitments while having saturation and no UEF.
These are logical countermodels to sufficiency claims, not biological models.
The file reports axiom dependencies for its main results.

## Scope of the checks

Lean checks the encoded conclusions against their definitions and assumptions.
The initial file does not establish the physical predicates, full token
continuity, or UEF qualification. The physical adequacy of a model and its
correspondence to an example require separate evidence and review.

The [Specification](../../IER/IER-specification.md) controls IER's normative
commitments. This application does not diagnose experience, identity, or unity.

Return to the [applications index](../README.md).
