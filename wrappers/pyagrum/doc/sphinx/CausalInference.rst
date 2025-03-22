Causal Formula
^^^^^^^^^^^^^^
`CausalFormula` is the class that represents a causal query in a causal model. Mainly it consits in

- a reference to the CausalModel
- Three sets of variables name that represent the 3 sets of variable in the query P(set1 | doing(set2),knowing(set3)).
- the AST for compute the query.

.. autoclass:: pyagrum.causal.CausalFormula

Causal Inference
^^^^^^^^^^^^^^^^

Obtaining and evaluating a CausalFormula is done using one these functions :

.. autofunction:: pyagrum.causal.causalImpact

.. autofunction:: pyagrum.causal.doCalculusWithObservation

.. autofunction:: pyagrum.causal.identifyingIntervention

Other functions for causal inference
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

.. autofunction:: pyagrum.causal.backdoor_generator

.. autofunction:: pyagrum.causal.frontdoor_generator
