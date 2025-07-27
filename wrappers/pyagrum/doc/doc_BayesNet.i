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






%feature("docstring") gum::BayesNet
"
BayesNet represents a Bayesian network.

BayesNet(name='') -> BayesNet
    Parameters:
      - **name** (*str*) -- the name of the Bayes Net

BayesNet(source) -> BayesNet
    Parameters:
      - **source** (*pyagrum.BayesNet*) -- the Bayesian network to copy
"

%feature("docstring") gum::BayesNet::add
"
Add a variable to the pyagrum.BayesNet.

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable added
descr : str
	the description of the variable (following :ref:`fast syntax<Quick specification of (randomly parameterized) graphical models>`)
nbrmod : int
	the number of modalities for the new variable
id : int
	the variable forced id in the pyagrum.BayesNet

Returns
-------
int
	the id of the new node

Raises
------
pyagrum.DuplicateLabel
    If variable.name() or id is already used in this pyagrum.BayesNet.
pyagrum.NotAllowed
    If nbrmod is less than 2
"

%feature("docstring") gum::BayesNet::addAMPLITUDE
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addAND
"
Add a variable, it's associate node and an AND implementation.

The id of the new variable is automatically generated.

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy.

Returns
-------
int
	the id of the added variable.

Raises
------
pyagrum.SizeError
    If variable.domainSize()>2
"

%feature("docstring") gum::BayesNet::addArc
"
Add an arc in the BN, and update arc.head's CPT.

Parameters
----------
head : Union[int,str]
	a variable's id (int) or name
head : Union[int,str]
	a variable's id (int) or name

Raises
------
pyagrum.InvalidEdge
    If arc.tail and/or arc.head are not in the BN.
pyagrum.DuplicateElement
    If the arc already exists.
"

%feature("docstring") gum::BayesNet::addCOUNT
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addEXISTS
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addFORALL
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added variable.
"

%feature("docstring") gum::BayesNet::addLogit
"
Add a variable, its associate node and a Logit implementation.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy
externalWeight : float
	the added external weight
id : int
	The proposed id for the variable.
Returns
-------
int
	the id of the added variable.

Raises
------
pyagrum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addMAX
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addMEDIAN
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addMIN
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
	the variable to be added

Returns
-------
int
	the id of the added value
"

%feature("docstring") gum::BayesNet::addSUM
"
Others aggregators

Parameters
----------
variable : pyagrum.DiscreteVariable
        the variable to be added

Returns
-------
int
        the id of the added value
"

%feature("docstring") gum::BayesNet::addNoisyAND
"
Add a variable, its associate node and a noisyAND implementation.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy
externalWeight : float
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.

Raises
------
pyagrum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addNoisyOR
"
Add a variable, it's associate node and a noisyOR implementation.

Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep the addNoisyOR as an alias for addNoisyORCompound.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy
externalWeight : float
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.

Raises
--------
pyagrum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addNoisyORCompound
"
Add a variable, it's associate node and a noisyOR implementation.

Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep the addNoisyOR as an alias for addNoisyORCompound.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy
externalWeight : float
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.

Raises
--------
pyagrum.DuplicateElement
    If id is already used
"

%feature("docstring") gum::BayesNet::addNoisyORNet
"
Add a variable, its associate node and a noisyOR implementation.

Since it seems that the 'classical' noisyOR is the Compound noisyOR, we keep the addNoisyOR as an alias for addNoisyORCompound.

(The id of the new variable can be automatically generated.)

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy
externalWeight : float
	the added external weight
id : int
	The proposed id for the variable.

Returns
-------
int
	the id of the added variable.
"

%feature("docstring") gum::BayesNet::addOR
"
Add a variable, it's associate node and an OR implementation.

The id of the new variable is automatically generated.

Warnings
--------
	If parents are not boolean, all value>1 is True

Parameters
----------
variable : pyagrum.DiscreteVariable
	The variable added by copy

Returns
-------
int
	the id of the added variable.

Raises
------
pyagrum.SizeError
    If variable.domainSize()>2
"

%feature("docstring") gum::BayesNet::addWeightedArc
"
Add an arc in the BN, and update arc.head's CPT.

