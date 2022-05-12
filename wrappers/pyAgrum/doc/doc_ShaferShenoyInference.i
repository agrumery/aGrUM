%feature("docstring") gum::ShaferShenoyInference
"
Class used for Shafer-Shenoy inferences.

ShaferShenoyInference(bn) -> ShaferShenoyInference
    Parameters:
        * **bn** (*pyAgrum.BayesNet*) -- a Bayesian network
"

<<<<<<< HEAD
%feature("docstring") gum::ShaferShenoyInference::getNumberOfThreads
"
returns the number of threads used by ShaferShenoyInference during inferences.

Returns
-------
int
	the number of threads used by ShaferShenoyInference during inferences
"

%feature("docstring") gum::ShaferShenoyInference::setNumberOfThreads
"
If the parameter n passed in argument is different from 0, ShaferShenoyInference will use n threads during inferences, hence overriding aGrUM's default number of threads.
If, on the contrary, n is equal to 0, the ShaferShenoyInference will comply with aGrUM's default number of threads.

Parameters
----------
n : int
	the number of threads to be used by ShaferShenoyInference
"

%feature("docstring") gum::ShaferShenoyInference::isGumNumberOfThreadsOverriden
"
Indicates whether ShaferShenoyInference currently overrides aGrUM's default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether ShaferShenoyInference currently overrides aGrUM's default number of threads
"

=======
>>>>>>> 5b6a6e72e ([pyAgrum] fixing some documentations)
