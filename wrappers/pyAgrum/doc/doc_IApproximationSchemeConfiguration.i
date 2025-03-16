/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *                                                                          *
 *   The aGrUM/pyAgrum library is free software; you can redistribute it    *
 *   and/or modify it under the terms of either :                           *
 *                                                                          *
 *    - the GNU Lesser General Public License as published by               *
 *      the Free Software Foundation, either version 3 of the License,      *
 *      or (at your option) any later version,                              *
 *    - the MIT license (MIT),                                              *
 *    - or both in dual license, as here.                                   *
 *                                                                          *
 *   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    *
 *                                                                          *
 *   This aGrUM/pyAgrum library is distributed in the hope that it will be  *
 *   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          *
 *   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS *
 *   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   *
 *   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER *
 *   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        *
 *   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  *
 *   OTHER DEALINGS IN THE SOFTWARE.                                        *
 *                                                                          *
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




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
float
	get the current running time in second (float)
"

%feature("docstring") gum::IApproximationSchemeConfiguration::epsilon
"
Returns
-------
float
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
float
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
float
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
eps : float
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
tiemout : float
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
rate : float
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
