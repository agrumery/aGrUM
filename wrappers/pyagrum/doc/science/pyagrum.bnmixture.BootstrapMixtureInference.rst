BootstrapMixtureInference runs the same inference (`pyagrum.LazyPropagation` by default), with the same evidence, in
the reference network and in every network of a `pyagrum.bnmixture.BootstrapMixture` (networks learned from bootstrap
copies of the data :cite:`friedman1999data`). The posterior it returns is the
one of the reference network only; the networks of the bootstrap copies give, for each value of the variable, the
quantile interval defined in `pyagrum.bnmixture.BootstrapMixture` (empirical quantiles of levels 0.2 and 0.8 by
default, not weighted).
