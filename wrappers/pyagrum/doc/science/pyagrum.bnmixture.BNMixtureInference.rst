BNMixtureInference runs the same inference (`pyagrum.LazyPropagation` by default), with the same evidence :math:`e`,
in every network of a `pyagrum.bnmixture.BNMixture`, and returns the posterior of the mixture distribution
:math:`P = \sum_i w_i P_{B_i} / \sum_i w_i`:

.. math::

   P(x \mid e) = \frac{\sum_i w_i\, P_{B_i}(e)\, P_{B_i}(x \mid e)}{\sum_i w_i\, P_{B_i}(e)}.

The weight of each network is updated by the probability it gives to the evidence, as in Bayesian model averaging
:cite:`hoeting1999bayesian`; a
network in which :math:`e` is impossible does not count. Without evidence, this is the weighted mean of the posteriors.
With evidence, the inference engine must compute :math:`P_{B_i}(e)` (`pyagrum.LazyPropagation`,
`pyagrum.ShaferShenoyInference`).
