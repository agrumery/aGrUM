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






%feature("docstring") PRMexplorer
"
PRMexplorer helps navigate through probabilistic relational models.

PRMexplorer() -> PRMexplorer
    default constructor
"

%feature("docstring") PRMexplorer::classAggregates
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of aggregates in the class

Raises
------
  pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::classAttributes
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of attributes

Raises
------
  pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::classDag
"
Parameters
----------
class_name : str
	a class name

Returns
-------
tuple
	a description of the DAG

Raises
------
  pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::classImplements
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of interfaces implemented by the class
"

%feature("docstring") PRMexplorer::classParameters
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of parameters

Raises
------
  pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::classReferences
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of references

Raises
------
pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::classSlotChains
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of class slot chains

Raises
------
pyagrum.IndexError
	if the class is not in the PRM
"

%feature("docstring") PRMexplorer::classes
"
Returns
-------
list
	the list of classes
"

%feature("docstring") PRMexplorer::cpf
"
Parameters
----------
class_name : str
	a class name

attribute : str
	an attribute

Returns
-------
pyagrum.Tensor
	the tensor of the attribute

Raises
------
pyagrum.OperationNotAllowed
	If the class element doesn't have any gum::Tensor (like a gum::PRMReferenceSlot).
pyagrum.IndexError
	If the class is not in the PRM
pyagrum.IndexError
	If the attribute in parameters does not exist
"

%feature("docstring") PRMexplorer::getDirectSubClass
"
Parameters
----------
class_name : str
	a class name

Returns
-------
list
	the list of direct subclasses

Raises
------
pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::getDirectSubInterfaces
"
Parameters
----------
interface_name : str
	an interface name

Returns
-------
list
	the list of direct subinterfaces

Raises
------
pyagrum.IndexError
	If the interface is not in the PRM
"

%feature("docstring") PRMexplorer::getDirectSubTypes
"
Parameters
----------
type_name : str
	a type name

Returns
-------
list
	the list of direct subtypes

Raises
------
pyagrum.IndexError
	If the type is not in the PRM
"

%feature("docstring") PRMexplorer::getImplementations
"
Parameters
----------
interface_name : str
	an interface name

Returns
-------
str
	the list of classes implementing the interface

Raises
------
pyagrum.IndexError
	If the interface is not in the PRM
"

%feature("docstring") PRMexplorer::getLabelMap
"
Parameters
----------
type_name : str
	a type name

Returns
-------
dict
	a dict containing pairs of label and their values

Raises
------
pyagrum.IndexError
	If the type is not in the PRM
"

%feature("docstring") PRMexplorer::getLabels
"
Parameters
----------
type_name : str
	a type name

Returns
-------
list
	the list of type labels

Raises
------
pyagrum.IndexError
	If the type is not in the PRM
"

%feature("docstring") PRMexplorer::getSuperClass
"
Parameters
----------
class_name : str
	a class name

Returns
-------
str
	the class extended by class_name

Raises
------
pyagrum.IndexError
	If the class is not in the PRM
"

%feature("docstring") PRMexplorer::getSuperInterface
"
Parameters
----------
interface_name : str
	an interface name

Returns
-------
str
	the interace extended by interface_name

Raises
------
pyagrum.IndexError
	If the interface is not in the PRM
"

%feature("docstring") PRMexplorer::getSuperType
"
Parameters
----------
type_name : str
	a type name

Returns
-------
str
	the type extended by type_name

Raises
------
pyagrum.IndexError
	If the type is not in the PRM
"

%feature("docstring") PRMexplorer::getalltheSystems
"
Returns
-------
list
	the list of all the systems and their components
"

%feature("docstring") PRMexplorer::interAttributes
"
Parameters
----------
interface_name : str
	an interface

allAttributes : bool
	True if supertypes of a custom type should be indicated

Returns
-------
list
	the list of (<type>,<attribute_name>) for the given interface

Raises
------
pyagrum.IndexError
	If the type is not in the PRM
"

%feature("docstring") PRMexplorer::interReferences
"
Parameters
----------
interface_name : str
	an interface

Returns
-------
list
	the list of (<reference_type>,<reference_name>,<True if the reference is an array>) for the given interface

Raises
------
pyagrum.IndexError
	If the type is not in the PRM
"

%feature("docstring") PRMexplorer::interfaces
"
Returns
-------
list
	the list of interfaces in the PRM
"

%feature("docstring") PRMexplorer::isAttribute
"
Parameters
----------
class_name : str
	a class name
att_name : str
	the name of the attribute to be tested

Returns
-------
bool
	True if att_name is an attribute of class_name

Raises
------
pyagrum.IndexError
	If the class is not in the PRM
pyagrum.IndexError
	If att_name is not an element of class_name
"

%feature("docstring") PRMexplorer::isClass
"
Parameters
----------
name : str
	an element name

Returns
-------
bool
	True if the parameter correspond to a class in the PRM
"

%feature("docstring") PRMexplorer::isInterface
"
Parameters
----------
name : str
	an element name

Returns
-------
bool
	True if the parameter correspond to an interface in the PRM
"

%feature("docstring") PRMexplorer::isType
"
Parameters
----------
name : str
	an element name

Returns
-------
bool
	True if the parameter correspond to a type in the PRM
"

%feature("docstring") PRMexplorer::load
"
Load a PRM into the explorer.

Parameters
----------
filename : str
	the name of the o3prm file
classpath : str
	the classpath of the PRM

Raises
------
pyagrum.FatalError
	If file not found
"

%feature("docstring") PRMexplorer::types
"
Returns
-------
list
	the list of the custom types in the PRM
"

%feature("docstring") PRMexplorer::aggType
"
min/max/count/exists/forall/or/and/amplitude/median
"
