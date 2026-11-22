# IER-unity.lean explained

[`IER-unity.lean`](IER-unity.lean) is an exploratory formalization of three pieces of IER's unity work. It checks what follows from the stated rules for constitutive membership and stage continuity, and computes one Game of Life example. The physical tests for applying the membership and continuity rules are supplied as parameters.

## Running the file

Run this command from the directory containing `IER-unity.lean`.

```sh
lean IER-unity.lean
```

The file was checked with Lean 4.19.0 and requires no Mathlib installation. Lean checks the proof of each `theorem` against its statement. The four `#print axioms` commands at the end report that the theorems have no axiom dependencies. The physical premises you see in the theorem statements are still premises. For example, applying the fork theorem requires evidence of a second candidate that meets the first four continuity clauses.

## Constitutive membership

`MembershipRelations` gives names to the five clauses in IER's *Processes* account. Each is a proposition (`Prop`) about a process in a declared context.

| Lean field | What it asks about the process |
| --- | --- |
| `contribution` | Does the process help determine the operation's admissible continuation? |
| `globalBearing` | Does the operation's restriction constrain the process's own possible continuation? |
| `jointResolution` | Are the relevant alternatives settled through the same non-externalizable successor organization? |
| `diachronicUptake` | Do the consequences of contribution and bearing enter production of the operation's later organization? |
| `commonTyping` | Does the evidence for all four clauses concern the same operation, restriction, variables, grain, scale, regime, intervention family, and interval? |

`ConstitutiveMember` joins the five clauses with `∧`, Lean's symbol for “and.” In `noMembershipWithoutJointResolution`, the proof supposes membership holds and extracts its third clause with `member.2.2.1`. That gives joint resolution, contradicting the premise that joint resolution fails. To apply the theorem to a real process, evidence would have to establish that failure at the declared typing. An unanswered question about joint resolution supplies no such evidence.

## Stage continuity

`MeetsFirstFour r a b` says that the transition from stage `a` to stage `b` meets continuous closure, operational successor production, typed consequence inheritance, and operation-mediated change. `Adj_dia` adds uniqueness in both directions. Every candidate successor of `a` meeting those four clauses must be `b`, and every candidate predecessor of `b` meeting them must be `a`.

`forkBlocksAdjacency` shows what happens when another stage `c` also meets the first four clauses as a successor to `a`. If we suppose `Adj_dia r a b`, successor uniqueness forces `c = b`. Since `c` and `b` are distinct, the supposed adjacency fails. Exchanging `b` and `c` gives the same result for the other branch. `convergenceBlocksAdjacency` uses predecessor uniqueness to give the corresponding result for two distinct predecessors.

Both proofs range over the declared `Stage` type. Applying them requires a physically justified comparison domain and a way to assess the four clauses on its candidate transitions. The two lemmas concern a single transition. IER's account of a continuing token uses a maximal chain of such transitions.

## Game of Life

`nextCell` implements the local Life rule. `centerAfter` specializes it to a live center with two live neighbors and two edits that turn on distinct additional neighbors at the same starting board.

| Edit A | Edit B | Live neighbors | Center at next step |
| --- | --- | --- | --- |
| Off | Off | 2 | Alive |
| On | Off | 3 | Alive |
| Off | On | 3 | Alive |
| On | On | 4 | Dead |

`individuallyEnabledButJointlyIncompatible` uses `by decide` to have Lean check all four outcomes by computation. The result gives a concrete counterexample to the inference from individual compatibility to joint compatibility for these edits. The model is a single-cell update with specified neighbor counts.

## What this establishes about unity

The file checks consequences of the membership and continuity definitions and one bounded model calculation. IER's further claim is conditional on experiential identity. A qualifying token operation is the experience and its subject, and experiential unity expresses the oneness of that operation. Applying this claim requires an account of the operation's scope and qualification. The file leaves those physical questions open, along with whether either of two nested operations qualifies as a UEF and whether the larger operation subsumes any experience of the smaller one.

IER source sections in `IER-unity-context.md` are *The Full Membership Relation*, *Physical Stage Continuity*, *Access, joint compatibility and changing interfaces*, and *Why unity follows under identity*.
