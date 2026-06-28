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


%feature("docstring") gum::PAG
"
PAG represents a Partial Ancestral Graph, the output of the FCI algorithm.

A PAG is an undirected graph whose edges carry endpoint marks. Each edge
between nodes x and y has two marks, one at each endpoint:

- ``EdgeMark_Circle`` (o): uncertain endpoint
- ``EdgeMark_Tail`` (-): definite non-ancestor (tail)
- ``EdgeMark_Arrowhead`` (>): definite ancestor (arrowhead)

PAG() -> PAG
    default constructor

PAG(src) -> PAG
    Parameters:
        - **src** (*pyagrum.PAG*) -- the PAG to copy

Typically obtained via ``BNLearner.learnPAG()`` after calling ``useFCI()``.
"


%feature("docstring") gum::PAG::addEdge
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
markAtX : int, optional
    mark at the x endpoint (``EdgeMark_Circle``, ``EdgeMark_Tail``, or ``EdgeMark_Arrowhead``)
markAtY : int, optional
    mark at the y endpoint (``EdgeMark_Circle``, ``EdgeMark_Tail``, or ``EdgeMark_Arrowhead``)
"


%feature("docstring") gum::PAG::markAt
"
Return the endpoint mark on the dst side of edge (src, dst).

The mark at dst indicates what the edge says about dst from src's perspective:
``marks_[Arc(src,dst)]`` = mark at the dst endpoint.

Parameters
----------
src : int
    id of the source node
dst : int
    id of the destination node

Returns
-------
int
    ``EdgeMark_Circle`` (0), ``EdgeMark_Tail`` (1), or ``EdgeMark_Arrowhead`` (2)
"


%feature("docstring") gum::PAG::setMarkAt
"
Set the endpoint mark on the dst side of edge (src, dst).

Parameters
----------
src : int
    id of the source node
dst : int
    id of the destination node
m : int
    new mark: ``EdgeMark_Circle`` (0), ``EdgeMark_Tail`` (1), or ``EdgeMark_Arrowhead`` (2)
"


%feature("docstring") gum::PAG::isArrowhead
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


%feature("docstring") gum::PAG::isTail
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


%feature("docstring") gum::PAG::isCircle
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


%feature("docstring") gum::PAG::isDefinitelyDirected
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


%feature("docstring") gum::PAG::isBidirected
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


%feature("docstring") gum::PAG::isDefCollider
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


%feature("docstring") gum::PAG::reorientAllWith
"
Set all endpoint marks in the PAG to the given mark.

Parameters
----------
m : int
    mark to set everywhere: ``EdgeMark_Circle`` (0), ``EdgeMark_Tail`` (1), or ``EdgeMark_Arrowhead`` (2)
"


%feature("docstring") gum::PAG::toMixedGraph
"
Convert the PAG to a MixedGraph by interpreting definite edge orientations.

- Definitely directed edges become arcs.
- Bidirected edges become two arcs (one in each direction).
- Undirected or circle edges become undirected edges.

Returns
-------
pyagrum.MixedGraph
"


%feature("docstring") gum::PAG::toDot
"
Return a Graphviz dot representation of the PAG.

Endpoint marks are rendered as:
- Circle: ``odot``
- Tail: ``none``
- Arrowhead: ``normal``

Returns
-------
str
    dot-format string
"


%feature("docstring") gum::PAG::addNode
"
Add a new node to the PAG and return its id.

Returns
-------
int
    the new NodeId
"


%feature("docstring") gum::PAG::addNodeWithId
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


%feature("docstring") gum::PAG::addNodes
"
Add n new nodes to the PAG.

Parameters
----------
n : int
    number of nodes to add

Returns
-------
list of int
    the new NodeIds
"


%feature("docstring") gum::PAG::clearEdges
"
Remove all edges and their endpoint marks from the PAG.

Nodes are preserved; only edges and their associated marks are removed.
"


%feature("docstring") gum::PAG::empty
"
Check whether the PAG has no nodes.

Returns
-------
bool
    True if the PAG contains no nodes
"


%feature("docstring") gum::PAG::emptyEdges
"
Check whether the PAG has no edges.

Returns
-------
bool
    True if the PAG contains no edges
"


%feature("docstring") gum::PAG::eraseNeighbours
"
Remove all edges adjacent to a given node (and their marks).

Parameters
----------
n : int
    id of the node
"


%feature("docstring") gum::PAG::existsEdge
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


%feature("docstring") gum::PAG::existsNode
"
Check whether a node with the given id exists in the PAG.

Parameters
----------
id : int
    the id to check

Returns
-------
bool
    True if the node exists
"


%feature("docstring") gum::PAG::size
"
Return the number of nodes in the PAG.

Returns
-------
int
    number of nodes
"


%feature("docstring") gum::PAG::sizeEdges
"
Return the number of edges in the PAG.

Returns
-------
int
    number of edges
"
