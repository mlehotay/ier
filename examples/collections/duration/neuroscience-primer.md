# Neuroscience Primer for *Four Durations, Not One*

## Status and Purpose

Status: reference  
Authority: non-canonical

This primer is for readers of Informational Experiential Realism (IER) who are
not specialists in functional neuroimaging or duration perception. It explains
the experiment, analysis, terminology, and evidential limits of Centanino,
Fortunato, and Bueti's 2026 study of visual-duration categorization.

It is a standalone methodological companion to *Four Durations, Not One*. The
division of labor is deliberate:

- this primer explains what the neuroscience paper measured, modeled, found,
  and inferred;
- *Four Durations, Not One* develops the IER reinterpretation and experimental
  program;
- *The Identification Problem for Physicalist Theories of Experience* explains
  why stimulus-brain-behavior relations do not by themselves establish an
  experiential-physical identity.

The central reading rule is simple: do not collapse four different objects just
because each can be described using the word *duration*.

| Object | How it enters the study | What it is |
| --- | --- | --- |
| Physical stimulus duration, $d$ | Experimentally controlled | The display time of the visual stimulus |
| Modeled preferred duration, $\mu_d$ | Fitted to condition-dependent BOLD estimates | The peak of an assumed response function at a cortical vertex |
| Point of subjective equality, $PSE_i$ | Estimated from participant $i$'s choices | The physical duration at the 50% boundary between shorter and longer reports |
| Experienced duration | Present but not independently characterized | The phenomenal temporal organization that an account such as IER seeks to explain |

The experiment established relations among the first three. Beyond the reports
used to estimate the PSE, it did not independently characterize the fourth.

## The Experiment

Thirteen healthy adults performed a visual-duration categorization task during
7-tesla functional magnetic resonance imaging (fMRI). The data had originally
been collected for an earlier study and were reanalyzed for this article.

Before scanning, participants learned a 0.5-second reference duration. During
scanning, each trial presented a blurred circular patch of colored Gaussian
noise for one of six comparison durations: $0.2,\ 0.3,\ 0.4,\ 0.6,\ 0.7,\ 0.8\ \text{seconds}$.

The patch appeared at one of four positions in the lower visual field. Position
was irrelevant to the participant's task but allowed the researchers to test a
model that was sensitive to duration and invariant to spatial position.

The patch changed randomly on every display frame. Longer trials therefore
contained not only more elapsed physical time but also more successive visual
input. The design cannot separate effects of display duration, sustained
stimulation, and amount of frame-by-frame sensory change.

After stimulus offset, the fixation cross remained white for a randomized
0.9-1.2 seconds. It then turned black, cueing the participant to report whether
the comparison had been shorter or longer than the learned reference. The
response was made with one of two fingers of the right hand. Participants had
two seconds to respond, and reaction speed was not emphasized.

Each participant completed 480 trials across ten scanning runs. The task
therefore involved more than exposure to a duration:

- dynamic visual stimulation;
- memory for a learned reference;
- comparison of the current stimulus with that reference;
- categorization into one of two instructed alternatives;
- withholding the overt response until the delayed cue;
- preparation and execution of a mapped finger response;
- and sustained task-directed attention.

It was an explicit timing task, not a passive assay of temporal phenomenology.

## From Neural Activity to BOLD Estimates

The blood-oxygen-level-dependent (BOLD) signal is a hemodynamic measurement. It
tracks changes in blood oxygenation associated with aggregate local neural
activity. It does not record individual neurons, and its time course is much
slower than the activity that produces it.

The scanner acquired a whole-brain volume every 1.32 seconds. The visual stimuli
lasted only 0.2-0.8 seconds. Event-related analysis can estimate whether the
BOLD response differs across those conditions, but it cannot reconstruct the
millisecond-by-millisecond neural trajectory of a single trial from these data.