Parameters
----------
head : Union[int,str]
	a variable's id (int) or name
tail : Union[int,str]
	a variable's id (int) or name
causalWeight : float
	the added causal weight

Raises
------
pyagrum.InvalidArc
    If arc.tail and/or arc.head are not in the BN.
pyagrum.InvalidArc
    If variable in arc.head is not a NoisyOR variable.
"

%feature("docstring") gum::BayesNet::beginTopologyTransformation
"
When inserting/removing arcs, node CPTs change their dimension with a cost in time.
begin Multiple Change for all CPTs
These functions delay the CPTs change to be done just once at the end of a sequence of topology modification, begins a sequence of insertions/deletions of arcs without changing the dimensions of the CPTs.
"

%feature("docstring") gum::BayesNet::changeTensor
"
change the CPT associated to nodeId to newPot delete the old CPT associated to nodeId.

Parameters
----------
var : Union[int,str]
	the current name or the id of the variable
newPot : pyagrum.Tensor
	the new tensor

Raises
------
pyagrum.NotAllowed
    If newPot has not the same signature as __probaMap[NodeId]
"

%feature("docstring") gum::BayesNet::changeVariableName
"
Changes a variable's name in the gum::BayesNet.

This will change the gum::DiscreteVariable names in the gum::BayesNet.

Parameters
----------
var : Union[int,str]
	the current name or the id of the variable
new_name : str
	the new name of the variable

Raises
------
pyagrum.DuplicateLabel
    If new_name is already used in this BayesNet.
pyagrum.NotFound
    If no variable matches id.
"

%feature("docstring") gum::BayesNet::changeVariableLabel
"
change the label of the variable associated to nodeId to the new value.

Parameters
----------
var : Union[int,str]
	the current name or the id of the variable
old_label : str
	the new label
new_label : str
	the new label

Raises
------
pyagrum.NotFound
    if id/name is not a variable or if old_label does not exist.
"

%feature("docstring") gum::BayesNet::endTopologyTransformation
"
Terminates a sequence of insertions/deletions of arcs by adjusting all CPTs dimensions.
End Multiple Change for all CPTs.

Returns
-------
pyagrum.BayesNet
"

%feature("docstring") gum::BayesNet::clear
"
Clear the whole BayesNet
"

%feature("docstring") gum::BayesNet::erase
"
Remove a variable from the gum::BayesNet.

Removes the corresponding variable from the gum::BayesNet and from all of it's children gum::Tensor.

If no variable matches the given id, then nothing is done.

Parameters
----------
var : Union[int,str,pyagrum.DiscreteVariable]
	the current name, the id of the variable or a reference to the variable
"

%feature("docstring") gum::BayesNet::eraseArc
"
Removes an arc in the BN, and update head's CTP.

If (tail, head) doesn't exist, the nothing happens.

Parameters
----------
arc : pyagrum.Arc when calling eraseArc(arc)
	The arc to be removed.
head : Union[int,str]
	a variable's id (int) or name for the head when calling eraseArc(head,tail)
tail : Union[int,str]
	a variable's id (int) or name for the tail when calling eraseArc(head,tail)
"

%feature("docstring") gum::BayesNet::fastPrototype
"
Create a Bayesian network with a dot-like syntax which specifies:
    - the structure 'a->b->c;b->d<-e;'.
    - the type of the variables with different syntax:

      - by default, a variable is a pyagrum.RangeVariable using the default domain size ([2])
      - with 'a[10]', the variable is a pyagrum.RangeVariable using 10 as domain size (from 0 to 9)
      - with 'a[3,7]', the variable is a pyagrum.RangeVariable using a domainSize from 3 to 7
      - with 'a[1,3.14,5,6.2]', the variable is a pyagrum.DiscretizedVariable using the given ticks (at least 3 values)
      - with 'a{top|middle|bottom}', the variable is a pyagrum.LabelizedVariable using the given labels.
      - with 'a{-1|5|0|3}', the variable is a pyagrum.IntegerVariable using the sorted given values.
      - with 'a{-0.5|5.01|0|3.1415}', the variable is a pyagrum.NumericalDiscreteVariable using the sorted given values.

