# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class LazyPropagationTestCase(pyAgrumTestCase):
  def testEvidenceImpactWithNodeId(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/asia.bif'), [],
                    verbose=False)  # verbose=False : don't want to see the warnings

    ie = gum.LazyPropagation(bn)
    with self.assertRaises(gum.InvalidArgument):
      res = ie.evidenceImpact(0, [0, 1, 2])

    res = ie.evidenceImpact(0, [1, 2])

    self.assertEqual(res.nbrDim(), 2)  # 2 indep 0 given 1

    self.assertEqual(res.extract({"tuberculosis?": 0}), gum.getPosterior(bn, target=0, evs={1: 0}))
    self.assertEqual(res.extract({"tuberculosis?": 1}), gum.getPosterior(bn, target=0, evs={1: 1}))

  def testEvidenceImpactWithName(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/asia.bif'), [],
                    verbose=False)  # verbose=False : don't want to see the warnings

    ie = gum.LazyPropagation(bn)
    with self.assertRaises(gum.InvalidArgument):
      res = ie.evidenceImpact("visit_to_Asia?", ["visit_to_Asia?", "tuberculosis?", "tuberculos_or_cancer?"])

    with self.assertRaises(gum.NotFound):
      res = ie.evidenceImpact("visit_to_Asia?", ["toto", "tuberculosis?", "tuberculos_or_cancer?"])

    res = ie.evidenceImpact("visit_to_Asia?", ["tuberculosis?", "tuberculos_or_cancer?"])

    self.assertEqual(res.nbrDim(), 2)  # 2 indep 0 given 1

    self.assertEqual(res.extract({"tuberculosis?": 0}),
                     gum.getPosterior(bn, target="visit_to_Asia?", evs={"tuberculosis?": 0}))
    self.assertEqual(res.extract({"tuberculosis?": 1}),
                     gum.getPosterior(bn, target="visit_to_Asia?", evs={"tuberculosis?": 1}))

  def testEvidenceJointImpact(self):
    bn = gum.fastBN("A->B->C->D;A->E->D;F->B;C->H;")
    ie = gum.LazyPropagation(bn)

    res = ie.evidenceJointImpact(["D", "E"], ["A", "B", "C", "F"])

    joint = bn.cpt("A") * bn.cpt("B") * bn.cpt("C") * bn.cpt("D") * bn.cpt("E") * bn.cpt("F") * bn.cpt("H")
    pADCE = joint.margSumIn(["A", "C", "D", "E"])
    pAC = pADCE.margSumOut(["D", "E"])

    self.assertEqual(res, pADCE / pAC)

  def testJointMutualInformation(self):
    bn = gum.fastBN("A->B->C->D;A->E->D;F->B;C->H;")
    ie = gum.LazyPropagation(bn)
    ie.makeInference()

    with self.assertRaises(gum.InvalidArgument):
      ie.jointMutualInformation([0])

    self.assertAlmostEqual(ie.I(0, 1), ie.jointMutualInformation([0, 1]))

    ie = gum.LazyPropagation(bn)
    ie.addJointTarget([1, 4, 3])
    ie.addAllTargets()
    ie.makeInference()

    byHandJMI = 0
    byHandJMI -= ie.jointPosterior({1, 3, 4}).entropy()
    byHandJMI += ie.jointPosterior({1, 4}).entropy() + ie.jointPosterior({1, 3}).entropy() + ie.jointPosterior(
        {4, 3}).entropy()
    byHandJMI -= ie.posterior(1).entropy() + ie.posterior(4).entropy() + ie.posterior(3).entropy()

    ie2 = gum.LazyPropagation(bn)
    JMI = ie2.jointMutualInformation({1, 3, 4})
    self.assertAlmostEqual(JMI, byHandJMI)

    ie = gum.LazyPropagation(bn)
    ie.addJointTarget({0, 1, 2, 3})
    ie.addAllTargets()
    ie.makeInference()

    byHandJMI = 0
    byHandJMI -= ie.jointPosterior({0, 1, 2, 3}).entropy()
    byHandJMI += ie.jointPosterior({0, 1, 2}).entropy() + ie.jointPosterior({0, 1, 3}).entropy() + ie.jointPosterior(
        {0, 2, 3}).entropy() + ie.jointPosterior({1, 2, 3}).entropy()
    byHandJMI -= ie.jointPosterior({0, 1}).entropy() + ie.jointPosterior({0, 2}).entropy() + ie.jointPosterior(
        {0, 3}).entropy() + ie.jointPosterior({1, 2}).entropy() + ie.jointPosterior(
        {1, 3}).entropy() + ie.jointPosterior({2, 3}).entropy()
    byHandJMI += ie.posterior(0).entropy() + ie.posterior(1).entropy() + ie.posterior(2).entropy() + ie.posterior(
        3).entropy()

    ie2 = gum.LazyPropagation(bn)
    JMI = ie2.jointMutualInformation({0, 1, 2, 3})
    self.assertAlmostEqual(JMI, byHandJMI)

ts = unittest.TestSuite()
addTests(ts, LazyPropagationTestCase)
