---
ier:
  tier: T0
  role: EXAMPLE
  layer: dynamics
  domain:
    - examples
    - dynamics
  category: cellular_automaton
  status: non_canonical
  filename: IER-GoL-sequential-argument
---

# A Sequential IER Analysis of a Four-Cell Game of Life Region

NON-CANONICAL | EXAMPLE-SCOPED | WORKING ARGUMENT

This argument follows the dependency order in the [claims summary](../../../traceability/claims-summary.md): describe an operation, specify its admissible continuation, test synchronic closure and ownership, then test constitutive membership and diachronic continuity. The two five-clause relations below are **constitutive membership** and **stage continuity**. The exercise tests what an exact cellular-automaton description can establish; it makes no attribution of experience to a pattern or its physical computer.

## 1. Fix the candidate before testing it

Take the standard B3/S23 Game of Life on an infinite square lattice, with synchronous updates and dead cells outside the displayed pattern. Let $Q$ be four sites in a $2\times2$ square. At tick $t$, each $i\in Q$ has a binary variable $x_i(t)\in\{0,1\}$, and $q_t=(x_1,x_2,x_3,x_4)$ is the regional configuration. The *rule* is a transition function, not an additional process or four independent continuation conditions: a dead site is born with exactly three live neighbors; a live site survives with two or three live neighbors. Each cell's next value depends on its current value and all eight neighbor values, including sites outside $Q$.

For the worked trajectory, start with the four sites of $Q$ live and every other site dead. This is the block still life: each live site has three live neighbors, and every outside site has fewer than three. Thus the entire board, not only $Q$, remains unchanged at the next tick. Hold the infinite-lattice rule and this initial condition fixed throughout the main test. The four coordinates are variables of a mathematical model; treating them as physical variables of the computer would require a separate account of implementation, grain, boundary, and interventions.

The candidate process is the repeated Life update as represented by this trajectory. Calling $Q$ a region does not yet show that it is an owned operation. The infinite board is the complete abstract state; $q_t$ alone is not a closed transition input because neighboring sites outside $Q$ can affect it.

## 2. State the continuation problem

Let $s_t$ be the complete board and $T$ the B3/S23 map. From a fixed complete state, the abstract outgoing successor support is

$$A(s_t)=\{T(s_t)\}=\{s_{t+1}\}. $$

For the still life, $s_{t+1}=s_t$. The regional projection is likewise $\pi_Q A(s_t)=\{1111\}$. For any nontrivial partition $P=\{C_1,\ldots,C_k\}$ of $Q$, the projections of this **same** regional successor set satisfy

$$\pi_Q A(s_t)=\prod_{j=1}^{k}\pi_{C_j} A(s_t).$$

Every factor is a singleton, so the equality holds for every partition. On the declared abstract fixed-state fibre, the **current canonical** synchronic support non-factorization condition therefore **fails**. A visually cohesive block, local interaction, and recurrence do not change that mathematical result. It says nothing by itself about the physical computer's admissible successor support at a justified physical grain.

If we vary the starting board, we obtain a different object: the set of outputs across that intervention family. For example, deleting one live corner of the block produces a three-cell L; on the next tick the missing corner is born and the other three cells survive, so the block returns. This perturbation result shows a basin of attraction in the Life map. It does not add a second successor to $A(s_t)$ for the unperturbed board. The [regional support proposal](IER-GoL-support-admissibility.md) and [predicate construction](IER-GoL-predicate-nondecomposability.md) study such alternative-start or filtered sets and must keep their typing distinct from the fixed-state fibre.

### The live singleton issue

The [active-issues index](../../../_work/active/active-issues.md) points to two unresolved questions that this example must not conceal. First, the [singleton issue note](../../../_work/active/singleton-futures-non-doers.md) records the author's correction that singleton support does not define a non-doer. A proposed doer could have a single successor at one frontier and still have capacity for collapse under other conditions; a proposed non-doer's collapse incapacity does not require a singleton. The present non-factorization condition excludes a literal singleton at any tested qualifying frontier, irrespective of that capacity distinction. Second, the [physical-determination issue](../../../_work/active/determinism-and-successor-support-issue.md) asks whether fixed-source support tests the combinability of outcomes while missing unity in the activity that produces them. The Life calculation illustrates that distinction sharply: local updates can depend on other cells across source variations even though the one fixed-source successor set factorizes.

