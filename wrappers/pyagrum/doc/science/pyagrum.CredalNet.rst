A credal network :cite:`cozman2000credal` replaces each conditional probability table of a Bayesian network by
credal sets: for each variable :math:`X_i` and each configuration :math:`\pi_i` of its parents, a closed convex set
:math:`K(X_i \mid \pi_i)` of distributions, given in pyAgrum by its vertices. The local sets are separately specified
(one per parent configuration) and the network represents their **strong extension**

.. math::

   K(X) = \mathrm{CH}\left\{ P : P(x) = \prod_i P_i(x_i \mid \pi_i),\ P_i(\cdot \mid \pi_i) \in K(X_i \mid \pi_i) \right\},

the convex hull of the joint distributions of all the Bayesian networks whose conditional distributions are taken in
the local sets. An inference computes, for each value :math:`x` of a variable, the lower and upper posteriors
:math:`\min_{P \in K} P(x \mid e)` and :math:`\max_{P \in K} P(x \mid e)`; they are reached on combinations of
vertices of the local sets :cite:`cozman2000credal`, which is what `pyagrum.CNMonteCarloSampling` exploits.

**Building the credal sets.** pyAgrum offers three ways, all of which store the vertices of each local set.

- From two Bayesian networks giving lower and upper probabilities :math:`l_k \leq p_k \leq u_k`: ``intervalToCredal``
  enumerates the vertices of :math:`\{p : l \leq p \leq u, \sum_k p_k = 1\}`.
- From a Bayesian network of counts, ``idmLearning(s)`` applies the imprecise Dirichlet model
  :cite:`walley1996inferences`: with :math:`n_k` the count of the value :math:`k` for a parent configuration and
  :math:`N = \sum_k n_k`, the set is :math:`p_k \in [n_k/(N+s),\, (n_k+s)/(N+s)]`, whose vertices are
  :math:`(n + s\,e_k)/(N+s)`. In pyAgrum :math:`s` is an integer and :math:`s = 0` (the default) gives the precise
  frequencies. With ``keepZeroes``, a value never observed keeps a null probability, and a configuration where a
  single value was observed stays precise. ``lagrangeNormalization`` adds 1 to all the counts of the configurations
  that contain a zero.
- ``bnToCredal(beta, ...)`` makes a Bayesian network imprecise as in :cite:`hourbracq2013dynamic`: each distribution
  :math:`p` becomes :math:`p_k \in [(1-\varepsilon)\,p_k,\ (1-\varepsilon)\,p_k + \varepsilon]` (a linear-vacuous
  mixture, whose vertices put the mass :math:`\varepsilon` on one value), with
  :math:`\varepsilon = \beta^{\ln(N+1)}` where :math:`N` is the number of cases observed for the parent
  configuration: the more data, the less imprecision; :math:`\beta = 0` keeps the network precise, :math:`\beta = 1`
  or :math:`N = 0` makes the distribution vacuous. ``epsilonMin``, ``epsilonMax`` and ``epsilonMean`` summarize the
  :math:`\varepsilon` used.

**Approximate binarization.** `pyagrum.CNLoopyPropagation` needs binary variables. ``approximatedBinarization``
replaces each variable with :math:`d > 2` values by :math:`\lceil \log_2 d \rceil` binary variables (its bits
``X-b0``, ``X-b1``...): a bit has as parents the bits of the parents of the variable and its own preceding bits, and
its credal set is the interval between the lowest and the highest probability of the bit over the vertices of the
original set. Indicator variables ``X-v0``, ``X-v1``... give the probabilities of the values of :math:`X`. This is
the binarization recalled in :cite:`antonucci2010generalized`, without the exact reformulation proposed there: the
binarized network represents more distributions than the original one, so the bounds computed on it are an outer
approximation (wider intervals), which can be crude.
