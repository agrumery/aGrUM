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




%feature("docstring") gum::IBayesNet
"
Abstract class used by BayesNet.
"

%feature("docstring") gum::IBayesNet::dim
"
Returns the dimension (the number of free parameters) in this BayesNet.

Returns
-------
int
	the dimension of the BayesNet
"

%feature("docstring") gum::IBayesNet::jointProbability
"
Parameters
----------
i : pyagrum.instantiation
	an instantiation of the variables

Returns
-------
float
	a parameter of the joint probability for the BayesNet

Warnings
--------
a variable not present in the instantiation is assumed to be instantiated to 0
"

%feature("docstring") gum::IBayesNet::log2JointProbability
"
Parameters
----------
i : pyagrum.instantiation
	an instantiation of the variables

Returns
-------
float
	a parameter of the log joint probability for the BayesNet

Warnings
--------
a variable not present in the instantiation is assumed to be instantiated to 0
"

%feature("docstring") gum::IBayesNet::maxNonOneParam
"
Returns
-------
float
	The biggest value (not equal to 1) in the CPTs of the BayesNet
"

%feature("docstring") gum::IBayesNet::maxParam
"
Returns
-------
float
    the biggest value in the CPTs of the BayesNet
"

%feature("docstring") gum::IBayesNet::maxVarDomainSize
"
Returns
-------
int
	the biggest domain size among the variables of the BayesNet
"

%feature("docstring") gum::IBayesNet::minNonZeroParam
"
Returns
-------
float
    the smallest value (not equal to 0) in the CPTs of the IBayesNet
"

%feature("docstring") gum::IBayesNet::minParam
"
Returns
-------
float
    the smallest value in the CPTs of the IBayesNet
"

%feature("docstring") gum::IBayesNet::toDot
"
Returns
-------
str
    a friendly display of the graph in DOT format
"

%feature("docstring") gum::IBayesNet::arcs
"
Returns
-------
list
	The lisf of arcs in the IBayesNet
"

%feature("docstring") gum::IBayesNet::children
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

%feature("docstring") gum::IBayesNet::cpt
"
Returns the CPT of a variable.

Parameters
----------
VarId : int
	A variable's id in the pyagrum.IBayesNet.
name : str
	A variable's name in the pyagrum.IBayesNet.

Returns
-------
pyagrum.Tensor
	The variable's CPT.

Raises
------
pyagrum.NotFound
	If no variable's id matches varId.
"

%feature("docstring") gum::IBayesNet::parents
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

%feature("docstring") gum::IBayesNet::nodes
"
Returns
-------
Set[int]
    the set of ids
"

%feature("docstring") gum::IBayesNet::minimalCondSet
"
Returns, given one or many targets and a list of variables, the minimal set of those needed to calculate the target/targets.

Parameters
----------
target : int
	The id of the target
targets : List[int]
	The ids of the targets
list : List[int]
	The list of available variables

Returns
-------
Set[int]
	The minimal set of variables
"
