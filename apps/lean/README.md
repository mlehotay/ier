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
- [Explanation](IER-lean-explained.md): the definitions, four results, and their
  application limits.
- [Formalization plan](IER-lean-plan.md): scope, implementation sequence,
  evidence requirements, and proposed physical-example checks.

## Run the proof of concept

The saved proof was checked with Lean 4.19.0 and requires no Mathlib. From this
directory, run:

```sh
lean IER-unity.lean
```

The file includes four `#print axioms` commands for inspecting proof
dependencies. Its physical membership and continuity predicates are parameters;
applying the lemmas requires evidence for the stated premises.

## Scope of the checks

Lean checks the encoded conclusions against their definitions and assumptions.
The initial file does not establish the physical predicates, full token
continuity, or UEF qualification. The physical adequacy of a model and its
correspondence to an example require separate evidence and review.

The [Specification](../../IER/IER-specification.md) controls IER's normative
commitments. This application does not diagnose experience, identity, or unity.

Return to the [applications index](../README.md).
