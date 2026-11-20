CLGLearner learns a `pyagrum.clg.CLG` from a database of continuous variables: the structure with the PC algorithm,
whose independence tests bound the family-wise error rate (FWER) with Rademacher averages
:cite:`simionato2023bounding,simionato2024bounding`, then the parameters by least squares.

**The adjacency search** is the one of the original PC algorithm :cite:`spirtes2000causation`, as written in
Algorithms 3.1 and 3.2 of :cite:`colombo2014order`: starting from the complete graph, for :math:`\ell = 0, 1, \dots`,
each ordered pair :math:`(X, Y)` of adjacent variables is tested given the subsets of size :math:`\ell` of the other
current neighbours of :math:`X`; the edge is removed at the first independence found, the subset being kept as
separating set. The variables are taken in the order of the columns of the data.

**The independence test.** For :math:`X`, :math:`Y` and a set :math:`Z`, :math:`x` and :math:`y` are the residuals of
the least-squares regressions (with intercept) of :math:`X` and :math:`Y` on :math:`Z` (the raw values if :math:`Z` is
empty), centred over the :math:`K` records. The statistic is the r-centered statistic of :cite:`simionato2024bounding`
(Eq. 12), :math:`\bar r = \frac{1}{K}\sum_i r_i` with :math:`r_i = x_i y_i / \max(|x_i|, |y_i|)^2 \in [-1, 1]`, of
expectation 0 when :math:`X` and :math:`Y` are independent (Theorem 9); :math:`X \perp Y \mid Z` is accepted when
:math:`|\bar r| \leq D`, a bound on the deviation of all the statistics at once. The family :math:`\mathcal{F}` holds
the :math:`L(L-1)\,2^{L-3}` possible tests (:math:`L` variables); with :math:`n` (``n_sample``, 15) vectors
:math:`\sigma_j` of :math:`K` random signs, the Monte-Carlo empirical Rademacher average and the bound of Theorem 3.1
of :cite:`pellegrina2020mcrapper` are

.. math::

   R = \frac{1}{n}\sum_{j=1}^{n} \max_{f \in \mathcal{F}} \frac{1}{K}\sum_{i=1}^{K} \sigma_{j,i}\, r^f_i, \qquad
   \tilde R = R + 2z\sqrt{\frac{\lambda}{2nK}}, \qquad
   D = 2\tilde R + \frac{\sqrt{c\,(4K\tilde R + c\lambda)\,\lambda}}{K} + \frac{c\lambda}{K} + c\sqrt{\frac{\lambda}{2K}},

with :math:`\lambda = \ln(4/\delta)` (:math:`\delta` = ``fwer_delta``, 0.05), :math:`z = \max(|a|, |b|) = 1` and
:math:`c = b - a = 2` for the range :math:`[a, b] = [-1, 1]` of the terms. The bound holds for all the tests at once
with probability at least :math:`1 - \delta`: the FWER of the independence tests is at most :math:`\delta`.

**Orientation.** The v-structures are found from the separating sets, then the rules R1 to R3 of
:cite:`meek1995causal` are applied. A pyAgrum heuristic orients what remains: one undirected edge :math:`X - Y` is
oriented :math:`X \to Y` when the empirical standard deviation of :math:`X` is at most that of :math:`Y`, then the
rules are applied again, until every edge is oriented. This follows the increasing marginal variances, which often
agree with the causal order in simulated linear models but depend on the units of the variables
:cite:`reisach2021beware`.

**Parameters.** Each variable is regressed on its parents by ordinary least squares: the intercept gives ``mu``, the
slopes the arc coefficients, and ``sigma`` is the standard deviation of the residuals (divided by :math:`K`: maximum
likelihood); a root gets the mean and standard deviation of its values. ``fitParameters`` does the same on a given
structure.

**Implementation limit.** The skeleton may depend on the order of the columns, as the neighbourhoods change during a
level (the order-independent PC-stable variant of :cite:`colombo2014order` is not used). All the statistics are
computed before the first test, with up to two regressions each: the cost grows as :math:`L^2 2^L`, which limits the
learning to a few variables. The guarantee is paid in power: :math:`D` decreases as :math:`1/\sqrt{K}`, so that a
weak dependence needs many records (with six variables and five arcs of coefficients between 0.8 and 2, all the arcs
are found from about 10000 records).
