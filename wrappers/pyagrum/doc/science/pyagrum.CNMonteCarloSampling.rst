CNMonteCarloSampling explores the strong extension of a credal network (see `pyagrum.CredalNet`) by sampling its
vertices, as in :cite:`hourbracq2013dynamic`. Since the lower and upper posteriors are reached on combinations of
vertices of the local credal sets :cite:`cozman2000credal`, each sample is such a combination: for every variable and
every configuration of its parents, one vertex of the local set is drawn uniformly and independently. The resulting
Bayesian network is solved exactly (by `pyagrum.LazyPropagation`, with the evidence), and the bounds are updated:

.. math::

   \underline{P}(x \mid e) = \min_{k \leq N} P_k(x \mid e), \qquad
   \overline{P}(x \mid e) = \max_{k \leq N} P_k(x \mid e),

over the :math:`N` sampled networks :math:`P_k` (a network in which the evidence is impossible is skipped). Each
:math:`P_k` belongs to the strong extension, so the bounds are an **inner approximation**: the interval always lies
inside the exact one and grows towards it with the number of samples. It is not a statistical estimate: there is no
error bar, and an extreme combination of vertices that is unlikely under the uniform drawing (when the optimum needs a
precise vertex in many local sets) may be reached late, or never.

**The stopping criterion.** The samples are drawn by periods of ``periodSize`` (1000 by default, section
``CNMontecarlo`` of `pyagrum.config`, as ``maxTime``) samples. After each period, :math:`\varepsilon` is the largest
change of a lower or upper bound, over all the values of all the variables, during the period; the sampling stops when
:math:`\varepsilon` is at most ``epsilon``, by default :math:`1000` times the machine epsilon (about
:math:`2.2 \times 10^{-13}`): a whole period has found no new bound. A smaller ``epsilon`` is refused, since a change
below this tolerance is rounding. The sampling also stops after ``maxTime`` (60 seconds by default);
``minEpsilonRate`` and ``maxIter`` are disabled by default. Each sample is drawn from its own random generator, seeded
by the seed of the inference and the index of the sample, so the result does not depend on the number of threads.

**Dynamic networks.** For a dynamic credal network whose variables are named ``X_0``, ``X_1``, ... ``X_T``, the
default drawing is independent for each time slice (the dynamic strong extension of :cite:`hourbracq2013dynamic`).
With ``setRepetitiveInd(True)``, the slices of a variable whose tables have the same size share their drawing (one
drawing for the slice 0, one for the transition, reused by the later slices): the dynamic repetitive extension, a
subset of the strong one, which assumes a precise but ill-known transition model that does not change over time.
