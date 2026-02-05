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







%feature("docstring") gum::GibbsBNdistance
"
Class representing a Gibbs-Approximated computation of divergence and distance between BNs


GibbsBNdistance(P,Q) -> GibbsBNdistance
    Parameters:
        - **P** (*pyagrum.BayesNet*) -- a Bayesian network
        - **Q** (*pyagrum.BayesNet*) -- another Bayesian network to compare with the first one

GibbsBNdistance(gbnd) -> GibbsBNdistance
    Parameters:
        - **gbnd** (*pyagrum.GibbsBNdistance*) -- the Gibbs BNdistance to copy

Raises
------
  pyagrum.OperationNotAllowed
	If the 2BNs have not the same domain size of compatible node sets
"

%feature("docstring") gum::GibbsBNdistance::nbrDrawnVar
"
Returns
-------
int
  the number of variable drawn at each iteration
"

%feature("docstring") gum::GibbsBNdistance::isDrawnAtRandom
"
Returns
-------
bool
  True if variables are drawn at random
"

%feature("docstring") gum::GibbsBNdistance::setDrawnAtRandom
"
Parameters
----------
_atRandom : bool
  indicates if variables should be drawn at random
"

%feature("docstring") gum::GibbsBNdistance::setNbrDrawnVar
"
Parameters
----------
_nbr : int
  the number of variables to be drawn at each iteration
"
