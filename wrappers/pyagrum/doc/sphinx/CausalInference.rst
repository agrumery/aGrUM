Causal Inference (a.k.a. do-calculus)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

The main entry point for causal inference is :func:`pyagrum.causalImpact`, which computes the
causal effect of an intervention :math:`do(X=x)` on a target variable :math:`Y` within a
:class:`pyagrum.CausalModel`, using do-calculus identification.

.. autofunction:: pyagrum.causalImpact

.. autoclass:: pyagrum.CausalImpact

Counterfactual reasoning
^^^^^^^^^^^^^^^^^^^^^^^^

.. autofunction:: pyagrum.counterfactual

.. autofunction:: pyagrum.counterfactualModel
