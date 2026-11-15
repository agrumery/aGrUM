LoopyImportanceSampling runs first a loopy belief propagation :cite:`pearl1988probabilistic,murphy1999loopy` with the
same hard evidence, then the sampling of `pyagrum.ImportanceSampling` :cite:`shachter1990simulation`. The
posteriors :math:`\pi` of the loopy belief propagation initialize the estimator as :math:`n_v` virtual samples
(``virtualLBPSize``, 5000 by default): after :math:`N` real samples, the estimate is
:math:`(n_v\,\pi + N\,\hat p)/(n_v + N)`. The criteria on the precision (``epsilon`` and ``minEpsilonRate``, Wilson
score interval :cite:`wilson1927probable`) are only checked once :math:`N \geq n_v`, so that at least :math:`n_v`
samples are drawn (unless the time or iteration limit stops the sampling first). The estimator and its stopping
criterion are detailed in the notes of `pyagrum.WeightedSampling`.

The loopy belief propagation is exact on a polytree and approximate otherwise; its error enters the estimate with the
weight :math:`n_v/(n_v+N)`. It stops after at most 100 iterations (``lbp.maxIter``), converged or not. It mainly gives a
sensible estimate before the real samples dominate.

The share :math:`n_v/(n_v+N)` of the prior counts the real samples, not their effective size: when the weights are very
unequal, the prior is outweighed faster than the information of the samples grows.
