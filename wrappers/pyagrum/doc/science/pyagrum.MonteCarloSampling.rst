MonteCarloSampling is the probabilistic logic sampling of :cite:`henrion1988propagating`: each sample is drawn forward,
in a topological order, from the CPTs of the Bayesian network, and a sample that contradicts the hard evidence is
rejected as soon as an observed variable gets another value. The accepted samples are samples of the posterior, all of
weight 1: the estimator is a plain frequency and its size is the number of accepted samples. The estimator and its
stopping criterion (Wilson score interval :cite:`wilson1927probable`) are detailed in the notes of
`pyagrum.WeightedSampling`.

The price of rejection is the probability :math:`P(e)` of the evidence: about :math:`1/P(e)` draws are needed for one
accepted sample, which makes the method impractical for unlikely evidence (`pyagrum.WeightedSampling` and
`pyagrum.ImportanceSampling` never reject a draw because of the evidence, unless it is impossible). After
``sampling.maxConsecutiveRejections`` consecutive rejections (:math:`10^6` by default), or when the time limit is
reached before any accepted sample, the evidence is declared of null or too small probability.
