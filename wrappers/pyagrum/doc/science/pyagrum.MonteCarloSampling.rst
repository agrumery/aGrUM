MonteCarloSampling is the probabilistic logic sampling of :cite:`henrion1988propagating`: each sample is drawn forward,
in a topological order, from the CPTs of the Bayesian network, and the samples that contradict the evidence are
rejected. The sampling stops when the largest 95% Wilson score interval :cite:`wilson1927probable` of the estimated
probabilities is small enough (or a limit of time or iterations is reached).
The estimator and its stopping criterion are detailed in the notes of `pyagrum.WeightedSampling`.
