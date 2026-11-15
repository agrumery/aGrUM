MarginalShapValues computes the marginal (or interventional) Shapley values advocated by :cite:`janzing2020feature`
(see `pyagrum.explain.ConditionalShapValues` for the Shapley values and the options common to the explainers): the
features out of the coalition keep the values of a background sample :math:`b_1, \dots, b_N`, independently of the
values of the coalition,

.. math::

   p_S(y) = \frac{1}{N} \sum_{k=1}^{N} P\big(y \mid x_S, (b_k)_{F \setminus S}\big),

each term being the posterior of the target given all the features (a hybrid row). The logit, if any, is applied to
this mean. The background is a data frame with a column for each variable of the network (the target column is
ignored); without it, ``sample_size`` (1000) rows are drawn from the network, so that the values are then random.
When all the variables are features, the posterior given all of them depends only on the Markov blanket of
:math:`Y`, and the features outside the blanket get a null value.
