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
  # temporal X{2}, atemporal C{2}, X@0->X@1, C->X@1
  m = gum.KTBN(2)
  m.add(gum.LabelizedVariable("X", "", 2), True)
  m.add(gum.LabelizedVariable("C", "", 2), False)
  m.addArc("X", 0, "X", 1)
  m.addArc("C", AT, "X", 1)
  return m


class KTBNConstructionTestCase(pyAgrumTestCase):
  def testConstructorAndKOrder(self):
    m = gum.KTBN(3)
    self.assertEqual(m.k(), 3)
    self.assertTrue(m.empty())
    self.assertEqual(m.size(), 0)
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBN(0)
    self.assertEqual(gum.KTBN().k(), 2)  # default constructor: k defaults to 2

  def testCopyAndMove(self):
    a = _buildK2Model()
    a.generateCPTs()

    b = gum.KTBN(a)  # (deep) copy
    self.assertEqual(b.k(), 2)
    self.assertTrue(b.existsArc("X", 0, "X", 1))
    self.assertEqual(b.nbTemporalVars(), a.nbTemporalVars())
    self.assertEqual(b.nbAtemporalVars(), a.nbAtemporalVars())

    # deep-copy independence: mutating the copy must not touch the original
    b.add(gum.LabelizedVariable("Z", "", 2), True)
    self.assertTrue(b.exists("Z"))
    self.assertFalse(a.exists("Z"))
    b.eraseArc("X", 0, "X", 1)
    self.assertFalse(b.existsArc("X", 0, "X", 1))
    self.assertTrue(a.existsArc("X", 0, "X", 1))

  def testFromBN(self):
    # k=2 round-trip: fromBN must recover temporal/atemporal classification and k
    orig = _buildK2Model()
    orig.generateCPTs()
    bn = orig.toBN()
    self.assertEqual(bn.size(), 3)  # X[0], X[1], C

    restored = gum.KTBN.fromBN(bn)
    self.assertEqual(restored.k(), 2)
    self.assertTrue(restored.exists("X"))
    self.assertTrue(restored.exists("C"))
    self.assertTrue(restored.existsArc("X", 0, "X", 1))
    self.assertTrue(restored.existsArc("C", AT, "X", 1))

    # k=1 (order-0): X[0] alone is a valid temporal process, must not throw
    bn1 = gum.BayesNet()
    bn1.add(gum.LabelizedVariable("X[0]", "", 2))
    bn1.add(gum.LabelizedVariable("C", "", 2))
    bn1.addArc(bn1.idFromName("C"), bn1.idFromName("X[0]"))
    bn1.generateCPTs()
    m1 = gum.KTBN.fromBN(bn1)
    self.assertEqual(m1.k(), 1)
    self.assertTrue(m1.exists("X"))
    self.assertTrue(m1.exists("C"))

    # temporal -> atemporal arc is rejected
    bn2 = gum.BayesNet()
    bn2.add(gum.LabelizedVariable("X[0]", "", 2))
    bn2.add(gum.LabelizedVariable("X[1]", "", 2))
    bn2.add(gum.LabelizedVariable("C", "", 2))
    bn2.addArc(bn2.idFromName("X[1]"), bn2.idFromName("C"))
    bn2.generateCPTs()
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBN.fromBN(bn2)

    # future -> past arc is rejected
    bn3 = gum.BayesNet()
    bn3.add(gum.LabelizedVariable("X[0]", "", 2))
    bn3.add(gum.LabelizedVariable("X[1]", "", 2))
    bn3.addArc(bn3.idFromName("X[1]"), bn3.idFromName("X[0]"))
    bn3.generateCPTs()
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBN.fromBN(bn3)

    # a declared name that is not a node of the BN is rejected
    with self.assertRaises(gum.NotFound):
      gum.KTBN.fromBN(bn, {"Z"})

    # toBN() returns an INDEPENDENT deep copy: mutating it must not touch the KTBN
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()
    bnCopy = m.toBN()
    before = m.size()
    bnCopy.add(gum.LabelizedVariable("NEW", "", 2))
    self.assertFalse(m.exists("NEW"))
    self.assertEqual(m.size(), before)

    # bare "Y" (atemporal) collides with bracket "Y[0]"/"Y[1]" (temporal)
    bn4 = gum.BayesNet()
    bn4.add(gum.LabelizedVariable("Y", "", 2))
    bn4.add(gum.LabelizedVariable("Y[0]", "", 2))
    bn4.add(gum.LabelizedVariable("Y[1]", "", 2))
    bn4.generateCPTs()
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBN.fromBN(bn4)

  def testFromBNBracketFreeConvention(self):
    # basic k=2: X0/X1 (temporal, base "X") + C (atemporal, no trailing digit)
    bn = gum.BayesNet()
    bn.add(gum.LabelizedVariable("X0", "", 2))
    bn.add(gum.LabelizedVariable("X1", "", 2))
    bn.add(gum.LabelizedVariable("C", "", 2))
    bn.addArc(bn.idFromName("X0"), bn.idFromName("X1"))
    bn.addArc(bn.idFromName("C"), bn.idFromName("X1"))
    bn.generateCPTs()

    m = gum.KTBN.fromBN(bn)
    self.assertEqual(m.k(), 2)
    self.assertTrue(m.exists("X"))
    self.assertTrue(m.exists("C"))
    self.assertTrue(m.existsArc("X", 0, "X", 1))
    self.assertTrue(m.existsArc("C", AT, "X", 1))

    # k=1 (order-0): a lone "X0" is a complete process on its own, must not throw
    bn1 = gum.BayesNet()
    bn1.add(gum.LabelizedVariable("X0", "", 2))
    bn1.add(gum.LabelizedVariable("C", "", 2))
    bn1.addArc(bn1.idFromName("C"), bn1.idFromName("X0"))
    bn1.generateCPTs()
    m1 = gum.KTBN.fromBN(bn1)
    self.assertEqual(m1.k(), 1)
    self.assertTrue(m1.exists("X"))
    self.assertTrue(m1.exists("C"))

    # multi-digit base disambiguation: "X12" belongs to process "X" at slice 12
    bn2 = gum.BayesNet()
    bn2.add(gum.LabelizedVariable("X0", "", 2))
    bn2.add(gum.LabelizedVariable("X1", "", 2))
    bn2.generateCPTs()
    m2 = gum.KTBN.fromBN(bn2)
    self.assertTrue(m2.exists("X"))
    self.assertFalse(m2.exists("X1"))

    # missing slice: X0 and X12 present (k=13) but slices 1..11 absent, so the
    # process does not survive and is reclassified as atemporal, k falls back to 1
    bn3 = gum.BayesNet()
    bn3.add(gum.LabelizedVariable("X0", "", 2))
    bn3.add(gum.LabelizedVariable("X12", "", 2))
    bn3.generateCPTs()
    m3 = gum.KTBN.fromBN(bn3)
    self.assertEqual(m3.k(), 1)

    # temporal -> atemporal arc is rejected
    bn4b = gum.BayesNet()
    bn4b.add(gum.LabelizedVariable("X0", "", 2))
    bn4b.add(gum.LabelizedVariable("X1", "", 2))
    bn4b.add(gum.LabelizedVariable("C", "", 2))
    bn4b.addArc(bn4b.idFromName("X1"), bn4b.idFromName("C"))
    bn4b.generateCPTs()
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBN.fromBN(bn4b)

    # future -> past arc is rejected
    bn5 = gum.BayesNet()
    bn5.add(gum.LabelizedVariable("X0", "", 2))
    bn5.add(gum.LabelizedVariable("X1", "", 2))
    bn5.addArc(bn5.idFromName("X1"), bn5.idFromName("X0"))
    bn5.generateCPTs()
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBN.fromBN(bn5)

    # bare "Y" (atemporal) collides with digit-suffixed "Y0"/"Y1" (temporal)
    bn6 = gum.BayesNet()
    bn6.add(gum.LabelizedVariable("Y", "", 2))
    bn6.add(gum.LabelizedVariable("Y0", "", 2))
    bn6.add(gum.LabelizedVariable("Y1", "", 2))
    bn6.generateCPTs()
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBN.fromBN(bn6)

    # the two conventions never mix: a bracket-named node anywhere forces the
    # WHOLE network to be read under the bracket convention, so a digit-suffixed
    # name elsewhere ("Z9") is then read literally, atemporal
    bn7 = gum.BayesNet()
    bn7.add(gum.LabelizedVariable("Y[0]", "", 2))
    bn7.add(gum.LabelizedVariable("Y[1]", "", 2))
    bn7.add(gum.LabelizedVariable("Z9", "", 2))
    bn7.generateCPTs()
    m7 = gum.KTBN.fromBN(bn7)
    self.assertTrue(m7.exists("Y"))
    self.assertFalse(m7.exists("Z"))

  def testToString(self):
    m = _buildK2Model()
    s = m.toString()
    self.assertIn("k=2", s)
    self.assertIn("temporal processes (1)", s)
    self.assertIn("atemporal variables (1)", s)
    self.assertIn("arcs (2)", s)
    self.assertIn("X[0]", s)
    self.assertIn("X[1]", s)

  def testNodesArcsAndClear(self):
    m = _buildK2Model()
    self.assertEqual(m.sizeArcs(), 2)

    nodes = m.nodes()
    self.assertEqual(len(nodes), 3)  # one atemporal C + two temporal X slices
    nTemporal = sum(1 for base, slice_ in nodes if slice_ != AT)
    nAtemporal = sum(1 for base, slice_ in nodes if slice_ == AT)
    self.assertEqual(nAtemporal, 1)
    self.assertEqual(nTemporal, 2)

    arcs = m.arcs()
    self.assertEqual(len(arcs), 2)
    sawXX = (("X", 0), ("X", 1)) in arcs
    sawCX = (("C", AT), ("X", 1)) in arcs
    self.assertTrue(sawXX)
    self.assertTrue(sawCX)

    # clear() empties variables and arcs but keeps the order k
    m.clear()
    self.assertTrue(m.empty())
    self.assertEqual(m.size(), 0)
    self.assertEqual(m.sizeArcs(), 0)
    self.assertEqual(m.k(), 2)
    self.assertEqual(m.nodes(), ())
    self.assertEqual(m.arcs(), ())

  def testDotAndStreamOutput(self):
    m = _buildK2Model()

    dot = m.toDot()
    self.assertIn("digraph KTBN", dot)
    self.assertIn("cluster_0", dot)
    self.assertIn("cluster_1", dot)
    self.assertIn('"X[0]" -> "X[1]"', dot)

    undot = m.toUnrolledDot(3)
    self.assertIn("cluster_2", undot)
    self.assertNotIn("lightcyan", undot)

    undotHl = m.toUnrolledDot(3, True)
    self.assertIn("lightcyan", undotHl)

    bndot = m.bnToDot()
    self.assertIn("digraph", bndot)
    self.assertIn("X[0]", bndot)

    # NOTE: unlike BayesNet/MarkovRandomField, KTBN has no __str__ bound to
    # toString() yet (no operator<<-based %extend in the SWIG layer) -- str(m)
    # currently falls back to the default SWIG proxy repr. Calling toString()
    # directly, as intended, still works fine.
    self.assertIsInstance(m.toString(), str)


