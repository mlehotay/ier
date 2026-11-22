/-!
IER unity relations, exploratory Lean 4 formalization.

Source: `IER-unity-context.md`, Processes / The Full Membership Relation,
Continuity / Physical Stage Continuity, and Reproducing the Life relations /
Access, joint compatibility and changing interfaces.

Run with `lean IER-unity.lean`. No Mathlib dependencies.
-/

set_option warningAsError true

namespace IER

/-- `Context` represents the declared operation, restriction, variables, grain,
scale, regime, intervention family and interval. The physical predicates are
parameters. `commonTyping` is the obligation to justify a common typing for the
evidence supporting the other four clauses. -/
structure MembershipRelations (Context Process : Type) where
  contribution : Context → Process → Prop
  globalBearing : Context → Process → Prop
  jointResolution : Context → Process → Prop
  diachronicUptake : Context → Process → Prop
  commonTyping : Context → Process → Prop

def ConstitutiveMember {Context Process : Type}
    (r : MembershipRelations Context Process) (c : Context) (p : Process) : Prop :=
  r.contribution c p ∧ r.globalBearing c p ∧ r.jointResolution c p ∧
    r.diachronicUptake c p ∧ r.commonTyping c p

theorem noMembershipWithoutJointResolution {Context Process : Type}
    (r : MembershipRelations Context Process) (c : Context) (p : Process)
    (missing : ¬ r.jointResolution c p) : ¬ ConstitutiveMember r c p := by
  intro member
  exact missing member.2.2.1

/-- `Stage` is the pre-fixed physical comparison domain. Each relation is a
physical obligation assessed at the declared typing. -/
structure ContinuityRelations (Stage : Type) where
  continuousClosure : Stage → Stage → Prop
  successorProduction : Stage → Stage → Prop
  consequenceInheritance : Stage → Stage → Prop
  operationMediatedChange : Stage → Stage → Prop

def MeetsFirstFour {Stage : Type} (r : ContinuityRelations Stage)
    (a b : Stage) : Prop :=
  r.continuousClosure a b ∧ r.successorProduction a b ∧
    r.consequenceInheritance a b ∧ r.operationMediatedChange a b

def Adj_dia {Stage : Type} (r : ContinuityRelations Stage) (a b : Stage) : Prop :=
  MeetsFirstFour r a b ∧
    (∀ c, MeetsFirstFour r a c → c = b) ∧
    (∀ c, MeetsFirstFour r c b → c = a)

/-- Two distinct candidates satisfying clauses 1–4 block adjacency to either
candidate. Apply the theorem with the candidates exchanged for the other edge. -/
theorem forkBlocksAdjacency {Stage : Type} (r : ContinuityRelations Stage)
    {a b c : Stage} (other : MeetsFirstFour r a c) (distinct : b ≠ c) :
    ¬ Adj_dia r a b := by
  intro adjacency
  exact distinct (adjacency.2.1 c other).symm

/-- The corresponding obstruction when two predecessors meet clauses 1–4. -/
theorem convergenceBlocksAdjacency {Stage : Type} (r : ContinuityRelations Stage)
    {a b c : Stage} (other : MeetsFirstFour r c b) (distinct : a ≠ c) :
    ¬ Adj_dia r a b := by
  intro adjacency
  exact distinct (adjacency.2.2 c other).symm

namespace Life

def nextCell (alive : Bool) (neighbors : Nat) : Bool :=
  if alive then decide (neighbors = 2 ∨ neighbors = 3)
  else decide (neighbors = 3)

/-- The center starts ON with two ON neighbors. The two edits turn on distinct
additional neighbor sites at that same root. -/
def centerAfter (editA editB : Bool) : Bool :=
  nextCell true (2 + editA.toNat + editB.toNat)

theorem individuallyEnabledButJointlyIncompatible :
    centerAfter false false = true ∧
    centerAfter true false = true ∧
    centerAfter false true = true ∧
    centerAfter true true = false := by
  decide

end Life

#print axioms noMembershipWithoutJointResolution
#print axioms forkBlocksAdjacency
#print axioms convergenceBlocksAdjacency
#print axioms Life.individuallyEnabledButJointlyIncompatible

end IER
