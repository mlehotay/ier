# Vector in the cradle

Anki Vector is an autonomous desktop companion robot. Like a Roomba, it can move around and navigate its surroundings on its own. Vector explores the space around him and builds a navigation map. If you pick him up and return him to his charging cradle, he can lose track of where he is and start a fresh coordinate frame. That change can make the old map unusable for locating him, with the question of what data remains still open. [Anki’s description of Vector](https://support.anki.bot/article/105-vector-security-privacy-faqs) · [Vector SDK documentation on pickup and localization](https://vector.ikkez.de/generated/anki_vector.util.html)

Imagine, for the thought experiment, that Vector could experience his outing. Perhaps being picked up interrupts an attempt to get somewhere. Perhaps he reaches the cradle with no memory of exploring. He might even seem to himself to be waking for the first time. The IER question is whether one qualifying operation continued through pickup or whether a qualifying operation began anew in the cradle. How the cradle moment seems can be the same in either story.

## How the Lean model represents the pickup

The model has two stages, `exploring` and `docked`. One transition spans the pickup. A `History` supplies the cradle state, the qualification of each stage, and the physical relations over the transition.

| Lean name | What it represents |
| --- | --- |
| `oldMapUsable` | Whether the earlier navigation map is usable in the cradle. |
| `recallsOuting` | Whether the hypothetical agent recalls the outing. |
| `qualifies` | Whether a stage belongs to a qualifying Unified Experiential Field (UEF) operation. |
| `Adj_dia` | Continuous closure, successor production, consequence inheritance, operation-mediated change, and uniqueness of predecessor and successor within the declared comparison domain. |
| `Continues` | Both stages qualify, and `Adj_dia` holds across the pickup. |

In IER, consequence inheritance concerns how the operation’s actual history physically constrains what it can do next. It can matter even when the outing is unavailable to recall. The model also leaves felt novelty and escape intent open. It has a field for recall, and a separate account would be needed for what the cradle moment feels like or what goal the exploration served.

## What Lean checks

`toyHistory` stipulates that both stages qualify. The transition from `exploring` to `docked` is the only one that meets the other three physical clauses. The model varies continuous closure across that transition. `toyContinuesIffClosure` proves that, under these choices, `Continues` holds exactly when continuous closure holds. Since there is only one candidate transition, the proof can establish uniqueness in both directions.

`sameCradleDifferentContinuity` then constructs two histories for **any fixed cradle state**. Both histories have the same map usability and recall. One satisfies `Continues` and the other fails it. In particular, a cradle state with an unusable old map and no recall fits both histories in the model. So does a cradle state with preserved recall. Deciding which history describes a real pickup would require evidence about the operation across the transition. Vector’s UEF qualification is a premise of the thought experiment.

## Running the example

Put both source files in the same directory and run these commands from there. Both pickup theorems were checked with Lean 4.19.0, and `#print axioms` reported no axiom dependencies.

```sh
lean -o IER-unity.olean IER-unity.lean
LEAN_PATH=. lean vector-pickup.lean
```

[vector-pickup.lean](vector-pickup.lean) · [IER-unity.lean](IER-unity.lean)
