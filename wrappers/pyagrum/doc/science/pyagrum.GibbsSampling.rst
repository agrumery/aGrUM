GibbsSampling is a Markov chain Monte Carlo method :cite:`geman1984stochastic`: the evidence being fixed, each step
draws again the value of some variables given the current values of their Markov blanket, i.e. from the product of
their CPT and of the CPTs of their children :cite:`pearl1987evidential`. The first samples (burn-in) are discarded.
The sampling stops when the largest 95% Wilson score interval :cite:`wilson1927probable` of the estimated
probabilities is small enough (or a limit of time or iterations is reached).
The estimator and its stopping criterion are detailed in the notes of `pyagrum.WeightedSampling`.
