# Reproducing the Life relations

Status: implemented, bounded model evidence\
Authority: non-canonical\
Updated: 17 November 2026

This reference defines the relations tested by the public GoL app. All world
trajectories use `src/world.c`, the native B3/S23 engine. Literal boards,
closed-form neighborhood predicates and finite partition refinements check the
outputs. They do not run an alternative Life simulator.

Unity follows from the identity proposal, which is not diagnosable. Operating
as one organization is itself a unity question. These tests investigate physical
relations without diagnosing identity, unity, UEF qualification, ownership or
full token continuity. The earlier demand for an independent organization
classifier is withdrawn, not passed. No fixture receives an experiential verdict.

## Build, verify and reproduce

Use a Unix-like environment with Make, a C11 compiler and Python **3.10 or later**
(standard library only). Run these commands from `apps/game-of-life` in the public
repository. The app's tools, protocols and evidence are sufficient; no private
`_work`, private Git history, downloaded papers or root Makefile is required.

```bash
make test
make research-check-relations
```

`make test` includes `test-relations`: native response equivalence, intervention
compatibility, recurrent-subhistory/support comparisons and feedback self-checks.
`research-check-relations` replays both saved trajectory bundles and recalculates
both finite-analysis reports. Verification does not rewrite saved evidence.

To create fresh trajectories and analysis reports:

```bash
RELATION_RUN_DIR="$(mktemp -d)"
make research-mechanism OUTPUT_DIR="$RELATION_RUN_DIR/mechanism"
make research-check-mechanism RUN_DIR="$RELATION_RUN_DIR/mechanism"
make research-aspect-feedback OUTPUT_DIR="$RELATION_RUN_DIR/feedback"
make research-check-aspect-feedback RUN_DIR="$RELATION_RUN_DIR/feedback"
make -s research-response-aspects > "$RELATION_RUN_DIR/response-aspects.json"
make -s research-recurrent-subhistories > "$RELATION_RUN_DIR/recurrent-subhistories.json"
```

Creation refuses existing output directories. The analysis commands print JSON;
`-s` suppresses Make's command echo. Saved references are in
[investigations/runs/](investigations/runs/). Source fingerprints are checked;
compiler/library fingerprints and Git headers record the original capture and
need not be identical across machines. Reproduction requires identical native
boards, event histories and derived findings, rather than byte-identical builds.
The verifier separately checks saved artifact hashes and current native replay.

## Native inputs, production and history

For a site q, let N(q) be its eight neighboring sites and n their ON count.
The next value is ON iff `n = 3` or `(q is ON and n = 2)`. Bounded topology
supplies OFF beyond the board; toroidal topology wraps coordinates. Every
protocol fixes dimensions, topology, initial ON sites and the intervention family.
OFF input conditions are recorded alongside ON conditions.

Produced continuation means that a native update produces a value used by a
later native update. The recorder distinguishes `step` from external `install`
events, with segment resets on installation. Installing an equal checkpoint can
preserve all later boards while changing actual recorded history. This does not
establish full physical inheritance or continuity. The [native audit dossier](investigations/mechanism-phase4-native-audits.md)
and its protocol give literal fixtures and expected paths.

## Access, joint compatibility and changing interfaces

Access records whether a proposed local recruitment is enabled under a declared
preparation. Joint compatibility asks whether several individually enabling
changes work together at the **same root board**. On a 3×3 torus, start with an
ON center and two ON exterior sites. Adding either of two further exterior ON
sites individually preserves the center; adding both kills it. The four outputs
are `1,1,1,0`. This tests compatibility, not a conserved molecular resource,
endogenous recruitment mechanism or biological traffic rate.

At one cut a receiver is screened when all allowed changes outside its complete
input neighborhood leave its next output fixed. The audit varies all 128
assignments to seven exterior sites for each of three fixed input preparations:
384 checks. A separate two-step fixture retains an ON core after one step but
changes it after two; current screening does not imply indefinite immunity.
For the moving glider, recompute next-role neighborhoods minus current live
roles at every cut: their size stays 17 while their locations change.
Neither a ring nor a moving footprint thereby becomes a constitutive boundary.

## Response equivalence and action policy

Fix a finite state space M, allowed actions A, native next-state maps and the
observed outputs. Two states are response-equivalent iff they give the same
observations for every finite allowed action word. Begin with equal immediate
output signatures and repeatedly refine by successor-class signatures until
stable. This is the coarsest stable response partition for this finite contract.
It is a response aspect, not a material component or membership label.

