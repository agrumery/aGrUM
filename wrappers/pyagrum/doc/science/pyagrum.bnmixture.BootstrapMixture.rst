BootstrapMixture holds a reference Bayesian network, learned from a database, and the networks learned from
bootstrap copies of this database (see `pyagrum.bnmixture.BNMBootstrapLearner` for the random weights of the records
and :cite:`friedman1999data` for the method). The reference is the estimate; the other networks only measure its
stability.

**Confidence in an arc.** Identical networks are stored once, their weight being the number of copies that gave
them. The confidence in :math:`X \to Y` is the weighted frequency of the arc among the stored networks
:math:`G_1, \dots, G_m` of weights :math:`w_1, \dots, w_m`, the reference excluded:

.. math::

   \mathrm{conf}(X \to Y) = \frac{\sum_{i \,:\, X \to Y \in G_i} w_i}{\sum_i w_i},

that is, the frequency of the arc among the :math:`B` copies (`pyagrum.bnmixture.BootstrapMixture.arcConfidence`).

**Quantile intervals.** For each value :math:`x` of a variable, the interval around the posterior
:math:`P_{\mathrm{ref}}(x \mid e)` of the reference is given by two weighted quantiles of the posteriors
:math:`P_i(x \mid e)` of the stored networks, of levels ``leftQuantile`` (0.2) and ``rightQuantile`` (0.8) of the
section ``bnmixture`` of the configuration: by default a 60% interval, not 95%. With equal weights, the quantile of
level :math:`q` of the sorted values :math:`v_{(1)} \leq \dots \leq v_{(m)}` is definition 7 of
:cite:`hyndman1996sample`:

.. math::

   Q(q) = v_{(\lfloor h \rfloor)} + (h - \lfloor h \rfloor)\left(v_{(\lfloor h \rfloor + 1)} - v_{(\lfloor h \rfloor)}\right),
   \qquad h = (m-1)\,q + 1.

Otherwise, the weighted version of :cite:`akinshin2023weighted` is used: :math:`m` is replaced by Kish's effective
size :math:`m^* = (\sum_i w_i)^2 / \sum_i w_i^2`, and each sorted value gets the mass :math:`m^*` times the overlap of
its interval of cumulated normalized weights with :math:`[(h-1)/m^*,\, h/m^*]`, :math:`h = (m^*-1)\,q + 1`.
Normalizing the weights changes nothing. The posterior of the reference is not one of the values: it may lie outside
its interval.

**Implementation limit.** A network of weight 2 does not count as two networks of weight 1 (:math:`m^*` is smaller):
when several copies give the same network, the interval is not the quantile of definition 7 on the :math:`B`
posteriors of the copies.
