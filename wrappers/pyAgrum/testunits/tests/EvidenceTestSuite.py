############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestEvidence(pyAgrumTestCase):
  def tesIsEvidence(self):
    bn = gum.fastBN("A[10]->B;C")
    p1 = gum.Tensor(bn.cpt("A"))
    p2 = gum.Tensor(bn.cpt("A"))

    self.assertTrue(bn.cpt("A").isEvidence())
    self.assertFalse(bn.cpt("B").isEvidence())

    p1.fillWith(0.0)
    self.assertFalse(p1.isEvidence())
    p1.fillWith(1.0)
    self.assertTrue(p1.isEvidence())

  def tesCombine(self):
    bn = gum.fastBN("A[10]->B;C")
    p1 = gum.Tensor(bn.cpt("A"))
    p2 = gum.Tensor(bn.cpt("A"))
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

    gpot = gum.Tensor
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
