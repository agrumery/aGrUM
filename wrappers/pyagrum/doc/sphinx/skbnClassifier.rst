Classifier using Bayesian networks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:class:`pyagrum.skbn.BNClassifier` is a `scikit-learn <https://scikit-learn.org>`_-compatible
classifier built on top of a :class:`pyagrum.BayesNet`. It implements the standard ``fit`` /
``predict`` / ``predict_proba`` interface, making it easy to integrate Bayesian network
classifiers into existing scikit-learn pipelines.

.. autoclass:: pyagrum.skbn.BNClassifier
