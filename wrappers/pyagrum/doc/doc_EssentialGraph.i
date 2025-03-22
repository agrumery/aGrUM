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




%feature("docstring") gum::EssentialGraph
"
Class building the essential graph from a BN.

Essential graph is a mixed graph (Chain Graph) that represents the class of markov equivalent Bayesian networks (with the same independency model).

EssentialGraph(m) -> EssentialGraph
    Parameters:
      - **m** (*pyagrum.DAGmodel*) -- a DAGmodel
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
  the name of the variable in the model

Returns
-------
int
  the nodeId from the name of the variable in the model
"


%feature("docstring") gum::EssentialGraph::nameFromId
"
Parameters
----------
node : int
  the nodeId of the variable in the model

Returns
-------
str
  the name of the variable in the model from the nodeId
"