Note
----
  - If the dot-like string contains such a specification more than once for a variable, the first specification will be used.
  - the CPTs are randomly generated.
  - see also pyagrum.fastBN.

Examples
--------
>>> import pyagrum as gum
>>> bn=pyagrum.BayesNet.fastPrototype('A->B[1,3]<-C{yes|No}->D[2,4]<-E[1,2.5,3.9]',6)

Parameters
----------
dotlike : str
        the string containing the specification
domainSize : int or str
        the default domain size or the default domain for variables

Returns
-------
pyagrum.BayesNet
        the resulting Bayesian network
"

%feature("docstring") gum::BayesNet::generateCPT
"
Randomly generate CPT for a given node in a given structure.

Parameters
----------
node : Union[int,str]
	a variable's id (int) or name
"

%feature("docstring") gum::BayesNet::generateCPTs
"
Randomly generates CPTs for a given structure.
"

%feature("docstring") gum::BayesNet::loadBIF
"
Load a BIF file.

Parameters
----------
name : str
	the file's name
l : list
	list of functions to execute

Raises
--------
pyagrum.IOError
    If file not found
pyagrum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadBIFXML
"
Load a BIFXML file.

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

%feature("docstring") gum::BayesNet::loadDSL
"
Load a DSL file.

Parameters
----------
name : str
	the file's name
l : list
	list of functions to execute

Raises
------
pyagrum.IOError
    If file not found
pyagrum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadXDSL
"
Load a XDSL file.

Parameters
----------
name : str
	the file's name
l : list
	list of functions to execute

Raises
------
pyagrum.IOError
    If file not found
pyagrum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadNET
"
Load a NET file.

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

%feature("docstring") gum::BayesNet::loadO3PRM
"
Load an O3PRM file.

Warnings
--------
The O3PRM language is the only language allowing to manipulate not only DiscretizedVariable but also RangeVariable and LabelizedVariable.

Parameters
----------
name : str
	the file's name
system : str
	the system's name
classpath : str
	the classpath
l : list
	list of functions to execute

Raises
------
pyagrum.IOError
    If file not found
pyagrum.FatalError
    If file is not valid
"

%feature("docstring") gum::BayesNet::loadUAI
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

%feature("docstring") gum::BayesNet::reverseArc
"
Reverses an arc while preserving the same joint distribution.

Parameters
----------
tail
	(int) the id of the tail variable
head
	(int) the id of the head variable
tail
	(str) the name of the tail variable
head
	(str) the name of the head variable
arc : pyagrum.Arc
	an arc

Raises
------
pyagrum.InvalidArc
    If the arc does not exsit or if its reversal would induce a directed cycle.
"

%feature("docstring") gum::BayesNet::saveBIF
"
Save the BayesNet in a BIF file.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        False by default.
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::saveBIFXML
"
Save the BayesNet in a BIFXML file.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        False by default.
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::saveDSL
"
Save the BayesNet in a DSL file.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        False by default.
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::saveXDSL
"
Save the BayesNet in a XDSL file.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        (not used).
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::saveNET
"
Save the BayesNet in a NET file.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        False by default.
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::saveO3PRM
"
Save the BayesNet in an O3PRM file.

Warnings
--------
The O3PRM language is the only language allowing to manipulate not only DiscretizedVariable but also RangeVariable and LabelizedVariable.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        False by default.
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::saveUAI
"
Save the BayesNet in an UAI file.

Parameters
----------
name : str
	the file's name
allowModificationWhenSaving: bool
        False by default.
        if true, syntax errors are corrected when saving the file. If false, they throw a FatalError.
"

%feature("docstring") gum::BayesNet::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId :  Union[int,str]
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

%feature("docstring") gum::IBayesNet::check
"
Check if the BayesNet is consistent (variables, CPT, ...)

Returns
-------
List[str]
  list of found issues
"

%feature("docstring") gum::IBayesNet::memoryFootprint
"
get the size (in byte) of the (main footprint) of the BayesNet

Returns
-------
int
  the size in byte of the representation (of the parameters) of the BayesNet
