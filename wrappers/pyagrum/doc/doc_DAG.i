/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
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
 *   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   *
 *   licence (LICENSE.MIT) for more details.                                *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/




%feature("docstring") gum::DAG
"
DAG represents a Directed Graph.

DAG() -> DAG
    default constructor

DAG(src) -> DAG
    Parameters:
        - **src** (*pyagrum.DAG*) -- the DAG to copy
"

%feature("docstring") gum::DAG::addArc
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
  pyagrum.CycleDetected
    If a cycle is detected

"

%feature("docstring") gum::DAG::addNode
"
Returns
-------
int
  the new NodeId
"

%feature("docstring") gum::DAG::addNodes
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

%feature("docstring") gum::DAG::addNodeWithId
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

%feature("docstring") gum::DAG::arcs
"
Returns the set of arcs in the graph.

Returns
-------
Set
	the set of the arcs
"

%feature("docstring") gum::DAG::children
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

%feature("docstring") gum::DAG::clear
"
Remove all the nodes and arcs from the graph.
"

%feature("docstring") gum::DAG::empty
"
Check if the graph is empty.

Returns
-------
bool
    True if the graph is empty
"

%feature("docstring") gum::DAG::emptyArcs
"
Check if the graph doesn't contains arcs.

Returns
-------
bool
    True if the graph doesn't contains arcs
"

%feature("docstring") gum::DAG::eraseArc
"
Erase the arc between n1 and n2.

Parameters
----------
n1 : int
	the id of the tail node
n2 : int
	the id of the head node
"
%feature("docstring") gum::DAG::eraseChildren
"
Erase the arcs heading through the node's children.

Parameters
----------
n : int
	the id of the parent node
"

%feature("docstring") gum::DAG::eraseNode
"
Erase the node and all the related arcs.

Parameters
----------
id : int
	the id of the node
"

%feature("docstring") gum::DAG::eraseParents
"
Erase the arcs coming to the node.

Parameters
----------
n : int
	the id of the child node
"

%feature("docstring") gum::DAG::existsArc
"
Check if an arc exists bewteen n1 and n2.

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

%feature("docstring") gum::DAG::existsNode
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

%feature("docstring") gum::DAG::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::DAG::parents
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

%feature("docstring") gum::DAG::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::DAG::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::DAG::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::DAG::topologicalOrder
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

%feature("docstring") gum::DAG::hasDirectedPath
"
Check if a directedpath exists bewteen from and to.

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
