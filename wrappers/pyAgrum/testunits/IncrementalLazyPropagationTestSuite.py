# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


# The graph used for the tests and the domain sizes:
#
#  A             0             2
#   \             \             \
#    B   G         1   6         3   4
#     \ / \         \ / \         \ / \
#      C   H         2   7         2   3
#     / \           / \           / \
#    D   E         3   4         3   2
#   /             /             /
#  F             5             3


class IncrementalLazyPropagationTestCase(pyAgrumTestCase):
  def setUp(self):
    self.bn = gum.BayesNet()
    self.bn.add("A", 2)
    self.bn.add("B", 3)
    self.bn.add("C", 2)
    self.bn.add("D", 3)
    self.bn.add("E", 2)
    self.bn.add("F", 3)
    self.bn.add("G", 4)
    self.bn.add("H", 3)

    self.bn.addArc("A", "B")
    self.bn.addArc("B", "C")
    self.bn.addArc("C", "D")
    self.bn.addArc("C", "E")
    self.bn.addArc("D", "F")
    self.bn.addArc("G", "C")
    self.bn.addArc("G", "H")

    self.bn.cpt("A").fillWith([0.2, 0.8])
    self.bn.cpt("B").fillWith([0.3, 0.4, 0.3,
                               0.1, 0.3, 0.6])
    self.bn.cpt("C").fillWith([0.1, 0.9,  # 1
                               0.2, 0.8,  # 2
                               0.3, 0.7,  # 3
                               0.4, 0.6,  # 4
                               0.5, 0.5,  # 5
                               0.6, 0.4,  # 6
                               0.7, 0.3,  # 7
                               0.8, 0.2,  # 8
                               0.9, 0.1,  # 9
                               0.8, 0.2,  # 10
                               0.7, 0.3,  # 11
                               0.6, 0.4])  # 12
    self.bn.cpt("D").fillWith([0.3, 0.2, 0.5,
                               0.7, 0.1, 0.2])
    self.bn.cpt("E").fillWith([0.4, 0.6,
                               0.9, 0.1])
    self.bn.cpt("F").fillWith([0.2, 0.4, 0.4,
                               0.4, 0.1, 0.5,
                               0.7, 0.2, 0.1])
    self.bn.cpt("G").fillWith([0.1, 0.4, 0.2, 0.3])
    self.bn.cpt("H").fillWith([0.4, 0.3, 0.3,
                               0.1, 0.8, 0.1,
                               0.3, 0.4, 0.3,
                               0.7, 0.1, 0.2])

    # @todo add Potential.combine method in pyAgrum
    self.joint = self.bn.cpt("A") * \
                 self.bn.cpt("B") * \
                 self.bn.cpt("C") * \
                 self.bn.cpt("D") * \
                 self.bn.cpt("E") * \
                 self.bn.cpt("F") * \
                 self.bn.cpt("G") * \
                 self.bn.cpt("H")

    self.ie = gum.LazyPropagation(self.bn)

  def testPrior(self):
    self.assertEqual(self.ie.posterior("A"), self.joint.margSumIn(["A"]))
    self.assertEqual(self.ie.posterior("C"), self.joint.margSumIn(["C"]))
    self.assertEqual(self.ie.posterior("D"), self.joint.margSumIn(["D"]))
    self.assertEqual(self.ie.posterior("H"), self.joint.margSumIn(["H"]))

  def testPriorWithTargert(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("C")
    self.assertEqual(self.ie.posterior("A"), self.joint.margSumIn(["A"]))
    self.assertEqual(self.ie.posterior("C"), self.joint.margSumIn(["C"]))

    with self.assertRaises(gum.UndefinedElement):
      p = self.ie.posterior("D")
    with self.assertRaises(gum.UndefinedElement):
      p = self.ie.posterior("H")

  def testPriorWithTargetsEvidence(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("F")

    self.ie.addEvidence("B", 2)
    self.ie.addEvidence("D", [0.2, 0.6, 0.6])

    self.ie.makeInference()

    locpot = self.joint * \
             gum.Potential().add(self.bn.variable("B")).fillWith([0, 0, 1]) * \
             gum.Potential().add(self.bn.variable("D")).fillWith([0.2, 0.6, 0.6])

    self.assertEqual(self.ie.posterior("A"), locpot.margSumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("F"), locpot.margSumIn(["F"]).normalize())

  def testPriorWithTargetsOutsideEvidence(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("D")

    self.ie.addEvidence("A", [0.3, 0.7])
    self.ie.addEvidence("B", [0.3, 0.1, 0.8])
    self.ie.addEvidence("H", [0.4, 0.2, 0.3])

    self.ie.makeInference()

    locpot = self.joint * \
             gum.Potential().add(self.bn.variable("A")).fillWith([0.3, 0.7]) * \
             gum.Potential().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8]) * \
             gum.Potential().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])

    self.assertEqual(self.ie.posterior("A"), locpot.margSumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), locpot.margSumIn(["D"]).normalize())

    self.ie.chgEvidence("A", 0)
    self.ie.chgEvidence("B", [0.8, 0.4, 0.1])
    self.ie.chgEvidence("H", [0.2, 0.3, 0.6])

    locpot = self.joint * \
             gum.Potential().add(self.bn.variable("A")).fillWith([1, 0]) * \
             gum.Potential().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1]) * \
             gum.Potential().add(self.bn.variable("H")).fillWith([0.2, 0.3, 0.6])

    self.assertEqual(self.ie.posterior("A"), locpot.margSumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), locpot.margSumIn(["D"]).normalize())

    # error here !
    self.ie.chgEvidence("H", [0.9, 0.1, 0.3])

    locpot = self.joint * \
             gum.Potential().add(self.bn.variable("A")).fillWith([1, 0]) * \
             gum.Potential().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1]) * \
             gum.Potential().add(self.bn.variable("H")).fillWith([0.9, 0.1, 0.3])

    self.assertEqual(self.ie.posterior("A"), locpot.margSumIn(["A"]).normalize())
    print(self.ie.posterior("D"))
    print( locpot.margSumIn(["D"]).normalize())
    #self.assertEqual(self.ie.posterior("D"), locpot.margSumIn(["D"]).normalize())

    self.ie.chgEvidence("A", 0)
    self.ie.chgEvidence("H", [0.9, 0.1, 0.3])

    locpot = self.joint * \
             gum.Potential().add(self.bn.variable("A")).fillWith([1, 0]) * \
             gum.Potential().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1]) * \
             gum.Potential().add(self.bn.variable("H")).fillWith([0.9, 0.1, 0.3])

    self.assertEqual(self.ie.posterior("A"), locpot.margSumIn(["A"]).normalize())
    print(self.ie.posterior("D"))
    print( locpot.margSumIn(["D"]).normalize())
    #self.assertEqual(self.ie.posterior("D"), locpot.margSumIn(["D"]).normalize())

    self.ie.eraseEvidence("A")

    locpot = self.joint * \
             gum.Potential().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1]) * \
             gum.Potential().add(self.bn.variable("H")).fillWith([0.9, 0.1, 0.3])

    self.assertEqual(self.ie.posterior("A"), locpot.margSumIn(["A"]).normalize())
    print(self.ie.posterior("D"))
    print( locpot.margSumIn(["D"]).normalize())
    self.assertEqual(self.ie.posterior("D"), locpot.margSumIn(["D"]).normalize())


ts = unittest.TestSuite()

addTests(ts, IncrementalLazyPropagationTestCase)
