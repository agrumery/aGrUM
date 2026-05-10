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







%feature("docstring") gum::MarkovRandomField
"
MarkovRandomField represents a Markov random field.

MarkovRandomField(name='') -> MarkovRandomField
    Parameters:
      - **name** (*str*) -- the name of the Bayes Net

MarkovRandomField(source) -> MarkovRandomField
    Parameters:
      - **source** (*pyagrum.MarkovRandomField*) -- the Markov random field to copy
"

%feature("docstring") gum::MarkovRandomField::add
"
Add a variable to the pyagrum.MarkovRandomField.

Parameters
----------
variable : pyagrum.DiscreteVariable
    the variable added
name : str
    the variable name
nbrmod : int
    the number of modalities for the new variable
id : int
    the variable forced id in the pyagrum.MarkovRandomField

Returns
-------
int
    the id of the new node

Raises
------
pyagrum.DuplicateLabel
    If variable.name() is already used in this pyagrum.MarkovRandomField.
pyagrum.OperationNotAllowed
    If nbrmod is less than 2
pyagrum.DuplicateElement
    If id is already used.
"

%feature("docstring") gum::MarkovRandomField::addFactor
"
Add a factor from a list or a set of id or str. If the argument is a set, the order is the order of the IDs of the variables

Parameters
----------
seq : sequence (list or set) of int or string
    The sequence (ordered or not) of node id or names
"

%feature("docstring") gum::MarkovRandomField::changeVariableName
"
Changes a variable's name in the pyagrum.MarkovRandomField.

This will change the pyagrum.DiscreteVariable names in the pyagrum.MarkovRandomField.

Parameters
----------
car :int | str
    a variable's id (int) or name
new_name : str
    the new name of the variable

Raises
------
pyagrum.DuplicateLabel
    If new_name is already used in this MarkovRandomField.
pyagrum.NotFound
    If no variable matches id.
"

%feature("docstring") gum::MarkovRandomField::changeVariableLabel
"
change the label of the variable associated to nodeId to the new value.

Parameters
----------
var :int | str
    a variable's id (int) or name
old_label : str
    the old label
new_label : str
    the new label

Raises
------
pyagrum.NotFound
    if id/name is not a variable or if old_label does not exist.
"

%feature("docstring") gum::MarkovRandomField::endTopologyTransformation
"
Terminates a sequence of insertions/deletions of arcs by adjusting all CPTs dimensions.
End Multiple Change for all CPTs.

Returns
-------
pyagrum.MarkovRandomField
"

%feature("docstring") gum::MarkovRandomField::clear
"
Clear the whole MarkovRandomField
"

%feature("docstring") gum::MarkovRandomField::erase
"
Remove a variable from the pyagrum.MarkovRandomField.

Removes the corresponding variable from the pyagrum.MarkovRandomField and from all of it's children pyagrum.Tensor.

If no variable matches the given id, then nothing is done.

Parameters
----------
var :int | str | pyagrum.DiscreteVariable
    a variable's id (int) or name of variable or a reference of this variable to remove.
"

%feature("docstring") gum::MarkovRandomField::eraseArc
"
Removes an arc in the BN, and update head's CTP.

If (tail, head) doesn't exist, the nothing happens.

Parameters
----------
arc : pyagrum.Arc
    The arc to be removed.
head : int | str
    a variable's id (int) or name
tail : int | str
    a variable's id (int) or name
"

%feature("docstring") gum::MarkovRandomField::fastPrototype
"
Create a Markov random field with a modified dot-like syntax which specifies:
    - the structure ``a--b--c;b--d--e;``. The substring ``a--b--c`` indicates a factor with the scope (a,b,c).
    - the type of the variables with different syntax (cf documentation).

