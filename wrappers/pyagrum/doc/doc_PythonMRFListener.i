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
Listener for the modifications of the structure of an undirected graphical model (Markov random field): the
functions set on the listener are called when a node or an edge is added or removed.

Notes
-----
    A listener is usually created and returned by `pyagrum.MarkovRandomField.addStructureListener`; the model keeps
    it alive. Several listeners can be added to the same model.

Parameters
----------
model : pyagrum.MarkovRandomField
    the model to listen to
vnm : object
    the correspondence between the ids and the names of the nodes of ``model`` (given by ``addStructureListener``)
"


%feature("docstring") PythonMRFListener::setWhenEdgeAdded
"
Set the function called when an edge is added (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (i: int, j: int) called when the edge between i and j is added (the order of i and j is not specified)

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"

%feature("docstring") PythonMRFListener::setWhenEdgeDeleted
"
Set the function called when an edge is removed (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (i: int, j: int) called when the edge between i and j is removed (the order of i and j is not specified)

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"

%feature("docstring") PythonMRFListener::setWhenNodeAdded
"
Set the function called when a node is added (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (id: int, name: str) called when the node is added

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"

%feature("docstring") PythonMRFListener::setWhenNodeDeleted
"
Set the function called when a node is removed (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (id: int) called when the node is removed

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
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
