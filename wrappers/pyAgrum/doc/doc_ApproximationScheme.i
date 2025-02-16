/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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

%feature("docstring") gum::ApproximationScheme
"
Used to parametrize stopping criteria in approximate inference or learning algorithm.

ApproximationScheme(verbosity=False) -> ApproximationScheme
    Parameters:
      - **verbosity** (*bool*) -- to keep (or not) tracks of the learning process (history of epsilons)
"

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
error : float
"

%feature("docstring") gum::ApproximationScheme::updateApproximationScheme
"
Update the approximation scheme.
"
