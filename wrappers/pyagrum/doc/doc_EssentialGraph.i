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







%feature("docstring") gum::EssentialGraph
"
The essential graph of a Bayesian network, or a PDAG learned as such.

The essential graph (or completed PDAG) represents the Markov equivalence class of a Bayesian
network: the Bayesian networks with the same independence model. Its arcs have the same orientation
in all the Bayesian networks of the class; its undirected edges may have both orientations.

The names of the nodes are stored in the essential graph: it does not depend on the model it was
built from.

EssentialGraph(m) -> EssentialGraph
    Parameters:
      - **m** (*pyagrum.DAGmodel*) -- a Bayesian network (or any DAG model)

EssentialGraph(m, pdag) -> EssentialGraph
    Parameters:
      - **m** (*pyagrum.DAGmodel*) -- the model giving the names of the nodes
      - **pdag** (*pyagrum.PDAG*) -- the essential graph

EssentialGraph(pdag) -> EssentialGraph
    Parameters:
      - **pdag** (*pyagrum.PDAG*) -- the essential graph (e.g. learned by a constraint-based method), with the names of its nodes

References
----------
D. M. Chickering. A Transformational Characterization of Equivalent Bayesian Network Structures. UAI 1995, pp. 87-98.

S. A. Andersson, D. Madigan, M. D. Perlman. A characterization of Markov equivalence classes for acyclic digraphs. The Annals of Statistics 25(2):505-541, 1997.
"

%feature("docstring") gum::EssentialGraph::cpdag
"
Returns
-------
pyagrum.CPDAG
    the essential graph as a CPDAG

Raises
------
pyagrum.InvalidArgument
    if the essential graph is not a CPDAG (a learned PDAG may not be one when the data contradict each other)
"

%feature("docstring") gum::EssentialGraph::arcs
"
Returns
-------
list
    The lisf of arcs in the EssentialGraph
"

%feature("docstring") gum::EssentialGraph::children
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

%feature("docstring") gum::EssentialGraph::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId : int
    A variable's id in the pyagrum.EssentialGraph.
name : str
    A variable's name in the pyagrum.EssentialGraph.

Returns
-------
pyagrum.Tensor
    The variable's CPT.

Raises
------
pyagrum.NotFound
    If no variable's id matches varId.
"

%feature("docstring") gum::EssentialGraph::parents
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

%feature("docstring") gum::EssentialGraph::toDot
"
Drawn as a ``pyagrum.CPDAG`` (chain components in clusters, compelled arcs with the color of the key
``dot.compelledArcColor`` of ``pyagrum.config``), the nodes being labelled with the names of the variables.

Parameters
----------
options : pyagrum.DotOptions, optional
    what is given to the drawing beyond the look of ``pyagrum.config`` (size, label, colors of some nodes, width of some arcs...).

Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::EssentialGraph::sizeEdges
"
Returns
-------
int
    the number of edges in the graph
"

%feature("docstring") gum::EssentialGraph::edges
"
Returns
-------
List
  the list of the edges
"

%feature("docstring") gum::EssentialGraph::neighbours
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

%feature("docstring") gum::EssentialGraph::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::EssentialGraph::sizeNodes
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::EssentialGraph::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::EssentialGraph::pdag
"
Returns
-------
pyagrum.PDAG
    the PDAG (Partially Directed Graph)
"

%feature("docstring") gum::EssentialGraph::idFromName
"
Parameters
----------
name : str
  the name of a node

Returns
-------
int or None
  the id of the node with this name, or None if no node has this name
"

%feature("docstring") gum::EssentialGraph::nameFromId
"
Parameters
----------
node : int
  the id of a node

Returns
-------
str
  the name of the node
"

%feature("docstring") gum::EssentialGraph::connectedComponents
"
Return the connected components of the essential graph.

Each node is mapped to the id of its component root (an arbitrarily chosen node from the same component).

Returns
-------
dict[int, int]
    mapping node id → component root id
"

%feature("docstring") gum::EssentialGraph::ancestors
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

%feature("docstring") gum::EssentialGraph::descendants
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

%feature("docstring") gum::EssentialGraph::nodes
"
Return the set of node ids in the essential graph.

Returns
-------
set[int]
    the set of all node ids
"

%feature("docstring") gum::EssentialGraph::skeleton
"
Return the skeleton of the essential graph (the underlying undirected graph without arc orientations).

Returns
-------
pyagrum.UndiGraph
    the skeleton of the essential graph
"
