ConditionalShallValues computes Shall values (see `pyagrum.explain.MarginalShallValues`) whose game is the mean
log-likelihood of the background rows that agree with the instance on the coalition, an estimate of
:math:`E[\log P(X) \mid X_S = x_S]` under the distribution of the background (true to the data, in the sense of
:cite:`chen2020true`):

.. math::

   v(S) = \frac{1}{|B_S|} \sum_{b \in B_S} \log P(b), \qquad B_S = \{b_k : (b_k)_S = x_S\}.

**Implementation limit.** When no background row agrees with :math:`x_S` (large coalitions, rare values, small
background), :math:`v(S)` is set to 0 (the log-likelihood of a sure event; a null probability with ``log=False``), which biases the values: the background
must be large enough for the conditional means to exist.
