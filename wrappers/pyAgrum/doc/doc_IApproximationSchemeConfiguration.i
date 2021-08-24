%feature("docstring") gum::IApproximationSchemeConfiguration::burnIn
"
Returns
-------
int
	size of burn in on number of iteration
"

%feature("docstring") gum::IApproximationSchemeConfiguration::currentTime
"
Returns
-------
double
	get the current running time in second (double)
"

%feature("docstring") gum::IApproximationSchemeConfiguration::epsilon
"
Returns
-------
double
	the value of epsilon
"

%feature("docstring") gum::IApproximationSchemeConfiguration::history
"
Returns
-------
tuple
	the scheme history

Raises
------
pyAgrum.OperationNotAllowed
	If the scheme did not performed or if verbosity is set to false
"

%feature("docstring") gum::IApproximationSchemeConfiguration::maxIter
"
Returns
-------
int
	the criterion on number of iterations
"

%feature("docstring") gum::IApproximationSchemeConfiguration::maxTime
"
Returns
-------
double
	the timeout(in seconds)
"

%feature("docstring") gum::IApproximationSchemeConfiguration::messageApproximationScheme
"
Returns
-------
str
	the approximation scheme message
"

%feature("docstring") gum::IApproximationSchemeConfiguration::minEpsilonRate
"
Returns
-------
double
	the value of the minimal epsilon rate
"

%feature("docstring") gum::IApproximationSchemeConfiguration::nbrIterations
"
Returns
-------
int
	the number of iterations
"

%feature("docstring") gum::IApproximationSchemeConfiguration::periodSize
"
Returns
-------
int
	the number of samples between 2 stopping

Raises
------
pyAgrum.OutOfBounds
	If p<1
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setBurnIn
"
Parameters
----------
b : int
	size of burn in on number of iteration
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setEpsilon
"
Parameters
----------
eps : double
	the epsilon we want to use

Raises
------
pyAgrum.OutOfBounds
	If eps<0
"


%feature("docstring") gum::IApproximationSchemeConfiguration::setMaxIter
"
Parameters
----------
max : int
	the maximum number of iteration

Raises
------
pyAgrum.OutOfBounds
	If max <= 1
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setMaxTime
"
Parameters
----------
tiemout : double
	stopping criterion on timeout (in seconds)

Raises
------
pyAgrum.OutOfBounds
	If timeout<=0.0
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setMinEpsilonRate
"
Parameters
----------
rate : double
	the minimal epsilon rate
"

%feature("docstring") gum::IApproximationSchemeConfiguration::setPeriodSize
"
Parameters
----------
p : int
	number of samples between 2 stopping

Raises
------
pyAgrum.OutOfBounds
	If p<1
"


%feature("docstring") gum::IApproximationSchemeConfiguration::setVerbosity
"
Parameters
----------
v : bool
        verbosity
"

%feature("docstring") gum::IApproximationSchemeConfiguration::verbosity
"
Returns
-------
bool
	True if the verbosity is enabled
"
