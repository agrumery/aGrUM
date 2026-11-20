BNMixtureInference runs the same inference (`pyagrum.LazyPropagation` by default), with the same evidence :math:`e`,
in every network of a `pyagrum.bnmixture.BNMixture`, and returns the posterior of the mixture distribution :math:`P`:

.. math::

   P(x \mid e) = \frac{\sum_i w_i\, P_i(e)\, P_i(x \mid e)}{\sum_i w_i\, P_i(e)}.

Each weight is multiplied by the probability that its network gives to the evidence, as in Bayesian model averaging
:cite:`hoeting1999bayesian`: a network in which :math:`e` is impossible does not count, and without evidence this is
the weighted mean of the posteriors. With evidence, the inference engine must compute :math:`P_i(e)`
(`pyagrum.LazyPropagation`, `pyagrum.ShaferShenoyInference`).
