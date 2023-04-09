%feature("docstring") gum::VariableElimination
"
Class used for Variable Elimination inference algorithm.

Warnings
--------
  Even if this inference has the same API than the other (exact) inferences, its mode of operation is different and is specifically dedicated to the calculation of a single posterior. Any other use (for instancte for multiple targets) is possibly inefficient.

VariableElimination(bn) -> VariableElimination
    Parameters:
        - **bn** (*pyAgrum.BayesNet*) -- a Bayesian network
"
