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







%feature("docstring") PythonMRFListener
"
Listener for the modifications of an undirected graphical model (Markov random field). This listener is notified
when the structure of the model is changed.

PythonMRFListener(model, vnm) -> PythonMRFListener

Notes
-----
    This class is mainly instantiated automatically by the method `pyagrum.MarkovRandomField.addStructureListener`.

Parameters
----------
model : pyagrum.MarkovRandomField
    The model to listen to
vnm : VariableNodeMap
    the correspondence between the ids of the nodes and the names of the variables (usually
    ``model._variableNodeMap()``)
"


%feature("docstring") PythonMRFListener::setWhenEdgeAdded
"
Add the listener in parameter to the list of existing ones for adding an edge.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,j:int) called when an edge (i,j) is added
"

%feature("docstring") PythonMRFListener::setWhenEdgeDeleted
"
Add the listener in parameter to the list of existing ones for deleting an edge.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,j:int) called when an edge (i,j) is removed
"

%feature("docstring") PythonMRFListener::setWhenNodeAdded
"
Add the listener in parameter to the list of existing ones for adding a node.

Parameters
----------
pyfunc : lambda expression
    a function (i:int,s:str) called when a node of id i and name s is added.
"

%feature("docstring") PythonMRFListener::setWhenNodeDeleted
"
Add the listener in parameter to the list of existing ones for deleting a node.

Parameters
----------
pyfunc : lambda expression
    a function (i:int) called when a node of id i is removed.
"

%feature("docstring") PythonMRFListener::whenEdgeAdded
"
Called by aGrUM when an edge is added to the Markov random field: calls the function given to ``setWhenEdgeAdded``.

Parameters
----------
source : object
    the object which sends the notification (not used)
n1 : int
    the id of one node of the edge
n2 : int
    the id of the other node of the edge
"

%feature("docstring") PythonMRFListener::whenEdgeDeleted
"
Called by aGrUM when an edge is removed from the Markov random field: calls the function given to
``setWhenEdgeDeleted``.

Parameters
----------
source : object
    the object which sends the notification (not used)
n1 : int
    the id of one node of the edge
n2 : int
    the id of the other node of the edge
"

%feature("docstring") PythonMRFListener::whenNodeAdded
"
Called by aGrUM when a node is added to the Markov random field: calls the function given to
``setWhenNodeAdded`` with the id and the name of the node.

Parameters
----------
source : object
    the object which sends the notification (not used)
id : int
    the id of the new node
"

%feature("docstring") PythonMRFListener::whenNodeDeleted
"
Called by aGrUM when a node is removed from the Markov random field: calls the function given to
``setWhenNodeDeleted``.

Parameters
----------
source : object
    the object which sends the notification (not used)
id : int
    the id of the removed node
"
