############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""Static typing of the main user entry points of pyAgrum.

Never executed: checked by mypy through `act guideline --check pytyping` (against the installed pyagrum).
`assert_type` fails if an annotation changes, and `--warn-unused-ignores` fails if an expected error
disappears (e.g. an argument silently typed Any again).
"""

from typing_extensions import assert_type

import pyagrum as gum

# Bayesian networks
bn = gum.fastBN("A->B<-C")
assert_type(bn, gum.BayesNet)
assert_type(gum.loadBN("model.bif"), gum.BayesNet)
assert_type(gum.randomBN(n=4), gum.BayesNet)
gum.saveBN(bn, 3)  # type: ignore[arg-type]

ie = gum.LazyPropagation(bn)
ie.setEvidence({"A": 1, "C": [0.3, 0.7]})
ie.setEvidence(3)  # type: ignore[arg-type]
ie.makeInference()
assert_type(ie.posterior("B"), gum.Tensor)
assert_type(gum.getPosterior(bn, target="B", evs={"A": 0}), gum.Tensor)

learner = gum.BNLearner("data.csv")
assert_type(learner.learnBN(), gum.BayesNet)

# graphs
assert_type(gum.fastDAG("A->B"), gum.DAG)

# lazily imported submodules: typed through the TYPE_CHECKING block of pyagrum/__init__.py
assert_type(gum.fastMRF("A--B;B--C"), gum.MarkovRandomField)
assert_type(gum.loadMRF("model.uai"), gum.MarkovRandomField)
assert_type(gum.fastID("A->*D->$U"), gum.InfluenceDiagram)
assert_type(gum.loadID("model.bifxml"), gum.InfluenceDiagram)
assert_type(gum.loadKTBN("model.jgum"), gum.KTBN)
assert_type(gum.ShaferShenoyLIMIDInference(gum.fastID("A->*D->$U")).reversePartialOrder(), list[set[int]])

cn = gum.CredalNet(bn, bn)
assert_type(cn.credalNet_currentCpt(), dict[int, list[list[list[float]]]])
