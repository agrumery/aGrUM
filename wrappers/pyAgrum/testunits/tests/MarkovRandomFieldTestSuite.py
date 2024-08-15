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

import numpy
import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class MarkovRandomFieldTestCase(pyAgrumTestCase):
  def _fill(self, mrf):
    for i in [11, 21, 31, 41]:
      mrf.add(str(i), 3)
    mrf.add(str(51), 7)
    mrf.addFactor({"11", "31"}).fillWith(
      [0.0, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8])
    mrf.addFactor({"11", "41"}).fillWith(
      [0.2, 0.3, 0.4, 0.5, 0.6, 0.7, 0.8, 0.9, 1.0])
    mrf.addFactor({"31", "51"}).fillWith(0.97)
    mrf.addFactor({"21", "41", "51"}).fillWith(0.03)

  def testConstructor(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)

    self.assertEqual(mrf.size(), 5)
    self.assertEqual(mrf.sizeEdges(), 6)
    self.assertEqual(mrf.dim(), (3 * 3 + 3 * 3 + 3 * 7 + 3 * 3 * 7))
    self.assertEqual(mrf.__str__(),
                     "MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}")
    self.assertEqual(mrf.neighbours("41"), {0, 1, 4})
    self.assertEqual(mrf.neighbours(3), {0, 1, 4})

    self.assertEqual(mrf.variable(1).name(), "21")
    self.assertEqual(mrf.nodeId(mrf.variable(2)), 2)
    self.assertEqual(mrf.idFromName("31"), 2)
    self.assertEqual(mrf.variableFromName("41").name(), "41")

    self.assertEqual(mrf.maxVarDomainSize(), 7)
    self.assertEqual(mrf.minParam(), 0.0)
    self.assertEqual(mrf.minNonZeroParam(), 0.03)
    self.assertEqual(mrf.maxParam(), 1.0)
    self.assertEqual(mrf.maxNonOneParam(), 0.97)

  def testCopyConstructor(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)
    mn2 = gum.MarkovRandomField(mrf)
    self.assertEqual(
      mn2.__str__(), "MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}")

    for n in mrf.nodes():
      self.assertEqual(mrf.variable(n).name(), mn2.variable(n).name())
      self.assertEqual(mrf.neighbours(n), mn2.neighbours(n))

  def testCopyOperator(self):
    mrf = gum.MarkovRandomField()
    mn2 = gum.MarkovRandomField()
    self._fill(mrf)
    mn2 = mrf
    self.assertEqual(mn2.__str__(),
                     "MRF{nodes: 5, edges: 6, domainSize: 567, dim: 102}")
    for n in mrf.nodes():
      self.assertEqual(mrf.variable(n).name(), mn2.variable(n).name())
      self.assertEqual(mrf.neighbours(n), mn2.neighbours(n))

  def testEqualityOperators(self):
    mn1 = gum.MarkovRandomField()
    self._fill(mn1)
    mn2 = gum.MarkovRandomField()
    self.assertNotEqual(mn1, mn2)
    self._fill(mn2)
    self.assertEqual(mn1, mn2)
    mn2.generateFactors()
    self.assertNotEqual(mn1, mn2)

  def testInsertion(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)

    with self.assertRaises(gum.InvalidArgument):
      mrf.addFactor(gum.Potential())  # no empty factor

    with self.assertRaises(gum.InvalidArgument):
      mrf.addFactor({"11", "31"})  # already exist

    mn1 = gum.MarkovRandomField()
    self._fill(mn1)
    pot = gum.Potential().add(mn1.variable("11")).add(mn1.variable("21"))
    pot.randomDistribution()
    mn1.addFactor(pot)
    self.assertEqual(pot.__str__(), mn1.factor({"11", "21"}).__str__())

    mn1 = gum.MarkovRandomField()
    self._fill(mn1)
    pot = gum.Potential().add(mn1.variable("21")).add(mn1.variable("11"))
    pot.randomDistribution()
    mn1.addFactor(pot)

    # should be equal : does not depend of the order of vars in the MarkonNet
    self.assertEqual(pot.__str__(), mn1.factor({"11", "21"}).__str__())

    # but the data should be the same
    I = gum.Instantiation(pot)
    factor = mn1.factor({"21", "11"})
    I.setFirst()
    while not I.end():
      self.assertAlmostEqual(pot.get(I), factor.get(I), places=7)
      I.inc()

  def testIterations(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)

    cpt = 0
    for node in mrf.nodes():
      cpt += 1
    self.assertEqual(cpt, mrf.size())

    cpt = 0
    for edg in mrf.edges():
      cpt += 1
    self.assertEqual(cpt, mrf.sizeEdges())

  def testEraseFactor(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)
    with self.assertRaises(gum.InvalidArgument):
      mrf.eraseFactor({12, 14})
    mrf.eraseFactor({2, 4})

    self.assertEqual(mrf.size(), 5)
    self.assertEqual(mrf.sizeEdges(), 5)
    self.assertEqual(mrf.dim(), (3 * 3 + 3 * 3 + 3 * 3 * 7))
    self.assertEqual(mrf.__str__(),
                     "MRF{nodes: 5, edges: 5, domainSize: 567, dim: 81}")

  def testEraseFactorWithNames(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)
    with self.assertRaises(gum.InvalidArgument):
      mrf.eraseFactor({"31", "21"})
    mrf.eraseFactor({"31", "51"})

    self.assertEqual(mrf.size(), 5)
    self.assertEqual(mrf.sizeEdges(), 5)
    self.assertEqual(mrf.dim(), (3 * 3 + 3 * 3 + 3 * 3 * 7))
    self.assertEqual(mrf.__str__(),
                     "MRF{nodes: 5, edges: 5, domainSize: 567, dim: 81}")

  def testErase(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)
    with self.assertRaises(gum.InvalidArgument):
      mrf.erase(36)
    mrf.erase(3)
    self.assertEqual(mrf.size(), 4)
    self.assertEqual(mrf.sizeEdges(), 3)
    self.assertEqual(mrf.dim(), (3 * 3 + 3 * 7 + 3 * 7))
    self.assertEqual(mrf.__str__(),
                     "MRF{nodes: 4, edges: 3, domainSize: 189, dim: 51}")

  def testEraseWithNames(self):
    mrf = gum.MarkovRandomField()
    self._fill(mrf)
    with self.assertRaises(gum.NotFound):
      mrf.erase("36")
    mrf.erase("41")
    self.assertEqual(mrf.size(), 4)
    self.assertEqual(mrf.sizeEdges(), 3)
    self.assertEqual(mrf.dim(), (3 * 3 + 3 * 7 + 3 * 7))
    self.assertEqual(mrf.__str__(),
                     "MRF{nodes: 4, edges: 3, domainSize: 189, dim: 51}")

  def testToDot(self):
    mrf = gum.fastMRF("A--B--C;B--D;C--E;D--E--F")
    s1 = mrf.toDot()
    s2 = mrf.toDotAsFactorGraph()

  def testFromBN(self):
    bn = gum.fastBN("A->B->C<-D;C<-E->F<-G;F<-A")
    mrf = gum.MarkovRandomField.fromBN(bn)

    pbn = gum.Potential()
    pbn.fillWith(1)
    for nod in bn.nodes():
      self.assertEqual(bn.variable(nod).__str__(), mrf.variable(nod).__str__())
      pbn *= bn.cpt(nod)

    pmn = gum.Potential()
    pmn.fillWith(1)
    for f in mrf.factors():
      pmn *= mrf.factor(f)
    pmn.normalize()

    ppmn = gum.Potential(pbn)
    ppmn.fillWith(pmn)  # copy of pmn using pbn's variables
    diff = (pbn - ppmn).abs()
    self.assertEqual(pbn.domainSize(), diff.domainSize())
    self.assertLessEqual(diff.max(), 1e-10)
    self.assertEqual(mrf.graph(), bn.moralGraph())

  def testExistsEdge(self):
    mrf = gum.fastMRF("A--B--C;C--D;E--F--G")

    self.assertTrue(mrf.existsEdge(0, 1))
    self.assertTrue(mrf.existsEdge("A", "B"))
    self.assertTrue(mrf.existsEdge(1, 0))
    self.assertTrue(mrf.existsEdge("B", "A"))
    self.assertTrue(mrf.existsEdge(0, 2))
    self.assertTrue(mrf.existsEdge("A", "C"))
    self.assertFalse(mrf.existsEdge(3, 7))
    self.assertFalse(mrf.existsEdge("C", "G"))

  def testMinimalCondSet(self):
    mrf = gum.fastMRF("A--B--C;C--D;E--F--G;B--E")

    self.assertEqual(mrf.minimalCondSet(0, {1, 2, 3, 4, 5, 6}), {1, 2})
    self.assertEqual(mrf.minimalCondSet({0, 6}, {1, 2, 3, 4, 5}), {1, 2, 4, 5})
    self.assertEqual(mrf.minimalCondSet(3, {0, 4, 5, 6}), {0, 4})

    self.assertEqual(mrf.minimalCondSet(
      "A", {"B", "C", "D", "E", "F", "G"}), {1, 2})
    self.assertEqual(mrf.minimalCondSet(
      {"A", "G"}, {"B", "C", "D", "E", "F"}), {1, 2, 4, 5})
    self.assertEqual(mrf.minimalCondSet("D", {"A", "E", "F", "G"}), {0, 4})

  def testIndependence(self):
    mrf = gum.fastMRF("A--B--C;C--D;E--F--G;B--E;D--G;X")

    self.assertTrue(mrf.isIndependent("D", "X", {}))
    self.assertFalse(mrf.isIndependent("D", "A", {"C"}))
    self.assertTrue(mrf.isIndependent("D", "A", {"C", "G"}))
    self.assertFalse(mrf.isIndependent("G", "A", {"C", "F"}))
    self.assertTrue(mrf.isIndependent("G", "A", {"D", "E"}))

  def testOrderInsertion(self):
    mrf = gum.fastMRF("V0;V1;V2;V3")

    # V0 should be the first
    mrf.addFactor(["V0", "V1"])
    self.assertEqual(mrf.factor({"V0", "V1"}).variable(0).name(), "V0")
    self.assertEqual(mrf.factor({"V1", "V0"}).variable(0).name(), "V0")

    # V2 should be the first
    mrf.addFactor(["V2", "V1"])
    self.assertEqual(mrf.factor({"V2", "V1"}).variable(0).name(), "V2")
    self.assertEqual(mrf.factor({"V1", "V2"}).variable(0).name(), "V2")

    # V2 could not be the first
    mrf.addFactor({"V2", "V3"})
    self.assertEqual({mrf.factor({"V3", "V2"}).variable(0).name(), mrf.factor({"V2", "V3"}).variable(1).name()},
                      {"V2", "V3"})

    # V2 could not be the first
    mrf.addFactor({"V2", "V0"})
    self.assertEqual({mrf.factor({"V2", "V0"}).variable(0).name(), mrf.factor({"V0", "V2"}).variable(1).name()},
                      {"V2", "V0"})

  def testOrderInsertionWithId(self):
    mrf = gum.fastMRF("V0;V1;V2;V3")

    # V0 should be the first
    mrf.addFactor([0, 1])
    self.assertEqual(mrf.factor({0, 1}).variable(0).name(), "V0")
    self.assertEqual(mrf.factor({1, 0}).variable(0).name(), "V0")

    # V2 should be the first
    mrf.addFactor([2, 1])
    self.assertEqual(mrf.factor({2, 1}).variable(0).name(), "V2")
    self.assertEqual(mrf.factor({1, 2}).variable(0).name(), "V2")

    # 2 should be the first
    mrf.addFactor({2, 3})
    self.assertEqual(mrf.factor({2, 3}).variable(0).name(), mrf.variable(2).name())

    # 1 should be the first
    mrf.addFactor({2, 0})
    self.assertEqual(mrf.factor({2, 0}).variable(0).name(), mrf.variable(0).name())

  def testToFast(self):
    model=gum.fastMRF("A--B--C--D--E;F--G[3];H--I{a|b|c}--A--B;X")
    model2=gum.fastMRF(model.toFast())
    self.assertEqual(model.names(),model2.names())
    for _,n in model:
      self.assertEqual(model.variable(n).toFast(),
                       model2.variable(n).toFast())
      self.assertEqual({model.variable(i).name() for i in model.neighbours(n)},
                       {model2.variable(i).name() for i in model2.neighbours(n)})


ts = unittest.TestSuite()
addTests(ts, MarkovRandomFieldTestCase)
