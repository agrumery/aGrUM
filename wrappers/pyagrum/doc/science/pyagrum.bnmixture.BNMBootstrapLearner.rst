BNMBootstrapLearner learns a reference Bayesian network from the database, then the Bayesian networks of
bootstrap replicates of this database :cite:`efron1979bootstrap`. The replicates are not drawn by resampling: as in
the Bayesian bootstrap :cite:`rubin1981bayesian`, each one gives the records random weights drawn from a Dirichlet
distribution. The frequency of a feature (an arc) over the learned networks measures the confidence in this feature,
following :cite:`friedman1999data`.