The main general linear model (GLM) contained one regressor for each of the 24
duration-position combinations and a separate response-onset regressor. The
stimulus regressors were time-locked to stimulus offset and convolved with a
canonical hemodynamic response function. For each cortical vertex and
condition, the analysis produced bootstrapped beta weights; the median beta
weight was used in the subsequent response model.

Offset-locking deserves careful interpretation. At offset, the complete
physical duration of the comparison was available to the participant. Modeling
an event at offset does not imply that the relevant neural activity began only
then. It also does not directly reveal activity ramping or accumulating during
the preceding stimulus. It provides a common event time from which the slow
BOLD responses associated with the duration conditions can be estimated.

The delayed response cue and separate response regressor reduce a simple
conflation of stimulus offset with button execution. They do not isolate one
pure process. Sensory persistence, comparison, decision preparation, and other
trial operations can still contribute to condition-dependent BOLD estimates.

## How "Preferred Duration" Was Fitted

The researchers fitted a population-receptive-field-style model to the 24 GLM
beta weights at each cortical vertex. The model assumes a Gaussian response over
stimulus duration:

$$
R(d) \propto \exp\left[-\frac{(d-\mu_d)^2}{2\sigma_d^2}\right].
$$

It also assumes that the response is invariant to the stimulus's four tested
positions. The fitted parameters are:

- $\mu_d$: the duration predicted to evoke the largest response, called the
  *preferred duration*;
- $\sigma_d$: the width or sensitivity of the modeled response.

The article's later analyses focused on $\mu_d$. Only vertices whose initial
model fit explained at least 10% of the variance were advanced to iterative
optimization, and vertices with negative fitted response profiles were
excluded.

These details make "preferred duration" a model-dependent quantity. It is not:

- a raw BOLD observation;
- a direct recording from a duration-selective neuron;
- a duration liked or experienced by a brain region;
- proof that the response is generated by an internal clock;
- or a measure of experienced duration.

Within the tested range, a fitted peak summarizes how the estimated BOLD
response varied across duration conditions. A peak near the longest tested
duration can also be compatible with a response that rises monotonically over
the sampled range. It does not by itself demonstrate a genuine within-range
neural peak or a moment-by-moment accumulation process.

## Cortical Vertices, Regions, and Maps

A *vertex* is a point on a reconstructed cortical surface. Its BOLD value
summarizes signals from a spatially extended and biologically heterogeneous
volume of tissue. The paper's phrase "neuronal population" is therefore an
interpretation of aggregate fMRI data through a population-response model, not
the direct observation of a cell population with electrophysiology.

The researchers restricted their analysis to atlas-defined regions overlapping
group-level cortical activation at stimulus offset. They then grouped these
regions into nine broader functional streams. The main regional pattern can be
compressed as follows:

| Cortical group | Model-based result | Authors' functional interpretation |
| --- | --- | --- |
| Ventral and lateral visual cortex | Preferences concentrated near the long end of the range; comparatively weaker map organization | Early encoding of duration, possibly through monotonic sensory accumulation |
| Intraparietal and inferior parietal cortex | Preferences often distributed across the tested range | Readout and integration of temporal information |
| Lateral premotor cortex and caudal SMA | Preferences distributed across the tested range | Task- and action-related readout of duration |
| Rostral SMA, anterior insula, and inferior frontal cortex | Many preferences concentrated near the middle of the range | Abstract or categorical representation of the task boundary |
| Left motor and somatosensory cortex | Preferences concentrated toward short durations | Motor preparation associated with the right-hand response |

The first column names anatomy, the second summarizes fitted response
distributions, and the third states an interpretation. The experiment did not
independently perturb the regions or establish that information passed through
them in the proposed order.

A *topographic map* is a spatial arrangement in which nearby vertices tend to
have similar fitted preferences. The researchers assessed this organization
with spatial-autocorrelation and variogram analyses. Positive spatial
organization was found across the analyzed regions, with differences in its
strength and spatial scale.

A map of duration preferences is not a row of clocks. Nor does spatial order
settle what the mapped variable does. The map is a relation among fitted values
on the cortical surface; its mechanism and function require further evidence.

