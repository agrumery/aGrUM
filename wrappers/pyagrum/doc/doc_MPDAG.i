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

%feature("docstring") gum::MPDAG
"
MPDAG represents a maximally oriented PDAG: a CPDAG refined by background knowledge.

A PDAG represents the DAGs with its skeleton, its v-structures and containing its arcs (its consistent
extensions). Its maximal orientation (MPDAG) orients every edge that has the same orientation in all
these DAGs: it is the closure of the PDAG under the Meek rules (Meek 1995). A CPDAG with some imposed
arcs gives an MPDAG (Perković, Kalisch and Maathuis 2017). An MPDAG is not always a chain graph.

An MPDAG cannot be modified: it is built by :meth:`MPDAG.meekClosure`, :meth:`MPDAG.fromCPDAG` or
checked by :meth:`MPDAG.fromPDAG`. The represented DAGs are Markov equivalent: :meth:`dSeparation` is
the d-separation of any of them.

References
----------
C. Meek. Causal Inference and Causal Explanation with Background Knowledge. Proceedings of the 11th
Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 403-410. arXiv:1302.4972.

E. Perković, M. Kalisch, M. H. Maathuis. Interpreting and using CPDAGs with background knowledge.
Proceedings of the 33rd Conference on Uncertainty in Artificial Intelligence (UAI 2017), PMLR
R15:461-470. arXiv:1707.02171.

M. Wienöbst, M. Bannach, M. Liśkiewicz. Extendability of causal graphical models: Algorithms and
computational complexity. Proceedings of the 37th Conference on Uncertainty in Artificial
Intelligence, PMLR 161:1248-1257, 2021.
"

%feature("docstring") gum::MPDAG::meekClosure
"
The maximal orientation of an extendable PDAG (its closure under the Meek rules).

Parameters
----------
pdag : pyagrum.PDAG
  an extendable PDAG

Returns
-------
pyagrum.MPDAG

Raises
------
pyagrum.InvalidArgument
  If the PDAG has no consistent extension.
"

%feature("docstring") gum::MPDAG::fromPDAG
"
Checked conversion of a PDAG into an MPDAG: the PDAG must be extendable and closed under the Meek
rules.

Parameters
----------
pdag : pyagrum.PDAG
  a PDAG

Returns
-------
pyagrum.MPDAG

Raises
------
pyagrum.InvalidArgument
  If the PDAG is not an MPDAG.
"

%feature("docstring") gum::MPDAG::cpdag
"
Returns
-------
pyagrum.CPDAG
  the CPDAG of the Markov equivalence class of the represented DAGs
"

%feature("docstring") gum::ImmutableGraph< gum::PDAG >::graph
"
Returns
-------
pyagrum.PDAG
  a copy of the PDAG of the MPDAG
"
%feature("docstring") gum::MPDAG::dSeparation
"
Check if the sets of nodes X and Y are d-separated (by the set of nodes Z if given) in every DAG
represented by the MPDAG.

Parameters
----------
X : int | sequence of int
  a sequence of node ids (int) or a single node id (int)
Y : int | sequence of int
  a sequence of node ids (int) or a single node id (int)
Z : int | sequence of int (optional)
  a sequence of node ids (int) or a single node id (int)

Returns
-------
bool
  True if X and Y are d-separated (by Z if given), False otherwise.
"

%feature("docstring") gum::MPDAG::contains
"
Parameters
----------
dag : pyagrum.DAG
  a DAG

Returns
-------
bool
  True if the DAG is represented by the MPDAG.
"

%feature("docstring") gum::ImmutableMixedGraph< gum::PDAG >::toDot
"
The compelled arcs are drawn with the color of the key ``dot.compelledArcColor`` of ``pyagrum.config``. No
cluster is drawn: an MPDAG is not always a chain graph.

Parameters
----------
options : pyagrum.DotOptions, optional
    what is given to the drawing beyond the look of ``pyagrum.config`` (size, label, colors of some nodes and arcs...); a color given for an arc wins over the color of the compelled arcs.

Returns
-------
str
  a representation of the MPDAG in the DOT format
"

