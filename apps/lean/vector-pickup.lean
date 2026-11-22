import «IER-unity»

/-!
A two-stage thought experiment using `IER.Adj_dia`. The modeled transition
spans the pickup. The evidence fields describe hypothetical histories, with
no empirical claims about Vector or its experiential qualification.

From the directory containing both sources, run `lean -o IER-unity.olean
IER-unity.lean`, then `LEAN_PATH=. lean vector-pickup.lean`.
-/

set_option warningAsError true

namespace IER.Pickup

inductive Stage where
  | exploring
  | docked

/-- The map can become unusable without its bytes being erased. Recall is a
separate, hypothetical state of the agent in the cradle. -/
structure CradleState where
  oldMapUsable : Bool
  recallsOuting : Bool

structure History where
  cradle : CradleState
  qualifies : Stage → Prop
  physical : ContinuityRelations Stage

/-- Persistence across this single transition between two qualifying stages. -/
def Continues (h : History) : Prop :=
  h.qualifies .exploring ∧ h.qualifies .docked ∧
    Adj_dia h.physical .exploring .docked

def PickupTransition (a b : Stage) : Prop :=
  a = .exploring ∧ b = .docked

/-- Both endpoints qualify by stipulation. The other three physical clauses
hold on the modeled transition, which leaves closure as the varying clause. -/
def toyHistory (cradle : CradleState) (closure : Prop) : History where
  cradle := cradle
  qualifies := fun _ => True
  physical := {
    continuousClosure := fun a b => closure ∧ PickupTransition a b
    successorProduction := PickupTransition
    consequenceInheritance := PickupTransition
    operationMediatedChange := PickupTransition
  }

theorem toyContinuesIffClosure (cradle : CradleState) (closure : Prop) :
    Continues (toyHistory cradle closure) ↔ closure := by
  constructor
  · intro continued
    have adjacent : Adj_dia (toyHistory cradle closure).physical .exploring .docked :=
      continued.2.2
    exact adjacent.1.1.1
  · intro closed
    have transition : PickupTransition .exploring .docked := ⟨rfl, rfl⟩
    refine ⟨trivial, trivial, ?_⟩
    refine ⟨⟨⟨closed, transition⟩, transition, transition, transition⟩, ?_, ?_⟩
    · intro _ candidate
      have produced : PickupTransition .exploring _ := candidate.2.1
      exact produced.2
    · intro _ candidate
      have produced : PickupTransition _ .docked := candidate.2.1
      exact produced.1

/-- Any fixed cradle state is compatible with either verdict in this model,
including a state with absent recall or an unusable map. -/
theorem sameCradleDifferentContinuity (cradle : CradleState) :
    ∃ continuing interrupted : History,
      continuing.cradle = cradle ∧ interrupted.cradle = cradle ∧
      Continues continuing ∧ ¬ Continues interrupted := by
  refine ⟨toyHistory cradle True, toyHistory cradle False, rfl, rfl, ?_, ?_⟩
  · exact (toyContinuesIffClosure cradle True).2 trivial
  · exact (toyContinuesIffClosure cradle False).1

#print axioms toyContinuesIffClosure
#print axioms sameCradleDifferentContinuity

end IER.Pickup
