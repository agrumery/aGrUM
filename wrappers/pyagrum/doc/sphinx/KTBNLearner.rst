Learning k-TBNs
-----------------

:class:`pyagrum.ktbn.KTBNDatabaseGenerator` samples trajectory CSV
databases from a k-TBN, for learning experiments.

.. autoclass:: pyagrum.ktbn.KTBNDatabaseGenerator

:class:`pyagrum.ktbn.KTBNLearner` learns a k-TBN's structure and/or
parameters from trajectory CSVs, at a fixed order k.

.. autoclass:: pyagrum.ktbn.KTBNLearner

:class:`pyagrum.ktbn.KTBNAdaptiveLearner` additionally selects the
order k itself, by exploring every candidate in a range and keeping the best
one according to a cross-k model-selection criterion.

.. autoclass:: pyagrum.ktbn.KTBNAdaptiveLearner
