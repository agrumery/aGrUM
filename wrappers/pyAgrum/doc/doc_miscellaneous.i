%feature("docstring") gum::initRandom
"
Initialize random generator seed.

Parameters
----------
seed : int
  the seed used to initialize the random generator
"

%feature("docstring") gum::randomProba
"
Returns
-------
double
    a random number between 0 and 1 included (i.e. a proba). 
"

%feature("docstring") gum::randomDistribution
"
Parameters
----------
n : int
  The number of modalities for the ditribution.

Returns
-------
a random discrete distribution. 
"

%feature("docstring") gum::isOMP
"
Returns
-------
bool
  True if OMP has been set at compilation, False otherwise
"

%feature("docstring") gum::setNumberOfThreads
"
To avoid spare cycles (less then 100% CPU occupied), use more threads than logical processors (x2 is a good all-around value). 

Returns
-------
number : int
  the number of threads to be used
"

%feature("docstring") gum::getNumberOfLogicalProcessors
"
Returns
-------
int
  the number of logical processors
"

%feature("docstring") gum::getMaxNumberOfThreads
"
Returns
-------
int
  the max number of threads
"
