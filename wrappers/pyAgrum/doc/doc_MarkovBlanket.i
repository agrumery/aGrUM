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




%feature("docstring") gum::MarkovBlanket
"
Class building the Markov blanket of a node in a graph.

MarkovBlanket(m,n) -> MarkovBlanket
    Parameters:
        - **m** (*pyAgrum.DAGmodel*) -- a DAGmodel
        - **n** (int) -- a node id

MarkovBlanket(m,name) -> MarkovBlanket
    Parameters:
        - **m** (*pyAgrum.DAGmodel*) -- a DAGmodel
        - **name** (*str*) -- a node name
"

%feature("docstring") gum::MarkovBlanket::dag
"
Returns
-------
pyAgrum.DAG
  a copy of the DAG
"

%feature("docstring") gum::MarkovBlanket::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::MarkovBlanket::parents
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

%feature("docstring") gum::MarkovBlanket::children
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

%feature("docstring") gum::MarkovBlanket::sizeArcs
"
Returns
-------
int
    the number of arcs in the graph
"

%feature("docstring") gum::MarkovBlanket::arcs
"
Returns
-------
List
	the list of the arcs
"

%feature("docstring") gum::MarkovBlanket::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::MarkovBlanket::size
"
Returns
-------
int
    the number of nodes in the graph
"

%feature("docstring") gum::MarkovBlanket::sizeNodes
"
Returns
-------
int
	the number of nodes in the graph
"

%feature("docstring") gum::MarkovBlanket::hasSameStructure
"
Parameters
----------
pyAgrum.DAGmodel
	a direct acyclic model

Returns
-------
bool
    True if all the named node are the same and all the named arcs are the same
"
