/-!
Exploratory formalization of IER-slack, Structural Characterization,
Multiplicity Is Not Slack, and Slack Guarantees Identity Failure;
IER-saturation, Definition; IER-boundary-predicates, Pre-UEF Admissibility.

Run with `lean IER-slack.lean`. Lean 4.19.0; no Mathlib dependencies.
Physical pathway predicates and qualification commitments remain parameters.
-/

set_option warningAsError true

namespace IER.Slack

/-- One declared candidate, restriction, grain, interval and intervention context.
`Pathway` describes physical resolution pathways, not an extra successor space.
`defersIndefinitely` requires indefinite, rather than merely finite, deferral.
`externalResolution` means completion of the relevant burden, not external supply.
Physical adequacy of these predicates is an application obligation. -/
structure Relations (Context State Pathway : Type) where
  successor : Context → State → State → Prop
  available : Context → State → Pathway → Prop
  preservesIndependence : Context → State → Pathway → Prop
  localizes : Context → State → Pathway → Prop
  defersIndefinitely : Context → State → Pathway → Prop
  externalResolution : Context → State → Pathway → Prop

def IndependentPath {C S P : Type} (r : Relations C S P)
    (c : C) (s : S) (p : P) : Prop :=
  r.available c s p ∧ r.preservesIndependence c s p

def ResolutionPath {C S P : Type} (r : Relations C S P)
    (c : C) (s : S) (p : P) : Prop :=
  IndependentPath r c s p ∧
    (r.localizes c s p ∨ r.defersIndefinitely c s p ∨ r.externalResolution c s p)

def HasSlack {C S P : Type} (r : Relations C S P) (c : C) (s : S) : Prop :=
  ∃ p, ResolutionPath r c s p

/-- Exhaustion of the relevant independent resolution pathways at this typing.
This is a pre-UEF inquiry, not an intra-UEF quantity or qualification test. -/
def Saturated {C S P : Type} (r : Relations C S P) (c : C) (s : S) : Prop :=
  ∀ p, ¬ ResolutionPath r c s p

def Multiplicity {C S P : Type} (r : Relations C S P) (c : C) (s : S) : Prop :=
  ∃ a b, r.successor c s a ∧ r.successor c s b ∧ a ≠ b

theorem localizationGivesSlack {C S P : Type} (r : Relations C S P)
    {c : C} {s : S} {p : P} (path : IndependentPath r c s p)
    (localized : r.localizes c s p) : HasSlack r c s :=
  ⟨p, path, Or.inl localized⟩

theorem indefiniteDeferralGivesSlack {C S P : Type} (r : Relations C S P)
    {c : C} {s : S} {p : P} (path : IndependentPath r c s p)
    (defer : r.defersIndefinitely c s p) : HasSlack r c s :=
  ⟨p, path, Or.inr (Or.inl defer)⟩

theorem externalResolutionGivesSlack {C S P : Type} (r : Relations C S P)
    {c : C} {s : S} {p : P} (path : IndependentPath r c s p)
    (external : r.externalResolution c s p) : HasSlack r c s :=
  ⟨p, path, Or.inr (Or.inr external)⟩

theorem saturationIffNoSlack {C S P : Type} (r : Relations C S P) (c : C) (s : S) :
    Saturated r c s ↔ ¬ HasSlack r c s := by
  constructor
  · intro exhausted slack
    obtain ⟨p, path⟩ := slack
    exact exhausted p path
  · intro missing p path
    exact missing ⟨p, path⟩

theorem noIndependenceBlocksSlack {C S P : Type} (r : Relations C S P)
    {c : C} {s : S} (missing : ∀ p, ¬ r.preservesIndependence c s p) :
    ¬ HasSlack r c s := by
  intro slack
  obtain ⟨p, path⟩ := slack
  exact missing p path.1.2

/-- Explicit IER commitments, not independently proved physical diagnostics.
The three incompatibilities are the structural argument's premises. All uses
refer to the same `c` and `s`; no inference about other candidate scopes follows. -/
structure Commitments {C S P : Type} (r : Relations C S P) where
  uef : C → S → Prop
  experience : C → S → Prop
  globallyUnavoidable : C → S → Prop
  internallyUnavoidable : C → S → Prop
  nonExternalizable : C → S → Prop
  uefRequires : ∀ c s, uef c s →
    globallyUnavoidable c s ∧ internallyUnavoidable c s ∧ nonExternalizable c s
  localizationExcludes : ∀ c s p, IndependentPath r c s p → r.localizes c s p →
    ¬ globallyUnavoidable c s
  deferralExcludes : ∀ c s p, IndependentPath r c s p → r.defersIndefinitely c s p →
    ¬ internallyUnavoidable c s
  externalResolutionExcludes : ∀ c s p,
    IndependentPath r c s p → r.externalResolution c s p → ¬ nonExternalizable c s
  experienceRequiresUEF : ∀ c s, experience c s → uef c s

