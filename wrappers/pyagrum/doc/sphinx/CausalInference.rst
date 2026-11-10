Causal computation in pyAgrum
==============================

Causal Inference (a.k.a. do-calculus)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The main entry point for causal inference is :func:`pyagrum.causalImpact`, which computes the
causal effect of an intervention :math:`do(X=x)` on a target variable :math:`Y` within a
:class:`pyagrum.CausalModel`, using do-calculus identification.

.. pyagrum-functions::

   pyagrum.causalImpact

.. pyagrum-class:: pyagrum.CausalImpact

Counterfactual reasoning
^^^^^^^^^^^^^^^^^^^^^^^^

.. pyagrum-functions::

   pyagrum.counterfactual
   pyagrum.counterfactualModel

.. pyagrum-class:: pyagrum.Counterfactual
