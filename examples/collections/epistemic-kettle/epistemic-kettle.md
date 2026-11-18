# The Epistemic Kettle

## What Can Behavior Reveal About an Opaque Program?

This example treats an ordinary variable-temperature electric kettle as a deliberately simple case in AI explainability.

The kettle is assigned an intelligence task:

> heat water accurately to a requested temperature under changing physical conditions.

Human investigators can interact with the kettle normally. They can hear it warming, hear the heater disengage, see steam, open the lid, inspect the water, read the display, time the cycle, and repeat the task under different conditions.

What they cannot do is inspect the kettle's source code, firmware, programmed control logic, or internal computational state. They know that it has been constructed and programmed to perform the task, but they cannot determine how merely by reading the program.

They must explain the kettle from its behavior.

The governing question is therefore:

> When an opaque programmed system performs a task successfully, what can ordinary observation and intervention justify investigators in claiming about how it works?

## Status

This is a noncanonical worked example in the Epistemic Kettle collection.

It introduces no IER ontology, criteria, diagnostics, or new mathematical commitments.

Calling the kettle an intelligent system specifies its role in the example. It does not establish that the kettle:

- instantiates a Unified Experiential Field
- exhibits intrinsic closure
- has experience
- has cognition or agency in the specialized IER senses
- possesses an internal model
- or bears intrinsic constraint in the IER sense

The kettle's opacity is also methodological rather than canonical. Its ordinary behavior is available, while its programmed organization is unavailable for direct inspection. This differs from the stronger IER concept of epistemic opacity concerning the structural privacy of experiential organization.

## The Explainability Experiment

Suppose the kettle is set to:

$$ 190^\circ \mathrm{F}. $$

The user starts the cycle.

The kettle begins to warm. Its sound changes as heating continues. The display rises. At some point the heater disengages with a click. The displayed temperature may continue to rise as stored heat redistributes through the heating element, vessel, and water. Steam may become visible. Later the kettle beeps to report that the task is complete.

Suppose the resulting water temperature is close to 190°F.

The kettle has performed successfully. But that performance alone does not reveal how it was produced.

The kettle might use:

- a fixed cutoff threshold below the target
- a rule sensitive to the recent rate of temperature change
- a lookup table for expected overshoot
- repeated short heating pulses near the target
- a feedback-control law
- an adaptive relation learned across cycles
- a detailed internal thermal model
- a simple heuristic calibrated to the kettle's physical construction
- or some combination of these

The investigators observe competent behavior before they possess an explanation of it.

That is the basic explainability problem.

## Ordinary Observation Is Available

The investigators are not restricted to a single number delivered through an artificial interface.

They may use any ordinary observation available when operating a kettle:

- the selected target temperature
- the starting and displayed temperatures
- warming, boiling, clicking, and beeping sounds
- visible indicators
- elapsed time
- visible steam
- the condition and motion of the water
- whether heating appears continuous or intermittent
- the temperature reached when the kettle declares completion
- differences between repeated cycles

They may open the lid and look at the water. They may change the fill level, begin with warmer or cooler water, choose a different target, interrupt a cycle, restart the kettle while it is still warm, or compare its behavior in different ordinary environments.

The restriction is not sensory deprivation.

It is:

```text
ordinary physical and behavioral observation: available

direct access to programmed organization: unavailable
```

A later experiment may deliberately narrow the observation channel. Investigators might ask what can be inferred from only the display, only the completion signal, or only a recorded temperature trajectory. Such a restriction must be stated locally. It is not the default condition of the collection.

## Two Epistemic Problems

The kettle and its investigators face different problems.

The kettle must perform the temperature task. The investigators must explain how it performs that task.

```text
the kettle solving the temperature task
is not
humans solving the explainability problem
```

The kettle's programmed organization may preserve whatever distinctions are sufficient for successful control. The investigators may want a much richer account of sensing, internal state, physical dynamics, action selection, and failure.

These goals should not be conflated.