theorem slackExcludesUEF {C S P : Type} {r : Relations C S P}
    (ier : Commitments r) {c : C} {s : S} (slack : HasSlack r c s) :
    ¬ ier.uef c s := by
  intro qualifying
  obtain ⟨p, independent, modes⟩ := slack
  have necessary := ier.uefRequires c s qualifying
  rcases modes with localized | defer | external
  · exact ier.localizationExcludes c s p independent localized necessary.1
  · exact ier.deferralExcludes c s p independent defer necessary.2.1
  · exact ier.externalResolutionExcludes c s p independent external necessary.2.2

theorem slackExcludesExperience {C S P : Type} {r : Relations C S P}
    (ier : Commitments r) {c : C} {s : S} (slack : HasSlack r c s) :
    ¬ ier.experience c s := by
  intro experiential
  exact slackExcludesUEF ier slack (ier.experienceRequiresUEF c s experiential)

namespace Examples

/-- Two possible successors with an available local route. Its independence
is varied explicitly. This is a stipulated logical model, not Rab or a UEF. -/
def twoSuccessors (independent : Prop) : Relations Unit Bool Unit where
  successor := fun _ _ _ => True
  available := fun _ _ _ => True
  preservesIndependence := fun _ _ _ => independent
  localizes := fun _ _ _ => True
  defersIndefinitely := fun _ _ _ => False
  externalResolution := fun _ _ _ => False

theorem sameMultiplicity (independent : Prop) :
    Multiplicity (twoSuccessors independent) () false := by
  exact ⟨false, true, True.intro, True.intro, by decide⟩

theorem multiplicityWithSlack :
    Multiplicity (twoSuccessors True) () false ∧ HasSlack (twoSuccessors True) () false := by
  exact ⟨sameMultiplicity True, ⟨(), ⟨True.intro, True.intro⟩, Or.inl True.intro⟩⟩

theorem multiplicityWithoutSlack :
    Multiplicity (twoSuccessors False) () false ∧ ¬ HasSlack (twoSuccessors False) () false := by
  exact ⟨sameMultiplicity False, noIndependenceBlocksSlack _ (by intro p h; exact h)⟩

theorem multiplicityDoesNotEntailSlack :
    ¬ (∀ (r : Relations Unit Bool Unit), Multiplicity r () false → HasSlack r () false) := by
  intro implication
  exact multiplicityWithoutSlack.2 (implication _ multiplicityWithoutSlack.1)

/-- A satisfying commitment model with no UEF at this candidate. Necessary
features are absent, so exhaustion cannot be mistaken for qualification. -/
def unqualified (independent : Prop) : Commitments (twoSuccessors independent) where
  uef := fun _ _ => False
  experience := fun _ _ => False
  globallyUnavoidable := fun _ _ => False
  internallyUnavoidable := fun _ _ => False
  nonExternalizable := fun _ _ => False
  uefRequires := by intro c s h; exact False.elim h
  localizationExcludes := by intro c s p path localized missing; exact missing
  deferralExcludes := by intro c s p path defer missing; exact missing
  externalResolutionExcludes := by intro c s p path external missing; exact missing
  experienceRequiresUEF := by intro c s h; exact h

theorem saturationWithoutUEF :
    Saturated (twoSuccessors False) () false ∧ ¬ (unqualified False).uef () false := by
  exact ⟨(saturationIffNoSlack _ _ _).mpr multiplicityWithoutSlack.2, by intro h; exact h⟩

theorem slackExclusionExample : ¬ (unqualified True).experience () false :=
  slackExcludesExperience (unqualified True) multiplicityWithSlack.2

end Examples

#print axioms saturationIffNoSlack
#print axioms slackExcludesUEF
#print axioms slackExcludesExperience
#print axioms Examples.multiplicityDoesNotEntailSlack
#print axioms Examples.saturationWithoutUEF

end IER.Slack
