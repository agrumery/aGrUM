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

import pyagrum.ktbn as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

AT = gum.KTBN.ATEMPORAL


def _buildK2Model():
  m = gum.KTBN(2)
  m.add(gum.LabelizedVariable("X", "", 2), True)
  m.add(gum.LabelizedVariable("C", "", 2), False)
  m.addArc("X", 0, "X", 1)
  m.addArc("C", AT, "X", 1)
  return m


def _buildK3Model():
  m = gum.KTBN(3)
  m.add(gum.LabelizedVariable("X", "", 2), True)
  m.add(gum.LabelizedVariable("Y", "", 2), True)
  m.add(gum.LabelizedVariable("C", "", 2), False)
  m.addArc("X", 0, "X", 2)
  m.addArc("X", 1, "X", 2)
  m.addArc("X", 2, "Y", 2)
  m.addArc("C", AT, "X", 2)
  m.generateCPTs()
  return m


def _maxAbsCptDiff(a, b):
  ia, ib = gum.Instantiation(a), gum.Instantiation(b)
  d = 0.0
  ia.setFirst()
  ib.setFirst()
  while not ia.end():
    d = max(d, abs(a.get(ia) - b.get(ib)))
    ia.inc()
    ib.inc()
  return d


class KTBNCPTTestCase(pyAgrumTestCase):
  def testCPTShapes(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    self.assertEqual(m.cpt("X", 0).nbrDim(), 1)  # P(X@0): no parents
    self.assertEqual(m.cpt("X", 1).nbrDim(), 2)  # P(X@1 | X@0)

  def testFillCPT(self):
    m = _buildK2Model()  # parent order of X[1]: X[0] then C

    m.fillCPT("X", 0, {}, [0.7, 0.3])
    m.fillCPT("C", AT, {}, [0.4, 0.6])
    m.fillCPT("X", 1, {("C", AT): 0, ("X", 0): 0}, [0.9, 0.1])
    m.fillCPT("X", 1, {("C", AT): 0, ("X", 0): 1}, [0.2, 0.8])
    m.fillCPT("X", 1, {("C", AT): 1, ("X", 0): 0}, [0.6, 0.4])
    m.fillCPT("X", 1, {("C", AT): 1, ("X", 0): 1}, [0.3, 0.7])

    ref = _buildK2Model()
    ref.cpt("X", 0).fillWith([0.7, 0.3])
    ref.cpt("C").fillWith([0.4, 0.6])
    ref.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7])

    self.assertLess(_maxAbsCptDiff(m.cpt("X", 0), ref.cpt("X", 0)), 1e-9)
    self.assertLess(_maxAbsCptDiff(m.cpt("C"), ref.cpt("C")), 1e-9)
    self.assertLess(_maxAbsCptDiff(m.cpt("X", 1), ref.cpt("X", 1)), 1e-9)

    with self.assertRaises(gum.SizeError):
      m.fillCPT("X", 0, {}, [0.5])
    with self.assertRaises(gum.SizeError):
      m.fillCPT("X", 1, {("X", 0): 0}, [0.5, 0.5])
    with self.assertRaises(gum.OutOfBounds):
      m.fillCPT("X", 1, {("X", 0): 9, ("C", AT): 0}, [0.5, 0.5])

  def testFillCPTByName(self):
    m = _buildK2Model()

    m.fillCPT("X[0]", {}, [0.7, 0.3])
    m.fillCPT("C", {}, [0.4, 0.6])
    m.fillCPT("X[1]", {"C": 0, "X[0]": 1}, [0.9, 0.1])
    m.fillCPT("X[1]", {"C": 0, "X[0]": 0}, [0.2, 0.8])
    m.fillCPT("X[1]", {"C": 1, "X[0]": 1}, [0.6, 0.4])
    m.fillCPT("X[1]", {"C": 1, "X[0]": 0}, [0.3, 0.7])

    ref = _buildK2Model()
    ref.fillCPT("X", 0, {}, [0.7, 0.3])
    ref.fillCPT("C", AT, {}, [0.4, 0.6])
    ref.fillCPT("X", 1, {("X", 0): 1, ("C", AT): 0}, [0.9, 0.1])
    ref.fillCPT("X", 1, {("X", 0): 0, ("C", AT): 0}, [0.2, 0.8])
    ref.fillCPT("X", 1, {("X", 0): 1, ("C", AT): 1}, [0.6, 0.4])
    ref.fillCPT("X", 1, {("X", 0): 0, ("C", AT): 1}, [0.3, 0.7])

    self.assertLess(_maxAbsCptDiff(m.cpt("X", 0), ref.cpt("X", 0)), 1e-9)
    self.assertLess(_maxAbsCptDiff(m.cpt("C"), ref.cpt("C")), 1e-9)
    self.assertLess(_maxAbsCptDiff(m.cpt("X", 1), ref.cpt("X", 1)), 1e-9)

    with self.assertRaises(gum.SizeError):
      m.fillCPT("X[0]", {}, [0.5])
    with self.assertRaises(gum.NotFound):
      m.fillCPT("NONODE", {}, [0.5, 0.5])
    with self.assertRaises(gum.SizeError):
      m.fillCPT("X[1]", {"X[0]": 0}, [0.5, 0.5])
    with self.assertRaises(gum.OutOfBounds):
      m.fillCPT("X[1]", {"X[0]": 9, "C": 0}, [0.5, 0.5])

  def testFillCPTByModalityLabel(self):
    def build():
      m = gum.KTBN(2)
      m.add(gum.LabelizedVariable("X", "", ["no", "yes"]), True)
      m.add(gum.LabelizedVariable("C", "", ["low", "high"]), False)
      m.addArc("X", 0, "X", 1)
      m.addArc("C", AT, "X", 1)
      return m

    byIndex, byLabel, mixed = build(), build(), build()

    byIndex.fillCPT("X", 1, {("X", 0): 0, ("C", AT): 0}, [0.9, 0.1])
    byIndex.fillCPT("X", 1, {("X", 0): 1, ("C", AT): 0}, [0.2, 0.8])
    byIndex.fillCPT("X", 1, {("X", 0): 0, ("C", AT): 1}, [0.6, 0.4])
    byIndex.fillCPT("X", 1, {("X", 0): 1, ("C", AT): 1}, [0.3, 0.7])

    byLabel.fillCPT("X", 1, {("X", 0): "no", ("C", AT): "low"}, [0.9, 0.1])
    byLabel.fillCPT("X", 1, {("X", 0): "yes", ("C", AT): "low"}, [0.2, 0.8])
    byLabel.fillCPT("X", 1, {("X", 0): "no", ("C", AT): "high"}, [0.6, 0.4])
    byLabel.fillCPT("X", 1, {("X", 0): "yes", ("C", AT): "high"}, [0.3, 0.7])

    mixed.fillCPT("X", 1, {("X", 0): 0, ("C", AT): "low"}, [0.9, 0.1])
    mixed.fillCPT("X", 1, {("X", 0): "yes", ("C", AT): 0}, [0.2, 0.8])
    mixed.fillCPT("X[1]", {"X[0]": "no", "C": 1}, [0.6, 0.4])
    mixed.fillCPT("X[1]", {"X[0]": 1, "C": "high"}, [0.3, 0.7])

    self.assertLess(_maxAbsCptDiff(byLabel.cpt("X", 1), byIndex.cpt("X", 1)), 1e-9)
    self.assertLess(_maxAbsCptDiff(mixed.cpt("X", 1), byIndex.cpt("X", 1)), 1e-9)

    with self.assertRaises(gum.OutOfBounds):
      byLabel.fillCPT("X", 1, {("X", 0): "maybe", ("C", AT): 0}, [0.5, 0.5])
    with self.assertRaises(gum.OutOfBounds):
      byLabel.fillCPT("X[1]", {"X[0]": "maybe", "C": 0}, [0.5, 0.5])

  def testFillCPTLabelIsNotAnIndex(self):
    # on a range 2..5, index 1 is the modality "3": label and index differ
    m = gum.KTBN(2)
    m.add(gum.RangeVariable("R", "", 2, 5), True)
    m.addArc("R", 0, "R", 1)

    byLabel = gum.KTBN(m)
    m.fillCPT("R", 1, {("R", 0): 1}, [0.1, 0.2, 0.3, 0.4])
    byLabel.fillCPT("R", 1, {("R", 0): "3"}, [0.1, 0.2, 0.3, 0.4])
    self.assertLess(_maxAbsCptDiff(byLabel.cpt("R", 1), m.cpt("R", 1)), 1e-9)

    self.assertEqual(m.variable("R", 0).index("2"), 0)
    self.assertEqual(m.variable("R", 0).label(2), "4")

    with self.assertRaises(gum.NotFound):
      m.fillCPT("R", 1, {("R", 0): "9"}, [0.1, 0.2, 0.3, 0.4])

  def testGenerateSingleCPT(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)

    m.cpt("X", 0).fillWith([0.7, 0.3])
    m.cpt("X", 1).fillWith(0.0)
    x0before = gum.Tensor(m.cpt("X", 0))

    m.generateCPT("X", 1)

    self.assertLess(abs(m.cpt("X", 1).sum() - 2.0), 1e-9)
    self.assertLess(_maxAbsCptDiff(m.cpt("X", 0), x0before), 1e-9)

    with self.assertRaises(Exception):
      m.generateCPT("Z", 0)

  def testFillCPTMultiSliceParents(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 2)
    m.addArc("X", 1, "X", 2)

    m.fillCPT("X", 2, {("X", 1): 0, ("X", 0): 0}, [0.9, 0.1])
    m.fillCPT("X", 2, {("X", 1): 0, ("X", 0): 1}, [0.2, 0.8])
    m.fillCPT("X", 2, {("X", 1): 1, ("X", 0): 0}, [0.6, 0.4])
    m.fillCPT("X", 2, {("X", 1): 1, ("X", 0): 1}, [0.3, 0.7])

    ref = gum.KTBN(3)
    ref.add(gum.LabelizedVariable("X", "", 2), True)
    ref.addArc("X", 0, "X", 2)
    ref.addArc("X", 1, "X", 2)
    ref.cpt("X", 2).fillWith([0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7])

    self.assertLess(_maxAbsCptDiff(m.cpt("X", 2), ref.cpt("X", 2)), 1e-9)

  def testFillCPTNonBinary(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("Y", "", 3), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("C", AT, "Y", 0)

    m.fillCPT("Y", 0, {("C", AT): 0}, [0.2, 0.3, 0.5])
    m.fillCPT("Y", 0, {("C", AT): 1}, [0.1, 0.1, 0.8])

    ref = gum.KTBN(2)
    ref.add(gum.LabelizedVariable("Y", "", 3), True)
    ref.add(gum.LabelizedVariable("C", "", 2), False)
    ref.addArc("C", AT, "Y", 0)
    ref.cpt("Y", 0).fillWith([0.2, 0.3, 0.5, 0.1, 0.1, 0.8])

    self.assertLess(_maxAbsCptDiff(m.cpt("Y", 0), ref.cpt("Y", 0)), 1e-9)

    with self.assertRaises(gum.SizeError):
      m.fillCPT("Y", 0, {("C", AT): 0}, [0.5, 0.5])

  def testFillCPTNotAParent(self):
    m = _buildK2Model()
    m.add(gum.LabelizedVariable("D", "", 2), False)  # exists, never a parent

    with self.assertRaises(gum.InvalidArgument):
      m.fillCPT("X", 1, {("X", 0): 0, ("D", AT): 0}, [0.5, 0.5])
    with self.assertRaises(gum.InvalidArgument):
      m.fillCPT("X[1]", {"X[0]": 0, "D": 0}, [0.5, 0.5])
    with self.assertRaises(gum.NotFound):
      m.fillCPT("X[1]", {"X[0]": 0, "NOPE": 0}, [0.5, 0.5])

  def testGenerateAllCPTsNormalized(self):
    m = _buildK2Model()
    m.generateCPTs()

    self.assertLess(abs(m.cpt("X", 0).sum() - 1.0), 1e-9)
    self.assertLess(abs(m.cpt("C").sum() - 1.0), 1e-9)
    self.assertLess(abs(m.cpt("X", 1).sum() - 4.0), 1e-9)  # 2 parents

  def testBracketNotationCPT(self):
    m = _buildK2Model()
    m.fillCPT("X[0]", {}, [0.3, 0.7])
    m.fillCPT("C", {}, [0.4, 0.6])

    m.generateCPT("X[1]")
    self.assertLess(abs(m.cpt("X[1]").sum() - 4.0), 1e-9)

    with self.assertRaises(gum.NotFound):
      m.cpt("Z[0]")
    with self.assertRaises(gum.NotFound):
      m.generateCPT("Z[0]")


class KTBNUnrollTestCase(pyAgrumTestCase):
  def testUnrollStructureArbitraryLag(self):
    m = _buildK3Model()
    bn = m.unroll(5)

    self.assertEqual(bn.size(), 5 + 5 + 1)  # X:5, Y:5, C:1

    self.assertTrue(bn.existsArc("X[0]", "X[2]"))
    self.assertTrue(bn.existsArc("X[1]", "X[2]"))
    self.assertTrue(bn.existsArc("X[2]", "Y[2]"))
    self.assertTrue(bn.existsArc("C", "X[2]"))

    self.assertTrue(bn.existsArc("X[1]", "X[3]"))
    self.assertTrue(bn.existsArc("X[2]", "X[3]"))
    self.assertTrue(bn.existsArc("C", "X[3]"))
    self.assertTrue(bn.existsArc("X[3]", "Y[3]"))

    self.assertLess(_maxAbsCptDiff(bn.cpt("Y[3]"), bn.cpt("Y[2]")), 1e-12)

    with self.assertRaises(gum.OperationNotAllowed):
      m.unroll(2)

  def testUnrollHorizonEqualsK(self):
    m = _buildK3Model()  # k = 3
    bn = m.unroll(3)

    self.assertEqual(bn.size(), 3 + 3 + 1)
    self.assertTrue(bn.exists("X[2]"))
    self.assertFalse(bn.exists("X[3]"))
    self.assertTrue(bn.existsArc("X[0]", "X[2]"))
    self.assertTrue(bn.existsArc("C", "X[2]"))

  def testUnrollInitialArcsDoNotPropagate(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)  # initial arc, not the k-1 kernel
    m.generateCPTs()

    bn = m.unroll(5)
    self.assertTrue(bn.existsArc("X[0]", "X[1]"))
    self.assertFalse(bn.existsArc("X[1]", "X[2]"))
    self.assertFalse(bn.existsArc("X[2]", "X[3]"))

  def testUnrollReusesHomogeneousKernel(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    bn = m.unroll(3)
    self.assertLess(_maxAbsCptDiff(bn.cpt("X[1]"), bn.cpt("X[2]")), 1e-12)

  def testUnrollK1Memoryless(self):
    m = gum.KTBN(1)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("C", AT, "X", 0)
    m.generateCPTs()

    bn = m.unroll(4)
    self.assertEqual(bn.size(), 4 + 1)

    for t in range(4):
      xt = f"X[{t}]"
      self.assertTrue(bn.exists(xt))
      self.assertTrue(bn.existsArc("C", xt))
      self.assertEqual(len(bn.parents(bn.idFromName(xt))), 1)  # C only

    c0 = bn.cpt("X[0]")
    for t in range(1, 4):
      self.assertLess(_maxAbsCptDiff(c0, bn.cpt(f"X[{t}]")), 1e-12)

    with self.assertRaises(gum.OperationNotAllowed):
      m.unroll(0)

  def testUnrollNoTemporalVars(self):
    m = gum.KTBN(1)
    m.add(gum.LabelizedVariable("A", "", 2), False)
    m.add(gum.LabelizedVariable("B", "", 2), False)
    m.addArc("A", AT, "B", AT)
    m.generateCPTs()

    bn = m.unroll(3)
    self.assertEqual(bn.size(), 2)
    self.assertTrue(bn.exists("A"))
    self.assertTrue(bn.exists("B"))
    self.assertTrue(bn.existsArc("A", "B"))
    self.assertFalse(bn.exists("A[0]"))


ts = unittest.TestSuite()
addTests(ts, KTBNCPTTestCase)
addTests(ts, KTBNUnrollTestCase)
