BNMBootstrapLearner measures the confidence in a Bayesian network learned from a database by the bootstrap
:cite:`efron1979bootstrap`, applied to structure learning as in :cite:`friedman1999data`: the network learned from
the database (the reference) is compared to the networks learned from :math:`B` perturbed copies of this database
(``N``, 100 by default). Every network is learned by the same `pyagrum.BNLearner`, whose settings (algorithm, score,
prior...) are those given to BNMBootstrapLearner, by default those of `pyagrum.BNLearner`. The result is a
`pyagrum.bnmixture.BootstrapMixture`.

**The weights of the records.** No record is drawn: as in the Bayesian bootstrap :cite:`rubin1981bayesian`, each copy
keeps the :math:`n` records and gives them random weights, used as record weights by the learner:

.. math::

   (p_1, \dots, p_n) \sim \mathrm{Dirichlet}(1, \dots, 1), \qquad w_j = n\, p_j .

The :math:`p_j` are distributed as the gaps between :math:`n-1` sorted uniform draws on :math:`[0, 1]`. The weights
sum to :math:`n`, with :math:`\mathbb{E}[w_j] = 1` and :math:`\mathrm{Var}(w_j) = (n-1)/(n+1)`, close to the moments
of the counts of the resampling with replacement (mean 1, variance :math:`(n-1)/n`) :cite:`rubin1981bayesian`, but
with continuous weights, so that no record is left out. The effective sample size :math:`1/\sum_j p_j^2` of a copy
(see `pyagrum.WeightedSampling`) is about :math:`(n+1)/2`, as for the bootstrap.

**Implementation limit.** The confidence in an arc (`pyagrum.bnmixture.BootstrapMixture`) is counted on the arcs of
the learned networks, not on the features of their Markov equivalence classes as in :cite:`friedman1999data`: the
orientation of an arc that is not compelled may change from one copy to the other.
