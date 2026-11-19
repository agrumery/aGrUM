BootstrapMixture holds a reference Bayesian network, learned from a database, and the networks learned from
bootstrap copies of this database (see `pyagrum.bnmixture.BNMBootstrapLearner` for the random weights of the records
and :cite:`friedman1999data` for the method). The reference is the estimate; the other networks only measure its
stability.

**Confidence in an arc.** The confidence in :math:`X \to Y` is the weighted frequency of the arc among the networks
:math:`G_1, \dots, G_B` of the copies, the reference excluded:

.. math::

   \mathrm{conf}(X \to Y) = \frac{\sum_{i \,:\, X \to Y \in G_i} w_i}{\sum_i w_i},

all the weights :math:`w_i` being 1 (a network given by two copies is stored once with weight 2). It is the value of
`pyagrum.bnmixture.BootstrapMixture.arcConfidence`; ``existsArc`` counts the stored networks, whatever their weights.

**Quantile intervals.** For each value :math:`x` of a variable :math:`X`, the interval around the posterior
:math:`P_{\mathrm{ref}}(x \mid e)` of the reference is given by two empirical quantiles of the posteriors
:math:`P_i(x \mid e)` of the :math:`B` networks of the copies, of levels ``leftQuantile`` (0.2) and ``rightQuantile``
(0.8) of the section ``bnmixture`` of the configuration: by default a 60% interval, not 95%. The quantile of level
:math:`q` is the linear interpolation between the sorted values :math:`v_{(1)} \leq \dots \leq v_{(B)}` placed at
:math:`(k-1)/(B-1)`, definition 7 of :cite:`hyndman1996sample`:

.. math::

   Q(q) = v_{(\lfloor h \rfloor)} + (h - \lfloor h \rfloor)\left(v_{(\lfloor h \rfloor + 1)} - v_{(\lfloor h \rfloor)}\right),
   \qquad h = (B-1)\,q + 1.

With weights (a network given by two copies has weight 2), the weighted version of :cite:`akinshin2023weighted` is
used: :math:`B` is replaced by Kish's effective size :math:`B^* = (\sum_i w_i)^2 / \sum_i w_i^2`, and each sorted
value receives :math:`B^*` times the overlap of its interval of cumulated normalized weights with
:math:`[(h-1)/B^*,\, h/B^*]`, :math:`h = (B^*-1)\,q + 1`. Equal weights give the formula above; normalizing the weights
changes nothing; a network of weight 2 is not the same as two networks of weight 1. The posterior of the reference is
not one of the values: it may lie outside its interval.
