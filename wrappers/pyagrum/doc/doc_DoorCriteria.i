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


%feature("docstring") gum::DoorCriteria
"
Utility class implementing the backdoor and frontdoor criteria on a causal DAG.

All methods are static and take the DAG as their first argument. This class is
stateless: it does not store any model.

A **backdoor adjustment set** Z between X and Y blocks all spurious (non-causal)
paths from X to Y while leaving all directed causal paths open.

A **frontdoor adjustment set** Z between X and Y intercepts every directed path
from X to Y, has no open backdoor path from X to Z, and all backdoor paths from
Z to Y are blocked by X.

Notes
-----
High-level search for a single valid set is available via
:meth:`pyagrum.causal.CausalModel.backDoor` and
:meth:`pyagrum.causal.CausalModel.frontDoor`.

Examples
--------
>>> import pyagrum as gum
>>> bn = gum.BayesNet.fastPrototype('X->Z->Y')
>>> dag = bn.dag()
>>> x, y, z = bn.idFromName('X'), bn.idFromName('Y'), bn.idFromName('Z')
>>> gum.DoorCriteria.satisfiesBackdoorCriterion(dag, x, y, set())
True
>>> gum.DoorCriteria.enumerateFrontdoorSets(dag, x, y)
[{z}]
"

%feature("docstring") gum::DoorCriteria::satisfiesBackdoorCriterion
"
Test whether Z satisfies the backdoor criterion for the effect of X on Y.

A set Z satisfies the backdoor criterion with respect to (X, Y) if:

  1. No node in Z is a descendant of X in the causal DAG.
  2. Z blocks every backdoor path between X and Y (paths that begin with an
     arc pointing into X).

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the treatment variable.
Y : int
    NodeId of the outcome variable.
Z : set of int
    Candidate adjustment set (NodeIds).

Returns
-------
bool
    True if Z satisfies the backdoor criterion for (X, Y).
"

%feature("docstring") gum::DoorCriteria::satisfiesFrontdoorCriterion
"
Test whether Z satisfies the frontdoor criterion for the effect of X on Y.

A set Z satisfies the frontdoor criterion with respect to (X, Y) if:

  1. Z intercepts all directed paths from X to Y.
  2. There are no unblocked backdoor paths from X to Z.
  3. All backdoor paths from Z to Y are blocked by X.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the treatment variable.
Y : int
    NodeId of the outcome variable.
Z : set of int
    Candidate mediator set (NodeIds).

Returns
-------
bool
    True if Z satisfies the frontdoor criterion for (X, Y).
"

%feature("docstring") gum::DoorCriteria::existsUnblockedDirectedPath
"
Test whether a directed path from X to Y exists that is not blocked by Z.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the source variable.
Y : int
    NodeId of the target variable.
Z : set of int
    Blocking set (NodeIds). A directed path is blocked if it passes through
    a node in Z.

Returns
-------
bool
    True if at least one unblocked directed path from X to Y exists.
"

%feature("docstring") gum::DoorCriteria::nodesOnDirectedPaths
"
Return the set of nodes lying on any directed path from X to Y.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the source variable.
Y : int
    NodeId of the target variable.

Returns
-------
set of int
    NodeIds of all nodes (including X and Y) that lie on at least one
    directed path from X to Y. Empty if no directed path exists.
"

%feature("docstring") gum::DoorCriteria::backdoorReach
"
Return all nodes reachable from X via a backdoor path.

A backdoor path starts with an arc pointing *into* X (i.e. it begins by
going to a parent of X) and then follows any sequence of edges.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the source variable.

Returns
-------
set of int
    NodeIds of all nodes reachable from X via a backdoor path.
"

%feature("docstring") gum::DoorCriteria::hasBackdoorPath
"
Test whether an open backdoor path from X to Y exists given evidence on Z.

Parameters
----------
dag : pyagrum.DAG
    The causal DAG.
X : int
    NodeId of the treatment variable.
Y : int
    NodeId of the outcome variable.
Z : set of int
    Conditioning set (NodeIds).

Returns
-------
bool
    True if an open backdoor path from X to Y exists after conditioning on Z.
"

// enumerateBackdoorSets and enumerateFrontdoorSets are documented in
// causal_after_templates.i alongside their %extend %pythoncode wrappers.
// %feature("docstring") here would be dead code: those methods are overridden
// at the Python level (required because %feature("shadow") does not work on
// overloaded methods), so their SWIG-level docstring is never reached.