A controller can succeed without providing an explanation of itself. An investigator can predict the controller without identifying its actual program. A human explanatory model need not resemble any organization present inside the kettle.

This requires typed uses of the word *model*:

- an investigator model is an external explanation of the kettle
- a predictive surrogate reproduces or predicts some observable behavior
- a physical model describes heating and cooling dynamics
- a hypothesized internal model is a proposed organization inside the kettle

Evidence for one is not automatically evidence for the others.

## A Minimal Structural Description

The experiment can be described without assuming a particular controller architecture.

Let:

- $r$ be the requested temperature
- $x_t$ be the physical condition of the kettle and water at time $t$
- $s_t$ be whatever sensor input is available to the program
- $z_t$ be any internal program state, if such a state is used
- $a_t$ be the heating action
- $o_t$ be what the investigators can ordinarily observe

A candidate program might be expressed abstractly as:

$$ (z_{t+1},a_t)=F(z_t,s_t,r). $$

The physical kettle then changes according to some relation:

$$ x_{t+1}=G(x_t,a_t,e_t), $$

where $e_t$ collects relevant environmental conditions.

The investigators receive an observational trace:

$$ o_t=O(x_t,a_t). $$

The investigators can observe consequences of $F$ as they propagate through $G$ and become available through $O$. They cannot inspect $F$ directly.

This notation does not claim that the kettle literally contains variables with these names. It marks distinctions that the investigation must keep separate.

## Human Observation Is Not Kettle Sensing

The investigators and the kettle do not necessarily receive the same distinctions.

Investigators may see steam that the kettle does not detect. They may hear a cutoff click without knowing which internal condition caused it. They may observe the water directly while the kettle receives only an electrical signal from a temperature-sensitive component.

Conversely, the kettle may receive signals unavailable to ordinary human observation. It might distinguish heater state, sensor history, elapsed time, or internal thresholds without displaying them.

Therefore:

```text
investigator observation
is not
kettle input
```

This is one of the most important disciplines in the example.

An investigator must not infer that the kettle uses a visible distinction merely because the investigator can see it. Nor should the investigator assume that everything used by the program is externally displayed.

The explainability problem concerns relations between these two differently structured interfaces.

## Behavior Produces Equivalence Classes

Suppose two candidate programs produce the same observable behavior in every trial performed so far.

Relative to the current test set, investigators cannot distinguish them.

Write:

$$ M_1 \sim_B M_2 $$

when candidate explanations $M_1$ and $M_2$ remain behaviorally indistinguishable under the observations and interventions currently available.

This is not physical or computational identity.

A fixed-threshold program and a rate-sensitive program might behave identically when tested only with one litre of room-temperature water. A lookup table and a compact physical calculation might generate the same cutoff times throughout the tested range. A stateful program and a stateless approximation might agree when every trial begins from a cold kettle.

The epistemic object is therefore not necessarily one hidden program awaiting recovery.

It may be:

> a family of candidate organizations still compatible with the kettle's behavior.

The investigators should not manufacture uniqueness when the evidence supports only an equivalence class.

## Outcomes, Snapshots, and Trajectories

A final temperature near 190°F establishes task success under one condition. It carries little information about the process that produced it.

A single intermediate reading preserves more structure, but many different conditions can still produce the same displayed temperature.

A trajectory preserves more again:

$$ o_0,o_1,\ldots,o_n. $$

The trace may include:

- temperature rise
- changes in heating sound
- pauses or pulses
- the time of the cutoff click
- continued warming after cutoff
- settling behavior
- the delay before the completion beep
- differences between early and later cycles

Two programs that produce the same final temperature may produce different trajectories. Two trajectories that look alike at first may diverge near the target. Two cycles that appear identical while heating may behave differently after cutoff.

Temporal organization therefore refines the candidate explanation space.

It does not reveal the program directly. It preserves more discriminating consequences of that program.

## Intervention Makes Explanations Diverge

Passive observation may leave several explanations equally adequate.