class KTBNArcTestCase(pyAgrumTestCase):
  def testAddArcAndValidityRules(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("Y", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)

    m.addArc("X", 0, "X", 2)  # lag 2
    m.addArc("X", 1, "X", 2)  # lag 1
    m.addArc("X", 2, "Y", 2)  # intra-slice
    m.addArc("C", AT, "X", 2)  # atemporal -> temporal

    self.assertTrue(m.existsArc("X", 0, "X", 2))
    self.assertFalse(m.existsArc("X", 0, "X", 1))

    with self.assertRaises(gum.OperationNotAllowed):
      m.addArc("X", 2, "X", 0)  # future -> past
    with self.assertRaises(gum.OperationNotAllowed):
      m.addArc("X", 0, "C", AT)  # temporal -> atemporal

    m.addArc("X", 0, "Y", 0)
    with self.assertRaises(gum.InvalidDirectedCycle):
      m.addArc("Y", 0, "X", 0)  # cycle

    m.eraseArc("X", 0, "Y", 0)
    self.assertFalse(m.existsArc("X", 0, "Y", 0))

  def testArcCount(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("Y", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)

    self.assertEqual(m.sizeArcs(), 0)
    m.addArc("X", 0, "X", 1)
    self.assertEqual(m.sizeArcs(), 1)
    m.addArc("C", AT, "X", 1)
    self.assertEqual(m.sizeArcs(), 2)
    m.addArc("X", 0, "Y", 1)
    self.assertEqual(m.sizeArcs(), 3)
    m.eraseArc("X", 0, "Y", 1)
    self.assertEqual(m.sizeArcs(), 2)
    self.assertEqual(len(m.arcs()), m.sizeArcs())

  def testEraseArcEdgeCases(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    with self.assertRaises(Exception):
      m.eraseArc("X", 0, "X", 1)  # absent arc
    with self.assertRaises(gum.NotFound):
      m.eraseArc("Z", 0, "X", 1)  # unknown variable

  def testDuplicateArc(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)
    self.assertEqual(m.sizeArcs(), 1)
    with self.assertRaises(gum.DuplicateElement):
      m.addArc("X", 0, "X", 1)

  def testAtemporalArc(self):
    m = gum.KTBN(1)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.add(gum.LabelizedVariable("D", "", 3), False)

    m.addArc("C", AT, "D", AT)  # atemporal -> atemporal is allowed
    self.assertTrue(m.existsArc("C", AT, "D", AT))
    self.assertEqual(m.sizeArcs(), 1)

    ch = m.children("C", AT)
    self.assertEqual(len(ch), 1)
    self.assertEqual(ch[0][0], "D")

  def testBracketNotationArcs(self):
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("Y[0]", "", 2), False)

    m.addArc("Y[0]", "X[2]")
    self.assertTrue(m.existsArc("Y[0]", "X[2]"))
    self.assertTrue(m.existsArc("Y[0]", AT, "X", 2))

    m.eraseArc("Y[0]", "X[2]")
    self.assertFalse(m.existsArc("Y[0]", "X[2]"))

    m.addArc("X[0]", "X[2]")
    self.assertTrue(m.existsArc("X[0]", "X[2]"))
    self.assertTrue(m.existsArc("X", 0, "X", 2))

    with self.assertRaises(gum.NotFound):
      m.addArc("Z[0]", "X[2]")
    with self.assertRaises(gum.OperationNotAllowed):
      m.addArc("X[2]", "X[0]")

    m.addArc("Y[0]", "X[1]")
    ch = m.children("Y[0]")
    self.assertEqual(len(ch), 1)

    pa = m.parents("X[1]")
    self.assertEqual(len(pa), 1)
    self.assertEqual(pa[0], ("Y[0]", AT))


ts = unittest.TestSuite()
addTests(ts, KTBNConstructionTestCase)
addTests(ts, KTBNArcTestCase)
