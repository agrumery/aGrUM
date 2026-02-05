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







%feature("docstring") gum::DAGmodel
"
Abstract class used by IBayesNet and InfluenceDiagram.
"

%feature("docstring") gum::DAGmodel::log10DomainSize
"
Returns
-------
float
	The log10 domain size of the joint probability for the model.
"

%feature("docstring") gum::DAGmodel::completeInstantiation
"
Get an instantiation over all the variables of the model.

Returns
----------
  pyagrum.instantiation
	the complete instantiation
"

%feature("docstring") gum::DAGmodel::dag
"
Returns
-------
pyagrum.DAG
	a constant reference to the dag of this BayesNet.
"

%feature("docstring") gum::DAGmodel::empty
"
Returns
-------
bool
	True if the model is empty
"

%feature("docstring") gum::DAGmodel::hasSameStructure
"
Parameters
----------
pyagrum.DAGmodel
	a direct acyclic model

Returns
-------
bool
    True if all the named node are the same and all the named arcs are the same
"

%feature("docstring") gum::DAGmodel::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::DAGmodel::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::DAGmodel::topologicalOrder
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

%feature("docstring") gum::DAGmodel::moralGraph
"
Returns the moral graph of the BayesNet, formed by adding edges between all pairs of nodes that have a common child, and then making all edges in the graph undirected.

Returns
-------
pyagrum.UndiGraph
	The moral graph
"

%feature("docstring") gum::DAGmodel::property
"
Warnings
--------
Unreferenced function
"

%feature("docstring") gum::DAGmodel::propertyWithDefault
"
Warnings
--------
Unreferenced function
"

%feature("docstring") gum::DAGmodel::setProperty
"
Warnings
--------
Unreferenced function
"

%feature("docstring") gum::DAGmodel::arcs
"
Returns
-------
list
	The lisf of arcs in the graph
"

%feature("docstring") gum::DAGmodel::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::DAGmodel::idFromName
"
Returns a variable's id given its name in the graph.

Parameters
----------
name : str
	The variable's name from which the id is returned.

Notes
-----
  A convenient shortcut for `g.variableFromName(name)` is `g[name]`.

Returns
-------
int :
	The variable's node id.

Raises
------
pyagrum.NotFound
	If name does not match a variable in the graph
"

%feature("docstring") gum::DAGmodel::variableFromName
"
Parameters
----------
name : str
	a variable's name

Returns
-------
pyagrum.DiscreteVariable
	the variable

Raises
------
pyagrum.IndexError
	If the graph does not contain the variable
"


%feature("docstring") gum::DAGmodel::nodeId
"
Parameters
----------
var : pyagrum.DiscreteVariable
	a variable

Returns
-------
int
	the id of the variable

Raises
------
pyagrum.IndexError
	If the graph does not contain the variable
"

%feature("docstring") gum::DAGmodel::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::DAGmodel::variable
"
Parameters
----------
id : int
	a variable's id
name : str
	a variable's name

Returns
-------
pyagrum.DiscreteVariable
	the variable

Raises
------
pyagrum.IndexError
	If the graph does not contain the variable
"

%feature("docstring") gum::DAGmodel::variableNodeMap
"
Returns
-------
pyagrum.variableNodeMap
	the variable node map
"


%feature("docstring") gum::DAGmodel::ancestors
"
give the set of nodeid of ancestors of a node

Parameters
----------
norid : str|int
  the name or the id of the node

Returns
-------
Set[int]
  the set of ids of the ancestors of node `norid`.
"


%feature("docstring") gum::DAGmodel::descendants
"
give the set of nodeid of descendants of a node

Parameters
----------
norid : str|int
  the name or the id of the node

Returns
-------
Set[int]
  the set of ids of the descendants of node `norid`.
"

%feature("docstring") gum::DAGmodel::existsArc
"
Check if an arc exists

Parameters
---------
tail : str|int
  the name or id of the tail of the arc

head : str|int
  the name or the id of the head of the arc

Returns
-------
bool
  True if `tail->head` is an arc.
"

%feature("docstring") gum::DAGmodel::family
"
give the set of parents of a node and the node

Parameters
---------
norid : str|int
  the node

Returns
-------
Set[int]
  the set of nodeId of the family of the node `norid`
"

%feature("docstring") gum::DAGmodel::moralizedAncestralGraph
"
build a UndiGraph by moralizing the Ancestral Graph of a list of nodes

Parameters
----------
nodes : str|int|List[str|int]
  the list of of nodeIds or names

Warnings
--------
  gum.UndiGraph only knows NodeId. Hence the moralized ancestral graph does not include the names of the variables.graph

Returns
-------
gum.UndiGraph
  the moralized ancestral graph of the nodes
"
