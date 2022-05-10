%feature("docstring") gum::ShaferShenoyMNInference
"
Class used for Shafer-Shenoy inferences for Markov network.

ShaferShenoyInference(bn) -> ShaferShenoyInference
    Parameters:
        * **mn** (*pyAgrum.MarkovNet*) -- a Markov network
"

%feature("docstring") gum::ShaferShenoyMNInference::getNumberOfThreads
"
returns the number of threads used by ShaferShenoyMNInference during inferences.

Returns
-------
int
	the number of threads used by ShaferShenoyMNInference during inferences
"

%feature("docstring") gum::ShaferShenoyMNInference::setNumberOfThreads
"
If the parameter n passed in argument is different from 0, ShaferShenoyMNInference will use n threads during inferences, hence overriding aGrUM's default number of threads.
If, on the contrary, n is equal to 0, the ShaferShenoyMNInference will comply with aGrUM's default number of threads.

Parameters
----------
n : int
	the number of threads to be used by ShaferShenoyMNInference
"

%feature("docstring") gum::ShaferShenoyMNInference::isGumNumberOfThreadsOverriden
"
Indicates whether ShaferShenoyMNInference currently overrides aGrUM's default number of threads (see method setNumberOfThreads).

Returns
-------
bool
	A Boolean indicating whether ShaferShenoyMNInference currently overrides aGrUM's default number of threads
"
