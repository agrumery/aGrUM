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



%feature("docstring") gum::KTBNGenerator
"
KTBNGenerator draws a random :class:`pyagrum.ktbn.KTBN` template: variables, a
legal arc set and (optionally) random CPTs. It is the k-TBN counterpart of
pyAgrum's Bayesian network generators, mainly used to build ground-truth models
for learning experiments: sample trajectories from a generated model, learn it
back, and compare.

Acyclicity comes for free: a cross-slice arc can never close a cycle since the
slice index strictly increases along it, so the generator only needs to avoid
cycles among same-slice (temporal, lag 0) and atemporal-to-atemporal arcs, which
it does by drawing one random ranking per family and only admitting arcs from
lower to higher rank.

Examples
--------
>>> import pyagrum as gum
>>> import pyagrum.ktbn as ktbn
>>> gum.initRandom(42)                          # reproducible
>>> gen = ktbn.KTBNGenerator(3, 4, 1)            # k=3, 4 temporal, 1 atemporal
>>> gen.setDensity(0.15).setMaxParents(4)
>>> model = gen.generate()

KTBNGenerator(k, nbTemporal, nbAtemporal=0, maxArcs=0, maxModality=2) -> KTBNGenerator
    Parameters:
        - **k** (*int*) -- order of the generated k-TBN (number of template
          slices); must be >= 1
        - **nbTemporal** (*int*) -- number of temporal processes
        - **nbAtemporal** (*int*) -- number of atemporal variables
        - **maxArcs** (*int*) -- hard cap on the number of arcs; 0 (default)
          derives it from the density instead (see :func:`setDensity`)
        - **maxModality** (*int*) -- largest domain size; domains are drawn
          uniformly in [2, maxModality]

Raises
------
pyagrum.InvalidArgument
    if k is 0 or maxModality is below 2
"

%feature("docstring") gum::KTBNGenerator::generateKTBN
"
Fill out with a freshly drawn model (its previous content is discarded). Seed
with ``pyagrum.initRandom()`` for reproducibility.

Parameters
----------
out : pyagrum.ktbn.KTBN
    the k-TBN to fill
"

%feature("docstring") gum::KTBNGenerator::generate
"
Returns
-------
pyagrum.ktbn.KTBN
    a freshly drawn model
"

%feature("docstring") gum::KTBNGenerator::setDensity
"
Set the fraction of the legal arc set to draw, in [0,1]. Ignored when a non-zero
maxArcs was given to the constructor. Default 0.1.

Parameters
----------
density : float
    the density, in [0,1]

Returns
-------
pyagrum.ktbn.KTBNGenerator
    self, for chaining

Raises
------
pyagrum.OutOfBounds
    if density is outside [0,1]
"

%feature("docstring") gum::KTBNGenerator::setDomainRange
"
Set the range domain sizes are drawn uniformly from.

Parameters
----------
minModality : int
    the minimum domain size (>= 2)
maxModality : int
    the maximum domain size (>= minModality)

Returns
-------
pyagrum.ktbn.KTBNGenerator
    self, for chaining

Raises
------
pyagrum.InvalidArgument
    if minModality < 2 or maxModality < minModality
"

%feature("docstring") gum::KTBNGenerator::setMaxParents
"
Cap the number of parents of any node, which bounds CPT size. 0 (the default)
means unlimited -- a dense draw can then produce very large CPTs, so set this
when generating dense or high-k models.

Parameters
----------
maxParents : int
    the maximum number of parents per node

Returns
-------
pyagrum.ktbn.KTBNGenerator
    self, for chaining
"

%feature("docstring") gum::KTBNGenerator::setGuaranteeOrder
"
Force one arc of lag k-1 into the kernel slice, so the generated model's
effective Markov order really equals k (default True). Without this, a random
draw may end up with no arc of lag k-1, making the true order lower than k and
undetectable from data sampled off the model. No-op when k = 1 or there is no
temporal process.

Parameters
----------
on : bool
    whether to guarantee the effective order

Returns
-------
pyagrum.ktbn.KTBNGenerator
    self, for chaining
"

%feature("docstring") gum::KTBNGenerator::setGenerateCPTs
"
Whether to fill the CPTs with random values (default True). When False, only
the structure is drawn and the CPTs stay at their default content.

Parameters
----------
on : bool
    whether to generate random CPTs

Returns
-------
pyagrum.ktbn.KTBNGenerator
    self, for chaining
"

%feature("docstring") gum::KTBNGenerator::setNamePrefixes
"
Set the name prefixes used for generated variables: temporal processes are
named ``prefix0``, ``prefix1``, ... Defaults are ``\"X\"`` (temporal) and
``\"A\"`` (atemporal).

Parameters
----------
temporal : str
    the prefix for temporal process names
atemporal : str
    the prefix for atemporal variable names

Returns
-------
pyagrum.ktbn.KTBNGenerator
    self, for chaining

Raises
------
pyagrum.InvalidArgument
    if a prefix is empty or the two are equal
"

%feature("docstring") gum::KTBNGenerator::k
"
Returns
-------
int
    the order of the generated models
"

%feature("docstring") gum::KTBNGenerator::nbLegalArcs
"
Returns
-------
int
    how many arcs the k-TBN's own rules allow, given the current shape; the
    density is a fraction of this, and it is also the ceiling any maxArcs is
    silently clamped to
"
