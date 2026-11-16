# Game of Life Resources: Rules, Cellular Automata and Interpretation

Status: reference and exploratory discussion\
Authority: non-canonical\
Web resources reviewed: 2026-11-14

This guide connects Conway's Game of Life (GoL) to the wider cellular-automaton
family and explains how such mathematical worlds can sharpen questions about
metaphysics and cosmology. The philosophical comparisons below are proposed
uses of the models, rather than conclusions established by the linked sources.
The development article `IER/IER-game-of-life.md` owns the repository's
theoretical account; it is outside the current public selection.
[research/](../research/README.md) holds IER investigations.
For figures, see the [static display guide](gol-pattern-display.md).

## Reading the supplied resources

| Resource | Best use | Reading caution |
| --- | --- | --- |
| [Martin Gardner, October 1970](https://www.ibiblio.org/lifepatterns/october1970.html) | Primary historical introduction: board, rules and simultaneous generations. | Read conjectures and challenges as statements from 1970, rather than current open-problem lists. |
| [Life-like cellular automaton](https://en.wikipedia.org/wiki/Life-like_cellular_automaton) | Orientation to rule notation, variants and broader families. | A secondary index for finding terms and references. |
| [Conway's Game of Life](https://en.wikipedia.org/wiki/Conway%27s_Game_of_Life) | Broad overview of history, pattern families and computation. | Follow references before relying on historical priorities or technical results. |
| [Hmyzak's Life-like catalogue](https://hmyzak.cz/software/gameoflife/index.html) | Compact rule comparison and small pattern diagrams. | Rule names do not guarantee the same patterns work across rules. |
| [David Eppstein, “Cellular Automata: Replicators”](https://ics.uci.edu/~eppstein/ca/replicators/) | Research examples and downloadable patterns across several rules. | Its closing statement about absent explicit Life replicators reflects an earlier research state; do not repeat it as a current claim. |
| [Game of Life Lab guide](https://www.gameoflifelab.com/#guide) | A teaching progression from stability through cycles, motion and signal production. | Its reference facts and observations on finite boards are explicitly distinguished. |

The two Wikipedia pages are reading maps. Technical explanations here use the
original rule presentation, the named catalogue, Eppstein's research material
and direct mathematical deductions. [Golly's format documentation](https://golly.sourceforge.io/Help/formats.html)
adds a primary implementation reference for sharing patterns.

## What a cellular automaton specifies

A cellular automaton assigns a state to each site of a lattice and repeatedly
applies a local transition rule. A complete experiment specifies:

* **Space:** dimension, lattice, finite or infinite extent, and topology.
* **States:** binary, several discrete values, or a different state space in
  a generalized model.
* **Neighborhood:** which nearby states a site reads, including whether the
  central state is counted separately.
* **Transition:** how those inputs determine the next state.
* **Schedule:** which sites update and when.
* **Preparation:** initial states and any subsequent interventions.

These choices determine a world; an image alone does not specify one. Standard
Life uses a square lattice, two states, the eight surrounding sites (the
radius-one Moore neighborhood), and simultaneous updates. Gardner's
[original description](https://www.ibiblio.org/lifepatterns/october1970.html)
explains why a newborn must not affect another calculation in the same turn.

For a board $x_t$, write $n_t(p)$ for the number of live neighbors of site $p$,
excluding $p$ itself. A binary birth/survival rule specifies sets $B,S$:

$$ x_{t+1}(p)= \begin{cases} 1 & x_t(p)=0\ \text{and}\ n_t(p)\in B,\\ 1 & x_t(p)=1\ \text{and}\ n_t(p)\in S,\\ 0 & \text{otherwise}. \end{cases} $$

This is **outer-totalistic**: the center's state and the total of its neighbors
matter, while their arrangement does not. In the unrestricted binary,
radius-one Moore family, there are nine possible counts for each central
state: eighteen independent yes/no choices, giving $2^{18}=262{,}144$ B/S
rules. This count follows directly from the definition, before identifying
symmetries or imposing extra restrictions.

## Reading and comparing update rules

`B3/S23` means birth with exactly three neighbors and survival with two or
three. Every other case becomes dead. The older survival/birth notation
`23/3` denotes the same rule; always label the convention. An empty survival
set, as in `B2/S`, means no live site survives an update.

The following selections use [Hmyzak's catalogue](https://hmyzak.cz/software/gameoflife/index.html);
the implications of individual count choices follow from the definition above.

| Rule | Name | What to compare with Life |
| --- | --- | --- |
| `B3/S23` | Conway Life | Baseline for this repository. |
| `B36/S23` | HighLife | Adds birth at six neighbors; supports a small replicator. |
| `B2/S` | Seeds | All currently live cells die; later activity consists of births. |
| `B1357/S1357` | Replicator | The next state is the parity of the eight-neighbor count. |
| `B3/S012345678` | Life without Death | A live cell never turns off. |
| `B3678/S34678` | Day & Night | Exchanging live and dead preserves the dynamics. |
| `B36/S125` | 2x2 | Special behavior for configurations aligned into 2-by-2 blocks. |
| `B35678/S5678` | Diamoeba | Useful for comparing evolving extended regions and boundaries. |
| `B4678/S35678` | Anneal | Useful for comparing smoothing interfaces. |

Two cautions make comparisons more informative:

1. A change in rule can leave a particular trajectory unchanged if that
   trajectory never encounters a changed case. To distinguish Life and
   HighLife, choose a preparation with a dead site having six live neighbors.
2. A rule that supports replication does not make every seed a replicator.
   [Eppstein's examples](https://ics.uci.edu/~eppstein/ca/replicators/)
   pair particular patterns with particular rules, including HighLife.
   Replication here concerns copying a configuration; biological reproduction
   and Darwinian evolution require further specifications.

For Day & Night the symmetry can be checked algebraically: complementing a
board changes a neighbor count $n$ to $8-n$ and exchanges birth with failed
survival. In general the complemented rule has
$B'=\{n:8-n\notin S\}$ and $S'=\{n:8-n\notin B\}$.
These sets equal the original sets for Day & Night. This symmetry is a property
of the rule on the whole board; a fixed-dead boundary would also need to be
complemented to compare equivalent finite experiments.

### Different schedules are different dynamics

Synchronous Life calculates every output from the same previous board. An
in-place row-by-row sweep can read newly changed neighbors and creates a
different automaton, potentially favoring a spatial direction. Asynchronous
updates require a declared schedule or probability law. A random schedule
introduces an ensemble of possible histories; a fixed schedule need not.
Neither is merely another display speed for synchronous Life.

Changing the neighborhood is another independent axis. Four orthogonal
neighbors give a von Neumann neighborhood; larger radii read farther away.
Arrangement-sensitive rules can distinguish two neighborhoods with equal
counts. One-dimensional rules use a line of sites: stacking generations forms
a space-time diagram, whose vertical axis is time rather than a second spatial
dimension. Multi-state and reversible constructions likewise need definitions
beyond the binary B/S notation used here.

### Boundaries and the background matter

An infinite plane, a finite board with dead exterior, and a torus are different
models. Wrapping can return an emitted glider to its source. A bounded edge can
destroy it. Cropping a picture should change neither model nor evolution.

For radius-one synchronous updates, a site's state after $t$ steps depends only
on sites within Chebyshev distance $t$ initially. This follows by composing
one-neighborhood dependencies $t$ times. It gives a finite causal margin for
checking a finite observation window against an infinite-plane calculation.

Rules containing `B0` need special care: an entirely dead infinite board becomes
entirely live after one step. An algorithm storing only finitely many live
sites against a permanently dead background cannot represent that evolution
directly. The background itself must be modeled. Alternative-rule figures
should therefore state their background convention as well as their seed.

## What to observe beyond a lively picture

The [Lab's teaching sequence](https://www.gameoflifelab.com/#guide) suggests
starting with stability, repetition and translation before tackling long
transients and emitters. In this repo the corresponding baseline examples are
block, blinker/toad, glider and Gosper gun; see the
[pattern registry and checks](patterns.md).

Use explicit questions:

* Does the **whole configuration** recur at the same coordinates, or only up
  to translation? A glider has a four-step phase cycle modulo displacement.
* Does population stay constant while the live footprint changes? The
  [glider and toad examples](../research/README.md) separate turnover from
  locomotion.
* Does a repeating core emit new objects? Cropping away the output must not
  turn a growing whole configuration into an allegedly periodic whole.
* Does a trace actually repeat, or has it merely appeared settled during a
  finite observation interval?

On a fixed finite board of $N$ binary sites with a deterministic time-independent
rule, there are only $2^N$ states. Every trajectory eventually repeats and then
cycles, by the pigeonhole principle. This establishes eventual recurrence,
not a practical bound on the time needed to see it. An infinite board has no
such finite-state argument. Neither conclusion makes deterministic evolution
random: uncertainty about a seed or our ability to calculate is a separate
question from whether the update is uniquely specified.

## Mathematical games as tools for metaphysics

The following are methodological proposals. A small formal world makes an
argument's assumptions inspectable and can provide counterexamples to claims
of necessity. Transferring its lessons to physical reality requires a justified
mapping of states, relations, interventions and scale.

**Objects and persistence.** A glider retains a recognizable organization while
different fixed sites become live. This gives a precise example for asking
whether persistence requires unchanged components. It also exposes a further
problem: recognizing the same shape does not settle whether a destroyed and
recreated pattern is the same token. Compare production histories as well as
endpoint images.

**Emergence and explanation.** The local rule determines the next board, while
period, velocity and signal interaction describe longer-scale relations.
Descriptions at these scales can be useful without requiring extra microscopic
forces. Conversely, calling something emergent does not prove irreducibility,
agency or experience. Specify which explanatory task the larger-scale
description performs.

**Causation and individuation.** Remove one cell, or add one outside a current
footprint, and compare the continuation. Such interventions test dependence
over a declared horizon. Relevant dead neighbors matter too: an extra neighbor
can suppress a birth. Dependence alone does not determine membership in an
object; environmental disturbances can affect it without becoming its parts.

**Law, state and realization.** The rule, initial board and machine executing
updates answer different questions. Replacing an image with an identical image
can preserve appearance while changing its production history. The
development article `IER/IER-game-of-life.md` develops these distinctions
without inferring experiential qualification from organized continuation.
It also distinguishes Shannon descriptions from intrinsic informational
tension; a complex-looking board establishes neither tension nor its absence.

## Mathematical worlds as tools for cosmological reasoning

Life offers controlled comparisons between a law and an initial condition.
Hold the seed fixed and change the rule; hold the rule fixed and change the
seed. This helps separate claims about what laws permit from claims about what
a particular universe does. A few interesting examples establish possibility
within the model, rather than typicality across its histories.

Several cosmological questions become clearer when posed this way:

| Question | Controlled comparison | Limit of the analogy |
| --- | --- | --- |
| How can local rules permit distant coordination? | Trace overlapping causal neighborhoods and traveling signals. | Life's propagation bound is a lattice bound; it does not derive relativity. |
| Can local observations reveal global structure? | Compare plane and torus histories before and after wraparound becomes observable. | Agreement over a finite interval does not identify global topology. |
| Does an arrow of time require random laws? | Compare an empty board with a single isolated cell: both have the same empty successor. | Loss of predecessor distinctions does not by itself derive thermodynamic entropy or its arrow. |
| Are complex structures typical or selected? | Declare board size, seed distribution, horizon and selection criterion before counting outcomes. | A gallery supplies no probability measure over physical universes. |
| Does an observer change the world? | Contrast passive recording with editing cells or altering boundaries. | Neither operation models quantum measurement without additional physics. |

A cosmological use must eventually confront observation: identify the physical
counterparts of the lattice, time step, states and rule, and derive predictions
that discriminate alternatives. Interesting organization in Life is not
evidence that our universe is a cellular automaton, simulated, spatially
discrete or conscious. The game supplies exact examples for reasoning; an
empirical cosmology bears a further explanatory and evidential burden.

## Using these resources with the current simulator

The executable currently implements synchronous **B3/S23 only**, with bounded
or toroidal geometry. The rule table above is a study guide, not a list of
runtime options. The [RLE parser](rle-format.md) accepts Conway declarations;
startup and runtime loading currently use [`.cells`](cells-format.md).
Converting another rule's seed to `.cells` does not convert its dynamics.

Keep terminal geometry fixed during comparisons: resizing currently changes
the actual world. Use named fixtures and explicit coordinates for reproducible
preparations; a numeric random seed alone is not a portable board description.
For offline figures, preserve exact board data and consult the
[display guide](gol-pattern-display.md) before choosing a visual format.
