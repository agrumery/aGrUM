# -*- encoding: UTF-8 -*-
import unittest

import numpy
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class MarkovNetTestCase(pyAgrumTestCase):
  def _fill(self, mn):
    for i in [11, 21, 31, 41]:
      mn.add(str(i), 3)
    mn.add(str(51), 7)
    mn.addFactor({"11", "31"}).fillWith(
        [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])
    mn.addFactor({"11", "41"}).fillWith(
        [0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0])
    mn.addFactor({"31", "51"}).fillWith(0.97)
    mn.addFactor({"21", "41", "51"}).fillWith(0.03)

  def testConstructor(self):
    mn = gum.MarkovNet()
    self._fill(mn)

    self.assertEquals(mn.size(), 5)
    self.assertEquals(mn.sizeEdges(), 6)
    self.assertEquals(mn.dim(), (3 * 3 + 3 * 3 + 3 * 7 + 3 * 3 * 7))
    self.assertEquals(mn.__str__(),
                      "MN{nodes: 5, edges: 6, domainSize: 567, dim: 102}")
    self.assertEquals(mn.neighbours("41"), {0, 1, 4})
    self.assertEquals(mn.neighbours(3), {0, 1, 4})

    self.assertEquals(mn.variable(1).name(), "21")
    self.assertEquals(mn.nodeId(mn.variable(2)), 2)
    self.assertEquals(mn.idFromName("31"), 2)
    self.assertEquals(mn.variableFromName("41").name(), "41")

    self.assertEquals(mn.maxVarDomainSize(), 7)
    self.assertEquals(mn.minParam(), 0.0)
    self.assertEquals(mn.minNonZeroParam(), 0.03)
    self.assertEquals(mn.maxParam(), 1.0)
    self.assertEquals(mn.maxNonOneParam(), 0.97)

  def testCopyConstructor(self):
    mn = gum.MarkovNet()
    self._fill(mn)
    mn2 = gum.MarkovNet(mn)
    self.assertEquals(
        mn2.__str__(), "MN{nodes: 5, edges: 6, domainSize: 567, dim: 102}")

    for n in mn.nodes():
      self.assertEquals(mn.variable(n).name(), mn2.variable(n).name())
      self.assertEquals(mn.neighbours(n), mn2.neighbours(n))

  def testCopyOperator(self):
    mn = gum.MarkovNet()
    mn2 = gum.MarkovNet()
    self._fill(mn)
    mn2 = mn
    self.assertEquals(mn2.__str__(),
                      "MN{nodes: 5, edges: 6, domainSize: 567, dim: 102}")
    for n in mn.nodes():
      self.assertEquals(mn.variable(n).name(), mn2.variable(n).name())
      self.assertEquals(mn.neighbours(n), mn2.neighbours(n))

  def testEqualityOperators(self):
    mn1 = gum.MarkovNet()
    self._fill(mn1)
    mn2 = gum.MarkovNet()
    self.assertNotEqual(mn1, mn2)
    self._fill(mn2)
    self.assertEquals(mn1, mn2)
    mn2.generateFactors()
    self.assertNotEqual(mn1, mn2)

  def testInsertion(self):
    mn = gum.MarkovNet()
    self._fill(mn)

    with self.assertRaises(gum.InvalidArgument):
      mn.addFactor(gum.Potential())  # no empty factor

    with self.assertRaises(gum.InvalidArgument):
      mn.addFactor({"11", "31"})  # already exist

    mn1 = gum.MarkovNet()
    self._fill(mn1)
    pot = gum.Potential().add(mn1.variable("11")).add(mn1.variable("21"))
    pot.randomDistribution()
    mn1.addFactor(pot)
    self.assertEquals(pot.__str__(), mn1.factor({"11", "21"}).__str__())

    mn1 = gum.MarkovNet()
    self._fill(mn1)
    pot = gum.Potential().add(mn1.variable("21")).add(mn1.variable("11"))
    pot.randomDistribution()
    mn1.addFactor(pot)

    # should be different because of the sorting by order of the vars in pot.
    self.assertNotEqual(pot.__str__(), mn1.factor({"11", "21"}).__str__())

    # but the data should be the same
    I = gum.Instantiation(pot)
    factor = mn1.factor({"21", "11"})
    I.setFirst()
    while not I.end():
      self.assertAlmostEqual(pot.get(I), factor.get(I), places=7)
      I.inc()

  def testIterations(self):
    mn = gum.MarkovNet()
    self._fill(mn)

    cpt = 0
    for node in mn.nodes():
      cpt += 1
    self.assertEquals(cpt, mn.size())

    cpt = 0
    for edg in mn.edges():
      cpt += 1
    self.assertEquals(cpt, mn.sizeEdges())

  def testEraseFactor(self):
    mn = gum.MarkovNet()
    self._fill(mn)
    with self.assertRaises(gum.InvalidArgument):
      mn.eraseFactor({12, 14})
    mn.eraseFactor({2, 4})

    self.assertEquals(mn.size(), 5)
    self.assertEquals(mn.sizeEdges(), 5)
    self.assertEquals(mn.dim(), (3 * 3 + 3 * 3 + 3 * 3 * 7))
    self.assertEquals(mn.__str__(),
                      "MN{nodes: 5, edges: 5, domainSize: 567, dim: 81}")

  def testEraseFactorWithNames(self):
    mn = gum.MarkovNet()
    self._fill(mn)
    with self.assertRaises(gum.InvalidArgument):
      mn.eraseFactor({"31", "21"})
    mn.eraseFactor({"31", "51"})

    self.assertEquals(mn.size(), 5)
    self.assertEquals(mn.sizeEdges(), 5)
    self.assertEquals(mn.dim(), (3 * 3 + 3 * 3 + 3 * 3 * 7))
    self.assertEquals(mn.__str__(),
                      "MN{nodes: 5, edges: 5, domainSize: 567, dim: 81}")

  def testErase(self):
    mn = gum.MarkovNet()
    self._fill(mn)
    with self.assertRaises(gum.InvalidArgument):
      mn.erase(36)
    mn.erase(3)
    self.assertEquals(mn.size(), 4)
    self.assertEquals(mn.sizeEdges(), 3)
    self.assertEquals(mn.dim(), (3 * 3 + 3 * 7 + 3 * 7))
    self.assertEquals(mn.__str__(),
                      "MN{nodes: 4, edges: 3, domainSize: 189, dim: 51}")

  def testEraseWithNames(self):
    mn = gum.MarkovNet()
    self._fill(mn)
    with self.assertRaises(gum.NotFound):
      mn.erase("36")
    mn.erase("41")
    self.assertEquals(mn.size(), 4)
    self.assertEquals(mn.sizeEdges(), 3)
    self.assertEquals(mn.dim(), (3 * 3 + 3 * 7 + 3 * 7))
    self.assertEquals(mn.__str__(),
                      "MN{nodes: 4, edges: 3, domainSize: 189, dim: 51}")

  def testToDot(self):
    mn = gum.fastMN("A-B-C;B-D;C-E;D-E-F")
    s1 = mn.toDot()
    s2 = mn.toDotAsFactorGraph()

  def testFromBN(self):
    bn = gum.fastBN("A->B->C<-D;C<-E->F<-G;F<-A")
    mn = gum.MarkovNet.fromBN(bn)

    pbn = gum.Potential()
    pbn.fillWith(1)
    for nod in bn.nodes():
      self.assertEquals(bn.variable(nod).__str__(), mn.variable(nod).__str__())
      pbn *= bn.cpt(nod)

    pmn = gum.Potential()
    pmn.fillWith(1)
    for f in mn.factors():
      pmn *= mn.factor(f)
    pmn.normalize()

    ppmn = gum.Potential(pbn)
    ppmn.fillWith(pmn)  # copy of pmn using pbn's variables
    diff = (pbn - ppmn).abs()
    self.assertEquals(pbn.domainSize(), diff.domainSize())
    self.assertLessEqual(diff.max(), 1e-10)
    self.assertEquals(mn.graph(), bn.moralGraph())

  def testExistsEdge(self):
    mn = gum.fastMN("A-B-C;C-D;E-F-G")

    self.assertTrue(mn.existsEdge(0, 1))
    self.assertTrue(mn.existsEdge("A", "B"))
    self.assertTrue(mn.existsEdge(1, 0))
    self.assertTrue(mn.existsEdge("B", "A"))
    self.assertTrue(mn.existsEdge(0, 2))
    self.assertTrue(mn.existsEdge("A", "C"))
    self.assertFalse(mn.existsEdge(3, 7))
    self.assertFalse(mn.existsEdge("C", "G"))

  def testMinimalCondSet(self):
    mn = gum.fastMN("A-B-C;C-D;E-F-G;B-E")

    self.assertEqual(mn.minimalCondSet(0, {1, 2, 3, 4, 5, 6}), {1, 2})
    self.assertEqual(mn.minimalCondSet({0, 6}, {1, 2, 3, 4, 5}), {1, 2, 4, 5})
    self.assertEqual(mn.minimalCondSet(3, {0, 4, 5, 6}), {0, 4})

    self.assertEqual(mn.minimalCondSet(
        "A", {"B", "C", "D", "E", "F", "G"}), {1, 2})
    self.assertEqual(mn.minimalCondSet(
        {"A", "G"}, {"B", "C", "D", "E", "F"}), {1, 2, 4, 5})
    self.assertEqual(mn.minimalCondSet("D", {"A", "E", "F", "G"}), {0, 4})


ts = unittest.TestSuite()
addTests(ts, MarkovNetTestCase)
