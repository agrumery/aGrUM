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



%feature("docstring") gum::learning::KTBNDatabaseGenerator
"
KTBNDatabaseGenerator generates a database of trajectories from a
:class:`pyagrum.ktbn.KTBN`, one CSV file per trajectory. Unlike a plain Bayesian
network, a k-TBN describes a stochastic *process*, so its database is a set of
trajectories rather than a flat table of i.i.d. rows: each CSV file has one
column per base variable and T data rows, an atemporal variable keeping the same
value across all T rows.

Sampling follows the same forward (ancestral) sampling principle as pyAgrum's
Bayesian-network database generator, but never unrolls the k-TBN: it keeps only
the k-slice template and slides it forward in time, so memory stays independent
of the number of trajectories and their length.

Examples
--------
>>> import pyagrum.ktbn as ktbn
>>> kdbn = ...                                          # a k-TBN
>>> gen = ktbn.KTBNDatabaseGenerator(kdbn)
>>> # 1000 trajectories of length 10 -> out_dir/traj1.csv ... out_dir/traj1000.csv
>>> ll = gen.drawSamples(1000, 10, \"out_dir\", \"traj\")

KTBNDatabaseGenerator(kdbn) -> KTBNDatabaseGenerator
    Parameters:
        - **kdbn** (*pyagrum.ktbn.KTBN*) -- the k-TBN to sample from (only its
          k-slice template is copied; the k-TBN itself is not retained)
"

%feature("docstring") gum::learning::KTBNDatabaseGenerator::drawSamples
"
Generate trajectories, writing one CSV file per trajectory into dirPath. File
names are csvBaseName followed by the 1-based trajectory index and ``.csv``
(e.g. ``traj1.csv``, ``traj2.csv``, ...). Either every trajectory shares the
same horizon, or each trajectory gets its own (pass a list of horizons instead
of a sample count and a single horizon).

Examples
--------
>>> gen.drawSamples(100, 10, \"out_dir\", \"traj\")            # 100 trajectories, length 10
>>> gen.drawSamples([8, 10, 12], \"out_dir\", \"traj\")        # 3 trajectories, own lengths

Parameters
----------
nbSamples : int
    the number of trajectories to generate (fixed-length form)
nbTimeSlices : int or list[int]
    the horizon shared by every trajectory (fixed-length form), or a list
    giving each trajectory's own horizon (variable-length form); every value
    must be >= k
dirPath : str
    directory to write the CSV files into
csvBaseName : str
    stem for each file name (index and .csv appended)
mode : pyagrum.ktbn.KTBNDatabaseGenerator.VarOrderMode
    the column order of the base variables (default: RANDOM)
useLabels : bool
    render values as variable labels rather than modality indices (default True)
csvSeparator : str
    the column separator (must not contain a newline)

Returns
-------
tuple[float]
    the log2-likelihood of each generated trajectory

Raises
------
pyagrum.OperationNotAllowed
    if a horizon is smaller than k
"

%feature("docstring") gum::learning::KTBNDatabaseGenerator::setDiscretizedLabelModeRandom
"
Set discretized-variable label rendering to a uniform random draw within the
interval (the default; each labelled export then differs).
"

%feature("docstring") gum::learning::KTBNDatabaseGenerator::setDiscretizedLabelModeMedian
"
Set discretized-variable label rendering to the (deterministic) interval median.
"

%feature("docstring") gum::learning::KTBNDatabaseGenerator::setDiscretizedLabelModeInterval
"
Set discretized-variable label rendering to the interval label, e.g. ``\"[min,max[\"``.
"

%feature("docstring") gum::learning::KTBNDatabaseGenerator::nbVars
"
Returns
-------
int
    the number of base variable columns
"
