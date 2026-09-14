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



%feature("docstring") gum::KTBNInference
"
KTBNInference answers exact queries on a :class:`pyagrum.ktbn.KTBN` under any mix
of **observations** (soft or hard evidence) and hard **interventions**
(Pearl's do-operator), returning P(target[t] | obs, do(...)) for every declared
target at every time slice, without ever unrolling the model.

An **observation** is conditioning: it revises the whole network, ancestors
included. An **intervention** do(V[t]=v) is surgery: V[t] is cut from its causes
and forced to v, so the effect only reaches its descendants. Both may be
combined, and a node may carry both.

.. warning::
   The k-TBN is referenced, not copied: it must outlive the inference engine.

Examples
--------
>>> import pyagrum.ktbn as ktbn
>>> ie = ktbn.KTBNInference(model)
>>> ie.addObservation(\"Y\", 3, 1)      # observe Y[3] = 1
>>> ie.addIntervention(\"X\", 2, 1)     # do(X[2] = 1)
>>> ie.addTarget(\"X\")                 # want the whole P(X[t] | ...) series
>>> ie.makeInference(10)               # compute slices 0..9
>>> p3 = ie.posterior(\"X\", 3)
>>> px = ie.posteriors(\"X\")
>>> logpe = ie.logObservationProbability()

KTBNInference(ktbn) -> KTBNInference
    Parameters:
        - **ktbn** (*pyagrum.ktbn.KTBN*) -- the k-TBN to reason about
          (referenced, not copied)

Raises
------
pyagrum.InvalidArgument
    if ktbn is None
"

%feature("docstring") gum::KTBNInference::addIntervention
"
Record a hard intervention do(base[slice] = value). Recorded only; applied at
:func:`makeInference`. Re-intervening on the same node overwrites.

The batch form takes a dict {node: value}, node keyed by an engine name (e.g.
``\"X[5]\"``) or a ``(base, slice)`` tuple, all-or-nothing validated before
anything is recorded.

Examples
--------
>>> ie.addIntervention(\"X\", 5, 0)                       # by (base, slice), index
>>> ie.addIntervention(\"X\", 5, \"high\")                  # by (base, slice), label
>>> ie.addIntervention(\"X[5]\", 0)                        # by engine name
>>> ie.addIntervention({\"X[5]\": 0, (\"Z\", 2): \"high\"})    # batch

Parameters
----------
base : str
    base name of the target (used together with slice)
slice : int
    slice of the target (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for a static node)
node_name : str
    alternatively, the target's engine name
value : int or str
    the forced modality, as an index or a label
interventions : dict
    for the batch form, one node -> value entry per intervention

Raises
------
pyagrum.NotFound
    if the node is unknown, or value is not a legal modality
pyagrum.InvalidArgument
    if the slice is invalid for the node's kind
pyagrum.OutOfBounds
    if a modality index is out of range
"

%feature("docstring") gum::KTBNInference::eraseIntervention
"
Remove a recorded intervention (silent no-op if absent).

Parameters
----------
base : str
    base name of the node (used together with slice)
slice : int
    slice of the node
node_name : str
    alternatively, the node's engine name
"

%feature("docstring") gum::KTBNInference::clearInterventions
"
Remove all recorded interventions.
"

%feature("docstring") gum::KTBNInference::hasIntervention
"
Parameters
----------
base : str
    base name of the node (used together with slice)
slice : int
    slice of the node
node_name : str
    alternatively, the node's engine name

Returns
-------
bool
    True if the node carries an intervention
"

%feature("docstring") gum::KTBNInference::addObservation
"
Record an observation on base[slice]: a **hard** observation base[slice]=value
(conditioning: revises the whole network, ancestors included), or a **soft**
(likelihood) observation via a list of per-modality likelihoods (a one-hot
vector is equivalent to a hard observation). Recorded only; applied at
:func:`makeInference`. Re-observing the same node overwrites.

The batch form (``addObservations``) takes a dict {node: value} of hard
observations only, node keyed by an engine name or a (base, slice) tuple,
all-or-nothing validated before anything is recorded.

Examples
--------
>>> ie.addObservation(\"X\", 5, 0)                 # hard, by index
>>> ie.addObservation(\"X\", 5, \"high\")            # hard, by label
>>> ie.addObservation(\"X\", 5, [0.1, 0.9])         # soft (likelihoods)
>>> ie.addObservations({\"X[5]\": 0, (\"Z\", 2): 1})  # batch, hard only

Parameters
----------
base : str
    base name of the target (used together with slice)
slice : int
    slice of the target (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for a static node)
node_name : str
    alternatively, the target's engine name
value : int or str
    for a hard observation, the observed modality (index or label)
likelihood : list[float]
    for a soft observation, one non-negative, not-all-zero weight per modality
observations : dict
    for the batch form, one node -> value entry per hard observation

Raises
------
pyagrum.NotFound
    if the node is unknown, or value is not a legal modality
pyagrum.InvalidArgument
    if the slice is invalid, or likelihood has the wrong length, a negative
    entry, or is all zeros
"

%feature("docstring") gum::KTBNInference::eraseObservation
"
Remove a recorded observation (silent no-op if absent).

Parameters
----------
base : str
    base name of the node (used together with slice)
slice : int
    slice of the node
node_name : str
    alternatively, the node's engine name
"

%feature("docstring") gum::KTBNInference::clearObservation
"
Remove all recorded observations.
"

%feature("docstring") gum::KTBNInference::hasObservation
"
With no argument, tells whether any observation at all is recorded (when False,
:func:`makeInference` needs no backward pass and runs in horizon-independent
memory).

Parameters
----------
base : str, optional
    base name of the node (used together with slice)
slice : int, optional
    slice of the node
node_name : str, optional
    alternatively, the node's engine name

Returns
-------
bool
    True if the node (or, with no argument, any node) carries an observation
"

%feature("docstring") gum::KTBNInference::addTarget
"
Declare base as a target: a base variable whose marginals are wanted. The first
declared target switches the engine out of default-all-targets mode (with no
target declared, every base is a target). Recorded only; applied at
:func:`makeInference`.

Parameters
----------
base : str
    a temporal or atemporal base variable of the k-TBN

Raises
------
pyagrum.NotFound
    if base is neither a temporal nor an atemporal variable of the k-TBN
"

%feature("docstring") gum::KTBNInference::eraseTarget
"
Remove a target; when the last one is removed, default-all-targets mode is
restored.

Parameters
----------
base : str
    the base variable to un-target
"

%feature("docstring") gum::KTBNInference::clearTargets
"
Remove all targets (restores default-all-targets mode).
"

%feature("docstring") gum::KTBNInference::isTarget
"
Parameters
----------
base : str
    a base variable

Returns
-------
bool
    True if base is a target of the next inference
"

%feature("docstring") gum::KTBNInference::isInTargetMode
"
Returns
-------
bool
    True iff at least one explicit target has been declared; when False, every
    base is a target
"

%feature("docstring") gum::KTBNInference::makeInference
"
Run the interface algorithm over nbTimeSlices slices (0..nbTimeSlices-1) and
cache, for every targeted base, its marginal at every slice. Idempotent, and
called lazily by :func:`posterior`/:func:`posteriors` (with the last horizon)
whenever their cache is out of date.

Parameters
----------
nbTimeSlices : int
    the horizon: number of time slices to compute

Raises
------
pyagrum.InvalidArgument
    if nbTimeSlices is 0
pyagrum.FatalError
    if the recorded observations have probability 0 under the model
"

%feature("docstring") gum::KTBNInference::posterior
"
Return P(base[slice] | obs, do(...)). Lazily (re)runs :func:`makeInference` with
the last horizon if the cache is out of date. The returned reference is owned
by the engine and invalidated by the next :func:`makeInference` call: copy it to
keep it.

Parameters
----------
base : str
    base name of the queried node (used together with slice)
slice : int
    slice of the queried node (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for a static
    node)
node_name : str
    alternatively, the node's engine name

Returns
-------
pyagrum.Tensor
    the posterior distribution

Raises
------
pyagrum.OperationNotAllowed
    if makeInference has never been run
pyagrum.UndefinedElement
    if base is not a target
pyagrum.OutOfBounds
    if slice was not computed (>= the horizon)
"

%feature("docstring") gum::KTBNInference::posteriors
"
Return the whole marginal time-series of a targeted base: one
:class:`pyagrum.Tensor` per time slice (a single-element list, holding the
atemporal marginal, for an atemporal base). Lazily (re)runs
:func:`makeInference` if the cache is out of date.

Parameters
----------
base : str
    the base variable

Returns
-------
list[pyagrum.Tensor]
    the marginal at every computed slice

Raises
------
pyagrum.OperationNotAllowed
    if makeInference has never been run
pyagrum.UndefinedElement
    if base is not a target
"

%feature("docstring") gum::KTBNInference::logObservationProbability
"
Return log2 P(obs | do(...)) for the last run: the log-likelihood of the
recorded observations under the (possibly mutilated) model. 0 when nothing is
observed. Lazily (re)runs :func:`makeInference` if the cache is out of date.

Returns
-------
float
    the log2-probability of the observations

Raises
------
pyagrum.OperationNotAllowed
    if makeInference has never been run
"

%feature("docstring") gum::KTBNInference::observationProbability
"
Return P(obs | do(...)), i.e. 2 raised to :func:`logObservationProbability`.
Prefer the log form on long horizons, where this one underflows to 0.

Returns
-------
float
    the probability of the observations
"

%feature("docstring") gum::KTBNInference::ktbn
"
Returns
-------
pyagrum.ktbn.KTBN
    the k-TBN this engine reasons about
"

%feature("docstring") gum::KTBNInference::toString
"
Returns
-------
str
    a human-readable summary of the engine's state
"

%feature("docstring") gum::KTBNInference::windowJunctionTree
"
Introspection only: the junction tree of the repeating window, compiled once
from the k-slice template and re-entered at every step from slice k-1 on.

Returns
-------
pyagrum.JunctionTree
    the window's junction tree
"

%feature("docstring") gum::KTBNInference::interfaceSize
"
Introspection only: the size of the forward interface of the repeating window,
i.e. how many node occurrences have to cross each slice boundary.

Returns
-------
int
    the interface size
"
