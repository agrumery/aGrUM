Learner learns the conditional intensity matrices (see `pyagrum.ctbn.CIM`) and the graph of a `pyagrum.ctbn.CTBN` from
fully observed trajectories.

**Parameters.** For a variable :math:`X` with parents :math:`U`, the sufficient statistics of the trajectories are the
time :math:`T[x \mid u]` spent in :math:`x` while :math:`U = u`, and the number :math:`M[x, x' \mid u]` of transitions
from :math:`x` to :math:`x'` while :math:`U = u`, summed over all the trajectories :cite:`nodelman2003learning`.
The maximum likelihood estimates are

.. math::

   \hat q_{xx' \mid u} = \frac{M[x, x' \mid u]}{T[x \mid u]}, \qquad
   \hat q_{x \mid u} = \frac{M[x \mid u]}{T[x \mid u]}, \qquad M[x \mid u] = \sum_{x' \neq x} M[x, x' \mid u],

(no prior; a configuration never visited keeps rates 0).

**Structure.** The graph is learned by CTPC :cite:`bregoli2020constraint`, a PC-like algorithm: starting from the complete
directed graph, for each :math:`X` and for growing sizes :math:`b`, the arc :math:`Y \rightarrow X` is removed as soon
as :math:`X` is found independent of :math:`Y` given a subset :math:`S` of size :math:`b` of the other current parents.
As the dependence of a continuous-time process is not symmetric and follows time, no orientation step is needed, and
cycles are allowed. :math:`X` is declared independent of :math:`Y` given :math:`S` unless one of two tests rejects it:

- the *time to transition* test compares the rates without and with :math:`Y`, for every configuration, with the F
  test for two exponential means: :math:`F = \hat q_{x \mid s} / \hat q_{x \mid y,s}`, of Fisher law with
  :math:`(r_1, r_2)` degrees of freedom, :math:`r_1 = M[x \mid y, s]`, :math:`r_2 = M[x \mid s]`. It rejects when
  :math:`F` is out of the interval between the :math:`\alpha_F/2` and :math:`1 - \alpha_F/2` quantiles, for some state
  :math:`x`, with :math:`\alpha_F` = ``pyagrum.config['ctbn', 'fSignificance']`` (0.05);
- the *state to state* test (only when :math:`X` has at least 3 values, otherwise the target of a transition is known)
  compares the transition counts with a two-sample chi-square statistic

  .. math::

     \chi^2 = \sum_{x' \neq x} \frac{(K\, M[x,x' \mid y,s] - L\, M[x,x' \mid s])^2}{M[x,x' \mid y,s] + M[x,x' \mid s]},
     \quad K = \sqrt{\frac{M[x \mid s]}{M[x \mid y,s]}}, \quad L = \frac{1}{K},

  compared with the :math:`1 - \alpha_{\chi^2}` quantile of a :math:`\chi^2` law with :math:`|\mathrm{Val}(X)| - 1`
  degrees of freedom, with :math:`\alpha_{\chi^2}` = ``pyagrum.config['ctbn', 'chi2Significance']`` (0.05).

The two levels are those of each elementary test, without correction for the number of tests (one per state and
configuration of the parents), as in the reference implementation of the authors.

**Implementation limit.** The variables and their values not given by a template CTBN are those seen in the data.
