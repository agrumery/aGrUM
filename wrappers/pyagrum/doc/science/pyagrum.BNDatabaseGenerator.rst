BNDatabaseGenerator draws independent samples of a Bayesian network by probabilistic logic sampling
:cite:`henrion1988propagating`: the variables are drawn in a topological order, each from its CPT given the values
already drawn for its parents (see `pyagrum.MonteCarloSampling`). With evidence, a sample that contradicts it is
rejected, so that the database is a sample of the posterior; ``drawSamples`` then counts the accepted rows, and stops
with an error after ``sampling.maxConsecutiveRejections`` consecutive rejections. The value returned is the
log-likelihood (base 2) of the database in the network. The value written for a discretized variable is, by default,
drawn uniformly in the interval of its label (or its middle, or the label itself).