Suppose two candidates predict the same behavior when the kettle starts cold with one litre of water, but differ when the water volume changes. One predicts a fixed cutoff temperature. The other predicts that cutoff will move with the rate of warming.

Changing the fill level may make the candidates diverge.

Conceptually, for intervention $u$:

$$ M_1(u)\neq M_2(u). $$

Useful interventions might include:

- changing the water volume
- changing the starting temperature
- requesting different target temperatures
- running a second cycle while the vessel remains warm
- interrupting and restarting a cycle
- briefly opening the lid
- comparing cycles under different ambient conditions
- repeating identical trials to look for adaptation or drift

An intervention is valuable not merely because it creates more data. It is valuable when rival explanations predict different consequences.

The intervention still does not open a window onto the hidden program.

It changes the conditions under which differences among candidate programs may propagate into observable behavior.

> Intervention refines an equivalence class; it does not provide direct access to the program.

## The Attribution Problem

The program does not heat water by itself.

Its actions operate through a physical kettle containing a heater, vessel, sensor arrangement, water, and surrounding environment. Successful performance may depend on relations across that entire organization.

Suppose the heater clicks off when the display reads 187°F and the water subsequently settles near 190°F.

Investigators might explain this by proposing:

- an explicitly programmed three-degree offset
- a rate-dependent estimate of expected overshoot
- a lookup table indexed by recent heating history
- sensor lag that makes the displayed reading trail the relevant temperature
- residual energy in the heater and vessel
- a simple threshold calibrated to stable physical coasting
- or a combination of these

The same visible regularity can be attributed differently across program and physical dynamics.

This creates a central explainability danger. Investigators may attribute competence to an internal representation when it is produced partly by embodied physical organization. They may also dismiss a sophisticated policy as passive physics because both contribute to one observed trajectory.

The question is not only which variables matter.

It is also:

> Where, within the coupled organization, should the explanatory work be located?

## Decomposition Is a Hypothesis

An explanation decomposes the kettle's performance into proposed parts or relations.

One explanation might use:

```text
sensing
state estimation
overshoot prediction
heater action
completion decision
```

Another might use:

```text
current temperature
rate of change
fixed thresholds
timed delays
```

A third might emphasize:

```text
program rule
heater inertia
vessel heat
water mixing
sensor lag
```

These decompositions are not automatically equivalent. Nor does the usefulness of one show that the program itself contains corresponding modules or variables.

At least three decomposition problems should remain distinct.

### Performing-system decomposition

What physical and programmed organization actually produces the kettle's behavior?

### Evidence decomposition

What independently varying distinctions are carried by the observation and intervention record?

### Candidate-explanation decomposition

Along which dimensions do the surviving explanations differ?

These spaces need not align.

Several physical and computational causes may appear through one behavioral regularity. One effective predictive variable may correspond to no single programmed component. Two explanations may divide the system differently while predicting the same tested behavior.

Decomposition is therefore part of what must be justified, not a neutral description given in advance.

## Projection Is Not Decomposition

The investigators' observations preserve some distinctions in the kettle's operation and suppress others.

That relation may be described as a projection:

$$ O:X\rightarrow Y, $$

where $X$ is a richer space of program, physical, and environmental conditions and $Y$ is the organization preserved in ordinary observation.

A projection suppresses distinctions.

A decomposition proposes distinctions within the system, the evidence, or the candidate explanation space.

These operations move in different directions:

```text
projection
is not
decomposition
```

Decomposing the observable trace cannot restore distinctions that never became behaviorally available. Seeing that the kettle has distinct warming, cutoff, coasting, and completion phases does not prove that the program contains four corresponding modules.

First ask what the behavior preserves. Then ask which proposed organization could account for it.

## Effective Coordinates and Invariants

Investigators may discover that the behavior is predicted more reliably by combinations of observations than by physically intuitive variables considered separately.

Near the target, for example, an effective description might use:

$$ (e,\dot T,c), $$

