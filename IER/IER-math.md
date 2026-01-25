# **IER-math.md**

## **Orders of Information, Graph Structure, and Regime Coherence in IER**

**Version 10.9.0 — Mathematical Support Document**

---

## **Status and Authority**

This document is **supportive and non-normative**.

It:

* introduces no new ontological primitives
* introduces no empirical or diagnostic criteria
* introduces no epistemic access claims
* introduces no ethical rules

All ontological and normative authority resides exclusively in:

* `IER-specification.md`
* `IER-theory.md`
* `IER-dynamics.md`
* `IER-ethics.md`

The purpose of this document is to express the IER identity claim using **formal structural tools**—sets, relations, graphs, and closure—while remaining fully consistent with the core framework.

---

## **Informational Language Disclaimer**

All uses of the terms *information*, *structure*, *constraint*, *graph*, *state*, *transition*, and *choice space* in this document refer to **physical organization only**.

They do not refer to semantic content, representation, inference, probability, observer-relative description, or epistemic access.

Informational language is used strictly as shorthand for **physically instantiated constraint structure**, in accordance with **Specification II.A (Physical Information)**.

---

## **1. Minimal Formal Setup**

Let a physical system be associated with a set of physically possible **global configurations**:

* **State set:** $S$

Physical law constrains which configurations may succeed which others:

* **Transition relation:** $T \subseteq S \times S$

An ordered pair $(a,b) \in T$ means the system can physically transition from configuration $a$ to configuration $b$.

The pair $(S,T)$ induces a directed graph whose nodes are configurations and whose edges are physically admissible transitions.

No probabilities, representations, or observers are assumed.

*Notation conventions and mathematical background are summarized in **Appendix A**.*

---

## **2. Orders of Information**

IER distinguishes **orders of information** by the kind of structure specified. Each order adds structure not present at the previous order.

### **2.1 Configuration**

At the configuration order, only the elements of $S$ are specified.

* Configurations describe what is physically instantiated.
* No change, coordination, or constraint is implied.

Configuration alone is never sufficient for experience.

---

### **2.2 Flow**

At the flow order, the transition relation $T$ is added.

* Flow specifies which configurations may succeed which others.
* The induced directed graph encodes physically admissible evolution.

Flow describes reachability, not deliberation, agency, or control.

---

### **2.3 Constraint**

At the constraint order, admissible transitions are restricted.

A **constraint regime** is a subset:

* $R \subseteq T$

Interpreted as: only transitions in $R$ are admissible under that regime.

Constraint structure describes global restrictions on evolution. It does not, by itself, constitute experience.

---

### **2.4 Coherence**

At the coherence order, the question is whether a constraint regime has **system-level closure**.

A regime is coherent when its admissible transitions cannot be decomposed into independently satisfiable parts over a non-trivial partition of the system components.

Coherence specifies **what must evolve as one**.

IER identifies experience only at this order.

---

## **3. Regimes and Global Intrinsic Constraint**

A constraint regime $R$ induces a restricted transition graph on $S$.

A regime satisfies **global intrinsic constraint** when:

* admissible transitions cannot be specified independently for subsystems, and
* no external selector, arbitrator, or supervisor is required to maintain admissibility.

If admissible transitions for parts of the system can be defined and satisfied independently, the regime is not globally intrinsic.

Global intrinsic constraint is a structural property of the transition graph itself.

---

## **4. Unified Experiential Field (UEF)**

A **Unified Experiential Field** corresponds to a constraint regime that satisfies global intrinsic constraint.

A UEF is:

* not a particular configuration
* not a particular transition
* not a description or abstraction
* not a subsystem

It is the **coherent regime itself**, understood as system-level ownership of admissible futures.

At most one globally coherent regime can be satisfied by a system at a time.

---

## **5. Regime and Description**

The same physical system may admit multiple descriptive representations:

* coarse-grainings of configurations
* abstractions or quotient graphs
* alternative labeling schemes

Such descriptive changes alter how states are represented, but they do not alter the underlying transition structure.

Changes in descriptive order do not create or destroy constraint regimes and therefore do not create or destroy Unified Experiential Fields.

---

## **6. Choice Spaces**

Given a regime $R$ and a configuration $s \in S$, the **choice space** at $s$ is defined as:

$$
\text{Choice}(s,R) = \{\, s' \in S \mid (s,s') \in R \,\}
$$

A choice space is the set of physically admissible successor configurations under the regime.

Choice spaces represent physical admissibility only. They do not represent deliberation, decision-making, agency, or freedom.

The size or structure of a choice space is independent of whether a coherent regime exists.

---

## **7. Graph Operations**

The following operations are defined on the directed graph induced by $(S,T)$ and its regime-restricted subgraphs.

### **7.1 Restriction**

Removing edges from $T$ to form a regime $R \subseteq T$.

Restriction represents additional constraint. Restriction alone does not imply coherence.

---

### **7.2 Subgraph Selection**

Selecting a subset of configurations and the transitions among them.

Subgraphs may represent operating modes or restricted domains. A subgraph corresponds to a UEF only if it satisfies global intrinsic constraint.

---

### **7.3 Collapse / Quotienting**

Collapsing multiple configurations into a single node.

