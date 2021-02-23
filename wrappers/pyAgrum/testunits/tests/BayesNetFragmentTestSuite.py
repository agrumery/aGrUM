# -*- encoding: UTF-8 -*-
import unittest

import numpy
import math
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class BayesNetFragmentTestCase(pyAgrumTestCase):
  def fill(self):
    return gum.fastBN("v1;v2;v3;v4;v5[3];v6[3]<-v3<-v1->v4<-v2->v5<-v3;v4->v5")

  def fill2(self, bn1):
    bn2 = gum.fastBN("v1;v2;v3;v4;v5[3];v6[3]<-v3<-v1->v4<-v2->v5<-v3")
    for n in bn1.names():
      if n != "v5":
        bn2.cpt(n).fillWith(bn1.cpt(n))
    return bn2

  def testCreation(self):
    bn = self.fill()
    frag = gum.BayesNetFragment(bn)
    frag2 = gum.BayesNetFragment(frag)

  def testInstallNodes(self):
    bn = self.fill()
    frag = gum.BayesNetFragment(bn)

    self.assertEqual(frag.size(), 0)
    frag.installNode("v1")
    self.assertEqual(frag.size(), 1)
    self.assertTrue(not frag.empty())

    frag.installNode("v1")  # once again : no effect
    self.assertEqual(frag.size(), 1)
    self.assertEqual(frag.sizeArcs(), 0)

    frag.installNode("v6")  # second node, without arc v2-v6
    self.assertEqual(frag.size(), 2)
    self.assertEqual(frag.sizeArcs(), 0)

    frag.installNode("v3")  # third node, without arc v2-v3-v6
    self.assertEqual(frag.size(), 3)
    self.assertEqual(frag.sizeArcs(), 2)

    frag.installAscendants("v6")  # nothing should happen
    self.assertEqual(frag.size(), 3)
    self.assertEqual(frag.sizeArcs(), 2)

    frag.installAscendants("v5")
    self.assertEqual(frag.size(), 6)
    self.assertEqual(frag.sizeArcs(), 7)

    frag2 = gum.BayesNetFragment(bn)
    frag2.installAscendants("v5")
    self.assertEqual(frag2.size(), 5)
    self.assertEqual(frag2.sizeArcs(), 6)

  def testUninstallNode(self):
    bn = self.fill()
    frag = gum.BayesNetFragment(bn)
    frag.installAscendants("v6")
    self.assertEqual(frag.size(), 3)
    self.assertEqual(frag.sizeArcs(), 2)

    frag.uninstallNode("v3")
    self.assertEqual(frag.size(), 2)
    self.assertEqual(frag.sizeArcs(), 0)

  def testBayesNetMethods(self):
    bn = self.fill()
    frag = gum.BayesNetFragment(bn)

    self.assertTrue(frag.empty())
    frag.installNode("v1")
    self.assertTrue(not frag.empty())
    frag.installNode("v6")

    self.assertEqual(frag.dag().sizeArcs(), 0)
    self.assertEqual(frag.size(), 2)
    self.assertEqual(frag.dim(), (3 - 1) + (2 - 1))
    self.assertEqual(pow(10, frag.log10DomainSize()), 2 * 3)

    frag.installAscendants("v6")

    self.assertEqual(frag.dag().sizeArcs(), 2)
    self.assertEqual(frag.size(), 3)
    self.assertEqual(frag.dim(), (2 * (3 - 1)) + (2 * (2 - 1)) + (2 - 1))
    self.assertAlmostEqual(
      pow(10, frag.log10DomainSize()), 2 * 2 * 3, delta=1e-5)

    I = frag.completeInstantiation()
    I.setFirst()
    self.assertEqual(I.__str__(), "<v1:0|v3:0|v6:0>")

    while not I.end():
      p = bn.cpt("v1").get(I) * bn.cpt("v3").get(I) * bn.cpt('v6').get(I)
      self.assertAlmostEqual(frag.jointProbability(I), p, 1e-5)
      self.assertAlmostEqual(frag.log2JointProbability(I),
                             math.log(p, 2), delta=1e-5)
      I.inc()

  def testRelevantReasonning(self):
    # an inference for all the bn with an hard evidence and an inference for
    # the right fragment with a local CPT should be the same
    bn = self.fill()
    inf_complete = gum.LazyPropagation(bn)
    inf_complete.setEvidence({"v3": 1})
    inf_complete.makeInference()
    p = inf_complete.posterior("v6")

    frag = gum.BayesNetFragment(bn)
    frag.installAscendants("v6")
    marg = gum.Potential().add(frag.variable("v3"))
    marg.fillWith([0, 1])
    frag.installMarginal("v3", marg)
    self.assertEqual(frag.size(), 3)
    self.assertEqual(frag.sizeArcs(), 1)
    inf_frag = gum.LazyPropagation(frag)
    inf_frag.makeInference()

    for x1, x2 in zip(inf_complete.posterior("v6").tolist(),
                      inf_frag.posterior("v6").tolist()):
      self.assertAlmostEqual(x1, x2, delta=1e-5)

  def testInstallCPTs(self):
    bn = self.fill()
    frag = gum.BayesNetFragment(bn)
    frag.installAscendants("v6")
    self.assertEqual(frag.size(), 3)
    self.assertEqual(frag.sizeArcs(), 2)
    for nod in frag.nodes():
      self.assertTrue(frag.checkConsistency(nod))
    self.assertTrue(frag.checkConsistency())

    frag.installNode("v5")
    # 1->3->6 et 3->5 but 5 does not have all this parents (2,3 et 4)
    with self.assertRaises(gum.NotFound):
      v = frag.variable("v4").name()
    with self.assertRaises(gum.NotFound):
      v = frag.variable(bn.idFromName("v2")).name()
    self.assertEqual(frag.size(), 4)
    self.assertEqual(frag.sizeArcs(), 3)
    self.assertTrue(not frag.checkConsistency())
    self.assertTrue(not frag.checkConsistency("v5"))
    for nod in frag.nodes():
      if frag.variable(nod).name() != "v5":
        self.assertTrue(frag.checkConsistency(nod))

    newV5 = gum.Potential().add(frag.variable("v5"))
    newV5.fillWith([0, 0, 1])
    frag.installMarginal("v5", newV5)
    for nod in frag.nodes():
      self.assertTrue(frag.checkConsistency(nod))
    self.assertTrue(frag.checkConsistency())
    self.assertEqual(frag.size(), 4)
    self.assertEqual(frag.sizeArcs(), 2)

    frag.installAscendants("v4")
    self.assertTrue(not frag.checkConsistency())
    self.assertEqual(frag.size(), 6)
    self.assertEqual(frag.sizeArcs(), 6)

    frag.uninstallCPT("v5")
    for nod in frag.nodes():
      self.assertTrue(frag.checkConsistency(nod))
    self.assertTrue(frag.checkConsistency())
    self.assertEqual(frag.size(), 6)
    self.assertEqual(frag.sizeArcs(), 7)

    frag.uninstallNode("v4")
    self.assertTrue(not frag.checkConsistency())
    self.assertEqual(frag.size(), 5)
    self.assertEqual(frag.sizeArcs(), 4)

    newV5bis = gum.Potential().add(frag.variable("v5")).add(
      frag.variable("v2")).add(frag.variable("v3"))
    frag.installCPT("v5", newV5bis)
    self.assertTrue(frag.checkConsistency())
    self.assertEqual(frag.size(), 5)
    self.assertEqual(frag.sizeArcs(), 4)

  def testInferenceWithLocalsCPT(self):
    bn = self.fill()
    bn2 = self.fill2(bn)
    frag = gum.BayesNetFragment(bn)
    for i in bn.nodes():
      frag.installNode(i)
    self.assertTrue(frag.checkConsistency())
    self.assertEqual(frag.size(), 6)
    self.assertEqual(frag.sizeArcs(), 7)

    newV5 = gum.Potential().add(frag.variable("v5")).add(
      frag.variable("v2")).add(frag.variable("v3"))
    newV5.fillWith(bn2.cpt("v5"))
    frag.installCPT("v5", newV5)
    self.assertTrue(frag.checkConsistency())
    self.assertEqual(frag.size(), 6)
    self.assertEqual(frag.sizeArcs(), 6)

    ie2 = gum.LazyPropagation(bn2)
    ie2.makeInference()
    ie = gum.LazyPropagation(frag)
    ie.makeInference()

    for n in frag.names():
      for x1, x2 in zip(ie2.posterior(n).tolist(), ie.posterior(n).tolist()):
        self.assertAlmostEqual(
          x1, x2, delta=1e-5, msg="For variable '{}'".format(n))

  def testCopyToBN(self):
    bn = gum.fastBN("A->B->C->D;E<-C<-F")
    self.assertEqual(repr(bn.cpt("B").variable(1)), repr(bn.variable("A")))

    frag = gum.BayesNetFragment(bn)

    frag.installNode("B")
    self.assertFalse(frag.checkConsistency())
    with self.assertRaises(gum.OperationNotAllowed):
      minibn = frag.toBN()

    # checking if the nodes are well copied and referenced in frag and then in
    # minibn checking if the potential are well copied
    frag.installNode("A")
    self.assertTrue(frag.checkConsistency())
    self.assertEqual(repr(bn.variable("A")), repr(frag.variable("A")))
    self.assertEqual(repr(bn.variable("B")), repr(frag.variable("B")))
    self.assertEqual(str(bn.cpt("A")), str(frag.cpt("A")))
    self.assertEqual(str(bn.cpt("B")), str(frag.cpt("B")))
    self.assertEqual(repr(frag.cpt("B").variable(1)), repr(bn.variable("A")))
    self.assertEqual(repr(frag.cpt("B").variable(1)), repr(frag.variable("A")))

    minibn = frag.toBN()
    self.assertEqual(minibn.size(), 2)
    self.assertEqual(minibn.sizeArcs(), 1)
    self.assertNotEqual(repr(bn.variable("A")), repr(minibn.variable("A")))
    self.assertNotEqual(repr(bn.variable("B")), repr(minibn.variable("B")))
    self.assertEqual(str(bn.cpt("A")), str(minibn.cpt("A")))
    self.assertEqual(str(bn.cpt("B")), str(minibn.cpt("B")))
    self.assertEqual(repr(minibn.cpt("B").variable(1)), repr(minibn.variable("A")))
    self.assertNotEqual(repr(minibn.cpt("B").variable(1)), repr(frag.variable("A")))


ts = unittest.TestSuite()
addTests(ts, BayesNetFragmentTestCase)