where $e$ is temperature error, $\dot T$ is recent rate of rise, and $c$ is an empirically estimated coasting tendency.

These quantities may predict cutoff and final temperature without corresponding to named variables in the inaccessible program.

Different investigator models may use different coordinates while preserving the same relation. One may speak of expected overshoot. Another may use recent rate and elapsed heating time. A third may fit a response surface without assigning familiar physical meanings to its axes.

A change of coordinates can make a regularity easier to express. It does not add information or establish what the program literally computes.

The strongest warranted result may concern an invariant behavioral organization:

> across changes in description, the kettle's cutoff shifts systematically with these features of the preceding trajectory.

That claim is weaker than identifying a particular internal variable, but stronger than reporting an isolated correlation.

## False Conflation and False Factorization

The reconstruction can fail in two complementary directions.

### False conflation

Investigators may treat two candidate programs as identical because their current tests do not distinguish them.

This deletes a potentially consequential difference.

A fixed threshold and an adaptive policy may look the same under familiar conditions but diverge after an interrupted cycle or a change in fill level.

### False factorization

Investigators may identify useful parts and then assume those parts vary independently.

For example, they might describe:

```text
control rule
sensor behavior
heater dynamics
```

and then assume any locally plausible control rule can be combined with any locally plausible sensor and heater model.

But successful organizations may occupy only a restricted region of the apparent product space:

$$ X_{\mathrm{adm}} \subsetneq X_{\mathrm{program}} \times X_{\mathrm{sensor}} \times X_{\mathrm{physical}}. $$

A threshold that works with one sensor placement and heater inertia may fail with another. What appears to be a portable program feature may actually be calibrated to the physical organization.

The missing explanatory structure is not always another hidden variable. It may be the relation among the proposed parts.

> False conflation deletes difference.

> False factorization invents independence.

An adequate explanation must avoid both.

This is an ordinary methodological point. It does not establish canonical IER non-product structure or intrinsic closure.

## Rival Maps May Remain

Consider five explanations of accurate arrival at 190°F:

| Candidate map | Proposed organization |
| --- | --- |
| Threshold | Cut power at a fixed temperature below the target |
| Rate-sensitive | Adjust cutoff using the recent rate of warming |
| Lookup | Select a correction from previously calibrated conditions |
| Adaptive | Update the control relation across repeated cycles |
| Embodied heuristic | Use a simple rule whose success depends on stable kettle physics |

Several ordinary experiments may eliminate some candidates.

Changing the fill level may challenge a fixed threshold. Repeating hot-start cycles may expose hidden state. Varying target temperatures may distinguish a fixed offset from a proportional correction. Identical repeated trials may reveal adaptation.

Yet no finite set of ordinary tests guarantees that one uniquely correct program will remain.

Two different programs may implement the same policy throughout the tested domain. Two different decompositions may make identical predictions. A sophisticated program may deliberately reproduce the behavior of a simpler one.

The proper result may therefore be:

$$ [M] = \{M' : M' \text{ remains behaviorally indistinguishable under available tests}\}. $$

A surviving equivalence class is not a failed investigation.

It records which organizational differences the current evidence can and cannot support.

## Prediction, Imitation, and Explanation

Several achievements that are often grouped together should remain separate:

$$ \boxed{ \text{task success} \neq \text{behavioral prediction} \neq \text{behavioral imitation} \neq \text{program identification} \neq \text{mechanistic explanation} } $$

The kettle can succeed without explaining itself.

An investigator may predict exactly when the heater will disengage without knowing which programmed condition triggers the action.

A second device may imitate every tested kettle behavior using a different program.

An investigator model may identify a stable functional organization without recovering literal source code.

These are all genuine achievements, but they answer different questions.

Predictive success is evidence for an explanation. It is not automatic proof that the explanation reproduces the actual internal mechanism.

## Task Equivalence Inside the Kettle

The investigators seek distinctions that help explain the kettle. The kettle needs only distinctions that help perform its task.

Different physical conditions may require the same action:

