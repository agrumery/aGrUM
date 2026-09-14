The k-TBN model and its generator
----------------------------------

:class:`pyagrum.ktbn.KTBN` is the main class for representing and manipulating
k-order dynamic Bayesian networks in pyAgrum. It stores a compact template of
k time slices, distinguishing temporal processes from atemporal (static)
variables, and can be unrolled into a plain :class:`pyagrum.BayesNet` for any
number of time slices.

.. autoclass:: pyagrum.ktbn.KTBN

:class:`pyagrum.ktbn.KTBNGenerator` draws a random k-TBN template, mainly to
build ground-truth models for learning experiments.

.. autoclass:: pyagrum.ktbn.KTBNGenerator
