BootstrapMixture holds a reference Bayesian network, learned from a database, and the networks learned from
bootstrap copies of this database (see `pyagrum.bnmixture.BNMBootstrapLearner` for the random weights of the records
and :cite:`friedman1999data` for the method). The reference is the estimate; the other networks only measure its
stability.

**Confidence in an arc.** The confidence in :math:`X \to Y` is the weighted frequency of the arc among the networks
:math:`G_1, \dots, G_B` of the copies, the reference excluded:

.. math::

   \mathrm{conf}(X \to Y) = \frac{\sum_{i \,:\, X \to Y \in G_i} w_i}{\sum_i w_i},

all the weights :math:`w_i` being 1 (a network added twice gets weight 2).

**Quantile intervals.** For each value :math:`x` of a variable :math:`X`, the interval around the posterior
:math:`P_{\mathrm{ref}}(x \mid e)` of the reference is given by two empirical quantiles of the posteriors
:math:`P_i(x \mid e)` of the :math:`B` networks of the copies, of levels ``leftQuantile`` (0.2) and ``rightQuantile``
(0.8) of the section ``bnmixture`` of the configuration: by default a 60% interval, not 95%. The quantile of level
:math:`q` is the linear interpolation between the sorted values :math:`v_{(1)} \leq \dots \leq v_{(B)}` placed at
:math:`(k-1)/(B-1)`, definition 7 of :cite:`hyndman1996sample`:

.. math::

   Q(q) = v_{(\lfloor h \rfloor)} + (h - \lfloor h \rfloor)\left(v_{(\lfloor h \rfloor + 1)} - v_{(\lfloor h \rfloor)}\right),
   \qquad h = (B-1)\,q + 1.

The posteriors of the copies are not weighted, and the posterior of the reference is not one of them: it may lie
outside its interval.
