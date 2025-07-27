############################################################################
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
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
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

import pyagrum.ctbn as ct


class TestCtbnModel(pyAgrumTestCase):
  def testCtbn1(self):
    ctbn1 = ct.CTBN()
    A = gum.LabelizedVariable("A", "A", ["a0", "a1", "a2"])
    B = gum.LabelizedVariable("B", "B", ["b0", "b1", "b2"])
    C = gum.LabelizedVariable("C", "C", ["c0", "c1", "c2"])
    ctbn1.add(A)
    ctbn1.add(B)
    ctbn1.add(C)
    self.assertSetEqual(set(["A", "B", "C"]), set(ctbn1.names()))
    self.assertSetEqual(set([A, B, C]), set(ctbn1.variables()))

    ctbn1.addArc("A", "B")
    ctbn1.addArc("B", "C")
    self.assertSetEqual(set([(0, 1), (1, 2)]), ctbn1.arcs())
    self.assertIn("B", ctbn1.CIM("C").varNames)  # is B in C's cim ?

    ctbn1.eraseArc("B", "C")
    self.assertNotIn((1, 2), ctbn1.arcs())
    self.assertNotIn("B", ctbn1.CIM(2).varNames)  # was B removed from C's cim?

    self.assertEqual(0, ctbn1.node("A"))
    self.assertEqual(1, ctbn1.node("B"))
    self.assertEqual(2, ctbn1.node("C"))

    self.assertTupleEqual(ctbn1.labels("A"), ("a0", "a1", "a2"))

    self.assertTrue(A == ctbn1.variable("A"))
    self.assertTrue(B == ctbn1.variable("B"))
    self.assertTrue(C == ctbn1.variable("C"))
    self.assertSetEqual(set([A, B, C]), set(ctbn1.variables()))
    self.assertListEqual([0, 1, 2], ctbn1.nodes())
    self.assertListEqual(["A", "B", "C"], ctbn1.names())
    self.assertSetEqual(set([0]), ctbn1.parents("B"))
    self.assertSetEqual(set(), ctbn1.parents("C"))
    self.assertListEqual(["A"], ctbn1.parentNames("B"))
    self.assertListEqual([], ctbn1.parentNames("C"))
    self.assertSetEqual(set([1]), ctbn1.children("A"))
    self.assertListEqual(["B"], ctbn1.childrenNames("A"))
    self.assertTrue(ctbn1.equals(ctbn1))

    with self.assertRaises(ValueError):
      ctbn1.add(None)
    with self.assertRaises(NameError):
      ctbn1.add(gum.LabelizedVariable(""))
    with self.assertRaises(NameError):
      ctbn1.add(gum.LabelizedVariable("A", "A", ["0", "1"]))
    with self.assertRaises(gum.NotFound):
      ctbn1.addArc("A", "D")
    with self.assertRaises(gum.NotFound):
      ctbn1.eraseArc("E", "F")
    with self.assertRaises(gum.NotFound):
      ctbn1.name(5)
    with self.assertRaises(gum.NotFound):
      ctbn1.node("E")
    with self.assertRaises(gum.NotFound):
      ctbn1.labels(7)
    with self.assertRaises(gum.NotFound):
      ctbn1.variable(7)
    with self.assertRaises(gum.NotFound):
      ctbn1.parents(7)
    with self.assertRaises(gum.NotFound):
      ctbn1.parentNames(7)

  def testCtbn2(self):
    n = 10
    parMax = 3
    modal = 3
    valueRange = (0.5, 1.5)

    ctbn2 = ct.randomCTBN(valueRange, n, parMax, modal)
    v = ctbn2.variable(0)
    v_i = ctbn2.CIM(0).findVar(ct.CIM.varI(v.name()))
    v_j = ctbn2.CIM(0).findVar(ct.CIM.varJ(v.name()))
    P1 = ctbn2.CIM(0)._pot
    I1 = gum.Instantiation(P1)
    I2 = gum.Instantiation()
    I2.add(v_j)

    # Number of nodes check
    self.assertEqual(len(ctbn2.nodes()), n)
    # number of labels check
    self.assertEqual(len(v.labels()), modal)

    # value range check
    I1.setFirst()
    while not I1.end():
      if I1.val(v_i) != I1.val(v_j):
        self.assertTrue(valueRange[0] <= P1.get(I1) <= valueRange[1])
      I1.inc()

    # diagonal coefficient equals sum of line coefficients check
    I1.setFirst()
    while not I1.end():
      I2.setFirst()
      sumCim = 0.0
      while not I2.end():
        I1.setVals(I2)
        sumCim += P1.get(I1)
        I2.inc()
      self.assertAlmostEqual(sumCim, 0.0)
      I1.incOut(I2)

    # number of parents check
    for v in ctbn2.variables():
      self.assertTrue(len(ctbn2.parents(v.name())) <= modal)


ts = unittest.TestSuite()
addTests(ts, TestCtbnModel)
