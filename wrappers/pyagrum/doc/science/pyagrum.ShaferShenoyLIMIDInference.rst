ShaferShenoyLIMIDInference solves an influence diagram as a LIMID (limited memory influence diagram)
:cite:`lauritzen2001representing`: the no-forgetting assumption is not required (it can be added), the non-requisite
parents of the decisions are removed, the solvability of the LIMID is checked and gives an order of the decisions, and
the optimal policies are computed one after the other in this order by single policy updates (one pass is enough for a
soluble LIMID). As in :cite:`nilsson2000evaluating`, the propagation is a Shafer-Shenoy message passing
:cite:`shafer1990probability` in a junction tree of the reduced LIMID, whose factors are pairs of a probability and a
utility.
