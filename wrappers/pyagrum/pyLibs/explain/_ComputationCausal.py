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

import pyagrum as gum

# Calculations
import numpy as np


class CausalComputation:
  @staticmethod
  def _outOfCoalition(tau: list[int], inter: list[int]) -> list[int]:
    # Returns the intersection of the parameter inter and the complement of the coalition tau.
    return [i for i in inter if i not in tau]

  @staticmethod
  def _doCalculus(bn: gum.BayesNet, tau: list[int]) -> gum.BayesNet:
    # Creates a new Bayesian Network by removing incoming arcs to the nodes in tau.
    doNetTemplate = gum.BayesNet(bn)
    for i in tau:
      parents = doNetTemplate.parents(i)
      for j in parents:
        doNetTemplate.eraseArc(j, i)
    return doNetTemplate

  @staticmethod
  def _chgCpt(doNetTemplate: gum.BayesNet, tau: list[int], alpha: list[int]) -> None:
    # Changes the conditional probability tables (CPTs) of the nodes in tau to reflect the values in alpha.
    for i, j in zip(tau, alpha):
      doNetTemplate.cpt(i).fillWith(0.0)
      doNetTemplate.cpt(i)[int(j)] = 1.0

  @staticmethod
  def _weight(evidces: dict[int, int], count: int, doLazy: gum.LazyPropagation) -> np.ndarray:
    # Returns the evidces probability.
    # The signature must be : Dict[int, int], int, **kwargs
    doLazy.updateEvidence(evidces)
    return doLazy.evidenceProbability() * count