Neither issue lets us call this singleton mathematically non-factorizable. Nor does singleton cardinality itself establish experience, doer status, or non-doer status. A new criterion based on productive organization or a physically justified comparison family would be a different test requiring its own definition, boundary, and evidence. The current argument applies the accepted test while keeping that possible revision visible. One realized successor, literal singleton support at one frontier, and persistent incapacity for foreclosure are also different claims. Life supplies literal singleton support throughout this abstract deterministic trajectory; it settles no experiential capacity classification.

### Moving glider and an open regional boundary

The still-life block is not the only useful Life case. Place one glider on a sufficiently large torus, with every other cell dead and the complete board fixed. Standard B3/S23 then gives one complete successor at every tick. The glider translates and eventually revisits positions on the finite torus; recurrence is still later succession. On this **fixed empty-board trajectory**, it can continue without another pattern interfering.

Now describe only a moving glider region $G_t$, leaving its surrounding cells unspecified. The next regional pattern depends on the glider's current configuration **and** the states of cells in the update stencil crossing its boundary. At a declared grain, write this dependence schematically as $g_{t+1}=F(g_t,e_t)$, where $e_t$ supplies the relevant exterior inputs. If a permitted family $E(g_t)$ contains exterior configurations that interfere differently, the regional output set

$$B_E(g_t)=\{F(g_t,e):e\in E(g_t)\}$$

can have several members. That is a non-singleton **regional comparison across exterior completions**, even though each fixed complete board still has a single successor under $T$. The region is not an autonomous deterministic state description unless its boundary inputs are fixed or included. Calling its open description “nondeterministic” means that $g_t$ alone does not determine $g_{t+1}$; it does not mean the Life rule has become stochastic or that the actual complete-board fibre has branched.

The boundary and comparison family must be specified before using $B_E$ for any factorization test. A moving glider outline is a descriptive boundary, not an established IER self-boundary or ownership scope. Exterior influence can alter the glider's continuation without itself becoming a constitutive member. This is the distinction developed as a proposed regional experiment in the [support-admissibility note](IER-GoL-support-admissibility.md), which does not equate its alternative-start set with canonical fixed-source support.

## 3. Ask where the continuation is borne

The four cells' next values are jointly calculated from the board under one update rule, but the rule is specified outside any proposed four-cell owned operation. The region has no demonstrated internal, non-externalizable resolution of a non-factorizable successor problem under the current support test: the tested support factorizes. Its cross-cell update dependencies may still motivate the open *productive-unity* question, but they do not settle ownership. A proposed physical implementation could be investigated for ownership, but its hardware dynamics, boundaries, and replacement comparisons are absent here. Consequently no ownership scope is established for $Q$. This result limits the next two relation tests without making their individual clauses interchangeable.

## 4. Test all five constitutive-membership conditions

The proposed participants are the four cell-update processes within $Q$, assessed over successive block ticks. The columns distinguish what is visible in the abstract Life construction from what the full IER relation would require.

| Membership condition | Four-cell observation | Verdict for the full relation |
| --- | --- | --- |
| **1. Contribution** | Cell values enter the local B3/S23 neighborhood calculations. The one-corner deletion returns to the block, so a single-cell perturbation at this state does not by itself show an effect on the next complete state; other declared perturbations can. | **Partial descriptive match.** Rule participation is not yet contribution to an *owned admissible continuation* at a physical typing. |
| **2. Global bearing** | Each site's next value is constrained by the same update rule and the surrounding configuration. | **Unestablished.** A common external rule is not evidence that an internally owned global restriction bears on each participant. |
| **3. Joint resolution** | From the fixed board each part has one next value, and the same-fibre regional support factorizes across every partition. | **Fails under the current support criterion for this abstract candidate.** Cross-cell production remains visible, but whether another test could capture productive unity is an open design question. |
| **4. Diachronic uptake** | The next board becomes the input to the following tick. In the block trajectory this repeats $1111$; in the L-to-block trajectory the deletion is erased after one update. | **Ordinary state dependence only.** The required uptake of *both contribution and global bearing* into later owned organization is unestablished. |
| **5. Common typing** | Region, rule, tick interval, and perturbation comparison can be stated consistently at the abstract Life grain. | **Satisfied as bookkeeping for this model, not as physical qualification.** There is no justified mapping from these cell labels to one physical owned operation and restriction. |

