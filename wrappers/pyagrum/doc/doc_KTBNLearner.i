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



%feature("docstring") gum::learning::KTBNLearner
"
KTBNLearner learns a :class:`pyagrum.ktbn.KTBN` (structure and/or parameters),
at a fixed order k, from a set of trajectory CSV files (one file per trajectory,
as produced by :class:`pyagrum.ktbn.KTBNDatabaseGenerator`: one column per base
variable, one row per time step).

Internally, k-TBN learning is reduced to three ordinary Bayesian-network
learning problems, solved with the same score/algorithm/prior machinery as
:class:`pyagrum.BNLearner`, then glued back together: the **transition** table
(sliding windows of width k) learns the repeating kernel; the **initial** table
(the first k-1 time steps of each trajectory) learns the initial slices; the
**atemporal** table (one row per trajectory) learns the arcs between atemporal
variables. Temporal ordering and no-backward-arc constraints are applied
automatically.

Examples
--------
>>> import pyagrum.ktbn as ktbn
>>> # atemporal variables inferred from the data
>>> learner = ktbn.KTBNLearner(\"trajs/\", \"traj\", 500, 2)
>>> learner.useScoreBIC().useGreedyHillClimbing()
>>> model = learner.learnKTBN()
>>>
>>> # or state the classification explicitly (pass an actual `set`, not a list)
>>> learner2 = ktbn.KTBNLearner(\"trajs/\", \"traj\", 500, 2, {\"C\", \"D\"})

KTBNLearner(dirPath, csvBaseName, nbSamples, k, atemporalVars, missingSymbols=['?'], induceTypes=True, ignoreMissingSymbols=False) -> KTBNLearner
    Structure-learning constructor with the temporal/atemporal classification
    supplied explicitly.

    Parameters:
        - **dirPath** (*str*) -- directory holding the trajectory CSV files
        - **csvBaseName** (*str*) -- stem of each file name (1-based index and
          ``.csv`` appended, e.g. ``\"traj\"`` -> ``traj1.csv``, ``traj2.csv``, ...)
        - **nbSamples** (*int*) -- number of CSV files to read
        - **k** (*int*) -- order of the k-TBN; must be >= 2
        - **atemporalVars** (*set[str]*) -- base names of the atemporal (static)
          variables; every other name found in the CSV header is temporal. Must
          be an actual Python ``set`` (a ``list``/``tuple`` at this position
          instead selects the atemporal-inferring overload)
        - **missingSymbols** (*list[str]*) -- symbols in the CSVs interpreted as
          missing values
        - **induceTypes** (*bool*) -- retype all-numeric columns as
          integer/range/continuous instead of plain labels
        - **ignoreMissingSymbols** (*bool*) -- drop a row carrying a missing
          symbol instead of refusing the whole database (see
          :func:`nbDroppedRows` for the bias this introduces)

KTBNLearner(dirPath, csvBaseName, nbSamples, k, missingSymbols=['?'], induceTypes=True, ignoreMissingSymbols=False) -> KTBNLearner
    Same, with the temporal/atemporal classification inferred instead: a base
    variable is atemporal iff its value never changes across the rows of any
    single trajectory (a heuristic -- prefer the explicit form when the
    classification is already known).

KTBNLearner(dirPath, csvBaseName, nbSamples, k, bn, atemporalVars=set(), missingSymbols=['?'], ignoreMissingSymbols=False) -> KTBNLearner
    Variable-schema constructor: types and domains are supplied via a reference
    :class:`pyagrum.BayesNet` (one node per base variable, bare names, arcs
    ignored) instead of inferred from the CSVs. Use this when a variable's full
    domain is not guaranteed to appear in the first trajectory -- notably
    atemporal variables, which only ever show one value per trajectory.

    Parameters:
        - **bn** (*pyagrum.BayesNet*) -- a BayesNet with one node per base
          variable, providing the variable types and domains
"

%feature("docstring") gum::learning::KTBNLearner::learnKTBN
"
Learn the k-TBN's structure and CPTs.

Returns
-------
pyagrum.ktbn.KTBN
    the learned k-TBN
"

%feature("docstring") gum::learning::KTBNLearner::learnParameters
"
Learn only the CPTs, using the arc structure of an already-known k-TBN.

Parameters
----------
structure : pyagrum.ktbn.KTBN
    a k-TBN with the same base variables (names and domains) used to construct
    this learner; a mismatch raises at learn time
takeIntoAccountScore : bool
    whether to use the recorded score/prior when estimating parameters
    (default True)

Returns
-------
pyagrum.ktbn.KTBN
    a new k-TBN with structure's arcs and freshly learned CPTs
"

%feature("docstring") gum::learning::KTBNLearner::useScoreAIC
"
Use the AIC score for structure learning.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useScoreBD
"
Use the BD score for structure learning.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useScoreBDeu
"
Use the BDeu score for structure learning.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useScoreBIC
"
Use the BIC score for structure learning.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useScoreLog2Likelihood
"
Use the raw log2-likelihood score for structure learning.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useScoreMDL
"
Use the MDL score for structure learning.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useScorefNML
"
Use the fNML score for structure learning.
"

%feature("docstring") gum::learning::KTBNLearner::checkScorePriorCompatibility
"
Returns
-------
str
    a warning message if the current score and prior are incompatible, an
    empty string otherwise
"

%feature("docstring") gum::learning::KTBNLearner::useGreedyHillClimbing
"
Use greedy hill-climbing for structure search.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useExtendedGreedyHillClimbing
"
Use greedy hill-climbing extended with arc-reversal moves.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useLocalSearchWithTabuList
"
Use local search with a tabu list.

Parameters
----------
tabu_size : int
    the tabu list size (default 100)
nb_decrease : int
    the number of non-improving moves tolerated before stopping (default 2)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useMIIC
"
Use the constraint-based MIIC algorithm for structure search.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useNMLCorrection
"
Use the NML correction for MIIC's independence tests.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useMDLCorrection
"
Use the MDL correction for MIIC's independence tests.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::useNoCorrection
"
Disable correction for MIIC's independence tests.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::latentVariables
"
Returns
-------
tuple[tuple[str, str]]
    (tail, head) engine-name pairs of arcs MIIC flagged as hiding a latent
    variable (merged from the three internal learners); empty when the
    algorithm is not MIIC
"

%feature("docstring") gum::learning::KTBNLearner::useSmoothingPrior
"
Use a Laplace/BDeu-style smoothing prior.

Parameters
----------
weight : float
    the prior weight (default 1.0)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addForbiddenArc
"
Forbid an arc from ever appearing in the learned structure.

Parameters
----------
tailNode, headNode : str
    engine names (e.g. ``\"X[1]\"``, ``\"C\"``) of the two endpoints
tailBase, headBase : str
    alternatively, base names of the two endpoints (used together with the
    slices below)
tailSlice, headSlice : int
    slices of the endpoints (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for static)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::eraseForbiddenArc
"
Undo a previous :func:`addForbiddenArc`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addMandatoryArc
"
Force an arc to be part of the learned structure.

Parameters
----------
tailNode, headNode : str
    engine names of the two endpoints
tailBase, headBase : str
    alternatively, base names of the two endpoints (used together with the
    slices below)
tailSlice, headSlice : int
    slices of the endpoints (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for static)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::eraseMandatoryArc
"
Undo a previous :func:`addMandatoryArc`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addForbiddenIntraSliceArc
"
Forbid tailBase -> headBase at every intra-slice position (i.e. for every slice
t, tailBase[t] -> headBase[t]).

Parameters
----------
tailBase, headBase : str
    base names of the two temporal variables

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining

Raises
------
pyagrum.InvalidArgument
    if either endpoint is unknown or atemporal
"

%feature("docstring") gum::learning::KTBNLearner::eraseForbiddenIntraSliceArc
"
Undo a previous :func:`addForbiddenIntraSliceArc`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addForbiddenArcAllSlices
"
Forbid tailBase -> headBase at every causally-possible slice pair (every lag):
tailBase can never be an ancestor of headBase in the learned k-TBN.

Parameters
----------
tailBase, headBase : str
    base names of the two variables

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::eraseForbiddenArcAllSlices
"
Undo a previous :func:`addForbiddenArcAllSlices`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addNoParentNode
"
Declare a node as a root (forbid it from having any parent).

Parameters
----------
base : str
    base name of the node (used together with slice)
slice : int
    slice of the node (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for a static node)
name : str
    alternatively, the node's engine name (e.g. ``\"X[2]\"``, ``\"C\"``)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::eraseNoParentNode
"
Undo a previous :func:`addNoParentNode`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addNoChildrenNode
"
Declare a node as a leaf (forbid it from having any child).

Parameters
----------
base : str
    base name of the node (used together with slice)
slice : int
    slice of the node (:attr:`pyagrum.ktbn.KTBN.ATEMPORAL` for a static node)
name : str
    alternatively, the node's engine name

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::eraseNoChildrenNode
"
Undo a previous :func:`addNoChildrenNode`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::addPossibleEdge
"
Add a candidate edge for MIIC: once at least one edge has been listed, only
explicitly listed edges are explored by the structure search.

Parameters
----------
tail, head : str
    engine names of the two endpoints
tailBase, headBase : str
    alternatively, base names of the two endpoints (used together with the
    slices below)
tailSlice, headSlice : int
    slices of the endpoints

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::erasePossibleEdge
"
Undo a previous :func:`addPossibleEdge`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::allowArcAdditions
"
Allow or forbid arc additions during structure search.

Parameters
----------
allow : bool
    whether to allow arc additions (default True)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::allowArcDeletions
"
Allow or forbid arc deletions during structure search.

Parameters
----------
allow : bool
    whether to allow arc deletions (default True)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::allowArcReversals
"
Allow or forbid arc reversals during structure search.

Parameters
----------
allow : bool
    whether to allow arc reversals (default True)

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::setMaxIndegree
"
Cap the number of parents of any single node.

Parameters
----------
max_indegree : int
    the maximum in-degree

Returns
-------
pyagrum.ktbn.KTBNLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNLearner::k
"
Returns
-------
int
    the order k of the k-TBN being learned
"

%feature("docstring") gum::learning::KTBNLearner::nbCols
"
Returns
-------
int
    the number of base variable columns (temporal + atemporal)
"

%feature("docstring") gum::learning::KTBNLearner::nbRows
"
Returns
-------
tuple[int]
    the number of time steps in each trajectory CSV (one entry per sample, in
    load order); the raw trajectory length, not the transition table's
    sliding-window row count
"

%feature("docstring") gum::learning::KTBNLearner::isConstraintBased
"
Returns
-------
bool
    True if the current structure-learning algorithm is constraint-based (MIIC)
"

%feature("docstring") gum::learning::KTBNLearner::isScoreBased
"
Returns
-------
bool
    True if the current structure-learning algorithm is score-based (BIC, AIC, ...)
"

%feature("docstring") gum::learning::KTBNLearner::toString
"
Returns
-------
str
    a human-readable summary of the learner's current configuration
"

%feature("docstring") gum::learning::KTBNLearner::state
"
Returns
-------
tuple[tuple[str, str, str]]
    the settings, as (key, value, comment) tuples
"

%feature("docstring") gum::learning::KTBNLearner::copyState
"
Copy all score/algorithm/prior/constraint settings from another KTBNLearner
(does not copy the database).

Parameters
----------
learner : pyagrum.ktbn.KTBNLearner
    the learner to copy settings from
"

%feature("docstring") gum::learning::KTBNLearner::nbSamples
"
Returns
-------
int
    the number of trajectory CSV files loaded
"

%feature("docstring") gum::learning::KTBNLearner::hasMissingValues
"
Returns
-------
bool
    always False: incomplete rows are dropped by construction (see
    :func:`nbDroppedRows` to learn whether the CSVs actually had any)
"

%feature("docstring") gum::learning::KTBNLearner::nbDroppedRows
"
Number of rows dropped from the internal databases because they carried a
missing symbol.

.. warning::
   Dropping rows inflates the log-likelihood computed on the result, and does
   so more for larger k (a larger k spans more rows per window, so a single
   missing value costs more of them). Prefer complete trajectories whenever
   comparing likelihoods across models or orders.

Returns
-------
int
    the number of dropped rows, summed across the three internal tables
"

%feature("docstring") gum::learning::KTBNLearner::isIgnoringMissingSymbols
"
Returns
-------
bool
    True if incomplete rows are dropped rather than rejected outright
"

%feature("docstring") gum::learning::KTBNLearner::names
"
Returns
-------
tuple[str]
    base names (no slice suffix), one per base variable, in the original CSV
    header order
"

%feature("docstring") gum::learning::KTBNLearner::domainSizes
"
Returns
-------
tuple[int]
    domain sizes of the base variables, in the same order as :func:`names`
"

%feature("docstring") gum::learning::KTBNLearner::domainSize
"
Parameters
----------
base : str
    a base variable name (e.g. ``\"X\"``); an engine name (e.g. ``\"X[1]\"``) is
    also accepted

Returns
-------
int
    the domain size of base
"