The implemented example uses the eight exterior bits of a 3×3 torus as M
(256 masks), and sets the center to 0 or 1 before each native step. Observe the
next center. With R = three exterior ON, T = two and D = all other counts:

| Aspect | Center input 0: output, next aspect | Center input 1: output, next aspect |
| --- | --- | --- |
| D (172 masks) | 0, D | 0, D |
| T (28 masks) | 0, D | 1, D |
| R (56 masks) | 1, D | 1, R |

All 512 native output/next-state transitions satisfy this table. Stability gives
equality for all words by induction; the three different immediate output
signatures give the three-class lower bound. Observing exterior count instead
produces nine classes; observing the whole exterior gives 256.

Enlarge the action policy to include setting any named exterior bit to 0 or 1.
All 4,096 assignments and 2,048 held-bit probes are checked natively. The stable
response partition now has 256 classes. For a short witness, exterior masks
3 and 5 are both T. Force bit 1 OFF: the resulting aspects are D and T, with
next-center outputs 0 and 1 under center input 1. A quotient must respect the
actual allowed actions; the earlier three-class quotient cannot serve the
enlarged policy. The [response-aspect report](investigations/runs/2026-11-17-response-aspects.json)
records the exact action and native storage checks.

## Upkeep, loss and external restoration

For each of all 56 three-ON exterior masks, compare three native histories:

| Preparation | Native or recorded path |
| --- | --- |
| Center ON | The four-ON board stays fixed for four updates: R1 → R1 → R1 → R1 → R1. |
| Center OFF | Three ON → full nine ON → empty → empty → empty: R0 → D1 → D0 → D0 → D0. |
| Center OFF, then restoration | After the first update, externally install the original exterior plus ON center; three further native updates preserve it: R0 → D1 → install-R1 → R1 → R1 → R1. |

Here the suffix is the center value. Stable and removed branches have equal
first center outputs but different later responses. Restoration matches the
stable endpoint with an explicitly different event history. All 168 preparations
and two named-edit witnesses match literal complete-board oracles. The
[feedback dossier](investigations/response-aspect-feedback-017.md) records five
native recordings, 674 updates and 902 snapshots. External restoration is not
native material repair; persistent response conditions are not full self-maintenance.

## Recurrent generators and context-relative support

For each literal block, blinker, toad, glider, and near/far block/toad pair,
enumerate every subset of the initial ON sites. Evolve natively for the declared
period. Retain nonempty histories that remain inside the reference's live sites
at every phase and recur at the declared translation. Order these histories by
inclusion of their time/site ON occurrences; select the minimal histories and
check whether they cover the reference at every phase. Both topologies give
matching results over 17,648 initial subsets across 16 fixtures. These are
recurrent generators at this selection policy, not independently certified
organizations. OFF support and effects outside the selected live history still
matter.

The same checker enumerates minimal OFF support certificates with current ON
literals fixed, distinguishes shared support from independent generators, and
checks how the same block can be neutral, promote an otherwise absent birth or
suppress an otherwise present birth in different exterior contexts. It also
checks that a retained ON core can coexist with loss of its enabling OFF ring.
The [complete report](investigations/runs/2026-11-17-recurrent-subhistories.json.gz)
includes phase histories, support certificates and native-board transcript hashes.
The reference is gzip-compressed JSON to avoid storing repeated certificate text
uncompressed; the verifier reads it with Python’s standard library. Fresh
`research-recurrent-subhistories` output is ordinary JSON.

## Service representation and interpretation limits

For any cut A/B of a deterministic update, local output maps can receive the
required opposite-side input values. The audit reconstructs all 512 torus
outputs under two cuts: 1,024 matches. Responsive channels can preserve feedback;
this transition representation does not certify separately continuing
organizations or diagnose disunity. Conversely, reciprocal effects, recurrence
and recovery do not certify one organization.

If two receivers have equal complete initial states, the same native map f and
identical full input histories u, then `z(t+1)=f(z(t),u(t))` gives equal receiver
histories by induction. Matching a scalar supply can omit regulatory inputs;
matching all inputs still does not make the producing mechanisms identical.
These are conditional limits on replacement arguments, not unity tests.

The model does not simulate Rab chemistry, Ran gradients, ATP turnover, motors
or host maintenance. No complete material renewal, intrinsic tension, owned
membership or full diachronic continuity is measured. A biological analogy or
software replay cannot supply those missing physical facts. The implementation
and expected-output checks were reviewed by the implementing assistant;
independent personnel review is not claimed.
