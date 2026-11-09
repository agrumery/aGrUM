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


%feature("docstring") gum::MarkedGraph
"
MarkedGraph represents a graph whose edges carry a mark at each of their ends.

Each edge between nodes x and y has two marks, one at each end (``pyagrum.EdgeMark``):

- ``EdgeMark.Circle`` (o): undetermined end
- ``EdgeMark.Tail`` (-): tail
- ``EdgeMark.Arrowhead`` (>): arrowhead

There is at most one edge between two nodes: ``x->y`` (tail at x, arrowhead at y), ``x<->y``, ``x--y``,
``x o-> y``, ``x o-o y``... A marked graph is not an undirected graph: its skeleton is given by ``skeleton()``
and its directed view by ``parents``, ``children``, ``spouses`` and ``undirectedNeighbours``.

MarkedGraph() -> MarkedGraph
    default constructor

MarkedGraph(src) -> MarkedGraph
    Parameters:
        - **src** (*pyagrum.MarkedGraph*) -- the marked graph to copy

See also ``pyagrum.fastMarkedGraph``, ``pyagrum.PAG`` and ``pyagrum.MAG``.

References
----------
T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics 30(4):962-1030, 2002.

J. Zhang. Causal reasoning with ancestral graphs. Journal of Machine Learning Research 9:1437-1474, 2008.

J. Zhang. On the completeness of orientation rules for causal discovery in the presence of latent confounders and
selection bias. Artificial Intelligence 172(16-17):1873-1896, 2008.
"

%feature("docstring") gum::MarkedGraph::addEdge
"
Add an edge between two nodes with specified endpoint marks.

Signatures:

  ``addEdge(x, y)``
      Adds a Circle-Circle edge (both endpoints uncertain).

  ``addEdge(x, y, markAtX, markAtY)``
      Adds an edge with explicit marks. ``markAtX`` is the mark on x's side
      (visible from y), ``markAtY`` is the mark on y's side (visible from x).

Parameters
----------
x : int
    id of the first node
y : int
    id of the second node
markAtX : pyagrum.EdgeMark, optional
    mark at the x endpoint (``pyagrum.EdgeMark.Circle``, ``.Tail`` or ``.Arrowhead``)
markAtY : pyagrum.EdgeMark, optional
    mark at the y endpoint (``pyagrum.EdgeMark.Circle``, ``.Tail`` or ``.Arrowhead``)
"


%feature("docstring") gum::MarkedGraph::markAt
"
Return the endpoint mark on the dst side of edge (src, dst).

The mark at dst indicates what the edge says about dst from src's perspective:

Parameters
----------
src : int
    id of the source node
dst : int
    id of the destination node

Returns
-------
pyagrum.EdgeMark
    the mark (``pyagrum.EdgeMark.Circle``, ``.Tail`` or ``.Arrowhead``)
"


%feature("docstring") gum::MarkedGraph::setMarkAt
"
Set the endpoint mark on the dst side of edge (src, dst).

Parameters
----------
src : int
    id of the source node
dst : int
    id of the destination node
mark : pyagrum.EdgeMark
    the new mark
"


%feature("docstring") gum::MarkedGraph::isArrowhead
"
Return True if the mark at dst (seen from src) is an arrowhead.

Parameters
----------
src : int
dst : int

Returns
-------
bool
"


%feature("docstring") gum::MarkedGraph::isTail
"
Return True if the mark at dst (seen from src) is a tail.

Parameters
----------
src : int
dst : int

Returns
-------
bool
"


%feature("docstring") gum::MarkedGraph::isCircle
"
Return True if the mark at dst (seen from src) is a circle.

Parameters
----------
src : int
dst : int

Returns
-------
bool
"


%feature("docstring") gum::MarkedGraph::isDefinitelyDirected
"
Return True if the edge x-y is definitely directed from x to y.

An edge is definitely directed from x to y when x has a tail and y has an arrowhead.

Parameters
----------
x : int
y : int

Returns
-------
bool
"


%feature("docstring") gum::MarkedGraph::isBidirected
"
Return True if both endpoints of edge (x, y) are arrowheads (bidirected edge).

Parameters
----------
x : int
y : int

Returns
-------
bool
"


%feature("docstring") gum::MarkedGraph::isDefCollider
"
Return True if z is a definite collider on the path x-z-y.

z is a definite collider when both marks at z (from x and from y) are arrowheads.

Parameters
----------
x : int
z : int
y : int

Returns
-------
bool
"


%feature("docstring") gum::MarkedGraph::reorientAllWith
"
Set all endpoint marks in the graph to the given mark.

Parameters
----------
mark : pyagrum.EdgeMark
    the mark to set everywhere (``pyagrum.EdgeMark.Circle``, ``Tail`` or ``Arrowhead``)
