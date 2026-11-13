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







%feature("docstring") PythonBNListener
"
Listener for the modifications of the structure of a directed graphical model (Bayesian network, influence
diagram): the functions set on the listener are called when a node or an arc is added or removed.

Notes
-----
    A listener is usually created and returned by `pyagrum.BayesNet.addStructureListener` or
    `pyagrum.InfluenceDiagram.addStructureListener`; the model keeps it alive. Several listeners can be added to the
    same model.

Parameters
----------
model : pyagrum.BayesNet | pyagrum.InfluenceDiagram
    the model to listen to
vnm : object
    the correspondence between the ids and the names of the nodes of ``model`` (given by ``addStructureListener``)
"


%feature("docstring") PythonBNListener::setWhenArcAdded
"
Set the function called when an arc is added (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (tail: int, head: int) called when the arc (tail, head) is added

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"

%feature("docstring") PythonBNListener::setWhenArcDeleted
"
Set the function called when an arc is removed (it replaces the function previously set for this event on this listener).

Parameters
----------
pyfunc : callable
    a function (tail: int, head: int) called when the arc (tail, head) is removed

Raises
------
pyagrum.InvalidArgument
    if ``pyfunc`` is not callable
"

%feature("docstring") PythonBNListener::setWhenNodeAdded
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

%feature("docstring") PythonBNListener::setWhenNodeDeleted
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

%feature("docstring") PythonBNListener::whenArcAdded
"
Called by aGrUM when an arc is added to the model: calls the function given to ``setWhenArcAdded``.

Parameters
----------
source : object
    the object which sends the notification (not used)
src : int
    the id of the tail of the arc
dst : int
    the id of the head of the arc
"

%feature("docstring") PythonBNListener::whenArcDeleted
"
Called by aGrUM when an arc is removed from the model: calls the function given to ``setWhenArcDeleted``.

Parameters
----------
source : object
    the object which sends the notification (not used)
src : int
    the id of the tail of the arc
dst : int
    the id of the head of the arc
"

%feature("docstring") PythonBNListener::whenNodeAdded
"
Called by aGrUM when a node is added to the model: calls the function given to ``setWhenNodeAdded``
with the id and the name of the node.

Parameters
----------
source : object
    the object which sends the notification (not used)
id : int
    the id of the new node
"

%feature("docstring") PythonBNListener::whenNodeDeleted
"
Called by aGrUM when a node is removed from the model: calls the function given to ``setWhenNodeDeleted``.

Parameters
----------
source : object
    the object which sends the notification (not used)
id : int
    the id of the removed node
"
