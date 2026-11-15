LoopyGibbsSampling runs first a loopy belief propagation :cite:`pearl1988probabilistic,murphy1999loopy` with the same
hard evidence, then the sampling of `pyagrum.GibbsSampling` :cite:`geman1984stochastic,pearl1987evidential`. The
posteriors :math:`\pi` of the loopy belief propagation initialize the estimator as :math:`n_v` virtual samples
(``virtualLBPSize``, 5000 by default): after :math:`N` real samples, the estimate is
:math:`(n_v\,\pi + N\,\hat p)/(n_v + N)`. The criteria on the precision (``epsilon`` and ``minEpsilonRate``, Wilson
score interval :cite:`wilson1927probable`) are only checked once :math:`N \geq n_v`, so that at least :math:`n_v`
samples are drawn (unless the time or iteration limit stops the sampling first). The estimator and its stopping
criterion are detailed in the notes of `pyagrum.WeightedSampling`.

The loopy belief propagation is exact on a polytree and approximate otherwise; its error enters the estimate with the
weight :math:`n_v/(n_v+N)`. It stops after at most 100 iterations (``lbp.maxIter``), converged or not. It mainly gives a
sensible estimate before the real samples dominate.

With a large ``epsilon`` (0.2 for instance), the criterion is met as soon as it is checked
(:math:`n = n_v + N \geq 2 n_v`): the sampling draws about :math:`n_v` samples, and the result is then an equal mix of
the loopy belief propagation and of the Gibbs chain. With the default ``epsilon`` (0.01), the real samples dominate.
