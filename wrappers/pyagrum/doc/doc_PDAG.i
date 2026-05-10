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


%feature("docstring") gum::PDAG::ancestors
"
give the set of nodeid of ancestors of a node

Parameters
----------
norid : str|int
  the name or the id of the node

Returns
-------
set
  the set of ids of the ancestors of node `norid`.
"

%feature("docstring") gum::PDAG::descendants
"
give the set of nodeid of descendants of a node

Parameters
----------
norid : str|int
  the name or the id of the node

Returns
-------
set
  the set of ids of the descendants of node `norid`.
"

%feature("docstring") gum::PDAG::moralGraph
"
Returns the moral graph of the PDAG, formed by adding edges between all pairs of nodes that have a common child, and then making all edges in the graph undirected.

Returns
-------
pyagrum.UndiGraph
    The moral graph
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
pyagrum.UndiGraph
  the moralized ancestral graph of the nodes from the DAG.
"
%feature("docstring") gum::PDAG::addNode
"
Add a new node to the graph and return its id.

Returns
-------
int
    the id of the new node
"

%feature("docstring") gum::PDAG::addNodeWithId
"
Add a node with a specific id.

Parameters
----------
id : int
    the id of the new node

Raises
------
pyagrum.DuplicateElement
    if a node with this id already exists
"

%feature("docstring") gum::PDAG::addNodes
"
Add n new nodes to the graph.

Parameters
----------
n : int
    the number of nodes to add
"

%feature("docstring") gum::PDAG::arcs
"
Returns the set of arcs in the graph.

Returns
-------
set
    the set of the arcs
"

%feature("docstring") gum::PDAG::boundary
"
Boundary of a node: neighbours (via edges), children, and parents.

Parameters
----------
id : int
    the id of the node

Returns
-------
set
    the set of adjacent node ids
"

%feature("docstring") gum::PDAG::children
"
Parameters
----------
id : int
    the id of the parent node

Returns
-------
Set
    the set of all children ids
"

%feature("docstring") gum::PDAG::edges
"
Returns
-------
List
    the list of the edges
"

%feature("docstring") gum::PDAG::empty
"
Check if the graph has no nodes.

Returns
-------
bool
    True if there are no nodes in the graph
"

%feature("docstring") gum::PDAG::emptyArcs
"
Check if the graph has no arcs.

Returns
-------
bool
    True if the graph contains no arcs
"

%feature("docstring") gum::PDAG::emptyEdges
"
Check if the graph has no edges.

Returns
-------
bool
    True if the graph contains no edges
"

%feature("docstring") gum::PDAG::eraseArc
"
Remove an arc from the graph.

Parameters
----------
tail : int
    the id of the tail node
head : int
    the id of the head node

Raises
------
pyagrum.InvalidArc
    if the arc does not exist
"

%feature("docstring") gum::PDAG::eraseChildren
"
Erase all arcs from a node to its children.

Parameters
----------
id : int
    the id of the node
"

%feature("docstring") gum::PDAG::eraseEdge
"
Remove an edge from the graph.

Parameters
----------
n1 : int
    one endpoint of the edge
n2 : int
    the other endpoint

Raises
------
pyagrum.InvalidEdge
    if the edge does not exist
"

%feature("docstring") gum::PDAG::eraseNeighbours
"
Erase all edges adjacent to a given node.

Parameters
----------
id : int
    the id of the node
"

%feature("docstring") gum::PDAG::eraseParents
"
Erase all arcs incoming to a node from its parents.

Parameters
----------
id : int
    the id of the node
"

%feature("docstring") gum::PDAG::existsArc
"
Check whether an arc exists between two nodes.

Parameters
----------
tail : int
    the id of the tail node
head : int
    the id of the head node

Returns
-------
bool
    True if the arc (tail, head) exists
"

%feature("docstring") gum::PDAG::existsEdge
"
Check whether an edge exists between two nodes.

Parameters
----------
n1 : int
    one endpoint
n2 : int
    the other endpoint

Returns
-------
bool
    True if the edge exists
"

%feature("docstring") gum::PDAG::existsNode
"
Check whether a node exists.

Parameters
----------
id : int
    the node id to check

Returns
-------
bool
    True if the node exists
"

%feature("docstring") gum::PDAG::hasMixedReallyOrientedPath
"
Check if there is a strictly oriented path from node1 to node2 (all arcs, no edges).

Parameters
----------
node1 : int
    the start node id
node2 : int
    the end node id

Returns
-------
bool
    True if such a path exists
"

%feature("docstring") gum::PDAG::neighbours
"
Parameters
----------
id : int
    the id of the checked node

Returns
-------
Set
    the set of node ids linked by an edge to the given node
"

%feature("docstring") gum::PDAG::parents
"
Parameters
----------
id : int
    the id of the child node

Returns
-------
Set
    the set of parent node ids
"

%feature("docstring") gum::PDAG::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::PDAG::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::PDAG::sizeEdges
"
Returns
-------
int
    the number of edges in the graph
"
