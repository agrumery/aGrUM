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
 *   See LICENCES for more details.                                         *
 *                                                                          *
 *   SPDX-FileCopyrightText: Copyright 2005-2025                            *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
 *   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      *
 *                                                                          *
 *   Contact  : info_at_agrum_dot_org                                       *
 *   homepage : http://agrum.gitlab.io                                      *
 *   gitlab   : https://gitlab.com/agrumery/agrum                           *
 *                                                                          *
 ****************************************************************************/






%feature("docstring") gum::InfluenceDiagram
"
InfluenceDiagram represents an Influence Diagram.

InfluenceDiagram() -> InfluenceDiagram
    default constructor

InfluenceDiagram(source) -> InfluenceDiagram
    Parameters:
        - **source** (*pyagrum.InfluenceDiagram*) -- the InfluenceDiagram to copy
"

%feature("docstring") gum::InfluenceDiagram::add
"
Add a variable, it's associate node and it's CPT.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy that will be a chance node.
descr: str
  the descr of the variable following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>` extended for :func:`pyagrum.fastID`.
nbr_mod_or_id : int
	if the first argument is `variable`, this set an optional fixed id for the node. If the first argument is `descr`, this gives the default number of modalities
	for the variable. Note that if a utility node is described in `descr`, this value is overriden by 1.

Returns
-------
int
    the id of the added variable.

Raises
------
  pyagrum.DuplicateElement
	  If already used id or name.
"

%feature("docstring") gum::InfluenceDiagram::addArc
"
Add an arc in the ID, and update diagram's tensor nodes cpt if necessary.

Parameters
----------
tail : Union[int,str]
	a variable's id (int) or name
head : Union[int,str]
	a variable's id (int) or name

Raises
------
  pyagrum.InvalidEdge
	If arc.tail and/or arc.head are not in the ID.
  pyagrum.InvalidEdge
	If tail is a utility node
"

%feature("docstring") gum::InfluenceDiagram::addChanceNode
"
Add a chance variable, it's associate node and it's CPT.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable added by copy.
id : int
	the chosen id. If 0, the NodeGraphPart will choose.

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!!

Returns
-------
int
    the id of the added variable.

Raises
------
pyagrum.DuplicateElement
	If id(<>0) is already used
"

%feature("docstring") gum::InfluenceDiagram::addDecisionNode
"
Add a decision variable.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable added by copy.
id : int
	the chosen id. If 0, the NodeGraphPart will choose.

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!!

Returns
-------
int
    the id of the added variable.

Raises
------
pyagrum.DuplicateElement
	If id(<>0) is already used
"

%feature("docstring") gum::InfluenceDiagram::addUtilityNode
"
Add a utility variable, it's associate node and it's UT.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable added by copy
id : int
	the chosen id. If 0, the NodeGraphPart will choose

Warnings
--------
give an id (not 0) should be reserved for rare and specific situations !!!

Returns
-------
int
    the id of the added variable.

Raises
------
pyagrum.InvalidArgument
	If variable has more than one label
pyagrum.DuplicateElement
	If id(<>0) is already used
"

%feature("docstring") gum::InfluenceDiagram::arcs
"
Returns
-------
list:
	the list of all the arcs in the Influence Diagram.
"

%feature("docstring") gum::InfluenceDiagram::chanceNodeSize
"
Returns
-------
int
	the number of chance nodes.
"

%feature("docstring") gum::InfluenceDiagram::changeVariableName
"
Parameters
----------
var : Union[int,str]
	a variable's id (int) or name
new_name : str
	the name of the variable

Raises
------
pyagrum.DuplicateLabel
	If this name already exists
pyagrum.NotFound
	If no nodes matches id.
"

%feature("docstring") gum::InfluenceDiagram::children
"
Parameters
----------
var : Union[int,str]
	a variable's id (int) or name

Returns
-------
Set
	the set of all the children
"

%feature("docstring") gum::InfluenceDiagram::cpt
"
Returns the CPT of a variable.

Parameters
----------
var : Union[int,str]
	a variable's id (int) or name

Returns
-------
pyagrum.Tensor
	The variable's CPT.

Raises
------
pyagrum.NotFound
	If no variable's id matches varId.
"

%feature("docstring") gum::InfluenceDiagram::decisionNodeSize
"
Returns
-------
int
	the number of decision nodes
"

%feature("docstring") gum::InfluenceDiagram::decisionOrderExists
"
Returns
-------
bool
	True if a directed path exist with all decision node
"

%feature("docstring") gum::InfluenceDiagram::erase
"
Erase a Variable from the network and remove the variable from all his childs.

If no variable matches the id, then nothing is done.

Parameters
----------
id : int
	The id of the variable to erase.
var :  Union[int,str,pyagrum.DiscreteVariable]
	a variable's id (int) or name or th reference on the variable to remove.
"

%feature("docstring") gum::InfluenceDiagram::eraseArc
"
Removes an arc in the ID, and update diagram's tensor nodes cpt if necessary.

If (tail, head) doesn't exist, the nothing happens.

Parameters
----------
arc : pyagrum.Arc
	The arc to be removed whn calling eraseArc(arc)
tail : Union[int,str]
	a variable's id (int) or name when calling eraseArc(tail,head)
head : Union[int,str]
	a variable's id (int) or name when calling eraseArc(tail,head)
"

%feature("docstring") gum::InfluenceDiagram::existsPathBetween
"
Returns
-------
bool
	true if a path exists between two nodes.
"

%feature("docstring") gum::InfluenceDiagram::getDecisionGraph
"
Returns
-------
pyagrum.DAG
	the temporal Graph.
"

%feature("docstring") gum::InfluenceDiagram::getDecisionOrder
"
Returns
-------
list
	the sequence of decision nodes in the directed path.

Raises
------
NotFound
	If such a path does not exist
"

%feature("docstring") gum::InfluenceDiagram::idFromName
"
Returns a variable's id given its name.

Parameters
----------
name : str
	the variable's name from which the id is returned.

Returns
-------
int
	the variable's node id.

Raises
------
pyagrum.NotFound
	If no such name exists in the graph.
"

%feature("docstring") gum::InfluenceDiagram::nodes
"
Returns
-------
set
    the set of ids
"

%feature("docstring") gum::InfluenceDiagram::isChanceNode
"
Parameters
----------
varId : int
	the tested node id.

Returns
-------
bool
	true if node is a chance node
"

%feature("docstring") gum::InfluenceDiagram::isDecisionNode
"
Parameters
----------
varId : int
	the tested node id.

Returns
-------
bool
	true if node is a decision node
"

%feature("docstring") gum::InfluenceDiagram::isUtilityNode
"
Parameters
----------
varId : int
	the tested node id.

Returns
-------
bool
	true if node is an utility node
"

%feature("docstring") gum::InfluenceDiagram::loadBIFXML
"
Load a BIFXML file.

Parameters
----------
name : str
	the name's file

Raises
------
pyagrum.IOError
	If file not found
pyagrum.FatalError
	If file is not valid
"

%feature("docstring") gum::InfluenceDiagram::names
"
Returns
-------
List[str]
	The names of the InfluenceDiagram variables
"


%feature("docstring") gum::InfluenceDiagram::nodeId
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
	If the InfluenceDiagram does not contain the variable
"

%feature("docstring") gum::InfluenceDiagram::parents
"
Parameters
----------
var : Union[int,str]
	a variable's id (int) or name

Returns
-------
set
    the set of the parents ids.
"

%feature("docstring") gum::InfluenceDiagram::saveBIFXML
"
Save the BayesNet in a BIFXML file.

Parameters
----------
name : str
	the file's name
"

%feature("docstring") gum::InfluenceDiagram::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::InfluenceDiagram::utility
"
Parameters
----------
var : Union[int,str]
	a variable's id (int) or name

Returns
-------
pyagrum.Tensor
	the utility table of the node

Raises
------
pyagrum.IndexError
	If the InfluenceDiagram does not contain the variable
"

%feature("docstring") gum::InfluenceDiagram::utilityNodeSize
"
Returns
-------
int
	the number of utility nodes
"

%feature("docstring") gum::InfluenceDiagram::variable
"
Parameters
----------
id : int
 	the node id

Returns
------
pyagrum.DiscreteVariable
	a constant reference over a variabe given it's node id

Raises
------
pyagrum.NotFound
	If no variable's id matches the parameter
"

%feature("docstring") gum::InfluenceDiagram::variableFromName
"
Parameters
----------
name : str
	a variable's name

Returns
-------
pyagrum.DiscreteVariable
	the variable

Notes
-----
  A convenient shortcut for `g.variableFromName(name)` is `g[name]`.

Raises
------
pyagrum.IndexError
	If the InfluenceDiagram does not contain the variable
"

%feature("docstring") gum::InfluenceDiagram::variableNodeMap
"
TBD
"

%feature("docstring") gum::InfluenceDiagram::fastPrototype
"
Create an Influence Diagram with a dot-like syntax which specifies:
    - the structure 'a->b<-c;b->d;c<-e;'.
    - a prefix for the type of node (chance/decision/utiliy nodes):

      - `a` : a chance node named 'a' (by default)
      - `$a` : a utility node named 'a'
      - `*a` : a decision node named 'a'

    - the type of the variables with different syntax as postfix:

      - by default, a variable is a pyagrum.RangeVariable using the default domain size (second argument)
      - with `'a[10]'`, the variable is a pyagrum.RangeVariable using 10 as domain size (from 0 to 9)
      - with `'a[3,7]'`, the variable is a pyagrum.RangeVariable using a domainSize from 3 to 7
      - with `'a[1,3.14,5,6.2]'`, the variable is a pyagrum.DiscretizedVariable using the given ticks (at least 3 values)
      - with `'a{top|middle|bottom}'`, the variable is a pyagrum.LabelizedVariable using the given labels.
      - with 'a{-1|5|0|3}', the variable is a pyagrum.IntegerVariable using the sorted given values.
      - with 'a{-0.5|5.01|0|3.1415}', the variable is a pyagrum.NumericalDiscreteVariable using the sorted given values.

Note
----
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the tensors (probabilities, utilities) are randomly generated.
  - see also pyagrum.fastID.

Examples
--------
>>> import pyagrum as gum
>>> bn=pyagrum.fastID('A->B[1,3]<-*C{yes|No}->$D<-E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize :int or str
        the default domain size or the default domain for variables

Returns
-------
pyagrum.InfluenceDiagram
        the resulting Influence Diagram
"