## The Rostral-Caudal SMA Difference

The supplementary motor area (SMA) did not show one uniform response profile.

- More caudal SMA regions contained preferences spanning the tested duration
  range.
- More rostral SMA regions contained many preferences near the range center.

The authors interpret this contrast as a possible transition from duration
readout to a more categorical task representation. The finding itself is a
difference between regional distributions of fitted parameters. It is not a
direct observation of continuous experience becoming discrete, nor does it
identify an experiential event boundary.

Only one right rostral SMA region, a32pr, showed the emphasized positive
association between preferred duration and participants' PSEs. It is therefore
inaccurate to say that rostral SMA as a whole tracked each participant's
behavioral boundary.

## The Point of Subjective Equality

For each participant, the researchers fitted a logistic psychometric function
to the fraction of "longer" responses at each comparison duration. The point of
subjective equality (PSE) is the physical comparison duration at which the
fitted probability of a "longer" response is 0.5.

The PSE locates a behavioral category boundary in physical-duration
coordinates. Because the learned reference was fixed at 0.5 seconds, the PSE's
displacement from 0.5 seconds operationalizes bias in the task. The PSE and bias
should not be treated as two interchangeable numbers in general: the PSE is the
boundary location; bias is its deviation from a specified reference or
normative equality point.

Calling the PSE a *decision criterion* is useful functional shorthand, but the
psychometric estimate does not isolate a single internal decision process. It
can reflect contributions from sensory processing, reference memory,
comparison, categorization, and response policy. Likewise, *subjective* in the
name of the statistic does not make it a direct description of temporal
phenomenology.

The source design contains an important three-way coincidence:

- the learned reference was 0.5 seconds;
- the mean and midpoint of the comparison range were 0.5 seconds;
- and the expected shorter-longer boundary lay near 0.5 seconds.

The study did not present a 0.5-second comparison; the boundary was estimated
between the 0.4- and 0.6-second conditions. Because reference, range center, and
expected category boundary coincide, medium-centered cortical preferences
cannot be assigned uniquely to any one of them. The authors explicitly note
that multiple duration ranges would help characterize candidate boundary
populations.

The researchers correlated each participant's PSE with that participant's
median preferred duration in each stream and region. Positive associations
emphasized in the article occurred at the stream level in:

- left inferior parietal cortex ($\tau = 0.44$);
- left anterior insula ($\tau = 0.44$);
- and right anterior insula ($\tau = 0.59$).

At the finer regional level, the emphasized positive associations occurred in:

- left FOP5 in anterior insula ($\tau = 0.43$);
- left and right AVI in anterior insula ($\tau = 0.49$ and $0.56$);
- and right a32pr in rostral SMA ($\tau = 0.43$).

No inferior frontal region showed the emphasized positive PSE association. The
sample contained thirteen participants, many correlations were examined, and
the reported PSE-correlation $p$-values were uncorrected for multiple tests.
These are candidate individual-difference associations, not a settled map of
subjective temporal boundaries.

## What the Correlation Clusters Show

The researchers also asked whether regional median preferred durations varied
together across participants. They calculated correlations among regions and
streams and used hierarchical clustering to produce a dendrogram. At the stream
level, three clusters were identified:

1. ventral and lateral visual cortex;
2. intraparietal cortex, anterior insula, and inferior frontal cortex;
3. inferior parietal, supplementary motor, premotor, and motor-somatosensory
   cortex.

Here *hierarchical clustering* names a statistical grouping procedure. The
dendrogram represents similarity among patterns of cross-participant
covariation. It does not establish:

- temporal precedence;
- the direction of signal flow;
- effective connectivity;
- causal dependence;
- or three successive processing stages.

The authors use the clustering together with the regional response patterns and
prior literature to propose a functional hierarchy of encoding, readout, and
categorization. That proposal may guide further research, but it is not a
result of the correlation matrix alone.

## How to Read the Paper's Key Terms

Several words in the article have narrower empirical meanings than an IER
reader may initially assign to them.

