import unittest

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum.bnmixture as BNM


class TestMixtureModel(pyAgrumTestCase):
  def testBNMixture_1(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn2 = gum.BayesNet(bn0)
    bn3 = gum.BayesNet(bn0)
    bn1.addArc('A', 'B')
    bn2.addArc('B', 'A')
    bn3.eraseArc("B", "C")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.)
    bnm.add("bn1", bn1, w=2.)
    bnm.add("bn2", bn2, w=3.)

    self.assertSetEqual(set(bnm.names()), {"bn0", "bn1", "bn2"})
    self.assertEqual(bnm.size(), 3)
    self.assertDictEqual({"bn0": 1., "bn1": 2., "bn2": 3.}, bnm.weights())
    bnm.setWeight("bn2", 4.5)
    self.assertEqual(bnm.weight("bn2"), 4.5)

    # testing add exceptions
    with self.assertRaises(gum.InvalidArgument):
      bnm.add("bn3", bn3, -3)
    with self.assertRaises(gum.InvalidArgument):
      bnm.add("refBN", bn3, 2)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.BayesNet(bn0)
      bn4.changeVariableName("A", "D")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.fastBN("A{0|1|2|3|4|5}->C<-B")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.ArgumentError):
      bnm.add("bn1", bn4, 1)

    bnm.add("bn3", bn3, 2)
    self.assertSetEqual(set(bnm.names()), {"bn0", "bn1", "bn2", "bn3"})
    bnm.remove("bn3")
    self.assertSetEqual(set(bnm.names()), {"bn0", "bn1", "bn2"})
    with self.assertRaises(gum.NotFound):
      bnm.remove("bn42")

    self.assertFalse(bnm.isNormalized())
    bnm.normalize()
    self.assertTrue(bnm.isNormalized())
    self.assertTrue(bnm.isValid())
    bnm.setWeight("bn0", 0)
    bnm.setWeight("bn1", 0)
    bnm.setWeight("bn2", 0)
    self.assertFalse(bnm.isValid())
    self.assertEqual(bnm.BN("bn0"), bn0)
    self.assertEqual(bnm.BN("bn1"), bn1)
    self.assertEqual(bnm.BN("bn2"), bn2)
    self.assertSetEqual(bnm.zeroBNs(), {"bn0", "bn1", "bn2"})
    self.assertEqual(bnm.existsArc("A", "C"), 3)
    self.assertEqual(bnm.existsArc("A", "B"), 1)
    self.assertEqual(bnm.existsArc("B", "A"), 1)
    self.assertEqual(bnm.existsArc("C", "A"), 0)
    self.assertEqual(bnm.existsArc("C", "B"), 0)
    self.assertEqual(bnm.variable("A"), bn0.variable("A"))
    bnm.setWeight("bn0", 1)
    bnm.setWeight("bn1", 2)
    bnm.setWeight("bn2", 3)
    bnm.updateRef()
    self.assertEqual(bnm._refBN, bnm.BN("bn2"))

  def testBootstrapMixture_1(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn2 = gum.BayesNet(bn0)
    bn3 = gum.BayesNet(bn0)
    bn1.addArc('A', 'B')
    bn2.addArc('B', 'A')
    bn3.eraseArc("B", "C")
    bnm = BNM.BootstrapMixture("bn0", bn0)
    bnm.add("bn1", bn1, w=2.)
    bnm.add("bn2", bn2, w=3.)

    self.assertSetEqual(set(bnm.names()), {"bn1", "bn2"})
    self.assertEqual(bnm.size(), 2)
    self.assertDictEqual({"bn1": 2., "bn2": 3.}, bnm.weights())
    bnm.setWeight("bn2", 4.5)
    self.assertEqual(bnm.weight("bn2"), 4.5)

    # testing add exceptions
    with self.assertRaises(gum.InvalidArgument):
      bnm.add("bn3", bn3, -3)
    with self.assertRaises(gum.InvalidArgument):
      bnm.add("bn0", bn3, 2)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.BayesNet(bn0)
      bn4.changeVariableName("A", "D")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.fastBN("A{0|1|2|3|4|5}->C<-B")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.ArgumentError):
      bnm.add("bn1", bn3, 1)

    bnm.add("bn3", bn3, 2)
    self.assertSetEqual(set(bnm.names()), {"bn1", "bn2", "bn3"})
    bnm.remove("bn3")
    self.assertSetEqual(set(bnm.names()), {"bn1", "bn2"})
    with self.assertRaises(gum.NotFound):
      bnm.remove("bn42")

    self.assertFalse(bnm.isNormalized())
    bnm.normalize()
    self.assertTrue(bnm.isNormalized())
    self.assertTrue(bnm.isValid())
    bnm.setWeight("bn1", 0)
    bnm.setWeight("bn2", 0)
    self.assertFalse(bnm.isValid())
    self.assertEqual(bnm.BN("bn1"), bn1)
    self.assertEqual(bnm.BN("bn2"), bn2)
    self.assertSetEqual(bnm.zeroBNs(), {"bn1", "bn2"})
    self.assertEqual(bnm.existsArc("A", "C"), 2)
    self.assertEqual(bnm.existsArc("A", "B"), 1)
    self.assertEqual(bnm.existsArc("B", "A"), 1)
    self.assertEqual(bnm.existsArc("C", "A"), 0)
    self.assertEqual(bnm.existsArc("C", "B"), 0)
    self.assertEqual(bnm.variable("A"), bn0.variable("A"))


ts = unittest.TestSuite()
addTests(ts, TestMixtureModel)
