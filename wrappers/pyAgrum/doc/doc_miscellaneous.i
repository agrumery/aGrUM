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

%feature("docstring") gum::randomGeneratorSeed
"
Returns
-------
int
  a randomly generated seed
"

%feature("docstring") gum::getNumberOfRunningThreads
"
Returns
-------
int
  the number of running threads
"

%feature("docstring") gum::getNestedParallelism
"
Returns
-------
bool
  True if nested parallelism is enabled
"

%feature("docstring") gum::setNestedParallelism
"
Parameters
----------
value : bool
  True if nested parallelism should be activated
"

%feature("docstring") gum::getDynamicThreadsNumber
"
Returns
-------
bool
  True if dynamic threads number is enabled
"

%feature("docstring") gum::setDynamicThreadsNumber
"
Parameters
----------
value : bool
  True if the number of threads should be dynamic
"

%feature("docstring") gum::getRandomGenerator
"
Returns
-------
tbw
  the random generator
"

%feature("docstring") gum::getThreadNumber
"
Returns
-------
int
  the number of thread
"

%feature("docstring") gum::randomValue
"
Returns
-------
int
  a value randomly drawn (0 or 1)
"