| Term in the source paper | Careful reading |
| --- | --- |
| **Neuronal population** | A population-level interpretation of vertex-wise BOLD data fitted with a response model, not directly recorded neurons |
| **Tuning** | A modeled relation between BOLD estimates and the six tested durations under an assumed Gaussian response function |
| **Discrete** | Different fitted populations prefer particular locations in the sampled duration range; not discrete moments or experiential events |
| **Categorical** | Medium-centered preferences are interpreted as representing the shorter-longer task boundary; the cause of the centering was not separated |
| **Subjective** | Some fitted preferences covary with an individual behavioral PSE; not an independent measurement of phenomenal structure |
| **Boundary population** | A proposed population whose preference reflects a learned or individual category criterion; not a demonstrated event-segmentation mechanism |
| **Hierarchy** | An anatomical ordering and functional interpretation supported by regional differences and covariance; not demonstrated causal flow |
| **Representation** | A functional description of duration-related neural organization; not proof of an internal timeline, an experienced content, or psychophysical identity |
| **Perception** | In this study, chiefly duration-categorization behavior and its modeled neural relations; phenomenology was not separately mapped |

The terms are not illegitimate. The point is to keep their model-based and
task-based meanings from silently becoming stronger ontological claims.

## The Evidential Ledger

The study's inferential sequence can be displayed without assigning every step
the same status.

| Evidential level | Item | Status |
| --- | --- | --- |
| Controlled | Physical display duration and visual position | Set by the experiment |
| Observed | Button choice and raw BOLD time series | Recorded from each participant |
| Estimated | Psychometric curve, PSE, and duration-position GLM beta weights | Derived from behavioral or BOLD data |
| Fitted | Vertex-wise preferred duration, $\mu_d$, under the Gaussian position-invariant model | Model-dependent parameter |
| Analyzed | Regional distributions, spatial maps, PSE correlations, and covariance clusters | Statistical relations among fitted quantities |
| Interpreted | Encoding, readout, categorization, boundary populations, and a functional cortical hierarchy | Authors' mechanistic account |
| Not independently characterized | Experienced-duration structure | Beyond the reports used to fit the PSE |
| Not tested | IER frontier organization or an experiential-physical identity | Outside the study's design |

This ledger does not downgrade modeling to guesswork. Models are indispensable
in neuroscience. It identifies where conclusions depend on assumptions and
where new evidence would be needed to move from one explanatory level to
another.

## What the Study Supports

Within the task and fitted model, the results support the following claims:

- cortical BOLD responses varied systematically across physical-duration
  conditions;
- fitted preferred durations were distributed differently across cortical
  regions;
- similar preferences were spatially organized within the analyzed regions;
- visual, parietal, frontal, insular, premotor, and motor regions did not all
  exhibit one homogeneous response profile;
- selected anterior-insular and rostral-SMA preferences covaried with
  participant-specific behavioral PSEs;
- and regional preferences co-varied across participants in patterns that can
  be summarized by three statistical clusters.

These findings provide a rich description of how a duration-categorization task
is related to distributed cortical activity. They constrain any account of how
organisms discriminate and categorize subsecond visual durations.

## What the Study Does Not Establish

The experiment does not establish:

- direct neuronal tuning from electrophysiological recordings;
- within-trial ramping or accumulation observed at subsecond resolution;
- one causal sequence from visual encoding through readout to categorization;
- information flow inferred from the correlation-derived clusters;
- that center-sensitive activity tracks PSE rather than reference memory or
  range statistics;
- that a PSE is an experiential event boundary;
- that inferior frontal cortex tracks individual PSEs;
- that the absence of a single time center is evidence against neural clocks;
- that timing, state-dependent, attention, memory, or decision models cannot
  explain the results;
- the internal organization of experienced duration;
- any IER operation, including admissible futures, collapse, welding,
  propagation, segmentation, or an integration envelope;
- or the identity of an experiential occurrence with a physical process.

The difference between these lists is the difference between source fidelity
and theoretical projection.

