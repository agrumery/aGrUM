CausalShallValues computes Shall values (see `pyagrum.explain.MarginalShallValues`) whose game follows the causal
Shapley values of :cite:`heskes2020causal` (see `pyagrum.explain.CausalShapValues`): the variables of the coalition
are set by an intervention, :math:`v(S) \approx E[\log P(X) \mid do(X_S = x_S)]`. In the mutilated network
:math:`P_S` (arcs into :math:`S` removed, :math:`S` set to :math:`x_S`), the background rows are given the values
:math:`x_S` on :math:`S`, and each distinct row :math:`b` so obtained is weighted by :math:`P_S(b_{\bar S})` (its
number of occurrences is not used):

.. math::

   v(S) = \frac{\sum_b P_S(b_{\bar S})\,\log P(b)}{\sum_b P_S(b_{\bar S})},
   \qquad v(F) = \log P(x).

All the variables are players: there is no target, hence no restriction to its ancestors. The values are true to
the model, not to the data: :math:`v(\emptyset) = \sum_b P(b) \log P(b) / \sum_b P(b)`, the opposite of the entropy
when the background covers the support, differs from the mean log-likelihood of the background used by the marginal
and conditional values.

**Implementation limit.** As for `pyagrum.explain.CausalShapValues`, the expectation is exact only when the background
contains every configuration of :math:`\bar S` with a non-zero probability under :math:`P_S`; otherwise it is
restricted to the configurations present (and renormalized).