This operation is descriptive. It simplifies representation but does not alter regime existence or coherence.

---

### **7.4 Partition and Decomposition**

Attempting to partition the system into components whose admissible transitions can be specified independently.

If such a decomposition succeeds, global intrinsic constraint fails.

This operation provides a structural exclusion of modular, supervisory, and arbitration-based architectures.

---

### **7.5 Coupling and Composition**

Adding cross-dependencies between components so that admissible transitions in one part depend on the global state.

Coupling is the structural operation that yields global intrinsic constraint.

---

## **8. Coherence Windows**

A **coherence window** is a subset $W \subseteq S$ such that:

* within $W$, the same coherent regime remains satisfied
* outside $W$, coherence fails or a different regime applies

Coherence windows describe **regime stability conditions**.

They do not describe degrees, amounts, or probabilities of experience.

---

## **9. Participation and Modulation**

Subsystems may participate in a coherent regime to varying extents.

Participation modulation refers to variation in how strongly local transitions are constrained by the global regime.

Modulation does not imply partial experience, multiple fields, or local ownership of coherence.

---

## **10. Structural Exclusions**

The following architectures fail to instantiate a Unified Experiential Field:

* modular systems with independently satisfiable transition structures
* arbitration or scheduler-based systems that alternate among regimes
* supervisory systems whose constraint can be factored out as external

These exclusions follow directly from the definitions of coherence and global intrinsic constraint.

---

## **11. Continuity and Cessation**

As long as a coherent regime remains satisfied across successive configurations, experiential continuity holds.

If coherence fails, the Unified Experiential Field ceases. Re-establishment of coherence constitutes a new instantiation.

No assumptions are made about memory, report, or narrative identity.

---

## **12. Non-Inference: Temporal Continuity Is Not a Timescale Threshold**

The requirement that a Unified Experiential Field persist across **non-zero duration** does **not** introduce a measurable temporal threshold, minimum interval, or calibratable timescale.

Temporal continuity in IER is a **regime-formal condition**: it specifies that the same globally coherent constraint regime must remain satisfied across successive configurations.

No duration—short or long—is sufficient or insufficient *by itself*. Duration does not function as a proxy, indicator, or criterion for experience.

---

## **13. Ethical Non-Inference**

Nothing in this document defines moral standing, assigns degrees of value, or licenses ethical inference.

Ethical conclusions are derived exclusively in `IER-ethics.md`.

---

## **14. Summary**

This document formalizes the IER identity claim using:

* state sets and transition relations
* orders of information
* constraint regimes and coherence
* graph operations and closure

Experience, when present, corresponds to a single coherent regime characterized by global intrinsic constraint.

Mathematics here clarifies structure. It does not explain experience, measure it, or provide epistemic access to it.

---

## **Appendix A — Mathematical Background**

This appendix lists the **minimal mathematical background** required to read
`IER-math.md` and `IER-saturation.md`.

No additional mathematical machinery is used anywhere in IER.

---

### **A.1 Sets and Relations**

IER models physical systems using sets and relations.

* **Set (mathematics)**
  [https://en.wikipedia.org/wiki/Set_(mathematics)](https://en.wikipedia.org/wiki/Set_%28mathematics%29)

* **Binary relation**
  [https://en.wikipedia.org/wiki/Binary_relation](https://en.wikipedia.org/wiki/Binary_relation)
  *(used to represent physically admissible transitions and regime-restricted admissibility)*

* **Partition of a set**
  [https://en.wikipedia.org/wiki/Partition_of_a_set](https://en.wikipedia.org/wiki/Partition_of_a_set)
  *(used to test whether admissibility can be decomposed across system components)*

---

### **A.2 Logic**

IER uses only elementary first-order logical structure.

* **Universal quantification ((\forall))**
  [https://en.wikipedia.org/wiki/Universal_quantification](https://en.wikipedia.org/wiki/Universal_quantification)
  *(used to express simultaneous admissibility across all blocks of a partition)*

---

### **A.3 Graph Structure**

Relations are interpreted structurally as directed graphs.

* **Directed graph**
  [https://en.wikipedia.org/wiki/Directed_graph](https://en.wikipedia.org/wiki/Directed_graph)
  *(states as nodes; admissible transitions as directed edges)*

* **Reachability (graph theory)**
  [https://en.wikipedia.org/wiki/Reachability](https://en.wikipedia.org/wiki/Reachability)
  *(used to characterize admissible futures and regime-restricted evolution)*

* **Induced subgraph**
  [https://en.wikipedia.org/wiki/Induced_subgraph](https://en.wikipedia.org/wiki/Induced_subgraph)
  *(used to represent regime restriction and coherence windows without adding structure)*

---

### **A.4 Factorability**

The central structural question in IER concerns whether admissibility relations can be decomposed.

* **Factorization**
  [https://en.wikipedia.org/wiki/Factorization](https://en.wikipedia.org/wiki/Factorization)
  *(used in the sense of **relation factorability across a partition**, not algebraic or probabilistic factorization)*

---

### **Appendix Orientation Signal**

> *IER uses only the minimal mathematics required to express structural admissibility and its failure.
> No probabilistic, computational, representational, or metric concepts are assumed or implied.*

---
