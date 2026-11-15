An influence diagram :cite:`howard2005influence` extends a Bayesian network with decision nodes and utility nodes to
represent a decision problem. A chance node has a CPT given its parents; a decision node has no table, its parents
being the information available when the decision is made; a utility node has no child, and its table gives a utility
for each configuration of its parents (in pyAgrum, its variable has a single value). The utilities are additive: a
strategy (a policy for each decision) is evaluated by the expected value of the sum of the utility nodes, and solving
the diagram means finding a strategy that maximizes this expected utility.

In the classical definition, the decisions are totally ordered by a directed path (``decisionOrder``,
``decisionOrderExists``) and a decision does not forget what was known at the previous ones (no-forgetting). The same
structure also represents a LIMID (limited memory influence diagram) :cite:`lauritzen2001representing`, without the
no-forgetting assumption: a decision only knows its parents. Which assumption is made belongs to the inference engine,
see `pyagrum.ShaferShenoyLIMIDInference`.
