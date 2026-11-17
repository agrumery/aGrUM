DiscreteTypeProcessor turns the columns of a database into discrete variables. A numerical column is discretized when
it has more than ``discretizationThreshold`` distinct values (25 by default; a threshold below 1 is a proportion of
distinct values), otherwise its values become labels. The default method is ``quantile`` with 10 intervals; the
candidate cut points of the supervised methods are the midpoints between consecutive distinct values.

- ``quantile``, ``uniform``, ``kmeans``: intervals of equal frequency, of equal width, or the clusters of a
  one-dimensional k-means, with the given number of intervals.
- ``MDLP`` :cite:`fayyad1993multi` (supervised): an interval :math:`S` of :math:`N` samples is split recursively at
  the cut point :math:`T` that minimizes the class entropy :math:`E(T) = \frac{|S_1|}{N} H(S_1) +
  \frac{|S_2|}{N} H(S_2)`, while the gain satisfies the minimum description length criterion

  .. math::

     H(S) - E(T) > \frac{\log_2(N-1)}{N} + \frac{\log_2(3^k - 2) - [k H(S) - k_1 H(S_1) - k_2 H(S_2)]}{N},

  :math:`k, k_1, k_2` being the numbers of classes in :math:`S, S_1, S_2`.
- ``CAIM`` :cite:`kurgan2004caim` (supervised): cut points are added greedily, each maximizing
  :math:`\mathrm{CAIM} = \frac{1}{n} \sum_{r=1}^{n} \max_r^2 / M_r` (:math:`n` intervals, :math:`M_r` samples in the
  interval :math:`r`, :math:`\max_r` the count of its most frequent class), as long as it increases.
- ``NML`` :cite:`kontkanen2007mdl` (unsupervised): among the histograms with at most ``defaultNumberOfBins`` (10)
  intervals, the one that minimizes the NML code length of the data plus :math:`\log\binom{E}{K-1}` for the choice of
  its :math:`K-1` cut points among the :math:`E` candidates; the global optimum is found by dynamic programming in
  :math:`O(E^2 K)`. The limit of 10 intervals is often reached: on the mixtures of Gaussians below, it is reached
  from 1000 samples on (for 5 to 10 samples out of 10 with 1000 samples, for all of them with 3000). The result is
  then the best histogram with 10 intervals, not the one chosen by the criterion: a larger limit (the parameter of
  the method) lets the criterion choose the number of intervals, at a cost linear in this limit.

Choices of pyAgrum: MDLP and CAIM accept any number of classes. As in :cite:`kurgan2004caim`, cut points are added
to CAIM at least until there are as many intervals as classes; MDLP makes no forced cut and raises an error when no
cut point satisfies the criterion (another method must then be chosen for this column); NML returns at least two
intervals. The candidate cut points of NML are the midpoints of consecutive distinct values, one of the choices
discussed in :cite:`kontkanen2007mdl` (section 4), and not the points :math:`x \pm \varepsilon/2` around each value,
:math:`\varepsilon` being the precision of the data: with an :math:`\varepsilon` smaller than the gaps between
values, these allow bins of width :math:`\varepsilon` around isolated values, which the criterion favours. On the
protocol of the article (mixtures of Gaussians close to those of its figures, data recorded with precision 0.1, 10
samples per size), the midpoints give the same squared Hellinger distance to the generating density as
:math:`\varepsilon = 0.1` (0.021 against 0.023 for 1000 samples, 0.0054 against 0.0057 for 10000, with the same
numbers of bins), while :math:`\varepsilon` set to half the smallest gap gives 0.20 with about 20 bins narrower than
the precision; on continuous data (1000 samples), the midpoints give 0.027 against 0.039. The midpoints exclude
empty bins.
