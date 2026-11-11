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

%feature("docstring") gum::CPDAG
"
CPDAG represents a completed partially directed acyclic graph (or essential graph): the representation
of a Markov equivalence class of DAGs.

Two DAGs are Markov equivalent iff they have the same skeleton and the same v-structures (Verma and
Pearl 1990). The CPDAG of a class has an arc for each compelled arc (same orientation in all the DAGs
of the class) and an undirected edge for each reversible one. It is a chain graph whose chain
components are chordal (Andersson, Madigan and Perlman 1997).

A CPDAG cannot be modified (its properties are global): it is built from a DAG
(:meth:`CPDAG.fromDAG`, Chickering 1995) or from a PDAG with a check (:meth:`CPDAG.fromPDAG`).
:meth:`dSeparation` gives the d-separation of every DAG of the class (they are Markov equivalent);
it is computed on a DAG of the class kept from the construction (:meth:`consistentExtension`). It is
also the c-separation of the chain graph of the CPDAG (a DAG and its essential graph are Markov
equivalent, Andersson, Madigan and Perlman 1997, and the c-separation is complete, Studený and
Bouckaert 1998).
"

%feature("docstring") gum::CPDAG::fromDAG
"
The CPDAG of the Markov equivalence class of a DAG (algorithm Find-Compelled of Chickering 1995).

Parameters
----------
dag : pyagrum.DAG
  a DAG

Returns
-------
pyagrum.CPDAG
  the CPDAG of its Markov equivalence class
"

%feature("docstring") gum::CPDAG::fromPDAG
"
Checked conversion of a PDAG into a CPDAG: the PDAG must be the CPDAG of its consistent extensions.

Parameters
----------
pdag : pyagrum.PDAG
  a PDAG

Returns
-------
pyagrum.CPDAG

Raises
------
pyagrum.InvalidArgument
  If the PDAG is not a CPDAG.
"

%feature("docstring") gum::ImmutableGraph< gum::ChainGraph >::graph
"
Returns
-------
pyagrum.ChainGraph
  a copy of the chain graph of the CPDAG
"
%feature("docstring") gum::CPDAG::dSeparation
"
Check if the sets of nodes X and Y are d-separated (by the set of nodes Z if given) in every DAG
represented by the CPDAG.

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

%feature("docstring") gum::CPDAG::contains
"
Parameters
----------
dag : pyagrum.DAG
  a DAG

Returns
-------
bool
  True if the DAG is represented by the CPDAG.
"

%feature("docstring") gum::ImmutableMixedGraph< gum::ChainGraph >::toDot
"
The chain components (nodes linked by undirected edges) are drawn in clusters (color of the key
``dot.chainComponentBgColor`` of ``pyagrum.config``) and the compelled arcs with the color of the key
``dot.compelledArcColor``.

Parameters
----------
options : pyagrum.DotOptions, optional
    what is given to the drawing beyond the look of ``pyagrum.config`` (size, label, colors of some nodes and arcs...); a color given for an arc wins over the color of the compelled arcs.

Returns
-------
str
  a representation of the CPDAG in the DOT format
"

