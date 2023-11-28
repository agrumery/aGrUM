%feature("docstring") gum::LazyPropagation<double>
"
Class used for Lazy Propagation

LazyPropagation(bn) -> LazyPropagation
    Parameters:
        - **bn** (*pyAgrum.BayesNet*) -- a Bayesian network
"

%feature("docstring") gum::LazyPropagation<double>::mpe
"
Find the Most Probable Explanation (MPE) using LazyPropagation using the evidence (if any)

Returns
-------
pyAgrum.Instantiation
  An instantiation representing the Most Probable Explanation.
"

%feature("docstring") gum::LazyPropagation<double>::mpeLog2Posterior
"
Find the Most Probable Explanation (MPE) using LazyPropagation and the (log) conditionnal probability of the solution (given the evidence if any)

Returns
-------
Tuple[pyAgrum.Instantiation, float]
    A tuple with the instantiation representing the Most Probable Explanation and the (log) probability
"