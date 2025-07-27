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






%feature("docstring") gum::CliqueGraph
"
CliqueGraph represents a Clique Graph.

CliqueGraph() -> CliqueGraph
    default constructor

CliqueGraph(src) -> CliqueGraph
    Parameter
        - **src** (*pyagrum.CliqueGraph*) -- the CliqueGraph to copy
"

%feature("docstring") gum::CliqueGraph::addToClique
"
Change the set of nodes included into a given clique and returns the new set

Parameters
----------
clique_id : int
  the id of the clique
node_id : int
  the id of the node

Raises
------
  pyagrum.NotFound
  If clique_id does not exist
  pyagrum.DuplicateElement
  If clique_id set already contains the ndoe
"

%feature("docstring") gum::CliqueGraph::clearEdges
"
Remove all edges and their separators
"

%feature("docstring") gum::CliqueGraph::clique
"
Parameters
----------
idClique : int
  the id of the clique

Returns
-------
Set[int]
  The set of nodes included in the clique

Raises
------
pyagrum.NotFound
  If the clique does not belong to the clique graph
"

%feature("docstring") gum::CliqueGraph::separator
"
Parameters
----------
clique1 : int
  one extremity of the edge
clique : int
  the other extremity of the edge

Returns
-------
Set[int]
  the separator included in a given edge

Raises
------
pyagrum.NotFound
  If the edge does not belong to the clique graph
"

%feature("docstring") gum::CliqueGraph::container
"
Parameters
----------
idNode : int
  the id of the node

Returns
-------
int
  the id of a clique containing the node

Raises
------
pyagrum.NotFound
  If no clique contains idNode
"

%feature("docstring") gum::CliqueGraph::containerPath
"
Parameters
----------
node1 : int
  the id of one node
node2 : int
  the id of the other node

Returns
-------
List
  a path from a clique containing node1 to a clique containing node2

Raises
------
pyagrum.NotFound
  If such path cannot be found
"

%feature("docstring") gum::CliqueGraph::eraseFromClique
"
Remove a node from a clique

Parameters
----------
clique_id : int
  the id of the clique
node_id : int
  the id of the node

Raises
------
pyagrum.NotFound
  If clique_id does not exist
"

%feature("docstring") gum::CliqueGraph::hasRunningIntersection
"
Returns
-------
bool
  True if the running intersection property holds
"

%feature("docstring") gum::CliqueGraph::isJoinTree
"
Returns
-------
bool
  True if the graph is a join tree
"

%feature("docstring") gum::CliqueGraph::separator
"
Parameters
----------
edge : pyagrum.Edge
  the edge to be checked
clique1 : int
    one extremity of the edge
clique : int
  the other extremity of the edge

Returns
-------
Set[int]
  the separator included in a given edge

Raises
------
pyagrum.NotFound
  If the edge does not belong to the clique graph
"

%feature("docstring") gum::CliqueGraph::setClique
"
changes the set of nodes included into a given clique

Parameters
----------
idClique : int
  the id of the clique
new_clique : Set[int]
  the new set of nodes to be included in the clique

Raises
------
pyagrum.NotFound
  If idClique is not a clique of the graph
"
