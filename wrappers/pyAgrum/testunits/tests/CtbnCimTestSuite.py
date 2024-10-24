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


class TestCtbnCIM(pyAgrumTestCase):
  def testcim1(self):
    cimA = ct.CIM()
    cimB = ct.CIM()

    ai = ct.CIM.varI("A")
    aj = ct.CIM.varJ("A")
    bi = ct.CIM.varI("B")
    bj = ct.CIM.varJ("B")

    A = gum.LabelizedVariable("A", "A", ["a", "b"])
    Ai = gum.LabelizedVariable(ai, ai, ["a", "b"])
    Aj = gum.LabelizedVariable(aj, aj, ["a", "b"])
    Bi = gum.LabelizedVariable(bi, bi, ["x", "y"])
    Bj = gum.LabelizedVariable(bj, bj, ["x", "y"])

    cimA.add(Ai)
    cimA.add(Aj)

    cimB.add(Bi)
    cimB.add(Bj)
    cimB.add(A)

    cimA[{ai: "a", aj: "a"}] = -8
    cimA[{ai: "a", aj: "b"}] = 8
    cimA[{ai: "b", aj: "a"}] = 2
    cimA[{ai: "b", aj: "b"}] = -2

    cimB[{"A": "a", bi: "x", bj: "x"}] = -5
    cimB[{"A": "a", bi: "x", bj: "y"}] = 5
    cimB[{"A": "a", bi: "y", bj: "x"}] = 4
    cimB[{"A": "a", bi: "y", bj: "y"}] = -4
    cimB[{"A": "b", bi: "x", bj: "x"}] = -1
    cimB[{"A": "b", bi: "x", bj: "y"}] = 1
    cimB[{"A": "b", bi: "y", bj: "x"}] = 1
    cimB[{"A": "b", bi: "y", bj: "y"}] = -1

    self.assertEqual(2, cimA.nbrDim())
    self.assertEqual(3, cimB.nbrDim())

    self.assertFalse(cimA.isParent(Ai))
    self.assertFalse(cimA.isParent(Aj))
    self.assertTrue(cimB.isParent(A))
    self.assertFalse(cimB.isParent(Bi))
    self.assertFalse(cimB.isParent(Bj))

    amal = cimA * cimB
    self.assertEqual(4, amal.nbrDim())

    self.assertEqual(amal[{ai: "a", aj: "a", bi: "x", bj: "x"}], -13)
    self.assertEqual(amal[{ai: "a", aj: "a", bi: "y", bj: "y"}], -12)
    self.assertEqual(amal[{ai: "b", aj: "b", bi: "x", bj: "x"}], -3)
    self.assertEqual(amal[{ai: "b", aj: "b", bi: "y", bj: "y"}], -3)
    self.assertEqual(amal[{ai: "a", aj: "b", bi: "x", bj: "y"}], 0)
    self.assertEqual(amal[{ai: "a", aj: "b", bi: "y", bj: "x"}], 0)
    self.assertEqual(amal[{ai: "b", aj: "a", bi: "x", bj: "y"}], 0)
    self.assertEqual(amal[{ai: "b", aj: "a", bi: "y", bj: "x"}], 0)
    self.assertEqual(amal[{ai: "a", aj: "a", bi: "x", bj: "y"}], 5)
    self.assertEqual(amal[{ai: "a", aj: "b", bi: "x", bj: "x"}], 8)
    self.assertEqual(amal[{ai: "a", aj: "a", bi: "y", bj: "x"}], 4)
    self.assertEqual(amal[{ai: "b", aj: "a", bi: "x", bj: "x"}], 2)
    self.assertEqual(amal[{ai: "b", aj: "a", bi: "y", bj: "y"}], 2)
    self.assertEqual(amal[{ai: "a", aj: "b", bi: "y", bj: "y"}], 8)
    self.assertEqual(amal[{ai: "b", aj: "b", bi: "x", bj: "y"}], 1)
    self.assertEqual(amal[{ai: "b", aj: "b", bi: "y", bj: "x"}], 1)

  def testcim2(self):
    cimA = ct.CIM()
    cimB = ct.CIM()
    cimX = ct.CIM()
    cimY = ct.CIM()

    xi = ct.CIM.varI("X")
    xj = ct.CIM.varJ("X")
    yi = ct.CIM.varI("Y")
    yj = ct.CIM.varJ("Y")

    A = gum.LabelizedVariable("A", "A", ["a0", "a1"])
    B = gum.LabelizedVariable("B", "B", ["b0", "b1"])
    X = gum.LabelizedVariable("X", "X", ["x0", "x1"])
    Y = gum.LabelizedVariable("Y", "Y", ["y0", "y1"])
    Xi = gum.LabelizedVariable(xi, xi, ["x0", "x1"])
    Xj = gum.LabelizedVariable(xj, xj, ["x0", "x1"])
    Yi = gum.LabelizedVariable(yi, yi, ["y0", "y1"])
    Yj = gum.LabelizedVariable(yj, yj, ["y0", "y1"])

    cimA.add(A)
    cimB.add(B)

    cimX.add(Xi)
    cimX.add(Xj)
    cimX.add(A)
    cimX.add(Y)

    cimY.add(Yi)
    cimY.add(Yj)
    cimY.add(B)
    cimY.add(X)

    cimX[{"A": "a0", "Y": "y0"}] = [[-1, 1],
                                    [2, -2]]
    cimX[{"A": "a0", "Y": "y1"}] = [[-5, 5],
                                    [6, -6]]
    cimY[{"B": "b1", "X": "x0"}] = [[-3, 3],
                                    [4, -4]]
    cimY[{"B": "b1", "X": "x1"}] = [[-7, 7],
                                    [8, -8]]

    self.assertEqual(1, cimA.nbrDim())
    self.assertEqual(1, cimB.nbrDim())
    self.assertEqual(4, cimX.nbrDim())
    self.assertEqual(4, cimY.nbrDim())

    amal = cimX * cimY

    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x0", yi: "y0", yj: "y0"}], -4)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x0", yi: "y1", yj: "y0"}], 3)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x0", yi: "y0", yj: "y0"}], 1)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x0", yi: "y1", yj: "y0"}], 0)

    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x0", yi: "y0", yj: "y1"}], 4)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x0", yi: "y1", yj: "y1"}], -9)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x0", yi: "y0", yj: "y1"}], 0)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x0", yi: "y1", yj: "y1"}], 5)

    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x1", yi: "y0", yj: "y0"}], 2)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x1", yi: "y1", yj: "y0"}], 0)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x1", yi: "y0", yj: "y0"}], -9)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x1", yi: "y1", yj: "y0"}], 7)

    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x1", yi: "y0", yj: "y1"}], 0)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x0", xj: "x1", yi: "y1", yj: "y1"}], 6)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x1", yi: "y0", yj: "y1"}], 8)
    self.assertEqual(amal[{"A": "a0", "B": "b1", xi: "x1", xj: "x1", yi: "y1", yj: "y1"}], -14)

    self.assertSetEqual(set(amal.varNames), set([xi, xj, yi, yj, "A", "B"]))


ts = unittest.TestSuite()
addTests(ts, TestCtbnCIM)
