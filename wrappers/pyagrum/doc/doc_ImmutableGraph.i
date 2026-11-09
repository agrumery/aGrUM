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

// The read access shared by the immutable graphs (CPDAG, MPDAG, MAG): written once on the template
// bases, for all their instantiations. graph() and toDot() are documented with each class.

%feature("docstring") gum::ImmutableGraph::nodes
"
Returns
-------
set[int]
  the set of the node ids of the graph
"

%feature("docstring") gum::ImmutableGraph::edges
"
Returns
-------
set[tuple[int, int]]
  the set of the edges of the graph: the undirected edges of a CPDAG or an MPDAG, the edges (with
  any marks) of a MAG
"

%feature("docstring") gum::ImmutableGraph::size
"
Returns
-------
int
  the number of nodes
"

%feature("docstring") gum::ImmutableGraph::sizeEdges
"
Returns
-------
int
  the number of edges (see :meth:`edges`)
"

%feature("docstring") gum::ImmutableGraph::empty
"
Returns
-------
bool
  True if the graph has no node
"

%feature("docstring") gum::ImmutableGraph::existsNode
"
Parameters
----------
node : int
  the id of a node

Returns
-------
bool
  True if the node exists
"

%feature("docstring") gum::ImmutableGraph::existsEdge
"
Parameters
----------
first : int
  the id of a node
second : int
  the id of another node

Returns
-------
bool
  True if the edge first-second exists (see :meth:`edges`)
"

%feature("docstring") gum::ImmutableGraph::nameFromId
"
Return the name of a node, or its id as a string if no name was set.

The names of the nodes are kept from the graph the immutable graph was built from.

Parameters
----------
id : int
  the node id

Returns
-------
str
  the name associated with the node, or ``str(id)`` if the node has no name
"

%feature("docstring") gum::ImmutableGraph::idFromName
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

%feature("docstring") gum::ImmutableGraph::hasName
"
Parameters
----------
id : int
  the node id

Returns
-------
bool
  True if the node has a name
"

%feature("docstring") gum::ImmutableGraph::connectedComponents
"
The connected components of the graph (of its skeleton).

Returns
-------
dict[int, int]
  each node id mapped to the id of the root of its component
"

%feature("docstring") gum::ImmutableGraph::skeleton
"
Returns
-------
pyagrum.UndiGraph
  the undirected graph with the same nodes (ids and names) and the same adjacencies
"

%feature("docstring") gum::ImmutableGraph::toString
"
Returns
-------
str
  a string representation of the graph
"

%feature("docstring") gum::ImmutableMixedGraph::arcs
"
Returns
-------
set[tuple[int, int]]
  the set of the arcs of the graph (the arcs with the same orientation in all the represented DAGs)
"

%feature("docstring") gum::ImmutableMixedGraph::parents
"
Parameters
----------
node : int
  the id of a node

Returns
-------
set[int]
  the parents of the node
"

%feature("docstring") gum::ImmutableMixedGraph::children
"
Parameters
----------
node : int
  the id of a node

Returns
-------
set[int]
  the children of the node
"

%feature("docstring") gum::ImmutableMixedGraph::neighbours
"
Parameters
----------
node : int
  the id of a node

Returns
-------
set[int]
  the undirected neighbours of the node
"

%feature("docstring") gum::ImmutableMixedGraph::sizeArcs
"
Returns
-------
int
  the number of arcs
"

%feature("docstring") gum::ImmutableMixedGraph::existsArc
"
Parameters
----------
tail : int
  the id of the tail
head : int
  the id of the head

Returns
-------
bool
  True if the arc tail->head exists
"

%feature("docstring") gum::ImmutableMixedGraph::consistentExtension
"
Returns
-------
pyagrum.DAG
  a represented DAG (a consistent extension: same skeleton, same v-structures, containing the arcs),
  kept from the construction of the graph
"
