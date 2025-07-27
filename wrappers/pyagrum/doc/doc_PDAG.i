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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/






%feature("docstring") gum::PDAG
"
PDAG represents a graph with both arcs and edges.

PDAG() -> PDAG
    default constructor

PDAG(src) -> PDAG
    Parameters:
        - **src** (*pyagrum.PDAG*) --the PDAG to copy
"
%feature("docstring") gum::PDAG::addArc
"
Add an arc from tail to head.

Parameters
----------
tail : int
  the id of the tail node
head : int
  the id of the head node

Raises
------
  pyagrum.InvalidNode
    If head or tail does not belong to the graph nodes.

  PyAgrum.InvalidDirectedCycle
    if the arc would create a (mixed) cycle.
"


%feature("docstring") gum::PDAG::mixedOrientedPath
"
Parameters
----------
node1 : int
	the id form which the path begins
node2 : int
	the id to witch the path ends

Returns
-------
List
	 a path from node1 to node2, using edges and/or arcs (following the direction of the arcs). If no path is found, the returned list is empty.
"

%feature("docstring") gum::PDAG::mixedUnorientedPath
"
Parameters
----------
node1 : int
	the id from which the path begins
node2 : int
	the id to which the path ends

Returns
-------
List
	 a path from node1 to node2, using edges and/or arcs (not necessarily following the direction of the arcs). If no path is found, the list is empty.

"


%feature("docstring") gum::PDAG::addEdge
"
Insert a new edge into the graph.

Parameters
----------
n1 : int
  the id of one node of the new inserted edge
n2 : int
  the id of the other node of the new inserted edge

Raises
------
  pyagrum.InvalidNode
    If n1 or n2 does not belong to the graph nodes.
"


%feature("docstring") gum::PDAG::cSeparation
"
Check if the sets of nodes X and Y are c-separated (by the set of nodes Z if given) in the PDAG.

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
  True if X and Y are c-separated (by Z if given), False otherwise.
"


%feature("docstring") gum::PDAG::moralizedAncestralGraph
"
Compute the moralized ancestral graph of the nodes from the DAG.

Parameters
----------
nodes : int | sequence of int
  a sequence of node ids (int) or a single node id (int)
Returns
------- 
gum::UndiGraph
  the moralized ancestral graph of the nodes from the DAG.
"