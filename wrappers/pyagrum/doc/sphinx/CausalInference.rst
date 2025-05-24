Causal Inference (a.k.a. do-calculus)
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

Obtaining and evaluating a CausalFormula is done using one these functions :

.. autofunction:: pyagrum.causal.causalImpact

.. autofunction:: pyagrum.causal.doCalculusWithObservation

.. autofunction:: pyagrum.causal.identifyingIntervention

Other functions for causal inference
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

`CausalFormula` is the class that represents a causal query within a causal model. It mainly consists of:

- a reference to the associated :class:`pyagrum.CausalModel`,
- three sets of variable names representing the groups in the query $P(set1 | do(set2), know(set3))$,
- and an abstract syntax tree (AST) used to compute or display the query.

.. autoclass:: pyagrum.causal.CausalFormula

.. autofunction:: pyagrum.causal.backdoor_generator

.. autofunction:: pyagrum.causal.frontdoor_generator
