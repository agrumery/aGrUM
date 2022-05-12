%feature("docstring") gum::VariableElimination
"
Class used for Variable Elimination inference algorithm.

VariableElimination(bn) -> VariableElimination
    Parameters:
        * **bn** (*pyAgrum.BayesNet*) -- a Bayesian network
"
<<<<<<< HEAD

%feature("docstring") gum::VariableElimination::getNumberOfThreads
"
returns the number of threads used by VariableElimination during inferences.

Returns
-------
int
	the number of threads used by VariableElimination during inferences
"

%feature("docstring") gum::VariableElimination::setNumberOfThreads
"
If the parameter n passed in argument is different from 0, VariableElimination will use n threads during inferences, hence overriding aGrUM's default number of threads.
If, on the contrary, n is equal to 0, the VariableElimination will comply with aGrUM's default number of threads.

Parameters
----------
n : int
	the number of threads to be used by VariableElimination
"

%feature("docstring") gum::VariableElimination::isGumNumberOfThreadsOverriden
"
Indicates whether VariableElimination currently overrides aGrUM's default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether VariableElimination currently overrides aGrUM's default number of threads
"

=======
>>>>>>> 5b6a6e72e ([pyAgrum] fixing some documentations)
