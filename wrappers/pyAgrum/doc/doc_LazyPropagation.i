%feature("docstring") gum::LazyPropagation
"
Class used for Lazy Propagation

LazyPropagation(bn) -> LazyPropagation
    Parameters:
        * **bn** (*pyAgrum.BayesNet*) -- a Bayesian network
"

%feature("docstring") gum::LazyPropagation::getNumberOfThreads
"
returns the number of threads used by LazyPropagation during inferences.

Returns
-------
int
	the number of threads used by LazyPropagation during inferences
"

%feature("docstring") gum::LazyPropagation::setNumberOfThreads
"
If the parameter n passed in argument is different from 0, LazyPropagation will use n threads during inferences, hence overriding aGrUM's default number of threads.
If, on the contrary, n is equal to 0, the LazyPropagation will comply with aGrUM's default number of threads.

Parameters
----------
n : int
	the number of threads to be used by LazyPropagation
"

%feature("docstring") gum::LazyPropagation::isGumNumberOfThreadsOverriden
"
Indicates whether LazyPropagation currently overrides aGrUM's default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether LazyPropagation currently overrides aGrUM's default number of threads
"