"


%feature("docstring") gum::IBayesNet::evEq
"
This method is used to set an observation on a quasi-continuous variables (:class:`pyagrum
.DiscretizedVariable` with a large number of ticks)  that the variable is equal to a given value.

Note
----
  - see also :meth:`Tensor.evEq`
  - see also :meth:`BayesNet.evGt`, :meth:`BayesNet.evLt`, :meth:`BayesNet.evIn`

Examples
--------
>>> import pyagrum as gum
>>> bn = gum.fastBN('A[0:10:20]'') # DiscretizedVariable from 0 to 10 in 20 steps
>>> print(bn.evEqu('A',5)
  A                                                                                                                                                                                                    |
[0;0.5[  |[0.5;1[  |[1;1.5[  |[1.5;2[  |[2;2.5[  |[2.5;3[  |[3;3.5[  |[3.5;4[  |[4;4.5[  |[4.5;5[  |[5;5.5[  |[5.5;6[  |[6;6.5[  |[6.5;7[  |[7;7.5[  |[7.5;8[  |[8;8.5[  |[8.5;9[  |[9;9.5[  |[9.5;10] |
---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|
 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 1.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  |

Parameters
----------
var : Union[int,str]
  the current name or the id of the variable
value: float

Returns
-------
pyAgrum.Tensor
  The representation of the evidence as a :class:`~pyagrum.Tensor`.
"

%feature("docstring") gum::IBayesNet::evGt
"
This method is used to set an obvervation on a quasi-continuous variables (:class:`pyagrum
.DiscretizedVariable` with a large number of ticks) that the variable greater than a given value.

Note
----
  - see also :meth:`Tensor.evGt`
  - see also :meth:`BayesNet.evEq`, :meth:`BayesNet.evLt`, :meth:`BayesNet.evIn`

Examples
--------
>>> import pyagrum as gum
>>> bn = gum.fastBN('A[0:10:20]'') # DiscretizedVariable from 0 to 10 in 20 steps
>>> print(bn.eGt('A',5)
  A                                                                                                                                                                                                    |
[0;0.5[  |[0.5;1[  |[1;1.5[  |[1.5;2[  |[2;2.5[  |[2.5;3[  |[3;3.5[  |[3.5;4[  |[4;4.5[  |[4.5;5[  |[5;5.5[  |[5.5;6[  |[6;6.5[  |[6.5;7[  |[7;7.5[  |[7.5;8[  |[8;8.5[  |[8.5;9[  |[9;9.5[  |[9.5;10] |
---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|---------|
 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 0.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  | 1.0000  |

Parameters
----------
var : Union[int,str]
  the current name or the id of the variable
value: float

Returns
-------
pyAgrum.Tensor
  The representation of the evidence as a :class:`~pyagrum.Tensor`.
"


%feature("docstring") gum::IBayesNet::evLt
"
This method is used to set an obvervation on a quasi-continuous variables (:class:`pyagrum
.DiscretizedVariable` with a large number of ticks) that the variable is less than a given value.

Note
----
  - see also :meth:`Tensor.evLt`
  - see also :meth:`BayesNet.evEq`, :meth:`BayesNet.evGt`, :meth:`BayesNet.evIn`

Parameters
----------
var : Union[int,str]
  the current name or the id of the variable
value: float

Returns
-------
pyAgrum.Tensor
  The representation of the evidence as a :class:`~pyagrum.Tensor`.
"


%feature("docstring") gum::IBayesNet::evIn
"
This method is used to set an obvervation on a quasi-continuous variables (:class:`pyagrum
.DiscretizedVariable` with a large number of ticks) that the variable is less than a given value.

Note
----
  - see also :meth:`Tensor.evIn`
  - see also :meth:`BayesNet.evEq`, :meth:`BayesNet.evGt`, :meth:`BayesNet.evLt`

Parameters
----------
var : Union[int,str]
  the current name or the id of the variable
valueMin: float
  the minimum value
valueMax: float
  the maximum value

Returns
-------
pyagrum.Tensor
  The representation of the evidence as a :class:`~pyagrum.Tensor`.
"
