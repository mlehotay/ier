# Epistemic Kettle Lean example

[epistemic-kettle.lean](epistemic-kettle.lean) checks a small part of the
[Epistemic Kettle](../../examples/collections/epistemic-kettle/epistemic-kettle.md)
argument: observations constrain rival explanations, and an intervention can
make previously matching predictions diverge. It uses a stipulated prediction
table, not a simulation of heating or a reconstruction of real firmware.

## Candidate predictions

Three candidate accounts are called fixed threshold, rate sensitive, and lookup
table. These names label rival hypotheses; their internal computations are not
implemented. Each predicts a summary containing cutoff temperature, final
temperature, cutoff time, and beep time. Temperatures are Fahrenheit and times
are arbitrary toy ticks.

| Trial | Candidate | Cutoff | Final | Cutoff tick | Beep tick |
| --- | --- | --- | --- | --- | --- |
| Baseline | All three | 187 | 190 | 10 | 14 |
| Lower fill | Fixed threshold | 187 | 194 | 6 | 10 |
| Lower fill | Rate sensitive and lookup | 184 | 190 | 5 | 10 |
| Warm start | Fixed threshold | 187 | 190 | 3 | 7 |
| Warm start | Rate sensitive and lookup | 185 | 190 | 2 | 7 |

The table defines the example. It does not establish that a real controller
responds this way or that every fixed-threshold controller overshoots at lower
fill. A physical account would need sensor, heater, vessel, water, and
environmental dynamics as well as a controller.

## What the proofs establish

`Indistinguishable` means two candidates predict equal observations on every
trial in a declared test set. The generic lemmas prove that adding tests can
only refine agreement classes and that projecting observations to a poorer
channel preserves existing agreement.

The worked results show that:

- Baseline observations leave the threshold and rate-sensitive candidates
  indistinguishable.
- Adding the lower-fill trial distinguishes those two candidates.
- Equal final temperatures on the warm-start trial do not imply equal cutoff
  observations or timing.
- Rate-sensitive and lookup candidates remain indistinguishable throughout
  this three-trial menu despite being distinct candidate accounts.

The last result preserves underdetermination at a stated scope. It does not
claim that every possible physical observation or intervention would fail to
distinguish actual programs, nor that the surviving explanations have equal
plausibility. The selected summary is narrower than the collection's default
ordinary observations, which can also include sound, steam, and direct water
inspection. Investigator observations are not assumed to be controller inputs.

These are methodological results. They attribute neither experience nor its
absence to the kettle and do not establish IER closure, ownership, or continuity.

## Running the example

From this directory, run:

```sh
lean epistemic-kettle.lean
```

The file needs no Mathlib or local imports. It reports axiom dependencies for
all seven theorems. In the development repository, `make check-lean` checks
this example together with unity, slack, and Vector pickup.
