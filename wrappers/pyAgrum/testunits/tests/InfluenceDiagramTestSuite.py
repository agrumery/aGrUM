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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import numpy
import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class InfluenceDiagramTestCase(pyAgrumTestCase):
  def setUp(self):
    self.D1 = gum.LabelizedVariable("D1", "Décision 1", 2)
    self.D2 = gum.LabelizedVariable("D2", "Décision 2", 2)
    self.D3 = gum.LabelizedVariable("D3", "Décision 3", 2)
    self.D4 = gum.LabelizedVariable("D4", "Décision 4", 2)

    self.C1 = gum.LabelizedVariable("C1", "Chance 1", 2)
    self.C2 = gum.LabelizedVariable("C2", "Chance 2", 2)
    self.C3 = gum.LabelizedVariable("C3", "Chance 3", 2)
    self.C4 = gum.LabelizedVariable("C4", "Chance 4", 2)
    self.C5 = gum.LabelizedVariable("C5", "Chance 5", 2)

    self.U1 = gum.LabelizedVariable("U1", "Utility 1", 1)
    self.U2 = gum.LabelizedVariable("U2", "Utility 2", 1)

    self.diag = gum.InfluenceDiagram()

    self.diag.addChanceNode(self.C1)
    self.diag.addChanceNode(self.C2)
    self.diag.addChanceNode(self.C3)
    self.diag.addChanceNode(self.C4)
    self.diag.addChanceNode(self.C5)

    self.diag.addDecisionNode(self.D1)
    self.diag.addDecisionNode(self.D2)
    self.diag.addDecisionNode(self.D3)
    self.diag.addDecisionNode(self.D4)

    self.diag.addUtilityNode(self.U1)
    self.diag.addUtilityNode(self.U2)

    self.diag.addArc("D1", "C1")
    self.diag.addArc("C1", "C2")
    self.diag.addArc("C1", "U1")
    self.diag.addArc("C2", "D2")
    self.diag.addArc("C2", "D3")
    self.diag.addArc("D3", "C3")
    self.diag.addArc("D2", "C4")
    self.diag.addArc("C3", "C5")
    self.diag.addArc("C4", "C5")
    self.diag.addArc("C5", "U2")
    self.diag.addArc("D4", "U2")

  def testNodeType(self):
    self.assertTrue(self.diag.isDecisionNode("D1"))
    self.assertTrue(self.diag.isDecisionNode("D2"))
    self.assertTrue(self.diag.isDecisionNode("D3"))
    self.assertTrue(self.diag.isDecisionNode("D4"))

    self.assertTrue(self.diag.isChanceNode("C1"))
    self.assertTrue(self.diag.isChanceNode("C2"))
    self.assertTrue(self.diag.isChanceNode("C3"))
    self.assertTrue(self.diag.isChanceNode("C4"))
    self.assertTrue(self.diag.isChanceNode("C5"))

    self.assertTrue(self.diag.isUtilityNode("U2"))
    self.assertTrue(self.diag.isUtilityNode("U1"))

    self.assertFalse(self.diag.isChanceNode("D1"))
    self.assertFalse(self.diag.isChanceNode("U1"))
    self.assertFalse(self.diag.isDecisionNode("C1"))
    self.assertFalse(self.diag.isDecisionNode("U1"))
    self.assertFalse(self.diag.isUtilityNode("C1"))
    self.assertFalse(self.diag.isUtilityNode("D1"))

    self.assertEqual(self.diag.chanceNodeSize(), 5)
    self.assertEqual(self.diag.decisionNodeSize(), 4)
    self.assertEqual(self.diag.utilityNodeSize(), 2)

  def testCopyInfluenceDiagram(self):
    newone = gum.InfluenceDiagram(self.diag)

    self.assertEqual(newone.chanceNodeSize(), 5)
    self.assertEqual(newone.decisionNodeSize(), 4)
    self.assertEqual(newone.utilityNodeSize(), 2)

    for t, h in self.diag.arcs():
      self.assertTrue(newone.dag().existsArc(t, h))

    for i in self.diag.nodes():
      self.assertEqual(self.diag.variable(i).__str__(), newone.variable(i).__str__())

  def testBugInference(self):
    tst_id = gum.fastID("c1<-c->$u<-*d")

    tst_id.cpt("c").fillWith([0.5, 0.5])

    tst_id.cpt("c1")[{'c': 0}] = [1, 0]
    tst_id.cpt("c1")[{'c': 1}] = [0, 1]

    tst_id.utility("u")[{'c': 0, 'd': 0}] = [10]
    tst_id.utility("u")[{'c': 0, 'd': 1}] = [21]
    tst_id.utility("u")[{'c': 1, 'd': 0}] = [100]
    tst_id.utility("u")[{'c': 1, 'd': 1}] = [200]

    ie = gum.ShaferShenoyLIMIDInference(tst_id)
    ie.makeInference()
    self.assertEqual(ie.optimalDecision("d"), gum.Tensor().add(tst_id.variableFromName("d")).fillWith([0, 1]))
    self.assertEqual(ie.MEU()['mean'], 110.5)

  def testBugInferenceWithEvidence(self):
    tst_id = gum.fastID("c1<-c->$u<-*d")

    tst_id.cpt("c").fillWith([0.5, 0.5])

    tst_id.cpt("c1")[{'c': 0}] = [1, 0]
    tst_id.cpt("c1")[{'c': 1}] = [0, 1]

    tst_id.utility("u")[{'c': 0, 'd': 0}] = [10]
    tst_id.utility("u")[{'c': 0, 'd': 1}] = [21]
    tst_id.utility("u")[{'c': 1, 'd': 0}] = [100]
    tst_id.utility("u")[{'c': 1, 'd': 1}] = [200]

    ie = gum.ShaferShenoyLIMIDInference(tst_id)
    ie.setEvidence({'c': 0})
    ie.makeInference()
    self.assertEqual(ie.optimalDecision("d"), gum.Tensor().add(tst_id.variableFromName("d")).fillWith([0, 1]))
    self.assertEqual(ie.MEU()['mean'], 21)

    ie = gum.ShaferShenoyLIMIDInference(tst_id)
    ie.setEvidence({'c': 1})
    ie.makeInference()
    self.assertEqual(ie.optimalDecision("d"), gum.Tensor().add(tst_id.variableFromName("d")).fillWith([0, 1]))
    self.assertEqual(ie.MEU()['mean'], 200)

  def testBugInferenceWithEvidenceWithSemiFastSyntax(self):
    tst_id = gum.InfluenceDiagram()
    tst_id.addVariables(["c1", "c", "$u", "*d"])
    tst_id.addArcs([("c", "c1"),
                    ("c", "u"),
                    ("d", "u")])

    tst_id.cpt("c").fillWith([0.5, 0.5])

    tst_id.cpt("c1")[{'c': 0}] = [1, 0]
    tst_id.cpt("c1")[{'c': 1}] = [0, 1]

    tst_id.utility("u")[{'c': 0, 'd': 0}] = [10]
    tst_id.utility("u")[{'c': 0, 'd': 1}] = [21]
    tst_id.utility("u")[{'c': 1, 'd': 0}] = [100]
    tst_id.utility("u")[{'c': 1, 'd': 1}] = [200]

    ie = gum.ShaferShenoyLIMIDInference(tst_id)
    ie.setEvidence({'c': 0})
    ie.makeInference()
    self.assertEqual(ie.optimalDecision("d"), gum.Tensor().add(tst_id.variableFromName("d")).fillWith([0, 1]))
    self.assertEqual(ie.MEU()['mean'], 21)

    ie = gum.ShaferShenoyLIMIDInference(tst_id)
    ie.setEvidence({'c': 1})
    ie.makeInference()
    self.assertEqual(ie.optimalDecision("d"), gum.Tensor().add(tst_id.variableFromName("d")).fillWith([0, 1]))
    self.assertEqual(ie.MEU()['mean'], 200)

  def testToFast(self):
    model=gum.fastID("*A->B->$C<-*D->E;F[3]->$G;*H->I{a|b|c};X")
    model2=gum.fastID(model.toFast())
    self.assertEqual(model.names(),model2.names())
    for _,n in model:
      self.assertEqual(model.isChanceNode(n),model2.isChanceNode(n))
      self.assertEqual(model.isUtilityNode(n),model2.isUtilityNode(n))
      self.assertEqual(model.isDecisionNode(n),model2.isDecisionNode(n))
      self.assertEqual(model.variable(n).toFast(),
                       model2.variable(n).toFast())
      self.assertEqual({model.variable(i).name() for i in model.parents(n)},
                       {model2.variable(i).name() for i in model2.parents(n)})

ts = unittest.TestSuite()
addTests(ts, InfluenceDiagramTestCase)
