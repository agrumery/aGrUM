%feature("docstring") gum::LazyPropagation<double>
"
Class used for Lazy Propagation

LazyPropagation(bn) -> LazyPropagation
    Parameters:
        - **bn** (*pyAgrum.BayesNet*) -- a Bayesian network
"

%feature("docstring") gum::LazyPropagation<double>::mpe
"
Find the Most Probable Explanation (MPE) given the evidence (if any) added into LazyPropagation

Returns
-------
pyAgrum.Instantiation
  An instantiation of all the variables of the Bayes net representing the Most Probable Explanation.
"

%feature("docstring") gum::LazyPropagation<double>::mpeLog2Posterior
"
Find the Most Probable Explanation (MPE) given the evidence (if any) added into LazyPropagation as well as the log2 of its posterior probability

Returns
-------
Tuple[pyAgrum.Instantiation, float]
    A tuple with the instantiation of all the variables of the Bayes net representing the Most Probable Explanation and the log2 of its posterior probability
"
