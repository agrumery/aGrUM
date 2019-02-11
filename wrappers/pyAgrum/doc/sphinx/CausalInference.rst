Causal Formula
^^^^^^^^^^^^^^
`CausalFormula` is the class that represents a causal query in a causal model. Mainly it consits in

- a reference to the CausalModel
- Three sets of variables name that represent the 3 sets of variable in the query P(set1 | doing(set2),knowing(set3)).
- the AST for compute the query.

.. autoclass:: pyAgrum.causal.CausalFormula

Causal Inference
^^^^^^^^^^^^^^^^

Obtaining and evaluating a CausalFormula is done using one these functions :

.. autofunction:: pyAgrum.causal.causalImpact

.. autofunction:: pyAgrum.causal.doCalculusWithObservation

.. autofunction:: pyAgrum.causal.identifyingIntervention


