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







%feature("docstring") gum::DiGraph
"
DiGraph represents a Directed Graph.

DiGraph() -> DiGraph
    default constructor

DiGraph(src) -> DiGraph
    Parameters:
        - **src** (*pyagrum.DiGraph*) -- the digraph to copy
"

%feature("docstring") gum::DiGraph::addArc
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
"

%feature("docstring") gum::DiGraph::addNode
"
Returns
-------
int
  the new NodeId
"

%feature("docstring") gum::DiGraph::addNodes
"
Add a set of n nodes.

Parameters
----------
n : int
  the number of nodes to add.

Returns
-------
Set of int
  the new ids
"

%feature("docstring") gum::DiGraph::addNodeWithId
"
Add a node by choosing a new NodeId.

Parameters
----------
id : int
  The id of the new node

Raises
------
  pyagrum.DuplicateElement
  If the given id is already used
"

%feature("docstring") gum::DiGraph::arcs
"
Returns the set of arcs in the graph.

Returns
-------
Set
    the set of the arcs
"

%feature("docstring") gum::DiGraph::children
"
Parameters
----------
id : int
  the id of the parent

Returns
-------
Set
    the set of all the children
"

%feature("docstring") gum::DiGraph::clear
"
Remove all the nodes and arcs from the graph.
"

%feature("docstring") gum::DiGraph::empty
"
Check if the graph is empty.

Returns
-------
bool
    True if the graph is empty
"

%feature("docstring") gum::DiGraph::emptyArcs
"
Check if the graph doesn't contains arcs.

Returns
-------
bool
    True if the graph doesn't contains arcs
"

%feature("docstring") gum::DiGraph::eraseArc
"
Erase the arc between n1 and n2.

Parameters
----------
n1 : int
    the id of the tail node
n2 : int
    the id of the head node
"
%feature("docstring") gum::DiGraph::eraseChildren
"
Erase the arcs heading through the node's children.

Parameters
----------
n : int
    the id of the parent node
"

%feature("docstring") gum::DiGraph::eraseNode
"
Erase the node and all the related arcs.

Parameters
----------
id : int
    the id of the node
"

%feature("docstring") gum::DiGraph::eraseParents
"
Erase the arcs coming to the node.

Parameters
----------
n : int
    the id of the child node
"

%feature("docstring") gum::DiGraph::existsArc
"
Check if an arc exists between n1 and n2.

Parameters
----------
n1 : int
    the id of the tail node
n2 : int
    the id of the head node

Returns
-------
bool
    True if the arc exists
"

%feature("docstring") gum::DiGraph::existsNode
"
Check if a node with a certain id exists in the graph.

Parameters
----------
id : int
    the checked id

Returns
-------
bool
    True if the node exists
"

%feature("docstring") gum::DiGraph::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::DiGraph::parents
"
Parameters
----------
id :
    The id of the child node

Returns
-------
Set
    the set of the parents ids.
"

%feature("docstring") gum::DiGraph::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::DiGraph::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::DiGraph::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::DiGraph::topologicalOrder
"
Returns
-------
List
    the list of the nodes Ids in a topological order

Raises
------
pyagrum.InvalidDirectedCycle
  If this graph contains cycles
"

%feature("docstring") gum::DiGraph::connectedComponents
"
Returns the weakly connected components of the graph.

Each node is mapped to the id of its component root (an arbitrarily chosen
node from the same component).

Returns
-------
dict[int, int]
    mapping node id → component root id

See Also
--------
connectedComponentsList : returns a dict[int, set[int]] grouping nodes by component
connectedComponentsCount : returns the number of components
"

%feature("docstring") gum::DiGraph::hasDirectedPath
"
Check if a directedpath exists between from and to.

Parameters
----------
from : int
    the id of the first node of the (possible) path
to : int
    the id of the last node of the (possible) path

Returns
-------
bool
    True if the directed path exists
"

%feature("docstring") gum::DiGraph::directedPath
"
Return a shortest directed path from node1 to node2, or None if no such path exists.

Parameters
----------
node1 : int
    id of the source node
node2 : int
    id of the destination node

Returns
-------
list[int] or None
    ordered list of node ids along the directed path, or None if node2 is unreachable from node1
"

%feature("docstring") gum::DiGraph::directedUnorientedPath
"
Return a shortest path from node1 to node2 ignoring arc orientation, or None if no path exists.

Parameters
----------
node1 : int
    id of the source node
node2 : int
    id of the destination node

Returns
-------
list[int] or None
    ordered list of node ids (arcs may be traversed in either direction), or None if unreachable
"

%feature("docstring") gum::DiGraph::family
"
Return the family of a node: the node itself plus all its parents.

Parameters
----------
norid : int
    id of the node

Returns
-------
set[int]
    {norid} ∪ parents(norid)
"

%feature("docstring") gum::DiGraph::completeGraph
"
Create a complete directed graph with n nodes.

Parameters
----------
n : int
    number of nodes

Returns
-------
DiGraph
    directed graph where every ordered pair (i, j) with i≠j has arc i→j, with nodes 0..n-1
"

%feature("docstring") gum::DiGraph::ancestors
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

%feature("docstring") gum::DiGraph::descendants
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

%feature("docstring") gum::DiGraph::nameFromId
"
Return the name of a node, or its id as a string if no name was set.

Parameters
----------
id : int
    the node id

Returns
-------
str
    the name associated with the node, or ``str(id)`` if the node has no name
"

%feature("docstring") gum::DiGraph::idFromName
"
Return the id of the node with the given name, or None if no such name exists.

Parameters
----------
name : str
    the name to look up

Returns
-------
int or None
    the node id, or None if the name is not found
"

%feature("docstring") gum::DiGraph::setName
"
Assign a name to a node.

If the node already has a name, it is replaced. The name must not already be
used by another node.

Parameters
----------
id : int
    the node id
name : str
    the name to assign

Raises
------
pyagrum.InvalidNode
    If the node does not exist.
pyagrum.DuplicateElement
    If the name is already used by a different node.
"

%feature("docstring") gum::DiGraph::hasName
"
Check whether a node has an explicitly assigned name.

Parameters
----------
id : int
    the node id

Returns
-------
bool
    True if a name has been assigned to this node via setName
"