Examples
--------
>>> import pyagrum as gum
>>> bn=pyagrum.MarkovRandomField.fastPrototype('A--B[1,3]--C{yes|No}--D[2,4]--E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize : int or str
        the default domain size or the default domain for variables

Returns
-------
pyagrum.MarkovRandomField
        the resulting Markov random field
"

%feature("docstring") gum::MarkovRandomField::generateFactor
"
Randomly generate factor parameters for a given factor in a given structure.

Parameters
----------
node : int | str
    a variable's id (int) or name
"

%feature("docstring") gum::MarkovRandomField::generateFactors
"
Randomly generates factors parameters for a given structure.
"

%feature("docstring") gum::MarkovRandomField::loadUAI
"
Load an UAI file.

Parameters
----------
name : str
    the name's file
l : list
    list of functions to execute

Raises
------
pyagrum.IOError
    If file not found
pyagrum.FatalError
    If file is not valid
"

%feature("docstring") gum::MarkovRandomField::saveUAI
"
Save the MarkovRandomField in an UAI file.

Parameters
----------
name : str
    the file's name
"

%feature("docstring") gum::MarkovRandomField::factor
"
Returns the factor of a set of variables (if existing).

Parameters
----------
vars : set
    A set of ids or names of variable the pyagrum.MarkovRandomField.

Returns
-------
pyagrum.Tensor
    The factor of the set of nodes.

Raises
------
pyagrum.NotFound
    If no variable's id matches varId.
"

%feature("docstring") gum::MarkovRandomField::loadGUM
"
Load a jgum (JSON) or bgum (binary/msgpack) file.

Parameters
----------
name : str
    the file's path (extension: ``.jgum`` for JSON, ``.bgum`` for binary)
binary : bool
    if True, read as bgum (msgpack) regardless of extension (default: False)

Raises
------
pyagrum.IOError
    If file not found
pyagrum.FatalError
    If file content is not valid

See Also
--------
:ref:`jgum-bgum-format` : complete format reference
"

%feature("docstring") gum::MarkovRandomField::saveGUM
"
Save the MarkovRandomField in a jgum (JSON) or bgum (binary/msgpack) file.

Metadata properties (``software``, ``creation``, ``lastModification``) are updated automatically.

Parameters
----------
name : str
    the file's path
binary : bool
    if True, write as bgum (msgpack); otherwise write as jgum (JSON) (default: False)
indent : int
    indentation level for JSON output; -1 for compact, 2 for pretty-printed (default: 2)

See Also
--------
:ref:`jgum-bgum-format` : complete format reference
"

%feature("docstring") gum::MarkovRandomField::saveGUMstring
"
Serialize the MarkovRandomField to a jgum JSON string.

Metadata properties (``software``, ``creation``, ``lastModification``) are updated automatically.

Parameters
----------
indent : int
    indentation level; -1 for compact, 2 for pretty-printed (default: 2)

Returns
-------
str
    a JSON string representing the MarkovRandomField in jgum format

See Also
--------
:ref:`jgum-bgum-format` : complete format reference
"

%feature("docstring") gum::MarkovRandomField::loadGUMstring
"
Deserialize a MarkovRandomField from a jgum JSON string.

Parameters
----------
content : str
    a JSON string in jgum format

Raises
------
pyagrum.FatalError
    If the string is not valid jgum JSON or the type field does not match ``\"MRF\"``

See Also
--------
:ref:`jgum-bgum-format` : complete format reference
"

%feature("docstring") gum::MarkovRandomField::connectedComponents
"
Return the connected components of the Markov random field.

Each node is mapped to the id of its component root (an arbitrarily chosen node from the same component).

Returns
-------
dict[int, int]
    mapping node id → component root id
"

%feature("docstring") gum::MarkovRandomField::idFromName
"
Returns a variable's id given its name in the graph.

Parameters
----------
name : str
    The variable's name from which the id is returned.

Notes
-----
  A convenient shortcut for ``g.variableFromName(name)`` is ``g[name]``.

Returns
-------
int :
    The variable's node id.

Raises
------
pyagrum.NotFound
    If name does not match a variable in the graph
"

%feature("docstring") gum::MarkovRandomField::variableFromName
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

%feature("docstring") gum::MarkovRandomField::beginTopologyTransformation
"
Begin a sequence of structural modifications (factor additions/deletions).

Structural changes are batched until endTopologyTransformation is called, which then adjusts all factor dimensions.
"

%feature("docstring") gum::MarkovRandomField::edges
"
Returns
-------
set
    the set of edges in the Markov random field
"

%feature("docstring") gum::MarkovRandomField::eraseFactor
"
Remove the factor that covers a given set of variables.

Parameters
----------
vars : set[int] | list[str]
    the set of variable ids or names whose factor should be removed
"

%feature("docstring") gum::MarkovRandomField::factors
"
Return the table of all factors in the Markov random field.

Returns
-------
dict
    a dict mapping frozenset[int] (node id sets) to pyagrum.Tensor
"

%feature("docstring") gum::MarkovRandomField::fromBN
"
Create a Markov random field from a Bayesian network.

Parameters
----------
bn : pyagrum.BayesNet
    the Bayesian network to convert

Returns
-------
pyagrum.MarkovRandomField
    a new MRF with the same variables and moralised structure
"

%feature("docstring") gum::UGmodel::graph
"
Return the underlying undirected graph.

Returns
-------
pyagrum.UndiGraph
    the underlying graph
"

%feature("docstring") gum::MarkovRandomField::minimalCondSet
"
Return a minimal conditioning set of a target given source nodes in the MRF.

Parameters
----------
target : int | str | list[int|str]
    the target node id(s) or name(s)
soids : list[int|str]
    the list of source node ids or names

Returns
-------
set[int]
    the minimal conditioning set (as node ids)
"

%feature("docstring") gum::MarkovRandomField::neighbours
"
Return the set of neighbours of a node.

Parameters
----------
id : int
    the node id

Returns
-------
set[int]
    the set of neighbour node ids
"

%feature("docstring") gum::MarkovRandomField::nodes
"
Return the set of node ids in the model.

Returns
-------
set[int]
    the set of node ids
"

%feature("docstring") gum::UGmodel::size
"
Return the number of nodes (variables) in the graphical model.

Returns
-------
int
    the number of nodes
"

%feature("docstring") gum::MarkovRandomField::smallestFactorFromNode
"
Return the id set of the smallest factor that contains the given node.

Parameters
----------
node : int
    the node id

Returns
-------
set[int]
    the id set of the smallest factor containing this node

Raises
------
pyagrum.NotFound
    if no factor contains this node
"

%feature("docstring") gum::MarkovRandomField::variable
"
Return the variable associated with a given node id.

Parameters
----------
id : int
    the node id

Returns
-------
pyagrum.DiscreteVariable
    the variable

Raises
------
pyagrum.NotFound
    if the node id does not exist
"

%feature("docstring") gum::UGmodel::family
"
Return the family of a node: the node itself plus all its neighbours.

Parameters
----------
id : int
    the node id

Returns
-------
set[int]
    the node and all its neighbours
"

%feature("docstring") gum::UGmodel::existsEdge
"
Check whether an edge exists between two nodes.

Parameters
----------
n1 : int | str
    one endpoint (id or name)
n2 : int | str
    the other endpoint (id or name)

Returns
-------
bool
    True if the edge exists
"

%feature("docstring") gum::UGmodel::hasSameStructure
"
Check whether this model has the same undirected structure as another UGmodel.

Parameters
----------
other : pyagrum.MarkovRandomField
    the model to compare with

Returns
-------
bool
    True if the undirected structures are identical
"

%feature("docstring") gum::UGmodel::sizeEdges
"
Return the number of edges in the model.

Returns
-------
int
    the number of edges
"

%feature("docstring") gum::UGmodel::connectedComponents
"
Return the connected components of the undirected model.

Each node is mapped to the id of its component root.

Returns
-------
dict[int, int]
    mapping node id → component root id
"

%feature("docstring") gum::IMarkovRandomField::dim
"
Return the dimension (total number of free parameters) of the Markov random field.

Returns
-------
int
    the number of free parameters
"

%feature("docstring") gum::IMarkovRandomField::maxVarDomainSize
"
Return the maximum domain size among all variables in the model.

Returns
-------
int
    the maximum domain size
"

%feature("docstring") gum::IMarkovRandomField::minParam
"
Return the minimum parameter value across all factors.

Returns
-------
float
    the minimum factor parameter
"

%feature("docstring") gum::IMarkovRandomField::maxParam
"
Return the maximum parameter value across all factors.

Returns
-------
float
    the maximum factor parameter
"

%feature("docstring") gum::IMarkovRandomField::minNonZeroParam
"
Return the minimum non-zero parameter value across all factors.

Returns
-------
float
    the minimum non-zero factor parameter
"

%feature("docstring") gum::IMarkovRandomField::maxNonOneParam
"
Return the maximum parameter value strictly less than 1 across all factors.

Returns
-------
float
    the maximum non-one factor parameter
"

%feature("docstring") gum::IMarkovRandomField::toDot
"
Return a Graphviz dot representation of the Markov random field.

Returns
-------
str
    a dot-format string
"

%feature("docstring") gum::IMarkovRandomField::toDotAsFactorGraph
"
Return a Graphviz dot representation of the Markov random field as a factor graph.

Returns
-------
str
    a dot-format string with variable nodes and factor nodes
"