## Neural Timing Mechanisms and Temporal Phenomenology

The study and IER need not be competitors at one explanatory level.

| Question | Explanatory target |
| --- | --- |
| How long did the display physically persist? | Stimulus duration measured by the experimental apparatus |
| How does the nervous system remain sensitive to that persistence? | Sensory, neural, memory, comparison, and timing mechanisms |
| How does that sensitivity become a shorter-longer response? | Task organization, decision, and motor implementation |
| Why does an interval feel long, short, expanded, compressed, thick, empty, or flowing? | Temporal phenomenology |

IER's canonical duration account rejects an internal clock as the constituent
or sufficient explanation of temporal phenomenology. That claim should not be
expanded into an a priori neuroscientific denial that nervous systems contain
mechanisms capable of timing, interval discrimination, accumulation,
comparison, or temporally organized control.

A biological timing mechanism may be real and behaviorally necessary without
being an inner timeline inspected by a subject. It may contribute causally to
experience or report without being numerically identical to experienced
duration. Conversely, experienced duration may vary while physical duration or
discrimination performance remains stable.

The study principally addresses the middle two rows of the table. It does not
independently characterize the final row. Its results can constrain an IER
account because IER must remain compatible with organisms' demonstrated timing
capacities, but they do not yet choose between a timing mechanism and IER as
alternative explanations of one measured quantity.

## Experimental Psychophysics and Psychophysical Identity

The word *psychophysical* has two uses that must remain separate.

| Use | Question | Status in this study |
| --- | --- | --- |
| Experimental psychophysics | How do controlled stimulus values relate to behavioral responses? | Directly investigated |
| Psychophysical identity | Do an experiential description and a physical description refer to one occurrence? | Not investigated |

Reports are legitimate evidence about experience. The problem is not that the
shorter-longer choices are behavior and therefore irrelevant. The problem is
that the same choices are used to construct the PSE, after which the PSE is
treated as the study's subjective variable. There is no further, independently
specified structure of experienced duration against which the neural result can
be compared.

Even perfect covariation among stimulus duration, BOLD response, fitted
preference, and PSE would not by itself decide whether the experience is:

- identical with one of those physical processes;
- caused, realized, or constituted by it;
- identical with a wider physical organization;
- or not yet adequately characterized by the available experiential measure.

This is why the Centanino study functions in *The Identification Problem* as a
worked identification-gap analysis rather than as a positive psychophysical
identity candidate.

## Simplest Compression

Centanino, Fortunato, and Bueti used 7T fMRI while thirteen participants judged
six subsecond visual durations relative to a learned 0.5-second reference.
Their analysis estimated offset-locked BOLD responses and fitted a Gaussian
duration-response model at each cortical vertex. The resulting preferred
durations differed across regions: visual areas tended toward long-end
preferences, several parietal and premotor areas covered the range, and several
frontal and insular areas tended toward its center.

Selected anterior-insular and one rostral-SMA region showed positive
associations between preferred duration and participant PSEs. These limited,
uncorrected correlations link a model-dependent cortical parameter with a
behavioral category boundary. They do not turn the PSE into an experiential
event boundary or the fitted parameter into experienced duration.

The authors propose a hierarchy of duration encoding, readout, and
categorization. The evidence is compatible with that account but does not
directly establish its causal stages. For IER, the study is evidence about
neural and behavioral duration processing and a demonstration of the remaining
identification gap. It is not a cortical localization of frontier structure.

## Reference

Centanino, V., Fortunato, G., & Bueti, D. (2026). Neuronal populations across
the cortex underlie discrete, categorical, and subjective representations of
visual durations. *PLOS Biology, 24*(3), e3003704.
[https://doi.org/10.1371/journal.pbio.3003704](https://doi.org/10.1371/journal.pbio.3003704)

## Companion Documents

- *Four Durations, Not One*
- *The Identification Problem for Physicalist Theories of Experience*
- *IER-duration*
- *IER-subjective-time*
