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



%feature("docstring") gum::learning::KTBNAdaptiveLearner
"
KTBNAdaptiveLearner is the order-selecting counterpart of
:class:`pyagrum.ktbn.KTBNLearner`: instead of being given the order k,
it explores every candidate k in [kMin, kMax] (kMin starts at 2 and is raised
automatically by structural constraints naming a concrete slice), learns one
k-TBN per candidate with an internal :class:`pyagrum.ktbn.KTBNLearner`,
and keeps the one with the best cross-k model-selection score (BIC by default).

It exposes the **same configuration interface** as KTBNLearner (score,
algorithm, prior, structural constraints -- see that class's docstrings, valid
here too), except settings are only *recorded* and replayed on each per-k
learner at :func:`learnKTBN` time; a constraint whose slice does not fit a given
candidate is silently skipped for that candidate only.

Examples
--------
>>> import pyagrum.ktbn as ktbn
>>> learner = ktbn.KTBNAdaptiveLearner(\"trajs/\", \"traj\", 500, kMax=4)
>>> learner.useScoreBIC().useGreedyHillClimbing()
>>> model = learner.learnKTBN()
>>> learner.bestK()
>>> learner.scorePerCandidateK()

KTBNAdaptiveLearner(dirPath, csvBaseName, nbSamples, kMax, atemporalVars, missingSymbols=['?'], induceTypes=True) -> KTBNAdaptiveLearner
    Parameters:
        - **dirPath** (*str*) -- directory holding the trajectory CSV files
        - **csvBaseName** (*str*) -- stem of each file name
        - **nbSamples** (*int*) -- number of CSV files to read
        - **kMax** (*int*) -- largest order to explore; must be >= 2
        - **atemporalVars** (*set[str]*) -- base names of the atemporal
          variables (must be an actual Python ``set``)
        - **missingSymbols** (*list[str]*) -- symbols in the CSVs interpreted
          as missing values
        - **induceTypes** (*bool*) -- retype all-numeric columns

KTBNAdaptiveLearner(dirPath, csvBaseName, nbSamples, kMax, missingSymbols=['?'], induceTypes=True) -> KTBNAdaptiveLearner
    Same, with the atemporal classification inferred from the data instead of
    supplied explicitly (see the equivalent KTBNLearner constructor).

KTBNAdaptiveLearner(dirPath, csvBaseName, nbSamples, kMax, bn, atemporalVars=set(), missingSymbols=['?']) -> KTBNAdaptiveLearner
    Variable-schema constructor: types and domains supplied via a reference
    :class:`pyagrum.BayesNet` instead of inferred from the CSVs (see the
    equivalent KTBNLearner constructor).
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::learnKTBN
"
Learn the best k in [kMin, kMax] together with the structure and the CPTs: one
internal :class:`pyagrum.ktbn.KTBNLearner` is built and run per
candidate, the recorded configuration is replayed on each, and the k-TBN with
the best cross-k score is returned.

Returns
-------
pyagrum.ktbn.KTBN
    the learned k-TBN, at the selected order
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::bestK
"
Returns
-------
int
    the order k selected by the last :func:`learnKTBN` call

Raises
------
pyagrum.OperationNotAllowed
    if learnKTBN has not run yet
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::scorePerCandidateK
"
Returns
-------
tuple[tuple[int, float]]
    (k, score) pairs for k = kMin..kMax in ascending k order, from the last
    learnKTBN call -- the values order selection compared to pick
    :func:`bestK` (higher is better; the argmax is bestK)

Raises
------
pyagrum.OperationNotAllowed
    if learnKTBN has not run yet
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::latentVariables
"
Returns
-------
tuple[tuple[str, str]]
    (tail, head) engine-name pairs of arcs the selected model's MIIC run
    flagged as hiding a latent variable; empty when the recorded algorithm is
    not MIIC or none were found

Raises
------
pyagrum.OperationNotAllowed
    if learnKTBN has not run yet
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::useOrderScoreBIC
"
Select the best k by BIC (the default): the candidate maximising
log2-likelihood minus half the parameter count times log2(sample size).

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::useOrderScoreAIC
"
Select the best k by AIC: a lighter, sample-size-independent complexity
penalty than BIC.

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::useOrderScorefNML
"
Select the best k by fNML (factorized Normalized Maximum Likelihood): a
data-dependent penalty (unlike BIC/AIC), matching aGrUM's ScorefNML.

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::ignoreMissingSymbols
"
Learn and score on the fully observed data only, dropping every row and
scoring instance that carries a missing symbol.

.. warning::
   This inflates the log-likelihood, and inflates it more for larger k -- so it
   biases the very order selection this class performs. Inspect
   :func:`scorePerCandidateK` rather than trusting :func:`bestK` alone when
   missing values are frequent.

Parameters
----------
ignore : bool
    whether to drop incomplete rows/instances (default True)

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::isIgnoringMissingSymbols
"
Returns
-------
bool
    True if incomplete rows/instances are dropped (default False)
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::kMax
"
Returns
-------
int
    the largest order explored (the kMax constructor argument)
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::checkScorePriorCompatibility
"
Data-free check: evaluates the recorded (score, prior) pair as
:class:`pyagrum.ktbn.KTBNLearner` would, so an incompatible
combination can be caught before learnKTBN reads any trajectory.

Returns
-------
str
    a warning message if the current score and prior are incompatible, an
    empty string otherwise
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::toString
"
Returns
-------
str
    a human-readable summary of the recorded configuration (candidate order
    range, algorithm/score/correction/prior, structural constraints), plus the
    selected k once learnKTBN has run
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::state
"
Returns
-------
tuple[tuple[str, str, str]]
    the recorded configuration, as (key, value, comment) tuples
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::addForbiddenKernelArc
"
Forbid an arc from tailBase, lag slices before the kernel, to headBase in the
kernel: tailBase at slice k-1-lag -> headBase at slice k-1, for whichever k is
selected. Unlike a plain (base,slice) constraint, the slice moves with the
candidate, so it must be expressed relative to the kernel. Adaptive-only: the
fixed-k :class:`pyagrum.ktbn.KTBNLearner` has no moving kernel slice to
anchor it to.

Parameters
----------
tailBase, headBase : str
    base names of the two temporal variables
lag : int
    the lag before the kernel slice, in [0, kMax)

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::eraseForbiddenKernelArc
"
Undo a previous :func:`addForbiddenKernelArc`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::addMandatoryKernelArc
"
Force an arc from tailBase, lag slices before the kernel, to headBase in the
kernel. See :func:`addForbiddenKernelArc` for the kernel-relative convention.

Parameters
----------
tailBase, headBase : str
    base names of the two temporal variables
lag : int
    the lag before the kernel slice, in [0, kMax)

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"

%feature("docstring") gum::learning::KTBNAdaptiveLearner::eraseMandatoryKernelArc
"
Undo a previous :func:`addMandatoryKernelArc`, same arguments.

Returns
-------
pyagrum.ktbn.KTBNAdaptiveLearner
    self, for chaining
"
