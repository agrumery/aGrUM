DiscreteTypeProcessor for graphical Models
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

:class:`pyagrum.lib.discreteTypeProcessor.DiscreteTypeProcessor` preprocesses columns of a pandas
``DataFrame`` (or CSV file) to map them to discrete random variables suitable for use in graphical
models. It is used internally by :class:`pyagrum.BNLearner` when learning from data.

.. autoclass:: pyagrum.lib.discreteTypeProcessor.DiscreteTypeProcessor