Because the five conditions must hold together for the same operation and interval, the four cell-update processes are **not established** as constitutive members in the IER sense. In particular, the failed joint-resolution test cannot be repaired by calling their causal coupling “bilateral.”

## 5. Test all five diachronic-continuity conditions

Now compare two adjacent non-zero intervals of the still-life trajectory, rather than comparing instantaneous pictures alone. This is the abstract analogue of stages $a$ and $b$; it does not presuppose that either interval is a qualifying UEF stage.

| Continuity condition | Four-cell observation | Verdict for the full relation |
| --- | --- | --- |
| **1. Continuous owned closure** | The block persists without a blank tick, but its fixed-state successor fibre factorizes at each frontier and ownership is unestablished. | **Fails under the current criterion for this abstract candidate.** Uninterrupted recurrence does not establish uninterrupted owned closure. |
| **2. Operational successor production** | Applying $T$ to the earlier complete board produces the later board; the block is not reinserted between ticks. | **Satisfied as abstract causal succession only.** Production of a qualifying later *owned regime* is unestablished. |
| **3. Typed consequence inheritance** | The later state depends on the earlier state. The block trajectory carries no identified irreversible, history-bearing change that remains effective; a three-cell L and the block can both lead to the same block. | **Unestablished.** Generic state dependence and a many-to-one map do not demonstrate effective inheritance of an irreversible consequence. |
| **4. Operation-mediated change** | The unchanged block has no turnover, recruitment, release, or boundary migration to assess. The L-to-block change is a rule update, without an established membership relation. | **Unestablished.** The example supplies neither a qualifying change nor evidence that change is mediated by an owned operation. |
| **5. Unique successor and predecessor** | $T$ gives each complete board exactly one next board. Yet both the block and a three-cell L with dead exterior produce the block, so backward state uniqueness fails. | **Forward state uniqueness only.** IER requires exactly one *qualifying* successor and predecessor under conditions 1–4 in a predeclared comparison domain; deterministic update alone cannot supply that result. |

The L-to-block comparison is a counterexample to inferring two-sided *state* uniqueness from a deterministic forward rule. It is not a claim that two UEF tokens merge: no qualifying token has been established here. Likewise, the block's indefinite recurrence is not numerical subject persistence under the full five-clause stage relation.

## 6. What the sequential test establishes

The four binary values give a clear configuration, B3/S23 gives an exact transition, and the fixed complete state gives an exact successor. Those are useful starting points. At that same declared abstract grain, the successor support is singleton and factorizes, so the **current** intrinsic-closure test fails before ownership, full membership, or full continuity can be inferred. The clause-by-clause analysis also exposes separate gaps: common-rule dependence does not establish internal bearing or joint resolution, and forward deterministic succession does not establish inherited irreversible history or two-sided token uniqueness. The active singleton questions challenge the scope of the current support criterion; this Life example does not resolve them.

A different Life construction can ask a useful counterfactual question over multiple starting states, or a different physical investigation can study the computer that runs it. Each must state its own support, partition family, boundary, interval, interventions, and physical mapping. Neither changes the fixed-state calculation above. Even a physically justified full relation stack would leave IER's experiential identity proposal as an additional abductive commitment, rather than a theorem of Game of Life.

Canonical comparison: [Intrinsic Closure](../../../IER/IER-intrinsic-closure.md), [Ownership](../../../IER/IER-ownership.md), [Processes](../../../IER/IER-processes.md), and [Continuity](../../../IER/IER-continuity.md). Existing example boundary: [GoL orientation](gol-orientation.md).
