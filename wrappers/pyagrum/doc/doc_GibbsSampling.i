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







%feature("docstring") gum::GibbsSampling
"
Class for making Gibbs sampling inference in Bayesian networks.

GibbsSampling(bn) -> GibbsSampling
    Parameters:
      - **bn** (*pyagrum.BayesNet*) -- a Bayesian network

Notes
-----
The Gibbs chain must start from a configuration compatible with the evidence (with a positive
probability). When the Bayesian network contains zeros (deterministic relations), finding such a
configuration is a hard combinatorial problem in general, and it is searched heuristically:
impossible values are first removed, then a few random configurations are tried and gradually
repaired.

- If the removal of impossible values proves that the evidence is incompatible with the model,
  `makeInference()` raises :class:`pyagrum.IncompatibleEvidence`.
- If no compatible configuration is found, `makeInference()` raises
  :class:`pyagrum.OperationNotAllowed`. This does not prove that the evidence is impossible: with
  very rare compatible configurations (e.g. long chains of deterministic constraints), the search
  may fail and its outcome may depend on the random seed. An exact inference such as
  :class:`pyagrum.LazyPropagation` can check it (`evidenceProbability()` is positive).
"
