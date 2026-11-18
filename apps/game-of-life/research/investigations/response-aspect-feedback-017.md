# Proposal 0.17: response-aspect upkeep, loss and external restoration

Date: 17 November 2026\
Authority: non-canonical bounded native model evidence\
Root owner: todo-245, coordinated with todo-237\
Software: existing recorder/C engine; portable checkers in app tools

## Question and frozen protocol

Does proposal 0.17's response-aspect example have the predicted native return
dependence, loss and restoration histories? Does the coarse response quotient
remain valid when a named exterior edit is admitted? These questions are ready
without assigning a whole-organization or unity label.

The [protocol](../protocols/response-aspect-feedback-017-v1.json) fixes all
predictions before execution. It identifies source
`bded9716:_work/unity/development/gol-relational-proposal.md`, SHA-256
`7d303d087e16b92005dbf76b1df89a9384fb1dc9bf9321e308066317071aadff`.
The original stronger P3-T4 diagnostic question is not an experimental endpoint.

## Cases and independent expectations

Use a complete 3×3 torus, B3/S23, center (1,1), and the eight exterior sites in
row-major order. Enumerate all 56 exterior masks with exactly three ON sites.
The independent oracle follows the exact topology: each site sees the other
eight; total population three gives the full board, four preserves the input,
and nine gives the empty board. No independent simulator advances a world.

For each mask compare:

| Branch | Preparation/actions | Predicted complete-board and aspect/center path |
| --- | --- | --- |
| Stable | Center ON; four native updates. | The complete four-ON board persists. R1 → R1 → R1 → R1 → R1. |
| Center removed | Center OFF; four native updates. | Three ON → all nine ON → empty → empty → empty. R0 → D1 → D0 → D0 → D0. |
| Externally restored | Center OFF; one native update; install the original exterior with ON center; three native updates. | Three ON → all nine ON → **install** original four ON → unchanged thereafter. R0 → D1 → install-R1 → R1 → R1 → R1. |

R is three exterior ON sites; T is two; D covers the remaining counts. These
are response classes, not material components. The first center output in the
stable/removed pair is equal despite different exterior aspects, while their
later outputs differ. Restoration has the stable endpoint but an external
install/segment break, not a native repair event.

The edit witness starts from exterior masks 3 and 5, both T, with center ON.
Force exterior bit 1 OFF while preserving other values, then step natively.
The same edit gives D/T and next-center outputs 0/1; the complete outputs are
empty/full. Thus the three-class quotient does not commute with the enlarged
action policy. It remains valid for its original policy; the previously derived
256-class refinement handles named exterior assignments.

## Execution, primary evidence and reproduction

The portable checker is [research_aspect_feedback.py](../../tools/research_aspect_feedback.py).
Run from the app directory:

```bash
make test-relations
make research-aspect-feedback OUTPUT_DIR=/tmp/gol-feedback-new
make research-check-aspect-feedback RUN_DIR=research/investigations/runs/2026-11-17-response-aspect-feedback
```

The [relation reference](../relations.md) supplies all tested definitions and
public reproduction commands. No private proposal file or Git commit is loaded;
its identifier above records the original prediction source. This portable
capture replaces the superseded root-owned capture, with identical physical
findings and app-relative source fingerprints.

Creation requires a fresh directory. Use another output path to reproduce.
Verification is read-only. The checker uses the existing native recorder and
compares complete literal expected boards, event types, segment resets,
response sequences, artifact/fixture/proposal/source fingerprints and native
replay. Self-checks reproduce two fresh bundles, reject overwrites and forged
derived results, and verify the read-only property. No app feature is added.

The [saved report](runs/2026-11-17-response-aspect-feedback/results.json) indexes
11 artifacts: protocol, five manifests and five complete native recordings.
They cover 168 parameterized branch preparations and two edit witnesses,
with 674 native updates. The five recordings contain 902 board snapshots;
external installs distinguish preparations and restoration from native updates.
The report SHA-256 is
`c007a3e28453a72329bad593f69db9755a848d9f50e89f6a98139bc84b461c28`.

All 56 matched triples and both edit witnesses agree with their independent
expectations. All restored endpoints match their stable counterparts. The
native and externally restored histories remain different in the raw events.
The creation, read-only verification and self-checks pass.

## Result and inference boundary

**SUPPORT**, at the specified native model scope, for return dependence,
upkeep/loss of a response condition, external restoration versus native history,
and the declared intervention-compatibility counterexample. There is no claim
that the baseline repaired material loss, that the center alone maintained an
organism, or that the exterior is one organization. Preserving a response class
is not establishing a particular material bearer or its turnover.

One assistant specified, implemented, reviewed and reproduced these bounded
cases; independent oracles are not independent personnel. This is not a new
biological study, another Life engine, a UEF/identity/unity diagnostic or a full
numerical-continuity test. Root physical-transfer and completion records own
any wider inference; the observed files are retained at their original scope.
