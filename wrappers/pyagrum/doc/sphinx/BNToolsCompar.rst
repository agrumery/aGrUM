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


.. autoclass:: pyagrum.MCBNDistance

In general, :class:`pyagrum.MCBNDistance` converges better (faster, with a smaller variance)
than :class:`pyagrum.GibbsBNdistance` for a given number of iterations. This is expected:
computing a distance between two BNs does not involve any observation (hard evidence) to
account for during sampling, so there is no need for the Markov chain machinery (mixing,
burn-in) that Gibbs sampling relies on to handle evidence. Drawing independent samples
directly from P is both simpler and, here, more efficient.

.. seealso::

   :doc:`pyAgrum.lib`
      :class:`pyagrum.lib.bn_vs_bn.GraphicalBNComparator` for structural comparison of two Bayesian networks.

   :doc:`BNToolsStructuralMetrics`
      :class:`pyagrum.StructuralMetrics`, the (lower-level) C++ class computing precision,
      recall, F-score, SHD and SID, used internally by :class:`pyagrum.lib.bn_vs_bn.GraphicalBNComparator`.
