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

import pyAgrum.ctbn as ct


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
    valueRange = (.5, 1.5)

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
