GibbsBNdistance computes the estimators of `pyagrum.MCBNDistance` (formulas, ``errorPQ``, limits and stopping criterion,
with the defaults of the section ``gibbsKL``), but from the samples of a Gibbs chain on :math:`P`
:cite:`geman1984stochastic` instead of independent samples: no variable is observed, and each step draws again
``nbrDrawnVar`` variables (by default :math:`1 + \lfloor n/10 \rfloor` for :math:`n` variables, chosen at random), each
one given its Markov blanket, as in `pyagrum.GibbsSampling`. The chain starts from a configuration of positive
probability, and its first ``burnIn`` steps (2000 by default) are discarded.

The samples of the chain are correlated: for the same :math:`N`, they carry less information than independent samples,
and the stopping criterion, which looks at the change brought by the last sample, may stop while the estimate still
drifts slowly. When :math:`P` has null probabilities, the chain may also miss parts of the support of :math:`P`. Since
the samples of a Bayesian network without evidence can be drawn exactly and independently, `pyagrum.MCBNDistance` is
usually preferable.
