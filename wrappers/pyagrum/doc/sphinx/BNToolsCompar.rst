Comparison of Bayesian networks
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. figure:: _static/BNcomparison.png
    :align: center
    :alt: tools for comparing Bayesian networks

To compare Bayesian networks, one can compare their structure
(see :class:`pyagrum.lib.bn_vs_bn.GraphicalBNComparator`).
However BNs can also be compared as probability distributions.

.. autoclass:: pyagrum.ExactBNdistance


.. autoclass:: pyagrum.GibbsBNdistance

.. seealso::

   :doc:`pyAgrum.lib`
      :class:`pyagrum.lib.bn_vs_bn.GraphicalBNComparator` for structural comparison of two Bayesian networks.

   :doc:`BNToolsStructuralMetrics`
      :class:`pyagrum.StructuralMetrics`, the (lower-level) C++ class computing precision,
      recall, F-score, SHD and SID, used internally by :class:`pyagrum.lib.bn_vs_bn.GraphicalBNComparator`.