> stop heating now and allow the temperature to coast.

Relative to the control task, those conditions are equivalent even if investigators can distinguish them physically.

Write:

$$ x_1\sim_{\tau}x_2 $$

when states $x_1$ and $x_2$ imply the same action for task $\tau$.

Task equivalence differs from observational equivalence.

Two states may look different to investigators but be treated identically by the kettle. Two states may look identical to investigators while being distinguished by an internal sensor history. The two equivalence structures need not align.

This helps explain how competent performance can depend on a compressed organization. The kettle need not distinguish every physical cause if those causes do not alter the appropriate action.

## Scope and Informative Failure

An explanation should state where it has been tested.

A candidate map may work for:

- one range of water volumes
- one range of requested temperatures
- cold starts
- uninterrupted cycles
- ordinary room conditions

It may fail when the vessel is already warm, the cycle is interrupted, the lid is opened, the water volume changes sharply, or the target lies outside the tested range.

Failure need not mean only that one numerical parameter was wrong.

It may indicate:

- hidden internal state
- an omitted input
- adaptation across trials
- an incorrect program-body decomposition
- sensor lag or another physical dependency
- false factorization
- a changed operating regime
- a scope boundary

Failure is therefore structurally informative.

It shows where the current map stops preserving the organization relevant to the behavior.

A narrow explanation with an explicit scope is better than a supposedly complete explanation that fails unpredictably.

## What the Evidence Warrants

Explainability claims vary in strength.

The investigators might establish:

1. Observed regularity: the heater usually disengages below the selected target.
2. Scoped prediction: cutoff and final temperature can be predicted within tested conditions.
3. Functional organization: behavior depends on recent trajectory rather than current temperature alone.
4. Architectural preference: a family of stateful explanations fits interventions better than tested stateless alternatives.
5. Internal attribution: the kettle contains a particular variable, representation, or control module.
6. Literal identification: the inaccessible program has been uniquely reconstructed.

Each step requires additional warrant.

Behavioral evidence may strongly support the early claims while leaving later claims unresolved. Even decisive evidence against a fixed-threshold account does not by itself establish that the kettle represents overshoot, predicts a future temperature, or implements a particular named control architecture.

The investigators should make the strongest claim their evidence supports and no stronger.

## What the Kettle Does Not Teach

The example does not show that opaque programs are fundamentally unknowable.

It does not show that behavioral evidence is weak or merely subjective.

It does not show that all explanations are equally good.

It does not show that a privileged mechanistic decomposition never exists.

It does not establish that the kettle contains an internal model, learns, understands, experiences, or acts as an IER agent.

It does not turn behavioral equivalence into computational or physical identity.

It does not promote ordinary coupling or nonfactorability into IER intrinsic closure.

Its lesson is narrower:

> Behavior supports disciplined, progressively refined explanation without guaranteeing unique recovery of the programmed organization that produced it.

## Structural Summary

The epistemic kettle begins with competent performance.

Investigators can observe the kettle richly and ordinarily, but they cannot inspect its program.

A successful cycle establishes that the task was performed. A full trajectory preserves more explanatory structure than the final temperature alone. Interventions make some rival explanations diverge. Repeated tests refine the surviving candidate family.

Yet every reconstruction must keep several distinctions intact:

```text
kettle input
is not
investigator observation

kettle task organization
is not
investigator explanatory model

prediction
is not
imitation
is not
program identification

projection
is not
decomposition
is not
factorization
```

Some apparent differences vanish because the current tests cannot resolve them. Some apparent parts cannot be separated because successful performance depends on their relation. Several explanatory maps may remain adequate within the tested scope.

The investigation therefore proceeds as:

```text
competent performance
→
ordinary observation
→
candidate explanations
→
discriminating interventions
→
revised equivalence classes
→
scoped explanatory maps
```

The central question is not whether behavior reveals nothing or everything about an opaque program.

It is:

> Which claims does the behavior support, which interventions can refine those claims, and which differences must remain unresolved?
