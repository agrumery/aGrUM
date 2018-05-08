%feature("docstring") PRMexplorer
"
PRMexplorer helps navigate through probabilistic relational models.

Available constructors:
	``JTGenerator() -> JTGenerator``
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError 
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
pyAgrum.Potential
	the potential of the attribute

Raises
------
gum.OperationNotAllowed
	If the class element doesn't have any gum::Potential (like a gum::PRMReferenceSlot). 
gum.IndexError
	If the class is not in the PRM
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
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
gum.IndexError
	If the class is not in the PRM
gum.IndexError
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
gum.FatalError
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