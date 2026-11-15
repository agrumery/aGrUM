ImportanceSampling draws the samples from an importance function :math:`Q` and weights each sample by the ratio of its
probabilities in the Bayesian network and in :math:`Q` :cite:`shachter1990simulation`; the estimator and its stopping
criterion, with the effective size of a weighted sample, are detailed in the notes of `pyagrum.WeightedSampling`.

**The importance function.** :math:`Q` is the Bayesian network where each observed variable :math:`E` is fixed to its
value :math:`e`, both in its own CPT and in the CPTs of its children: the unobserved variables are drawn forward from
their CPTs given the drawn and observed values of their parents. As is, :math:`Q` is the importance function of
likelihood weighting, and the weight :math:`w = \prod_E P(e \mid \mathrm{pa}(E))` is the one of
`pyagrum.WeightedSampling`.

**The lifting of small probabilities.** With :math:`\delta = \varepsilon / d`, where :math:`\varepsilon` is the
``epsilon`` of the stopping criterion (0.01 by default) and :math:`d` the largest domain size of the variables, if the
smallest positive parameter of :math:`Q` is below :math:`\delta`, every positive parameter of every CPT of :math:`Q`
is raised by :math:`\delta` and the CPT is normalized again:

.. math::

   Q(x \mid u) = \frac{P(x \mid u) + \delta}{1 + k_u\,\delta} \quad \text{if } P(x \mid u) > 0,
   \qquad Q(x \mid u) = 0 \text{ otherwise},

where :math:`k_u` is the number of positive values of :math:`P(\cdot \mid u)`. The weight becomes

.. math::

   w = \prod_E P(e \mid \mathrm{pa}(E)) \prod_{X \notin E} \frac{P(x \mid \mathrm{pa}(X))}{Q(x \mid \mathrm{pa}(X))}.

Each ratio is at most :math:`1 + k_u\delta`: the values of tiny prior probability, which the evidence may make likely,
are drawn more often and weighted down, instead of being missed or drawn rarely with a few dominating weights. Since
:math:`Q` and :math:`P` have the same null parameters, the estimator stays consistent. This is close to the heuristic
initialization of AIS-BN :cite:`cheng2000ais`, which replaces each probability below a threshold :math:`\theta`
(0.04 in its experiments) by :math:`\theta`, taken from the largest probability of the distribution. pyAgrum differs:
the lifting is additive on all the positive parameters, its threshold follows ``epsilon`` (changing ``epsilon`` changes
:math:`Q`), and :math:`Q` is not adapted during the sampling.
