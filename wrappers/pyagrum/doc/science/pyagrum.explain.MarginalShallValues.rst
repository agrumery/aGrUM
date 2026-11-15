The Shall values (Shapley additive log-likelihood) are specific to pyAgrum: instead of a prediction, they share the
log-likelihood :math:`\log P(x)` of a complete instance :math:`x` among the variables. They are the Shapley values :cite:`shapley1953value` (see
`pyagrum.explain.ConditionalShapValues`) of a game whose players are all the variables of the network (the data must
give them all), the value :math:`v(S)` of a coalition being an expected log-likelihood when the variables of
:math:`S` take their values in :math:`x`. It is computed on a background sample :math:`b_1, \dots, b_N` (data frame of
the variables; without it, ``sample_size`` (1000) rows are drawn from the network) with the joint probability given by
the network (natural logarithm; the probability itself with ``log=False``). With :math:`v(F) = \log P(x)`, the values
sum to :math:`\log P(x) - v(\emptyset)`: a negative value marks a variable whose value makes the instance less likely
than the background. As for the Shap values, all the :math:`2^n - 1` coalitions are computed, duplicated rows are
explained once and the importance is the mean of :math:`|\phi_i|`.

**MarginalShallValues** completes the coalition with each background row, independently of :math:`x_S`:

.. math::

   v(S) = \frac{1}{N} \sum_{k=1}^{N} \log P\big(x_S, (b_k)_{\bar S}\big),

so that :math:`v(\emptyset)` is the mean log-likelihood of the background, an estimate of :math:`-H(P)` for a sample
of the network.
