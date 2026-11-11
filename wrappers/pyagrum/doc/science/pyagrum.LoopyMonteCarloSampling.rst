LoopyMonteCarloSampling runs first a loopy belief propagation :cite:`pearl1988probabilistic,murphy1999loopy`, then the
sampling of ``MonteCarloSampling`` :cite:`henrion1988propagating`. The posteriors of the loopy belief propagation
initialize the estimator, where they count as a number of virtual samples; the stopping criteria on the precision
(Wilson score interval :cite:`wilson1927probable`) are only checked once the real samples outnumber the virtual ones.
