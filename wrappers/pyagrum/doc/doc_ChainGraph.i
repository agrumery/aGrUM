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

%feature("docstring") gum::ChainGraph
"
ChainGraph represents a chain graph: a PDAG (arcs and undirected edges, at most one connection between
two nodes) without partially directed cycle, i.e. without any cycle containing at least one arc in
which the arcs are followed in their direction and the edges in either direction.

Equivalently, the connected components of the undirected part (the chain components) are linked by
arcs that form a directed acyclic graph over the components. DAGs and undirected graphs are chain
graphs.

This class is the graph only: the independence model read from a chain graph depends on its
interpretation (LWF, AMP or MVR). The methods `moralGraph`, `moralizedAncestralGraph` and
`cSeparation` follow the LWF interpretation.

ChainGraph() -> ChainGraph
    default constructor

ChainGraph(src) -> ChainGraph
    Parameters:
        - **src** (*pyagrum.ChainGraph*, *pyagrum.PDAG*, *pyagrum.DAG*, *pyagrum.UndiGraph* or *pyagrum.MixedGraph*) -- the graph to copy

Raises
------
pyagrum.InvalidPartiallyDirectedCycle
  If the graph to copy contains a partially directed cycle.
pyagrum.InvalidDirectedCycle
  If the mixed graph to copy contains a directed cycle.
pyagrum.InvalidArc
  If the mixed graph to copy connects two nodes by both an arc and an edge.

References
----------
S. L. Lauritzen, N. Wermuth. Graphical Models for Associations between Variables, some of which are
Qualitative and some Quantitative. The Annals of Statistics 17(1):31-57, 1989.
doi:10.1214/aos/1176347003.

M. Frydenberg. The chain graph Markov property. Scandinavian Journal of Statistics 17(4):333-353, 1990.

S. A. Andersson, D. Madigan, M. D. Perlman. Alternative Markov Properties for Chain Graphs.
Scandinavian Journal of Statistics 28(1):33-85, 2001. doi:10.1111/1467-9469.00224.

R. R. Bouckaert, M. Studený. Chain graphs: Semantics and expressiveness. ECSQARU 1995, LNCS 946,
pp. 69-76, Springer. doi:10.1007/3-540-60112-0_9.

R. R. Bouckaert, M. Studený. On chain graph models for description of conditional independence
structures. The Annals of Statistics 26(4):1434-1495, 1998. doi:10.1214/aos/1024691250.

D. Sonntag, J. M. Peña. Chain graph interpretations and their relations revisited. International
Journal of Approximate Reasoning 58:39-56, 2015. doi:10.1016/j.ijar.2014.12.001.
"

%feature("docstring") gum::ChainGraph::addArc
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
pyagrum.InvalidArc
  If an edge already connects tail and head.
pyagrum.InvalidDirectedCycle
  If the arc would create a directed cycle.
pyagrum.InvalidPartiallyDirectedCycle
  If the arc would create a partially directed cycle, i.e. if there is a path from head to tail
  following the arcs in their direction and the edges in either direction.
"

%feature("docstring") gum::ChainGraph::addEdge
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
pyagrum.InvalidEdge
  If an arc already connects n1 and n2.
pyagrum.InvalidPartiallyDirectedCycle
  If the edge would create a partially directed cycle.
"

%feature("docstring") gum::ChainGraph::cSeparation
"
Check if the sets of nodes X and Y are c-separated (by the set of nodes Z if given) in the chain graph
(LWF interpretation).

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

%feature("docstring") gum::ChainGraph::moralGraph
"
Returns the moral graph of the chain graph (LWF interpretation): the parents of each chain component
are married, then all the arcs become edges.

Returns
-------
pyagrum.UndiGraph
    The moral graph
"

%feature("docstring") gum::ChainGraph::hasMixedReallyOrientedPath
"
Check if a path from n1 to n2 exists, following the arcs in their direction and the edges in either
direction, with at least one arc.

Parameters
----------
n1 : int
  the id of the first node of the path
n2 : int
  the id of the last node of the path

Returns
-------
bool
  True if such a path exists (False if n1 and n2 are the same node)
"

%feature("docstring") gum::ChainGraph::moralizedAncestralGraph
"
Compute the moral graph (LWF interpretation) of the smallest ancestral set containing the nodes.

Parameters
----------
nodes : iterable of int
  the node ids (a set, a list...; a single node id is given as ``{id}``)

Returns
-------
pyagrum.UndiGraph
  the moralized ancestral graph of the nodes.
"
