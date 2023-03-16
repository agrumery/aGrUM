# (c) Copyright by Pierre-Henri Wuillemin, 2023
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
import unittest

import pyAgrum as gum
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
