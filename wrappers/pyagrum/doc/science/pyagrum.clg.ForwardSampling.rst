ForwardSampling draws independent samples of the joint distribution of a `pyagrum.clg.CLG` by ancestral sampling,
the continuous counterpart of the probabilistic logic sampling of :cite:`henrion1988propagating`: the variables are
drawn in a topological order, each one from its conditional density
:math:`\mathcal{N}(\mu_X + \sum_Y \beta_{YX}\, y, \sigma_X^2)` given the values just drawn for its parents. The
samples are exact draws of the model, without weights, and follow the random generator of aGrUM
(`pyagrum.initRandom`). There is no evidence: the samples are those of the prior joint distribution (the exact
posteriors are given by `pyagrum.clg.CLGVariableElimination`).

**Conventions of the statistics.** ``variance_sample``, ``stddev_sample`` and ``covariance_sample`` divide by
:math:`N-1` (unbiased estimates), so that the variances are the diagonal of the covariance matrix.
