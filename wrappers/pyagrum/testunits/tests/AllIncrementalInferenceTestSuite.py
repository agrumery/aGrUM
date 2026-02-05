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
  def _buildBN(self):
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
    self.bn.cpt("B").fillWith([0.3, 0.4, 0.3, 0.1, 0.3, 0.6])
    self.bn.cpt("C").fillWith(
      [
        0.1,
        0.9,  # 1
        0.2,
        0.8,  # 2
        0.3,
        0.7,  # 3
        0.4,
        0.6,  # 4
        0.5,
        0.5,  # 5
        0.6,
        0.4,  # 6
        0.7,
        0.3,  # 7
        0.8,
        0.2,  # 8
        0.9,
        0.1,  # 9
        0.8,
        0.2,  # 10
        0.7,
        0.3,  # 11
        0.6,
        0.4,
      ]
    )  # 12
    self.bn.cpt("D").fillWith([0.3, 0.2, 0.5, 0.7, 0.1, 0.2])
    self.bn.cpt("E").fillWith([0.4, 0.6, 0.9, 0.1])
    self.bn.cpt("F").fillWith([0.2, 0.4, 0.4, 0.4, 0.1, 0.5, 0.7, 0.2, 0.1])
    self.bn.cpt("G").fillWith([0.1, 0.4, 0.2, 0.3])
    self.bn.cpt("H").fillWith([0.4, 0.3, 0.3, 0.1, 0.8, 0.1, 0.3, 0.4, 0.3, 0.7, 0.1, 0.2])

    # @todo add Tensor.combine method in pyAgrum
    self.joint = (
      self.bn.cpt("A")
      * self.bn.cpt("B")
      * self.bn.cpt("C")
      * self.bn.cpt("D")
      * self.bn.cpt("E")
      * self.bn.cpt("F")
      * self.bn.cpt("G")
      * self.bn.cpt("H")
    )

  def setUp(self):
    raise (NotImplementedError("This class is a generic class for Incremental Inference"))

  def testGetPosterior(self):
    self.assertEqual(gum.getPosterior(self.bn, evs={}, target="A"), self.joint.sumIn(["A"]))
    self.assertEqual(gum.getPosterior(self.bn, evs={}, target=2), self.joint.sumIn(["C"]))
    self.assertEqual(gum.getPosterior(self.bn, evs={}, target="D"), self.joint.sumIn(["D"]))

    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("F")

    self.ie.addEvidence("B", 2)
    self.ie.addEvidence("D", [0.2, 0.6, 0.6])

    self.ie.makeInference()

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 0, 1])
      * gum.Tensor().add(self.bn.variable("D")).fillWith([0.2, 0.6, 0.6])
    )

    self.assertEqual(
      gum.getPosterior(self.bn, evs={1: 2, "D": [0.2, 0.6, 0.6]}, target="A"), posterior_joint.sumIn(["A"]).normalize()
    )
    self.assertEqual(
      gum.getPosterior(self.bn, evs={"B": 2, 3: [0.2, 0.6, 0.6]}, target="F"), posterior_joint.sumIn(["F"]).normalize()
    )

  def testPrior(self):
    self.assertEqual(self.ie.posterior("A"), self.joint.sumIn(["A"]))
    self.assertEqual(self.ie.posterior("C"), self.joint.sumIn(["C"]))
    self.assertEqual(self.ie.posterior("D"), self.joint.sumIn(["D"]))
    self.assertEqual(self.ie.posterior("H"), self.joint.sumIn(["H"]))

  def testPriorWithTarget(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("C")
    self.assertEqual(self.ie.posterior("A"), self.joint.sumIn(["A"]))
    self.assertEqual(self.ie.posterior("C"), self.joint.sumIn(["C"]))

    try:
      p = self.ie.posterior("D")
      self.assertTrue(type(self.ie) is gum.VariableElimination)
      self.assertEqual(p, self.joint.sumIn(["D"]))
    except gum.UndefinedElement:
      self.assertTrue(type(self.ie) is not gum.VariableElimination)  # error not correctly caught by with ...

    try:
      p = self.ie.posterior("H")
      self.assertTrue(type(self.ie) is gum.VariableElimination)
      self.assertEqual(p, self.joint.sumIn(["H"]))
    except gum.UndefinedElement:
      self.assertTrue(type(self.ie) is not gum.VariableElimination)  # error not correctly caught by with ...

  def testPriorWithTargetsEvidence(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("F")

    self.ie.addEvidence("B", 2)
    self.ie.addEvidence("D", [0.2, 0.6, 0.6])

    self.ie.makeInference()

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 0, 1])
      * gum.Tensor().add(self.bn.variable("D")).fillWith([0.2, 0.6, 0.6])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("F"), posterior_joint.sumIn(["F"]).normalize())

  def testPriorWithTargetsOutsideEvidence(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("D")

    self.ie.addEvidence("A", [0.3, 0.7])
    self.ie.addEvidence("B", [0.3, 0.1, 0.8])
    self.ie.addEvidence("H", [0.4, 0.2, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([0.3, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.chgEvidence("A", 0)
    self.ie.chgEvidence("B", [0.8, 0.4, 0.1])
    self.ie.chgEvidence("H", [0.2, 0.3, 0.6])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.2, 0.3, 0.6])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.chgEvidence("H", [0.9, 0.1, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.9, 0.1, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.chgEvidence("A", 0)
    self.ie.chgEvidence("H", [0.8, 0.2, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.8, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.eraseEvidence("A")

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.8, 0.4, 0.1])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.8, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.addEvidence("A", 0)
    self.ie.makeInference()
    self.ie.eraseEvidence("A")
    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

  def testPriorWithTargetsWithEvidenceChanged(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("D")

    self.ie.addEvidence("A", [0.3, 0.7])
    self.ie.addEvidence("B", [0.3, 0.1, 0.8])
    self.ie.addEvidence("H", [0.4, 0.2, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([0.3, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.eraseEvidence(0)  # "A"
    self.ie.addEvidence("E", [1, 0])
    self.ie.chgEvidence("H", [0.2, 0.3, 0.6])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("E")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.2, 0.3, 0.6])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.addEvidence("A", 0)
    self.ie.chgEvidence("E", [0.7, 0.7])
    self.ie.chgEvidence("H", 1)

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("E")).fillWith([0.7, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0, 1, 0])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

  def testPriorWithTargetsHardEvidenceValueChanged(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("D")

    self.ie.addEvidence("A", [0.3, 0.7])
    self.ie.addEvidence("B", [0, 1, 0])
    self.ie.addEvidence("H", [0.4, 0.2, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([0.3, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 1, 0])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.chgEvidence("A", 0)
    self.ie.chgEvidence("H", [0.2, 0.3, 0.6])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 1, 0])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.2, 0.3, 0.6])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.chgEvidence("A", [0, 1])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([0, 1])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 1, 0])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.2, 0.3, 0.6])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.chgEvidence("A", [0.3, 0.7])
    self.ie.chgEvidence("H", [0.4, 0.2, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([0.3, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 1, 0])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.eraseEvidence("A")

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 1, 0])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.addEvidence("A", 0)

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0, 1, 0])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

  def testPriorWithTargetsEvidenceChanged(self):
    self.ie.eraseAllTargets()
    self.ie.addTarget("A")
    self.ie.addTarget("D")

    self.ie.addEvidence("A", [0.3, 0.7])
    self.ie.addEvidence("B", [0.3, 0.1, 0.8])
    self.ie.addEvidence("H", [0.4, 0.2, 0.3])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([0.3, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.4, 0.2, 0.3])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.eraseEvidence("A")
    self.ie.addEvidence("E", [1, 0])
    self.ie.chgEvidence("H", [0.2, 0.3, 0.6])

    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("E")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0.2, 0.3, 0.6])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

    self.ie.addEvidence(0, 0)
    self.ie.chgEvidence("E", [0.7, 0.7])
    self.ie.chgEvidence("H", [0, 1, 0])
    posterior_joint = (
      self.joint
      * gum.Tensor().add(self.bn.variable("A")).fillWith([1, 0])
      * gum.Tensor().add(self.bn.variable("E")).fillWith([0.7, 0.7])
      * gum.Tensor().add(self.bn.variable("B")).fillWith([0.3, 0.1, 0.8])
      * gum.Tensor().add(self.bn.variable("H")).fillWith([0, 1, 0])
    )

    self.assertEqual(self.ie.posterior("A"), posterior_joint.sumIn(["A"]).normalize())
    self.assertEqual(self.ie.posterior("D"), posterior_joint.sumIn(["D"]).normalize())

  def testJointTarget(self):
    self.ie.eraseAllTargets()
    self.ie.addJointTarget({"A", "D"})
    self.ie.addEvidence("A", [0.3, 0.7])

    pjoint = self.joint * gum.Tensor().add(self.bn.variable("A")).fillWith([0.3, 0.7])
    self.assertEqual(self.ie.jointPosterior({"A", "D"}), pjoint.sumIn(["A", "D"]).normalize())
    self.assertEqual(self.ie.jointPosterior({"A", "C"}), pjoint.sumIn(["A", "C"]).normalize())


class IncrementalLazyPropagationTestCase(IncrementalLazyPropagationTestCase):
  def setUp(self):
    self._buildBN()
    self.ie = gum.LazyPropagation(self.bn)


class IncrementalShaferShenoyTestCase(IncrementalLazyPropagationTestCase):
  def setUp(self):
    self._buildBN()
    self.ie = gum.ShaferShenoyInference(self.bn)


class IncrementalVariableEliminationTestCase(IncrementalLazyPropagationTestCase):
  def setUp(self):
    self._buildBN()
    self.ie = gum.VariableElimination(self.bn)


ts = unittest.TestSuite()
addTests(ts, IncrementalLazyPropagationTestCase)
addTests(ts, IncrementalShaferShenoyTestCase)
addTests(ts, IncrementalVariableEliminationTestCase)
