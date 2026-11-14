# Q1 Draft Protocol: Recurrence Through Live-Site Turnover

Status: draft reconstruction protocol; not preregistered\
Authority: non-canonical\
Date: 2026-11-13

## Bounded question and readiness

For the specified glider and toad, can their measured recurrence coexist with
turnover of live sites, and how much continuously live overlap remains over
one cycle?

Readiness: **READY for the mathematical reconstruction**, using exact boards
and set operations. This does not qualify the app's proposed history recorder,
which is not implemented. Confirmation with that apparatus requires its own
qualification and a frozen version of this protocol.

The question comes from the user's Life-machine discussion and the physical
persistence question in [Persistence](../../../../IER/IER-persistence.md).
It is a model-local compatibility demonstration, not a necessary or sufficient
condition for experience. Expected answers were already discussed before this
protocol was written.

## Declared context

Use synchronous B3/S23 with eight neighbors, exact integer coordinates and no
random initialization. Use the app's
[glider fixture](../worked-examples/fixtures/glider.cells)
for generations 0–4 and
[toad fixture](../worked-examples/fixtures/toad.cells)
for generations 0–2. The worked examples give the expected full live sets.
Account for the loader's bounding-box normalization when assigning origins.

For the first software run, use a fixed 32x32 bounded world with diagram
origin (5,5), no resize and no interventions after initialization. A headless
runner should use the production world engine and preserve exact initial
boards; it is proposed in the
[statistics plan](../persistence-statistics-plan.md).
Until then, the native world tests are a limited software cross-check, not
a claim to have executed this exact instrumented protocol.

The comparison grain is one synchronous generation. The observation intervals
include both endpoints. Treat the complete live set as the measured footprint,
while retaining the full declared board for equality and neighbor calculations.
This is not an independently established constitutive boundary.

## Decision rule

For each case, list every complete live set L(t). Verify every transition
against B3/S23. Compute the interval union and intersection, and the survivors,
births and deaths for each adjacent pair.

The glider demonstrates the bounded compatibility if L(4) equals L(0)
translated by (+1,+1) while the intersection of L(0) through L(4) is empty.
The toad demonstrates recurrence with partial live overlap if L(2) equals
L(0), L(1) differs, and the interval intersection contains two sites.
Their interval union sizes must be calculated from the complete lists and
retained with the derivation; the glider's expected union is eleven and the
toad's is ten.

Zero continuously live overlap means that no grid site remained live throughout
the observation. It does not mean that every physical process in the computer
was replaced, or that all model sites ceased to exist.

## Controls and confounds

| Control | Expected discrimination |
| --- | --- |
| Block | Fixed recurrence with all four sites continuously live |
| Empty grid | No live footprint; no invented translating live shape |
| Translation of the initial pattern within a sufficient dead margin | Same counts and recurrence; coordinates shift consistently |
| Toad plus the outside perturbation fixture | Same first-step population can coexist with different boards and failed ordinary return |
| Complete-board comparison | Detect omitted births or extra components that summary counts could hide |

Unchanged display frames are not extra generations. Finite-board boundaries,
torus seams, random initial debris, in-place asynchronous updating, missed
initial frames and loader normalization are explicit confounds. Reject a run
whose manifest or initial board fails the context declaration. Save the failed
run and reason instead of repairing the record silently.

## Evidence and possible outcomes

Preserve code revision, rule, world geometry, topology, exact initial board,
placement, complete generation boards and the set/count derivation. For a
software study, distinguish expected fixtures from observed output and identify
the independent check. Add a dated result only after executing the chosen
apparatus; this draft has no run manifest or accepted result.

Agreement supports only the stated model compatibility. A discrepant board
requires checking the fixture, timing, rule and apparatus before interpretation.
Missing states make interval overlap indeterminate; endpoint agreement cannot
fill the missing history. Conflicting process selections route to a separate
boundary question.

## Separate next protocol

Q2 will compare generated and externally installed identical checkpoints.
Snapshot equality is predicted to persist under subsequent deterministic
updates, while logged production events differ. It needs a defined install
operation and reliable event capture before execution. It cannot be decided
from Q1's recurrence and overlap statistics. Neither Q1 nor Q2 establishes
all-five constitutive membership, full subject continuity or IER collapse.
