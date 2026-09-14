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



%feature("docstring") gum::KTBN
"
KTBN represents a k-order dynamic Bayesian network (k-TBN): a compact template of
k consecutive time slices that fully characterizes a time-homogeneous dynamic
process. It generalizes the classical 2-TBN by letting a variable at time t depend
on up to the k most recent time slices instead of a single step backward.

A KTBN distinguishes two kinds of variables:
    - **temporal** variables (processes), which evolve through time and have one
      instance per slice (added with :func:`addTemporal`);
    - **atemporal** variables, constant through time (e.g. a static context), which
      have a single instance (added with :func:`addAtemporal`).

Internally, nodes are addressed either by an engine name using bracket notation
(``\"X[2]\"`` for the 3rd slice of process ``X``, or a bare name such as ``\"C\"`` for
an atemporal variable), or, in most of the public API, by an explicit
``(base, slice)`` pair -- ``slice`` being :attr:`ATEMPORAL` (``-1``) for an atemporal
variable. Both spellings are accepted almost everywhere.

The template can be unrolled into a plain :class:`pyagrum.BayesNet` for any number
of time slices with :func:`unroll`.

KTBN(k=2) -> KTBN
    Parameters:
        - **k** (*int*) -- the order of the k-TBN (number of time slices in the
          template). Must be >= 1.

Raises
------
pyagrum.InvalidArgument
    If k is 0.
"

%feature("docstring") gum::KTBN::ATEMPORAL
"
Conventional slice value (``-1``) denoting an atemporal (static) variable, to be
used wherever a slice index is expected.
"

%feature("docstring") gum::KTBN::k
"
Returns
-------
int
    the order k of the k-TBN
"

%feature("docstring") gum::KTBN::size
"
Returns
-------
int
    the number of nodes in the template (all slices)
"

%feature("docstring") gum::KTBN::sizeArcs
"
Returns
-------
int
    the number of arcs in the template
"

%feature("docstring") gum::KTBN::empty
"
Returns
-------
bool
    True if the template contains no variable
"

%feature("docstring") gum::KTBN::clear
"
Remove all variables and arcs, keeping the order k.
"

%feature("docstring") gum::KTBN::add
"
Add a variable to the k-TBN.

A temporal variable (process) is cloned into k instances (one per slice); an
atemporal variable is added once.

Parameters
----------
var : pyagrum.DiscreteVariable
    the variable to add (added by copy)
temporal : bool
    whether the variable is temporal (default True)

Raises
------
pyagrum.DuplicateLabel
    if a variable with that base name already exists (of any kind -- base names
    are globally unique)
