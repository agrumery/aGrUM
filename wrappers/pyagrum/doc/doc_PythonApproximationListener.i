/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2026 by                                             *
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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2026                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/







%feature("docstring") PythonApproximationListener
"
Listener of the progress of an approximation algorithm (approximate inference, credal inference, EM in
`pyagrum.BNLearner`, sampled distance between Bayesian networks...).

PythonApproximationListener(algo) -> PythonApproximationListener

Parameters
----------
algo : object
    the approximation algorithm to listen to, for instance a `pyagrum.GibbsSampling` (it must stay alive as long as
    the listener)

Notes
-----
    The progress is notified every ``periodSize()`` iterations, and only while the algorithm runs:
    an algorithm which stops before the end of its first period notifies only its stop.
"

%feature("docstring") PythonApproximationListener::setWhenProgress
"
Set the function called when the algorithm notifies its progress (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (step: int, error: float, duration: float) called when the algorithm has done ``step`` iterations, with the current error and the elapsed time in seconds

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"

%feature("docstring") PythonApproximationListener::setWhenStop
"
Set the function called when the algorithm stops (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (message: str) called when the algorithm stops, with the reason of the stop

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"
%feature("docstring") PythonApproximationListener::whenProgress
"
Called by aGrUM at each progress notification of the algorithm: calls the function given to ``setWhenProgress``.

Parameters
----------
src : object
    the object which sends the notification (not used)
step : int
    the number of iterations done
error : float
    the current error
duration : float
    the elapsed time in seconds
"

%feature("docstring") PythonApproximationListener::whenStop
"
Called by aGrUM when the algorithm stops: calls the function given to ``setWhenStop``.

Parameters
----------
src : object
    the object which sends the notification (not used)
message : str
    the reason of the stop
"
