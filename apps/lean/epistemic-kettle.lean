/-!
A finite Epistemic Kettle example. Source: epistemic-kettle.md, Behavior
Produces Equivalence Classes, Outcomes, Snapshots, and Trajectories, and
Intervention Makes Explanations Diverge.

Predictions are stipulated observations, not simulated thermal dynamics or
empirical claims about a real kettle. The observation channel and test menu
are explicitly bounded. Nothing here establishes experience or UEF status.

Run `lean epistemic-kettle.lean`. Lean 4.19.0; no Mathlib dependencies.
-/

set_option warningAsError true

namespace IER.Kettle

/-- Agreement relative to a declared test set and observation channel.
`observe` supplies candidate predictions, not access to the hidden program. -/
def Indistinguishable {Model Trial Observation : Type}
    (observe : Model → Trial → Observation) (tests : Trial → Prop)
    (a b : Model) : Prop :=
  ∀ t, tests t → observe a t = observe b t

/-- Adding tests can only refine agreement classes; it need not distinguish
all candidates. This does not claim any intervention is physically available. -/
theorem moreTestsRefineAgreement {M T O : Type} (observe : M → T → O)
    {smaller larger : T → Prop} {a b : M}
    (included : ∀ t, smaller t → larger t)
    (agree : Indistinguishable observe larger a b) :
    Indistinguishable observe smaller a b := by
  intro t tested
  exact agree t (included t tested)

/-- Discarding observations cannot create a distinction between predictions
that already agree on the richer channel. -/
theorem projectionPreservesAgreement {M T O V : Type} (observe : M → T → O)
    (project : O → V) {tests : T → Prop} {a b : M}
    (agree : Indistinguishable observe tests a b) :
    Indistinguishable (fun m t => project (observe m t)) tests a b := by
  intro t tested
  exact congrArg project (agree t tested)

inductive Candidate where
  | fixedThreshold
  | rateSensitive
  | lookupTable
  deriving DecidableEq, Repr

inductive Trial where
  | baseline
  | lowerFill
  | warmStart
  deriving DecidableEq, Repr

/-- A deliberately selected observation summary, not all ordinary observations.
Temperatures are Fahrenheit; ticks are arbitrary units in the toy predictions. -/
structure Observation where
  cutoffF : Nat
  finalF : Nat
  cutoffTick : Nat
  beepTick : Nat
  deriving DecidableEq, Repr

/-- An illustrative prediction table. The lookup candidate agrees with the
rate-sensitive candidate on this finite menu; untested conditions remain open. -/
def predict : Candidate → Trial → Observation
  | _, .baseline => ⟨187, 190, 10, 14⟩
  | .fixedThreshold, .lowerFill => ⟨187, 194, 6, 10⟩
  | .rateSensitive, .lowerFill => ⟨184, 190, 5, 10⟩
  | .lookupTable, .lowerFill => ⟨184, 190, 5, 10⟩
  | .fixedThreshold, .warmStart => ⟨187, 190, 3, 7⟩
  | .rateSensitive, .warmStart => ⟨185, 190, 2, 7⟩
  | .lookupTable, .warmStart => ⟨185, 190, 2, 7⟩

def BaselineOnly (t : Trial) : Prop := t = .baseline

def WithLowerFill (t : Trial) : Prop := t = .baseline ∨ t = .lowerFill

theorem baselineAgreement :
    Indistinguishable predict BaselineOnly .fixedThreshold .rateSensitive := by
  intro t baseline
  cases baseline
  rfl

theorem addedTrialDistinguishes :
    ¬ Indistinguishable predict WithLowerFill .fixedThreshold .rateSensitive := by
  intro agree
  have different : predict .fixedThreshold .lowerFill ≠ predict .rateSensitive .lowerFill := by
    decide
  exact different (agree .lowerFill (Or.inr rfl))

/-- Task success alone misses a distinction preserved by the selected summary. -/
theorem sameFinalTemperatureDifferentObservations :
    (predict .fixedThreshold .warmStart).finalF = 190 ∧
    (predict .rateSensitive .warmStart).finalF = 190 ∧
    predict .fixedThreshold .warmStart ≠ predict .rateSensitive .warmStart := by
  decide

theorem lookupRivalRemains :
    Indistinguishable predict (fun _ => True) .rateSensitive .lookupTable := by
  intro t _
  cases t <;> rfl

/-- Agreement throughout this finite menu does not identify a unique candidate
organization. Candidate tags represent stipulated rivals, not recovered code. -/
theorem agreementDoesNotIdentifyCandidate :
    ∃ a b : Candidate, a ≠ b ∧ Indistinguishable predict (fun _ => True) a b := by
  exact ⟨.rateSensitive, .lookupTable, by decide, lookupRivalRemains⟩

#print axioms moreTestsRefineAgreement
#print axioms projectionPreservesAgreement
#print axioms baselineAgreement
#print axioms addedTrialDistinguishes
#print axioms sameFinalTemperatureDifferentObservations
#print axioms lookupRivalRemains
#print axioms agreementDoesNotIdentifyCandidate

end IER.Kettle
