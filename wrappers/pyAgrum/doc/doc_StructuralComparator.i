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




%feature("docstring") gum::StructuralComparator::compare
"
Use to compare the edges/arcs of two structure of the same type and same sizes (either DiGraph, UndiGraph or MixedGraph).

Could be use to compare a BN and its learned version.

Parameters
----------
ref :
	the structure of reference
test :
	the structure we want to test
"

%feature("docstring") gum::StructuralComparator::precision_skeleton
"
Rate of true postive over labelized edges.

Returns
-------
float
	the precision of the tested graph skeleton
"

%feature("docstring") gum::StructuralComparator::recall_skeleton
"
Rate of true postive over labelized edges.

Returns
-------
float
	the recall of the tested graph skeleton
"

%feature("docstring") gum::StructuralComparator::f_score_skeleton
"
Harmonic mean between recall and precision.

Returns
-------
float
	the tarmonic mean of the tested graph skeleton
"

%feature("docstring") gum::StructuralComparator::precision
"
Rate of true postive over postively labelized arcs/edges.

Returns
-------
float
	the precision of the tested graph
"

%feature("docstring") gum::StructuralComparator::recall
"
Rate of true postive over labelized arcs/edges.

Returns
-------
float
	the recall of the tested graph
"

%feature("docstring") gum::StructuralComparator::f_score
"
Harmonic mean between recall and precision.

Returns
-------
float
	the harmonic mean of the tested graph
"
