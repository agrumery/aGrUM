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







%feature("docstring") gum::UndiGraph
"
UndiGraph represents an Undirected Graph.

UndiGraph() -> UndiGraph
    default constructor

UndiGraph(src) -> UndiGraph
    Parameters!
        - **src** (*UndiGraph*) -- the pyagrum.UndiGraph to copy

"

%feature("docstring") gum::UndiGraph::addEdge
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

%feature("docstring") gum::UndiGraph::addNode
"
Returns
-------
int
  the new NodeId
"

%feature("docstring") gum::UndiGraph::addNodes
"
Add n nodes.

Parameters
----------
n : int
  the number of nodes to add.

Returns
-------
Set of int
  the new ids
"

%feature("docstring") gum::UndiGraph::addNodeWithId
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

%feature("docstring") gum::UndiGraph::clear
"
Remove all the nodes and edges from the graph.
"

%feature("docstring") gum::UndiGraph::edges
"
Returns
-------
List
  the list of the edges
"

%feature("docstring") gum::UndiGraph::empty
"
Check if the graph is empty.

Returns
-------
bool
    True if the graph is empty
"

%feature("docstring") gum::UndiGraph::emptyEdges
"
Check if the graph doesn't contains edges.

Returns
-------
bool
    True if the graph doesn't contains edges
"

%feature("docstring") gum::UndiGraph::eraseEdge
"
Erase the edge between n1 and n2.

Parameters
----------
n1 : int
  the id of the tail node
n2 : int
  the id of the head node
"

%feature("docstring") gum::UndiGraph::eraseNeighbours
"
Erase all the edges adjacent to a given node.

Parameters
----------
n : int
  the id of the node
"

%feature("docstring") gum::UndiGraph::eraseNode
"
Erase the node and all the adjacent edges.

Parameters
----------
id : int
  the id of the node
"

%feature("docstring") gum::UndiGraph::existsEdge
"
Check if an edge exists between n1 and n2.

Parameters
----------
n1 : int
  the id of one extremity of the edge
n2 : int
  the id of the other extremity if tge edge

Returns
-------
bool
    True if the arc exists
"

%feature("docstring") gum::UndiGraph::existsNode
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

%feature("docstring") gum::UndiGraph::hasUndirectedCycle
"
Checks whether the graph contains cycles.

Returns
-------
bool
    True if the graph contains a cycle
"

%feature("docstring") gum::UndiGraph::nodes
"
Returns
-------
set
    the set of ids
"


%feature("docstring") gum::UndiGraph::neighbours
"
Parameters
----------
id : int
    the id of the checked node

Returns
-------
Set
    The set of edges adjacent to the given node
"

%feature("docstring") gum::UndiGraph::partialUndiGraph
"
Parameters
----------
nodesSet : Set
    The set of nodes composing the partial graph

Returns
-------
pyagrum.UndiGraph
    The partial graph formed by the nodes given in parameter
"

%feature("docstring") gum::UndiGraph::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::UndiGraph::sizeEdges
"
Returns
-------
int
    the number of edges in the graph
"

%feature("docstring") gum::UndiGraph::connectedComponents
"
Returns the connected components of the graph.

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

%feature("docstring") gum::UndiGraph::undirectedPath
"
Return a shortest undirected path between two nodes, or None if no path exists.

Parameters
----------
node1 : int
    id of the first node
node2 : int
    id of the second node

Returns
-------
list[int] or None
    ordered list of node ids along the path, or None if the nodes are disconnected
"

%feature("docstring") gum::UndiGraph::hasUndirectedPath
"
Check whether two nodes are connected by an undirected path.

Parameters
----------
n1 : int
    id of the first node
n2 : int
    id of the second node

Returns
-------
bool
    True if a path exists between n1 and n2
"

%feature("docstring") gum::UndiGraph::chainComponents
"
Return the chain components (connected components) of the graph.

Each node is mapped to the id of its component root (an arbitrarily chosen node from the same component).

Returns
-------
dict[int, int]
    mapping node id → component root id
"

%feature("docstring") gum::UndiGraph::completeGraph
"
Create a complete undirected graph with n nodes.

Parameters
----------
n : int
    number of nodes

Returns
-------
UndiGraph
    graph where every pair of distinct nodes is connected by an edge, with nodes 0..n-1
"

%feature("docstring") gum::UndiGraph::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::UndiGraph::nameFromId
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

%feature("docstring") gum::UndiGraph::idFromName
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

%feature("docstring") gum::UndiGraph::setName
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

%feature("docstring") gum::UndiGraph::hasName
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
