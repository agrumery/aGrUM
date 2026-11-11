BootstrapMixture gathers a reference Bayesian network and the networks learned from Bayesian bootstrap replicates of
its database :cite:`efron1979bootstrap,rubin1981bayesian`. The frequency of an arc over these networks is the
confidence in this arc, and the spread of their posteriors gives quantile intervals around the posteriors of the
reference, in the spirit of the bootstrap analysis of Bayesian networks of :cite:`friedman1999data`.
