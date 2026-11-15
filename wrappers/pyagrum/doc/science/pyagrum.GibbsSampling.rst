GibbsSampling is a Markov chain Monte Carlo method :cite:`geman1984stochastic`, applied to Bayesian networks by
:cite:`pearl1987evidential`: the evidence being fixed, the chain moves from one configuration to the next by drawing
again the value of some unobserved variables, each one given the current values of the others. In a Bayesian network,
this conditional only depends on the Markov blanket of :math:`X` (parents, children and parents of the children):

.. math::

   P(x \mid \text{rest}) \propto P(x \mid \mathrm{pa}(X)) \prod_{C \in \mathrm{ch}(X)} P(c \mid \mathrm{pa}(C)).

When this product is null for every value, :math:`X` keeps its value.

**One step, one sample.** Each step draws ``nbrDrawnVar`` variables, by default :math:`1 + \lfloor n/2 \rfloor` for a
network of :math:`n` variables (``gibbs.percentDrawnSample`` = 50%, at most the number of unobserved variables that are
not deterministic, see below), chosen at random with replacement (``gibbs.drawnAtRandom``) or in turn. The configuration
reached after each step is one sample, of weight 1, for the estimator of `pyagrum.WeightedSampling`. The first
``burnIn`` steps (300 by default) are discarded, so that the chain forgets its starting configuration. That starting
configuration must have a positive probability: the values made impossible by the evidence are removed, then forward
samples are tried and repaired (see the docstring).

**Correlated samples.** Two consecutive samples share most of their values: :math:`N` samples of the chain carry the
information of fewer independent samples. The estimator counts them as :math:`N` independent samples, so that its
Wilson score intervals are too narrow and the stopping criterion is optimistic.

**Deterministic CPTs.** A variable whose CPT gives the probability 1 to a value for each configuration of its parents
is a function of them: moved alone, it would never change. Such a variable (if not observed) is not drawn: its value
is computed from its parents, and the move of a variable draws it jointly with the deterministic variables that depend
on it.

**Defaults.** GibbsSampling has its own stopping thresholds (section ``gibbs``), equal to the ones of the other
samplers: ``epsilon`` = 0.01 and ``minEpsilonRate`` = :math:`10^{-5}`. Since the samples are correlated, the precision
reached is lower than the one of independent samples.

**Implementation limit.** A CPT that is only partly deterministic (some null probabilities, without a probability 1
for each configuration of the parents) may prevent the chain from reaching some configurations of positive
probability from its starting point; the estimate is then biased whatever the number of samples. An exact inference
(`pyagrum.LazyPropagation`) or `pyagrum.WeightedSampling` do not have this limit.
