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

import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class ICIModelsForBNTestCase(pyAgrumTestCase):
  def setUp(self):
    pass

  def testLogit(self):
    bn = gum.BayesNet()
    age = bn.add(gum.RangeVariable("age", "", 35, 67))
    taux = bn.add(gum.RangeVariable("taux", "", 115, 171))
    angine = bn.add(gum.LabelizedVariable("angine", ""))
    vc = gum.LabelizedVariable("coeur", "", 0)
    vc.addLabel("NON").addLabel("OUI")
    coeur = bn.addLogit(vc, 14.4937)

    bn.addWeightedArc(age, coeur, -0.1256)
    bn.addWeightedArc(taux, coeur, -0.0636)
    bn.addWeightedArc(angine, coeur, 1.779)

    witness_age = ("50", "49", "46", "49", "62", "35", "67", "65", "47")
    witness_taux = ("126", "126", "144", "139", "154", "156", "160", "140", "143")
    witness_angine = ("1", "0", "0", "0", "1", "1", "0", "0", "0")
    witness_coeur = ("OUI", "OUI", "OUI", "OUI", "OUI", "OUI", "NON", "NON", "NON")

    witness_proba = (0.8786, 0.5807, 0.3912, 0.3773, 0.2127, 0.8760, 1 - 0.0163, 1 - 0.0710, 1 - 0.3765)

    inst = gum.Instantiation(bn.cpt(coeur))
    for i in range(len(witness_age)):
      inst.chgVal(bn.variable(age), bn.variable(age)[witness_age[i]])
      inst.chgVal(bn.variable(taux), bn.variable(taux)[witness_taux[i]])
      inst.chgVal(bn.variable(angine), bn.variable(angine)[witness_angine[i]])
      inst.chgVal(bn.variable(coeur), bn.variable(coeur)[witness_coeur[i]])
      self.assertAlmostEqual(bn.cpt(coeur).get(inst), witness_proba[i], places=3)


ts = unittest.TestSuite()
addTests(ts, ICIModelsForBNTestCase)
