Counterfactual answers the question "given that ``profile`` :math:`e` was observed, what would the distribution of
``on`` :math:`Y` have been, had ``whatif`` :math:`X` been set to :math:`x'`?", written :math:`P(Y_{x'} \mid e)`. It
follows the structural account of counterfactuals :cite:`pearl2009causality` (Section 7.1), popularized in
:cite:`pearl2018why` (chapter 8), the parentless observed variables of the model playing the role of the exogenous
variables :math:`U`. Its three steps (abduction of :math:`U` given :math:`e`, action :math:`do(X = x')`, prediction of
:math:`Y`) are done at once in the twin network of :cite:`balke1994probabilistic` (``counterfactualModel``):

- the real world is the observational Bayesian network;
- the counterfactual world is made of a copy :math:`V^*` of each variable of :math:`X` and of each of their
  descendants: :math:`X^*` has no parent, any other :math:`V^*` has the conditional probability table of :math:`V`,
  its parents being the copies of the parents of :math:`V` when they exist, the parents themselves otherwise;
- the other variables, in particular :math:`U`, are shared by both worlds.

With :math:`e` observed in the real world, the exact inference of the twin network gives

.. math::

   P(Y_{x'} \mid e) = P(Y^* \mid e, X^* = x') = \sum_u P(u \mid e)\, P(Y^* \mid x', u),

where :math:`P(u \mid e)` is the joint posterior of the shared variables: the dependence created by the evidence
between them is kept. :math:`X^*` has no parent and no observed descendant, hence is independent of :math:`e`; the
joint posterior of :math:`(Y^*, X^*)` divided by the prior of :math:`X^*` gives the result for every :math:`x'`.

Without ``values``, the result is a tensor over :math:`Y` and the values of ``whatif``; ``values`` selects the
counterfactual values. A causal model with latent variables is refused (``InvalidArgument``): their distributions are
unknown.

**Implementation limit.** The variables with parents keep their conditional probability tables, the same in both
worlds: the result is the counterfactual of the structural model only when every variable with parents is a
deterministic function of them, all the uncertainty being carried by the parentless variables.
