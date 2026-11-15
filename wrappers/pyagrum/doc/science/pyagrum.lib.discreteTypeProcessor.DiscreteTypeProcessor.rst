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
- ``NML`` :cite:`kontkanen2007mdl` (unsupervised): the histogram whose cut points minimize the NML code length of the
  data, computed by dynamic programming with up to ``defaultNumberOfBins`` (10) intervals, the data being measured
  with the precision :math:`\varepsilon` of half the smallest gap between two distinct values.

Choices of pyAgrum: as in :cite:`kurgan2004caim`, cut points are added to CAIM at least until there are as many
intervals as classes, so its first cut is always made; MDLP makes no forced cut and raises an error when no cut point
satisfies the criterion (another method must then be chosen for this column); NML returns at least two intervals.

**Implementation limit.** MDLP and CAIM accept only a binary class.
