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

%feature("docstring") PythonBNListener
"
Listener for Bayesian Network's modifications. This listener is notified when the structure of the BN is changed.

PythonBNListener(bn:gum.BayesNet,vnm:gum.VariableNodeMap) -> PythonBNListener
    default constructor

Note
----
    This class est mainly automatically instantiated using the method gum.BayesNet.addStructureListener.

Parameters
----------
bn : BaysNet
    The bayes net to listen to
vnm : VarNodeMap
    A translation unit between id of node and name of variable (usually : bn.variableNodeMap()).
"


%feature("docstring") PythonBNListener::setWhenArcAdded
"
Add the listener in parameter to the list of existing ones for adding an arc.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,j:int) called when when an arc (i,j) is added
"

%feature("docstring") PythonBNListener::setWhenArcDeleted
"
Add the listener in parameter to the list of existing ones for deleting an arc.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,j:int) called when when an arc (i,j) is removed
"

%feature("docstring") PythonBNListener::setWhenNodeAdded
"
Add the listener in parameter to the list of existing ones for adding a node.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,s:str) called when a node of id i and name s is added.
"

%feature("docstring") PythonBNListener::setWhenNodeDeleted
"
Add the listener in parameter to the list of existing ones for deleting an arc.

Parameters
----------
pyfunc : lambda expression
    a function (i:int) called when a node of id i and name s is removed.
"
