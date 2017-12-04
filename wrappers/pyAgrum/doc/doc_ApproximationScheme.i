%feature("docstring") gum::ApproximationScheme::disableEpsilon
"
Disable epsilon as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::enableEpsilon
"
Enable epsilon as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::isEnabledEpsilon
"
Returns
-------
bool
  True if epsilon is used as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::disableMinEpsilonRate
"
Disable a min epsilon rate as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::enableMinEpsilonRate
"
Enable a min epsilon rate as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::isEnabledMinEpsilonRate
"
Returns
-------
bool
  True if epsilon rate is used as a stopping criterion
"

%feature("docstring") gum::ApproximationScheme::disableMaxIter
"
Disable max iterations as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::enableMaxIter
"
Enable max iterations as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::isEnabledMaxIter
"
Returns
-------
bool
  True if max iterations is used as a stopping criterion
"

%feature("docstring") gum::ApproximationScheme::disableMaxTime
"
Disable max time as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::enableMaxTime
"
Enable max time as a stopping criterion.
"

%feature("docstring") gum::ApproximationScheme::isEnabledMaxTime
"
Returns
-------
bool
  True if max time is used as a stopping criterion
"

%feature("docstring") gum::ApproximationScheme::stateApproximationScheme
"
Returns
-------
int
  the state of the approximation scheme
"

%feature("docstring") gum::ApproximationScheme::initApproximationScheme
"
Initiate the approximation scheme.
"

%feature("docstring") gum::ApproximationScheme::startOfPeriod
"
Returns
-------
bool
  True if it is a start of a period
"

%feature("docstring") gum::ApproximationScheme::remainingBurnIn
"
Returns
-------
int
  the number of remaining burn in
"

%feature("docstring") gum::ApproximationScheme::stopApproximationScheme
"
Stop the approximation scheme.
"

%feature("docstring") gum::ApproximationScheme::continueApproximationScheme
"
Continue the approximation scheme.

Parameters
----------
error : double
"

%feature("docstring") gum::ApproximationScheme::updateApproximationScheme
"
Update the approximation scheme.
"