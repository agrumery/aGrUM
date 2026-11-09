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
The essential graph of a Bayesian network: the CPDAG of its Markov equivalence class, with the names
of its variables.

The essential graph (or completed PDAG) represents the Markov equivalence class of a Bayesian
network: the Bayesian networks with the same independence model. Its arcs have the same orientation
in all the Bayesian networks of the class; its undirected edges may have both orientations.

An EssentialGraph is a ``pyagrum.CPDAG`` (immutable, with all its methods). The names of the nodes
are stored in the essential graph: it does not depend on the model it was built from.

EssentialGraph(m) -> EssentialGraph
    Parameters:
      - **m** (*pyagrum.DAGmodel*) -- a Bayesian network (or any DAG model)

EssentialGraph(m, pdag) -> EssentialGraph
    Parameters:
      - **m** (*pyagrum.DAGmodel*) -- the model giving the names of the nodes
      - **pdag** (*pyagrum.PDAG*) -- the essential graph: it must be a CPDAG (pyagrum.InvalidArgument otherwise)

EssentialGraph(pdag) -> EssentialGraph
    Parameters:
      - **pdag** (*pyagrum.PDAG*) -- the essential graph, with the names of its nodes: it must be a CPDAG (pyagrum.InvalidArgument otherwise)

EssentialGraph(cpdag) -> EssentialGraph
    Parameters:
      - **cpdag** (*pyagrum.CPDAG*) -- the essential graph, with the names of its nodes

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
    the essential graph as a (plain) CPDAG
"

%feature("docstring") gum::EssentialGraph::children
"
Parameters
----------
id : int | set[int]
  the id of a node, or a set of ids

Returns
-------
Set
    the set of all the children
"

%feature("docstring") gum::EssentialGraph::parents
"
Parameters
----------
id : int | set[int]
    the id of a node, or a set of ids

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

%feature("docstring") gum::EssentialGraph::sizeNodes
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
    a copy of the essential graph as a PDAG
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