pyagrum.InvalidArgument
    if adding the variable would create a name collision between a temporal and
    an atemporal variable (e.g. an atemporal ``\"X[0]\"`` when a temporal process
    ``\"X\"`` already exists)
"

%feature("docstring") gum::KTBN::addTemporal
"
Shortcut for ``add(var, True)``: adds a temporal process.

Parameters
----------
var : pyagrum.DiscreteVariable
    the variable to add (added by copy)
"

%feature("docstring") gum::KTBN::addAtemporal
"
Shortcut for ``add(var, False)``: adds an atemporal (static) variable.

Parameters
----------
var : pyagrum.DiscreteVariable
    the variable to add (added by copy)
"

%feature("docstring") gum::KTBN::erase
"
Remove a variable (and all its incident arcs). For a temporal process, all k
slice nodes are removed. The kind (temporal or atemporal) is determined
automatically.

Parameters
----------
base : str
    the base name of the variable to remove

Raises
------
pyagrum.NotFound
    if no variable with that name exists
"

%feature("docstring") gum::KTBN::changeVariableName
"
Rename a variable (temporal process or atemporal variable). For a temporal
process, all k slice names are updated accordingly.

Parameters
----------
oldBase : str
    the current base name
newBase : str
    the new base name

Raises
------
pyagrum.NotFound
    if no variable named oldBase exists
pyagrum.DuplicateLabel
    if a variable named newBase already exists
pyagrum.InvalidArgument
    if newBase is empty or would create a name collision
"

%feature("docstring") gum::KTBN::exists
"
Parameters
----------
base : str
    a base name

Returns
-------
bool
    True if a variable with this base name exists
"

%feature("docstring") gum::KTBN::temporalVarNames
"
Returns
-------
set[str]
    the base names of the temporal processes
"

%feature("docstring") gum::KTBN::atemporalVarNames
"
Returns
-------
set[str]
    the base names of the atemporal variables
"

%feature("docstring") gum::KTBN::nbTemporalVars
"
Returns
-------
int
    the number of temporal processes
"

%feature("docstring") gum::KTBN::nbAtemporalVars
"
Returns
-------
int
    the number of atemporal variables
"

%feature("docstring") gum::KTBN::nodes
"
Returns
-------
tuple[tuple[str, int]]
    all nodes as (base, slice) pairs; an atemporal node uses :attr:`ATEMPORAL`
    as its slice
"

%feature("docstring") gum::KTBN::parents
"
Returns the parents of a node, as (base, slice) pairs.

Parameters
----------
base : str
    the base name of the node (used together with slice)
slice : int
    the slice of the node (:attr:`ATEMPORAL` for an atemporal node)
node_name : str
    alternatively, the node's engine name (e.g. ``\"X[1]\"``, or ``\"C\"``)

Returns
-------
tuple[tuple[str, int]]
    the parents, as (base, slice) pairs (:attr:`ATEMPORAL` if atemporal)
"

%feature("docstring") gum::KTBN::children
"
Returns the children of a node, as (base, slice) pairs.

Parameters
----------
base : str
    the base name of the node (used together with slice)
slice : int
    the slice of the node (:attr:`ATEMPORAL` for an atemporal node)
node_name : str
    alternatively, the node's engine name (e.g. ``\"X[1]\"``, or ``\"C\"``)

Returns
-------
tuple[tuple[str, int]]
    the children, as (base, slice) pairs (:attr:`ATEMPORAL` if atemporal)
"

%feature("docstring") gum::KTBN::variable
"
Returns the variable of a (process, slice) couple.

Parameters
----------
base : str
    the base name of the node
slice : int
    the slice of the node (:attr:`ATEMPORAL` for an atemporal node)
node_name : str
    alternatively, the node's engine name

Returns
-------
pyagrum.DiscreteVariable
    the variable

Raises
------
pyagrum.NotFound
    if no such variable exists
pyagrum.OutOfBounds
    if slice is out of range for a temporal variable
pyagrum.OperationNotAllowed
    if the temporal/atemporal kind does not match slice
"

%feature("docstring") gum::KTBN::timeSlice
"
Parameters
----------
var : pyagrum.DiscreteVariable
    a variable of this k-TBN

Returns
-------
int
    the time slice of var, or :attr:`ATEMPORAL` if it is atemporal

Raises
------
pyagrum.NotFound
    if var is not a node of this k-TBN
"

%feature("docstring") gum::KTBN::baseName
"
Parameters
----------
var : pyagrum.DiscreteVariable
    a variable of this k-TBN

Returns
-------
str
    the base name of var (without the bracket-encoded slice)

Raises
------
pyagrum.NotFound
    if var is not a node of this k-TBN
"

%feature("docstring") gum::KTBN::addArc
"
Add an arc between two (process, slice) endpoints.

Parameters
----------
tailBase, headBase : str
    base names of the tail and head variables
tailSlice, headSlice : int
    slices of the tail and head (:attr:`ATEMPORAL` for a static endpoint)
tail, head : str
    alternatively, engine names for both endpoints (bracket notation)

Raises
------
pyagrum.NotFound
    if an endpoint does not exist
pyagrum.OutOfBounds
    if a slice is out of range for a temporal endpoint
pyagrum.OperationNotAllowed
    if the arc violates temporal causality (a temporal endpoint pointing to an
    atemporal one, or an arc from a future slice to a past slice)
pyagrum.DuplicateElement
    if the arc already exists
pyagrum.InvalidDirectedCycle
    if the arc would create a cycle
"

%feature("docstring") gum::KTBN::eraseArc
"
Remove an arc between two (process, slice) endpoints.

Parameters
----------
tailBase, headBase : str
    base names of the tail and head variables
tailSlice, headSlice : int
    slices of the tail and head (:attr:`ATEMPORAL` for a static endpoint)
tail, head : str
    alternatively, engine names for both endpoints

Raises
------
pyagrum.NotFound
    if an endpoint, or the arc itself, does not exist
"

%feature("docstring") gum::KTBN::existsArc
"
Parameters
----------
tailBase, headBase : str
    base names of the tail and head variables
tailSlice, headSlice : int
    slices of the tail and head (:attr:`ATEMPORAL` for a static endpoint)
tail, head : str
    alternatively, engine names for both endpoints

Returns
-------
bool
    True if the arc exists
"

%feature("docstring") gum::KTBN::arcs
"
Returns
-------
tuple[tuple[tuple[str, int], tuple[str, int]]]
    all arcs, each as a (tail, head) pair of (base, slice) endpoints
"

%feature("docstring") gum::KTBN::cpt
"
Returns the conditional probability table of a (process, slice) couple. The
returned :class:`pyagrum.Tensor` is a live reference: use any of its methods
(e.g. ``fillWith``) to fill it, or use :func:`fillCPT` for an order-safe,
parent-name-based alternative.

Parameters
----------
base : str
    the base name of the node
slice : int
    the slice of the node (:attr:`ATEMPORAL` for an atemporal node)
node_name : str
    alternatively, the node's engine name

Returns
-------
pyagrum.Tensor
    the (mutable) conditional probability table
"

%feature("docstring") gum::KTBN::fillCPT
"
Fill one conditional distribution P(node | parent configuration), addressing the
node and its parents by their (base, slice) identity rather than by CPT
positional order.

Every parent of the node must be listed, in any order, as a dict entry keyed by
either an engine name (``\"X[1]\"``) or a ``(base, slice)`` tuple (use
:attr:`ATEMPORAL` as slice for a static parent). A parent's value is either its
modality index (int) or its modality label (str).

.. note::
   Key spellings may only be mixed in the same dict when the *target* node
   itself is addressed by its engine name (the ``node_name`` form below). When
   the target is addressed as ``(base, slice)``, every parent key must also be
   a ``(base, slice)`` tuple -- a plain engine-name key is then rejected.

.. warning::
   An unquoted integer is always a modality **index**, a quoted string always a
   modality **label**. The two differ for a variable whose modalities are
   themselves numeric.

Examples
--------
>>> m.fillCPT(\"C\", ktbn.KTBN.ATEMPORAL, {}, [0.4, 0.6])         # P(C), no parents
>>> m.fillCPT(\"X\", 1, {(\"X\", 0): 1, (\"C\", -1): 0}, [0.6, 0.4])  # P(X[1] | X[0]=1, C=0)
>>> m.fillCPT(\"X[1]\", {\"X[0]\": \"1\", \"C\": 0}, [0.6, 0.4])         # same, engine names

Parameters
----------
base : str
    base name of the target node (used together with slice)
slice : int
    slice of the target node (:attr:`ATEMPORAL` for a static node)
node_name : str
    alternatively, the target node's engine name
parents : dict
    one (engine-name or (base,slice)) -> (index or label) entry per parent
distribution : list[float]
    probabilities over the node's own modalities for that parent configuration;
    its length must equal the node's domain size

Raises
------
pyagrum.NotFound
    if the node or a parent does not exist, or if a modality label is unknown
pyagrum.OutOfBounds
    if a modality index is out of range
pyagrum.SizeError
    if distribution's length differs from the node's domain size, or if a
    parent is missing
pyagrum.InvalidArgument
    if a dict entry's node is not actually a parent of the target
"

%feature("docstring") gum::KTBN::generateCPTs
"
Randomly generate every CPT of the template.
"

%feature("docstring") gum::KTBN::generateCPT
"
Randomly generate the CPT of a single node.

Parameters
----------
base : str
    the base name of the node
slice : int
    the slice of the node (:attr:`ATEMPORAL` for an atemporal node)
node_name : str
    alternatively, the node's engine name
"

%feature("docstring") gum::KTBN::toBN
"
Returns
-------
pyagrum.BayesNet
    a deep copy of the underlying template, as a plain Bayesian network (nodes
    named with the bracket-encoded engine names)
"

%feature("docstring") gum::KTBN::unroll
"
Unroll the k-TBN into a standard Bayesian network with exactly nbTimeSlices time
slices. Slices 0..k-1 are copied verbatim from the template; every additional
slice replicates the transition kernel (slice k-1), shifting each temporal
parent's slice accordingly so that lags are preserved.

Parameters
----------
nbTimeSlices : int
    the total number of time slices of the unrolled network; must be >= k

Returns
-------
pyagrum.BayesNet
    the unrolled Bayesian network (nodes named ``base[slice]``, atemporal
    variables keeping their bare name)

Raises
------
pyagrum.OperationNotAllowed
    if nbTimeSlices < k
"

%feature("docstring") gum::KTBN::save
"
Save the k-TBN in a jgum (text/JSON) or bgum (binary/msgpack) file, selected by
the file extension (``.jgum`` for text, anything else for binary).

Parameters
----------
filename : str
    the destination file
"

%feature("docstring") gum::KTBN::load
"
Load a k-TBN from a file produced by :func:`save`, selected by the file
extension.

Parameters
----------
filename : str
    the GUM file to read

Returns
-------
pyagrum.ktbn.KTBN
    the loaded k-TBN

Raises
------
pyagrum.IOError
    if the file cannot be read or is not valid
"

%feature("docstring") gum::KTBN::fromBN
"
Build a k-TBN from an existing :class:`pyagrum.BayesNet` whose node names follow
the bracket notation (``base[t]`` for temporal, bare name for atemporal). The
order k is inferred as one plus the largest slice index found.

Parameters
----------
bn : pyagrum.BayesNet
    the source Bayesian network (copied)
atemporalNodes : set[str]
    node names to classify atemporal outright, to lift an ambiguity when a
    group of bracket-named nodes does not cover every slice (default: empty)

Returns
-------
pyagrum.ktbn.KTBN
    the reconstructed k-TBN

Raises
------
pyagrum.NotFound
    if a name in atemporalNodes is not a node of bn
pyagrum.OperationNotAllowed
    if the temporal structure of bn is inconsistent
"

%feature("docstring") gum::KTBN::toString
"
Returns
-------
str
    a human-readable description of the k-TBN
"

%feature("docstring") gum::KTBN::toDot
"
Returns
-------
str
    a Graphviz dot string with one cluster per time slice of the template
"

%feature("docstring") gum::KTBN::toUnrolledDot
"
Returns a Graphviz dot string of the k-TBN unrolled over T time slices, without
actually materializing the unrolled :class:`pyagrum.BayesNet`.

Parameters
----------
T : int
    total number of time slices to display; must be >= k
highlightReplicated : bool
    if True, shade the replicated slices (>= k) differently from the template
    slices (default False)

Returns
-------
str
    a Graphviz dot string

Raises
------
pyagrum.OperationNotAllowed
    if T < k
"

%feature("docstring") gum::KTBN::bnToDot
"
Returns
-------
str
    the Graphviz dot string of the underlying storage BayesNet, with nodes
    labelled by their internal engine names (no time-slice clustering)
"
