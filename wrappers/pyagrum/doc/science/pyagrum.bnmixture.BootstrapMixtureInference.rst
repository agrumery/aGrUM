BootstrapMixtureInference runs the same inference (`pyagrum.LazyPropagation` by default), with the same evidence, in
the reference network and in every network of a `pyagrum.bnmixture.BootstrapMixture` (networks learned from bootstrap
copies of the data :cite:`friedman1999data`). It returns the posterior of the reference network only; the other
networks give, for each value of the variable, the weighted quantile interval defined in
`pyagrum.bnmixture.BootstrapMixture` (levels 0.2 and 0.8 by default).
