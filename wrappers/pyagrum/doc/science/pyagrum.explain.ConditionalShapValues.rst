The explainers of `pyagrum.explain` compute Shapley values :cite:`shapley1953value`, as in SHAP
:cite:`lundberg2017unified`, exactly and by inference in the Bayesian network. For an instance :math:`x` and a game
:math:`v` on the set :math:`F` of the :math:`m` features (the players), the value of a feature :math:`i` is its mean
marginal contribution

.. math::

   \phi_i = \sum_{S \subseteq F \setminus \{i\}} \frac{|S|!\,(m - |S| - 1)!}{m!}\,\big(v(S \cup \{i\}) - v(S)\big),
   \qquad \sum_{i \in F} \phi_i = v(F) - v(\emptyset).

**The Shap explainers** (`pyagrum.explain.ConditionalShapValues`, `pyagrum.explain.MarginalShapValues`,
`pyagrum.explain.CausalShapValues`) explain the posterior of a target :math:`Y`: the features are the variables given
in the data, except :math:`Y`, and each value :math:`y` of the target has its own Shapley values, for the game
:math:`v(S) = g(p_S(y))`, where :math:`p_S(y)` is a probability of :math:`y` that depends on the explainer and
:math:`g` is the logit :math:`\log\frac{p}{1-p}` (default, ``logit=True``; a probability of 0 or 1 gives 0) or the
identity. The baseline is :math:`v(\emptyset)`. All the :math:`2^m - 1` coalitions are computed, so that the cost
grows exponentially with the number of features. Several rows are explained at once after removing the duplicated
rows (without data, 100 rows are drawn from the network); the importance of a feature is the mean of
:math:`|\phi_i|` over these distinct rows.

**ConditionalShapValues** uses the posterior given the values of the coalition, as advocated by
:cite:`aas2021explaining` for dependent features:

.. math::

   p_S(y) = P(y \mid x_S),

computed by exact inference (`pyagrum.LazyPropagation`), with :math:`p_\emptyset(y) = P(y)`. It needs no background
data. A coalition whose extra variable does not change the posterior (it is not in the minimal conditioning set of
:math:`Y` given the coalition) reuses the value of the smaller coalition. Unlike the marginal values, a feature
outside the Markov blanket of :math:`Y` may receive a non-null value, through its dependence with the features of
the blanket: the conditional values are "true to the data", the marginal ones "true to the model"
:cite:`chen2020true`.