"


%feature("docstring") gum::MarkedGraph::toDot
"
Return a Graphviz dot representation of the graph.

Endpoint marks are rendered as:
- Circle: ``odot``
- Tail: ``none``
- Arrowhead: ``normal``

Parameters
----------
options : pyagrum.DotOptions, optional
    what is given to the drawing beyond the look of ``pyagrum.config`` (size, label, colors of some nodes, width of some arcs...).

Returns
-------
str
    dot-format string
"


%feature("docstring") gum::MarkedGraph::addNode
"
Add a new node to the graph and return its id.

Returns
-------
int
    the id of the new node
"


%feature("docstring") gum::MarkedGraph::addNodeWithId
"
Add a node with a chosen id.

Parameters
----------
id : int
    the id of the new node

Raises
------
pyagrum.DuplicateElement
    if the given id is already used
"


%feature("docstring") gum::MarkedGraph::addNodes
"
Add n new nodes to the graph.

Parameters
----------
n : int
    number of nodes to add

Returns
-------
set of int
    the ids of the new nodes
"


%feature("docstring") gum::MarkedGraph::clearEdges
"
Remove all edges and their endpoint marks from the graph.

Nodes are preserved; only edges and their associated marks are removed.
"


%feature("docstring") gum::MarkedGraph::empty
"
Check whether the graph has no nodes.

Returns
-------
bool
    True if the graph contains no nodes
"


%feature("docstring") gum::MarkedGraph::emptyEdges
"
Check whether the graph has no edges.

Returns
-------
bool
    True if the graph contains no edges
"


%feature("docstring") gum::MarkedGraph::eraseNeighbours
"
Remove all edges adjacent to a given node (and their marks).

Parameters
----------
n : int
    id of the node
"


%feature("docstring") gum::MarkedGraph::existsEdge
"
Check whether an edge exists between two nodes.

Parameters
----------
n1 : int
    id of one endpoint
n2 : int
    id of the other endpoint

Returns
-------
bool
    True if the edge exists
"


%feature("docstring") gum::MarkedGraph::existsNode
"
Check whether a node with the given id exists in the graph.

Parameters
----------
id : int
    the id to check

Returns
-------
bool
    True if the node exists
"


%feature("docstring") gum::MarkedGraph::size
"
Return the number of nodes in the graph.

Returns
-------
int
    number of nodes
"


%feature("docstring") gum::MarkedGraph::sizeEdges
"
Return the number of edges in the graph.

Returns
-------
int
    number of edges
"
%feature("docstring") gum::MarkedGraph::parents
"
Parameters
----------
x : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that y->x
"

%feature("docstring") gum::MarkedGraph::children
"
Parameters
----------
x : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that x->y
"

%feature("docstring") gum::MarkedGraph::spouses
"
Parameters
----------
x : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that x<->y
"

%feature("docstring") gum::MarkedGraph::undirectedNeighbours
"
Parameters
----------
x : int
    the id of a node

Returns
-------
set[int]
    the nodes y such that x--y
"

%feature("docstring") gum::MarkedGraph::isUndirected
"
Return True if the edge x-y has a tail at both ends (x--y).

Parameters
----------
x : int
y : int

Returns
-------
bool
"

%feature("docstring") gum::MarkedGraph::skeleton
"
Returns
-------
pyagrum.UndiGraph
    the undirected graph of the adjacencies, with the names of the nodes
"

%feature("docstring") gum::MarkedGraph::connectedComponents
"
Returns
-------
dict[int, int]
    each node is mapped to the root of its connected component (in the skeleton)
"

%feature("docstring") gum::MarkedGraph::toString
"
Returns
-------
str
    a representation of the marked graph (e.g. ``0 o-> 1``)
"

%feature("docstring") gum::MarkedGraph::eraseEdge
"
Erase the edge (and its marks) if it exists.

Parameters
----------
n1 : int
n2 : int
"

%feature("docstring") gum::MarkedGraph::clear
"
Remove all the nodes and edges from the graph.
"

%feature("docstring") gum::MarkedGraph::clearNodes
"
Remove all the nodes, hence all the edges and their marks, from the graph (same as :meth:`clear`).
"

%feature("docstring") gum::MarkedGraph::edges
"
Returns
-------
List
  the list of the edges
"

%feature("docstring") gum::MarkedGraph::eraseNode
"
Erase the node and all the adjacent edges.

Parameters
----------
id : int
  the id of the node
"

%feature("docstring") gum::MarkedGraph::hasName
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

%feature("docstring") gum::MarkedGraph::idFromName
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

%feature("docstring") gum::MarkedGraph::nameFromId
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

%feature("docstring") gum::MarkedGraph::neighbours
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

%feature("docstring") gum::MarkedGraph::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::MarkedGraph::setName
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

