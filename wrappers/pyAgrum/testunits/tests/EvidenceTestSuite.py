# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
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


class TestEvidence(pyAgrumTestCase):
  def tesIsEvidence(self):
    bn = gum.fastBN("A[10]->B;C")
    p1 = gum.Potential(bn.cpt("A"))
    p2 = gum.Potential(bn.cpt("A"))

    self.assertTrue(bn.cpt("A").isEvidence())
    self.assertFalse(bn.cpt("B").isEvidence())

    p1.fillWith(0.0)
    self.assertFalse(p1.isEvidence())
    p1.fillWith(1.0)
    self.assertTrue(p1.isEvidence())

  def tesCombine(self):
    bn = gum.fastBN("A[10]->B;C")
    p1 = gum.Potential(bn.cpt("A"))
    p2 = gum.Potential(bn.cpt("A"))
    p1.randomCPT()
    M1 = p1.max()
    M2 = p2.max()
    m1 = p1.min()
    m2 = p2.min()

    self.assertAlmostEqual(max(M1, M2), (p1 | p2).max(), delta=1e-10)
    self.assertAlmostEqual(min(m1, m2), (p1 & p2).min(), delta=1e-10)

    self.assertAlmostEqual(M1, 1.0 - (~p1).min(), delta=1e-10)
    self.assertAlmostEqual(m1, 1.0 - (~p1).max(), delta=1e-10)
    self.assertAlmostEqual(10.0 - 1.0, (~p1).sum(), delta=1e-10)

  def testLikelihood(self):
    bn = gum.fastBN("A[10];B[1,10];C{1.0:20.0:10};D{1:100:10};E[1.0:20.0:10];X{A1|A2|A3|A4|A5|A6|A7|A8|A9|A10}")
    for i in bn.nodes():
      self.assertEqual(bn.cpt(i).domainSize(), 10)

    vA, vB, vC, vD, vE = [bn.variable(i) for i in "ABCDE"]

    gpot = gum.Potential
    self.assertEqual(gpot.evEq(vA, 4.0).tolist(), [0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evEq(vB, 4.0).tolist(), [0.0, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evEq(vC, 4.0).tolist(), [0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evEq(vD, 4.0).tolist(), [1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evEq(vE, 4.0).tolist(), [0.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])

    self.assertEqual(gpot.evIn(vA, 4.0, 6.0).tolist(), [0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0])
    self.assertEqual((gpot.evEq(vA, 4.0) | gpot.evEq(vA, 6)).tolist(), [0.0, 0.0, 0.0, 0.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0])
    self.assertEqual((~gpot.evIn(vA, 4.0, 6.0)).tolist(), [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0])


    self.assertEqual(gpot.evLt(vA, 4.0).tolist(), [1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evLt(vA, 0.0).tolist(), [1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evGt(vA, 4.0).tolist(), [0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0])
    self.assertEqual(gpot.evGt(vA, 8.0).tolist(), [0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0])
    self.assertEqual(gpot.evLt(vC, 7.0).tolist(), [1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evLt(vC, 1.0).tolist(), [1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0])
    self.assertEqual(gpot.evGt(vC, 7.0).tolist(), [0.0, 0.0, 0.0, 0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0])
    self.assertEqual(gpot.evGt(vC, 20.0).tolist(),[0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0])

ts = unittest.TestSuite()
addTests(ts, TestEvidence)
