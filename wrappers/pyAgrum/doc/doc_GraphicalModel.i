/****************************************************************************
 *   This file is part of the aGrUM/pyAgrum library.                        *
 *                                                                          *
 *   Copyright (c) 2005-2025 by                                             *
 *       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 *
 *       - Christophe GONZALES(_at_AMU)                                     *
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
 *      or (at your option) any later version.                              *
 *    - the MIT license (MIT)                                               *
 *    - or both in dual license, as here                                    *
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

%feature("docstring") gum::GraphicalModel
"
Abstract class for all PGM (associating set of variables and a graph).
"

%feature("docstring") gum::GraphicalModel::names
"
Set of names of variables in the model

Returns
-------
Set[str]
	The names of the graph variables
"

%feature("docstring") gum::GraphicalModel::ids
"
List of ids for a list of names of variables in the model

Parameters
----------
lov : List[str]
  List of variable names

Returns
-------
List[int]
	The ids for the list of names of the graph variables
"

%feature("docstring") gum::GraphicalModel::nodeset
"
Set of ids for a list of names of variables in the model

Parameters
----------
lov : List[str]
  List of variable names

Returns
-------
Set[int]
	The set of ids for the list of names of the graph variables
"

%feature("docstring") gum::GraphicalModel::empty
"
Check if there are some variables in the model.

Returns
-------
bool
	True if there is no variable in the model.
"

%feature("docstring") gum::GraphicalModel::exists
"
Check if a node with this name or id exists

Parameters
----------
norid: str|int
  name or id of the searched node

Returns
-------
bool
	True if there is a node with such a name or id
"


%feature("docstring") gum::GraphicalModel::log10DomainSize
"
returns the log10 of the domain size of the model defined as the product of the domain sizes of the variables in the model.

Returns
-------
float
	the log10 domain size.
"

%feature("docstring") gum::GraphicalModel::completeInstantiation
"
Give an instantiation over all the variables of the model

Returns
-------
pyAgrum.Instantiation
  a complete Instantiation for the model
"

%feature("docstring") gum::GraphicalModel::isIndependent
"
check if nodes X and nodes Y are independent given nodes Z

Parameters
----------
X : str|int|List[str|int]
      a list of of nodeIds or names
Y : str|int|List[str|int]
      a list of of nodeIds or names
Z : str|int|List[str|int]
      a list of of nodeIds or names

Raises
------
InvalidArgument
  if X and Y share variables

Returns
-------
bool
  True if X and Y are independent given Z in the model
"


%feature("docstring") gum::GraphicalModel::property
"
Returns the value associated to this property.

Properties are a way to keep some (name,value) together with de model.

Parameters
----------
name : str
  the name of the property

Raises
------
NotFound
  if no name property is found

Returns
-------
str
  The value associated to this name
"

%feature("docstring") gum::GraphicalModel::propertyWithDefault
"
Returns the value associated to this property or the default value if there is no such property.

Properties are a way to keep some information (name,value) together with de model.

Parameters
----------
name : str
  the name of the property
byDefault: str
  the value by default if no property has been found.

Returns
-------
str
  The value associated to this name or the value by default.
"

%feature("docstring") gum::GraphicalModel::setProperty
"
Create or change the couple (name,value) in the properties.

Properties are a way to keep some information (name,value) together with de model.

Parameters
----------
name : str
  the name of the property
value: str
  the value of the property.
"
