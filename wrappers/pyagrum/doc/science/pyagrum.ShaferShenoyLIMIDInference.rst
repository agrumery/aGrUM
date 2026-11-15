ShaferShenoyLIMIDInference solves an influence diagram as a LIMID (limited memory influence diagram)
:cite:`lauritzen2001representing`, where a decision only knows its parents, with one exception: when a directed path
goes through all the decisions (a classical influence diagram), the no-forgetting assumption is made, each decision
receiving the family of the previous one as parents. ``addNoForgettingAssumption`` gives this order explicitly (it is
then applied even when the decisions are not ordered by the graph; the order must list all the decisions).

**Reduction and solvability.** A parent :math:`n` of a decision :math:`d` is non-requisite, and its arc is removed,
when every utility node :math:`u` descending from :math:`d` is d-separated from :math:`n` by
:math:`\mathrm{fa}(d) \setminus \{n\}`. A decision :math:`d` is extremal when every such :math:`u` is d-separated
from the families of the other decisions by :math:`\mathrm{fa}(d)`; the LIMID is soluble if the decisions can be
ordered so that each one is extremal once the following ones are turned into chance nodes
:cite:`nilsson2000evaluating`. With the no-forgetting assumption, the diagram is always soluble. Otherwise, pyAgrum
removes an extremal decision (looked for by levels from the utilities upwards, the extremality being checked against
all the decisions not removed yet) until none remains; a LIMID found non soluble is refused.

**Single policy updating.** In this order, each decision gets an optimal policy, computed under the uniform policy for
the decisions not yet solved, and becomes a chance node with this policy; for a soluble LIMID one pass gives an
optimal strategy :cite:`lauritzen2001representing`. The policy maximizes, for each configuration of the parents, the
expected utility among the values of non-zero probability (ties broken by the higher probability, then by the first
value): the policies are deterministic.

**Propagation.** The messages follow the Shafer-Shenoy scheme (`pyagrum.ShaferShenoyInference`), in a junction tree
of the moral graph of
the reduced LIMID without its utility nodes (default triangulation), with factors that are pairs
:math:`(p, u)` of a probability and a utility :cite:`nilsson2000evaluating`:

.. math::

   (p_1, u_1) \otimes (p_2, u_2) = (p_1 p_2,\; u_1 + u_2), \qquad
   (p, u)^{\downarrow W} = \Big(\sum_{\overline W} p,\; \frac{\sum_{\overline W} p\,u}{\sum_{\overline W} p}\Big).

``MEU`` returns the mean and the variance of the total utility under the optimal strategy, computed by propagating
the first two moments of the utility.
