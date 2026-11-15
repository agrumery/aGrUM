CausalShapValues computes the causal Shapley values of :cite:`heskes2020causal` (see
`pyagrum.explain.ConditionalShapValues` for the Shapley values and the options common to the explainers): the game is
the expected prediction when the features of the coalition are set by an intervention,

.. math::

   p_S(y) = E\big[P(y \mid X_A) \mid do(X_S = x_S)\big],

where :math:`P(y \mid X_A)` is the posterior of the target given its ancestors :math:`A`. The players are only these
ancestors: an intervention on any other feature does not change :math:`Y`, and these features get a
null value. For a coalition of ancestors, :math:`p_S(y)` is the causal effect :math:`P(y \mid do(x_S))`.

The expectation is computed on a background sample (data frame with a column for each variable; without it,
``sample_size`` (1000) rows are drawn from the network): the arcs into :math:`S` are removed and :math:`S` is set to
:math:`x_S` (mutilated network :math:`P_S`); in each background row the players of :math:`S` are set to :math:`x_S`,
and each distinct configuration :math:`b` of the players so obtained is weighted by :math:`P_S(b_{\bar S})`,
:math:`\bar S` being the other players:

.. math::

   p_S(y) = \frac{\sum_b P_S(b_{\bar S})\,P(y \mid b)}{\sum_b P_S(b_{\bar S})}.

The number of occurrences of a configuration in the background is not used: its probability is already the weight.
For the empty coalition, :math:`p_\emptyset(y) = P(y)` (under the condition below).

**Implementation limit.** The expectation is exact when the background contains every configuration of the players
of :math:`\bar S` with a non-zero probability under :math:`P_S`; otherwise it is restricted to the configurations
present (and renormalized).
