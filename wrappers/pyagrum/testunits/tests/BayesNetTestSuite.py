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

import pyagrum.lib.bn_vs_bn as bvb
import unittest

import numpy
import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class BayesNetTestCase(pyAgrumTestCase):
  def setUp(self):
    self.var1 = gum.LabelizedVariable("var1", "1", 2)
    self.var2 = gum.LabelizedVariable("var2", "2", 2)
    self.var3 = gum.LabelizedVariable("var3", "3", 2)
    self.var4 = gum.LabelizedVariable("var4", "4", 2)
    self.var5 = gum.LabelizedVariable("var5", "(gum::Size) 5", 3)
    self.bufferlisten = ""
    self.bufferecoute = ""

  def fillTopo(self, bn, idList):
    idList.append(bn.add(self.var1))
    idList.append(bn.add(self.var2))
    idList.append(bn.add(self.var3))
    idList.append(bn.add(self.var4))
    idList.append(bn.add(self.var5))

    bn.addArc(idList[0], idList[2])
    bn.addArc(idList[2], idList[4])
    bn.addArc(idList[1], idList[3])
    bn.addArc(idList[0], idList[3])
    bn.addArc(idList[3], idList[4])
    bn.addArc(idList[1], idList[4])

  def fillBN(self, bn, idList):
    self.fillTopo(bn, idList)

    bn.cpt(idList[0]).fillWith([0.2, 0.8])

    bn.cpt(idList[1]).fillWith([0.3, 0.7])

    bn.cpt(idList[2]).fillWith([0.1, 0.9, 0.9, 0.1])

    bn.cpt(idList[3]).fillWith([0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0])

    bn.cpt(idList[4]).fillWith(
      [
        0.3,
        0.6,
        0.1,
        0.5,
        0.5,
        0.0,
        0.5,
        0.5,
        0.0,
        1.0,
        0.0,
        0.0,
        0.4,
        0.6,
        0.0,
        0.5,
        0.5,
        0.0,
        0.5,
        0.5,
        0.0,
        0.0,
        0.0,
        1.0,
      ]
    )


class TestConstructors(BayesNetTestCase):
  def testConstructor(self):
    _ = gum.BayesNet("French Touch Network")

  def testCopyConstructor(self):
    bn1 = gum.BayesNet()
    _ = gum.BayesNet(bn1)


class TestInsertions(BayesNetTestCase):
  def testVariables(self):
    bn = gum.BayesNet()
    idList = []

    idList.append(bn.add(self.var1))
    idList.append(bn.add(self.var2))
    idList.append(bn.add(self.var3))
    idList.append(bn.add(self.var4))
    idList.append(bn.add(self.var5))

    self.assertEqual(bn.size(), 5)

    i = 1
    for id_a in idList[:-1]:
      for id_b in idList[i : len(idList)]:
        self.assertNotEqual(id_a, id_b)
      i += 1

    varPtr = bn.variable(idList[0])
    self.assertEqual(varPtr, self.var1)

    self.assertEqual(bn.variable(idList[0]), self.var1)
    self.assertEqual(bn.variable(idList[1]), self.var2)
    self.assertEqual(bn.variable(idList[2]), self.var3)
    self.assertEqual(bn.variable(idList[3]), self.var4)
    self.assertEqual(bn.variable(idList[4]), self.var5)

  def testArcs(self):
    bn = gum.BayesNet()
    idList = []

    idList.append(bn.add(self.var1))
    idList.append(bn.add(self.var2))
    idList.append(bn.add(self.var3))
    idList.append(bn.add(self.var4))
    idList.append(bn.add(self.var5))

    bn.addArc(idList[0], idList[2])
    bn.addArc(idList[2], idList[4])
    bn.addArc(idList[1], idList[3])
    bn.addArc(idList[0], idList[3])
    bn.addArc(idList[3], idList[4])
    bn.addArc(idList[1], idList[4])

    with self.assertRaises(gum.DuplicateElement):
      bn.addArc(idList[1], idList[4])

    self.assertRaises(gum.InvalidDirectedCycle, bn.addArc, idList[4], idList[0])
    self.assertRaises(gum.InvalidNode, bn.addArc, 42, 666)

  def testEraseArcs(self):
    bn = gum.BayesNet()
    idList = []

    self.assertTrue(bn.empty())

    self.fillTopo(bn, idList)

    self.assertFalse(bn.empty())
    self.assertEqual(bn.size(), 5)

    bn.eraseArc(idList[0], idList[2])
    bn.eraseArc(idList[2], idList[4])
    bn.eraseArc(idList[1], idList[3])
    bn.eraseArc(idList[0], idList[3])
    bn.eraseArc(idList[3], idList[4])
    bn.eraseArc(idList[1], idList[4])

    self.assertFalse(bn.empty())

  def testFastBuilders(self):
    bn1 = gum.BayesNet()
    bn1.add(gum.RangeVariable("A", "A", 0, 2))
    bn1.add(gum.LabelizedVariable("B", "B", ["a", "b", "c"]))
    bn1.add(gum.RangeVariable("C", "C", 4, 7))
    bn1.add(gum.IntegerVariable("D", "D", [1, 3, 10]))
    bn1.add(gum.DiscretizedVariable("E", "E", [1, 1.5, 3, 3.14, 15]))
    bn1.addArc("A", "B")
    bn1.addArc("B", "C")
    bn1.addArc("C", "D")
    bn1.addArc("D", "E")
    self.assertEqual(len(bn1.check()), 5)  # every cpt is faulty

    bn2 = gum.fastBN("A->B{a|b|c}->C[4,7]->D{1|3|10}->E[1,1.5,3,3.14,15]", 3)
    self.assertEqual(len(bn2.check()), 0)  # but random

    bn3 = gum.BayesNet()
    bn3.add("A", 3)
    bn3.add("B{a|b|c}")
    bn3.add("C[4,7]")
    bn3.add("D{1|3|10}")
    bn3.add("E[1,1.5,3,3.14,15]")
    bn3.addArc("A", "B")
    bn3.addArc("B", "C")
    bn3.addArc("C", "D")
    bn3.addArc("D", "E")
    self.assertEqual(len(bn3.check()), 5)  # every cpt is faulty

    bn4 = gum.BayesNet()
    bn4.addVariables(["A", "B{a|b|c}", "C[4,7]", "D{1|3|10}", "E[1,1.5,3,3.14,15]"], 3)
    bn4.addArcs([("A", "B"), ("B", "C"), ("C", "D"), ("D", "E")])
    self.assertEqual(len(bn4.check()), 5)  # every cpt is faulty

    for name in "ABCDE":
      self.assertEqual(bn1.variable(name), bn2.variable(name))
      self.assertEqual(bn1.variable(name), bn3.variable(name))
      self.assertEqual(bn1.variable(name), bn4.variable(name))


class TestFeatures(BayesNetTestCase):
  def testMoralGraph(self):
    bn = gum.BayesNet()
    idList = []

    self.fillTopo(bn, idList)
    _ = bn.moralGraph()

  def testTopologicalOrder(self):
    bn = gum.BayesNet()
    idList = []

    self.fillTopo(bn, idList)
    topoOrder = bn.topologicalOrder()
    self.assertEqual(len(topoOrder), 5)

  def testChangeLabel(self):
    bn = gum.BayesNet()

    bn.loadBIF(self.agrumSrcDir("alarm.bif"))

    self.assertEqual(str(bn.variable(0)), "HISTORY:Labelized({TRUE|FALSE})")
    bn.variable(0).asLabelizedVar().changeLabel(0, "toto")
    self.assertNotEqual(str(bn.variable(0)), "HISTORY:Labelized({toto|FALSE})")
    bn.changeVariableLabel(0, "TRUE", "toto")
    self.assertEqual(str(bn.variable(0)), "HISTORY:Labelized({toto|FALSE})")

  def testStringAccessors(self):
    bn = gum.BayesNet()
    for s in "ABC":
      bn.add(gum.LabelizedVariable(s, "", 2))
    for arc in [("A", "C"), ("B", "C")]:
      bn.addArc(*arc)

    with self.assertRaises(gum.DuplicateElement):
      bn.addArc("A", "C")

    self.assertEqual(str(bn), "BN{nodes: 3, arcs: 2, domainSize: 8, dim: 6, mem: 96o}")

    bn.cpt("A").fillWith(1).normalize()
    bn.generateCPT("B")
    bn.generateCPT("C")
    with self.assertRaises(gum.NotFound):
      bn.cpt("XX")

    bn.reverseArc("A", "C")
    self.assertEqual(str(bn), "BN{nodes: 3, arcs: 3, domainSize: 8, dim: 7, mem: 112o}")

    with self.assertRaises(gum.InvalidArc):
      bn.reverseArc("A", "C")
    with self.assertRaises(gum.GraphError):
      bn.reverseArc("A", "C")
    with self.assertRaises(gum.NotFound):
      bn.reverseArc("A", "X")

    bn.erase("A")
    self.assertEqual(str(bn), "BN{nodes: 2, arcs: 1, domainSize: 4, dim: 3, mem: 48o}")

    self.assertEqual(bn.parents(bn.idFromName("C")), bn.parents("C"))

    with self.assertRaises(gum.NotFound):
      bn.erase("A")

    bn.eraseArc("B", "C")
    self.assertEqual(str(bn), "BN{nodes: 2, arcs: 0, domainSize: 4, dim: 2, mem: 32o}")

    with self.assertRaises(gum.NotFound):
      bn.eraseArc("B", "C")

  def testShortcutAdd(self):
    bn = gum.BayesNet()
    a = bn.add("A", 2)
    b = bn.add("B", 3)
    self.assertEqual(a, 0)
    self.assertEqual(b, 1)

    with self.assertRaises(gum.DuplicateLabel):
      bn.add("A", 5)
    with self.assertRaises(gum.InvalidArgument):
      bn.add("C", 0)

    bn.addArc("A", "B")
    self.assertEqual(bn.log10DomainSize(), numpy.log10(6.0))
    self.assertEqual(bn.dim(), 5)

  def testSomeFunctions(self):
    bn = gum.BayesNet()
    idList = []
    self.fillBN(bn, idList)

    self.assertEqual(bn.maxVarDomainSize(), 3)
    self.assertEqual(bn.minParam(), 0.0)
    self.assertEqual(bn.maxParam(), 1.0)
    self.assertEqual(bn.minNonZeroParam(), 0.1)
    self.assertEqual(bn.maxNonOneParam(), 0.9)

  def test_fastBN(self):
    bn = gum.fastBN("a->b->c;a->c")
    self.assertEqual(bn.size(), 3)
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertEqual(len(bn.arcs()), 3)
    self.assertEqual(bn.dim(), (2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1)))

    bn = gum.fastBN("a->b->c;a->c", 3)
    self.assertEqual(bn.size(), 3)
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertEqual(bn.dim(), (3 - 1) + (3 * (3 - 1)) + (3 * 3 * (3 - 1)))

    bn = gum.fastBN("a->b[5]->c;a->c")
    self.assertEqual(bn.size(), 3)
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertEqual(bn.dim(), (2 - 1) + (2 * (5 - 1)) + (2 * 5 * (2 - 1)))

    bn = gum.fastBN("a->b->c;a[1000]->c")
    self.assertEqual(bn.size(), 3)
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertEqual(bn.dim(), (2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1)))

    with self.assertRaises(gum.InvalidDirectedCycle):
      bn = gum.fastBN("a->b->c->a")

    bn = gum.fastBN("a{yes|maybe|no}->b->c;a->c")
    self.assertEqual(bn.size(), 3)
    self.assertEqual(bn.sizeArcs(), 3)
    self.assertEqual(bn.dim(), (3 - 1) + (3 * (2 - 1)) + (3 * 2 * (2 - 1)))

    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a{yes}->b->c;a->c")

    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a{yes|no|yes}->b->c;a->c")

    bn = gum.fastBN("a->b->c->d->e->f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 1 + 5 * 2)

    bn = gum.fastBN("a<-b<-c<-d<-e<-f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 1 + 5 * 2)

    bn = gum.fastBN("a<-b->c<-d->e<-f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b<-c->d<-e->f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b<-c;c->d<-e->f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b<-c->d;d<-e->f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b;b<-c;c->d;d<-e;e->f")
    self.assertEqual(bn.size(), 6)
    self.assertEqual(bn.sizeArcs(), 5)
    self.assertEqual(bn.dim(), 3 * 1 + 2 + 2 * 4)

  def testFastPrototypeVarType(self):
    bn = gum.fastBN("a")
    self.assertEqual(bn.variable("a").__str__(), "a:Range([0,1])")

    bn = gum.fastBN("a[0,1]")
    self.assertEqual(bn.variable("a").__str__(), "a:Range([0,1])")

    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a[0,0]")
    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a[1,0]")
    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a[1,1]")

    bn = gum.fastBN("a[5]")
    self.assertEqual(bn.variable("a").__str__(), "a:Range([0,4])")

    bn = gum.fastBN("a[2,5]")
    self.assertEqual(bn.variable("a").__str__(), "a:Range([2,5])")

    bn = gum.fastBN("a[-2,2]")
    self.assertEqual(bn.variable("a").__str__(), "a:Range([-2,2])")

    bn = gum.fastBN("a[-0.4,0.1,0.5,3.14,10]")
    self.assertEqual(bn.variable("a").__str__(), "a:Discretized(<[-0.4;0.1[,[0.1;0.5[,[0.5;3.14[,[3.14;10]>)")

    bn = gum.fastBN("a{-3|0|3}")
    self.assertEqual(bn.variable("a").__str__(), "a:Integer({-3|0|3})")

    bn = gum.fastBN("a{X|Y|Z}")
    self.assertEqual(bn.variable("a").__str__(), "a:Labelized({X|Y|Z})")

  def test_minimalCondSet(self):
    bn = gum.fastBN("A->C->E->F->G;B->C;B->D->F;H->E")
    iA, iB, iC, iD, iE, iF, iG, iH = [bn.idFromName(s) for s in "ABCDEFGH"]
    tous = {iA, iB, iC, iD, iE, iF, iG, iH}

    r = bn.minimalCondSet(iA, tous)
    self.assertEqual(r, {iA})

    r = bn.minimalCondSet(iA, tous - {iA})
    self.assertEqual(r, {iB, iC})

    r = bn.minimalCondSet(iA, {iE, iF, iG})
    self.assertEqual(r, {iE, iF})

    r = bn.minimalCondSet(iA, {iB, iC, iE, iF, iG})
    self.assertEqual(r, {iB, iC})

    r = bn.minimalCondSet(iA, {iC, iE, iF, iG})
    self.assertEqual(r, {iC, iE, iF})

    r = bn.minimalCondSet(iC, tous)
    self.assertEqual(r, {iC})

    r = bn.minimalCondSet(iC, tous - {iC})
    self.assertEqual(r, {iA, iB, iH, iE})

    r = bn.minimalCondSet(iC, {iE, iF, iG})
    self.assertEqual(r, {iE, iF})

    r = bn.minimalCondSet(iC, {iB, iE, iF, iG})
    self.assertEqual(r, {iE, iB})

    r = bn.minimalCondSet(iC, {iC, iE, iF, iG})
    self.assertEqual(r, {iC})

    # for set of targets
    tous = {iA, iB, iC, iD, iE, iF, iG, iH}

    r = bn.minimalCondSet([iE, iD], tous)
    self.assertEqual(r, {iE, iD})

    r = bn.minimalCondSet([iE, iD], tous - {iE})
    self.assertEqual(r, {iC, iD, iH, iF})

    r = bn.minimalCondSet([iE, iD], tous - {iE, iD})
    self.assertEqual(r, {iB, iC, iH, iF})

  def testClearBN(self):
    bn = gum.fastBN("A->B->C->D;E<-C<-F")
    bn.clear()
    self.assertTrue(bn.empty())

  def testExistsArc(self):
    bn = gum.fastBN("A->B->C<-D->E<-A<-G->F")

    self.assertTrue(bn.existsArc(0, 1))
    self.assertTrue(bn.existsArc("A", "B"))
    self.assertFalse(bn.existsArc(1, 0))
    self.assertFalse(bn.existsArc("B", "A"))
    self.assertFalse(bn.existsArc(0, 2))
    self.assertFalse(bn.existsArc("A", "C"))

  def testAncestors(self):
    bn = gum.fastBN("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J")

    self.assertEqual(bn.descendants(6), {0, 1, 4, 5})
    self.assertEqual(bn.descendants("G"), {0, 1, 4, 5})

    self.assertEqual(bn.descendants(1), set())
    self.assertEqual(bn.descendants("B"), set())

    self.assertEqual(bn.ancestors(1), {0, 2, 6, 8, 9})
    self.assertEqual(bn.ancestors("B"), {0, 2, 6, 8, 9})

    self.assertEqual(bn.ancestors(9), set())
    self.assertEqual(bn.ancestors("J"), set())

  def testMoralizedAncestralGraph(self):
    bn = gum.fastBN("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J")

    g = bn.moralizedAncestralGraph({"I", "J"})
    self.assertEqual(g.nodes(), {8, 9})
    self.assertEqual(g.edges(), set())

    g = bn.moralizedAncestralGraph({"A"})
    self.assertEqual(g.nodes(), {0, 6, 8})
    self.assertEqual(g.edges(), {(0, 6), (6, 8)})

    # V-structure
    g = bn.moralizedAncestralGraph({"C"})
    self.assertEqual(g.nodes(), {2, 8, 9})
    self.assertEqual(g.edges(), {(2, 8), (2, 9), (8, 9)})

    g = bn.moralizedAncestralGraph({"A", "D", "I", "H"})
    self.assertEqual(g.nodes(), {0, 2, 3, 6, 7, 8, 9})
    self.assertEqual(g.edges(), {(2, 8), (2, 9), (8, 9), (2, 3), (3, 7), (0, 6), (6, 8)})

    g = bn.moralizedAncestralGraph({"F", "B", "E", "H"})
    self.assertEqual(g.nodes(), {0, 1, 2, 3, 4, 5, 6, 7, 8, 9})
    self.assertEqual(
      g.edges(),
      {(2, 8), (2, 9), (8, 9), (2, 3), (3, 7), (0, 6), (6, 8), (0, 5), (0, 1), (3, 4), (0, 2), (0, 3), (0, 4), (1, 2)},
    )

  def testIsIndependent(self):
    bn = gum.fastBN("A->B<-C->D->E<-A->F;G->A;D->H;G<-I->C<-J")

    self.assertTrue(bn.isIndependent("I", "J", {}))
    self.assertFalse(bn.isIndependent("I", "J", {"C"}))
    self.assertFalse(bn.isIndependent("I", "J", {"H"}))
    self.assertTrue(bn.isIndependent("I", "J", {"F"}))
    self.assertFalse(bn.isIndependent("I", "J", {"E"}))
    self.assertFalse(bn.isIndependent("I", "J", {"E", "G"}))
    self.assertFalse(bn.isIndependent("I", "J", {"E", "G", "H"}))

    self.assertFalse(bn.isIndependent("I", "H", {}))
    self.assertTrue(bn.isIndependent("I", "H", {"C"}))
    self.assertTrue(bn.isIndependent("I", "H", {"C", "B"}))
    self.assertFalse(bn.isIndependent("I", "H", {"C", "E"}))
    self.assertFalse(bn.isIndependent("I", "H", {"C", "E", "B"}))
    self.assertTrue(bn.isIndependent("I", "H", {"C", "E", "B", "G"}))

  def testMutilateBN(self):
    bn = gum.fastBN("P2->N<-P1;A->E2<-N->E1")

    bn2, _ = gum.mutilateBN(bn, intervention={"N": ["1"]}, observation={})
    self.assertEqual(bn2.sizeArcs(), 1)
    self.assertEqual(bn2.size(), 5)

    bn2, _ = gum.mutilateBN(bn, intervention={}, observation={"N": [0, 1]})
    self.assertEqual(bn2.sizeArcs(), 3)
    self.assertEqual(bn2.size(), 6)

    bn2, _ = gum.mutilateBN(bn, intervention={"A": ["1"]}, observation={"N": [0.3, 0.7]})
    self.assertEqual(bn2.sizeArcs(), 4)
    self.assertEqual(bn2.size(), 5)

  def testMutilateBN2(self):
    bn = gum.fastBN("P2->N<-P1;A->E2<-N->E1")

    bn2, ev2 = gum.mutilateBN(bn, intervention={}, observation={"A": ["1"], "N": [1, 1]})

    ie = gum.LazyPropagation(bn)
    ie.setEvidence(ev2)
    ie.makeInference()

    ie2 = gum.LazyPropagation(bn2)
    ie2.setEvidence(ev2)
    ie2.makeInference()

    for n in bn2.names():
      self.assertListsAlmostEqual(ie.posterior(n).tolist(), ie2.posterior(n).tolist())

  def testMemoryFootprint(self):
    bn = gum.fastBN("P2->N<-P1;A->E2<-N->E1")
    self.assertEqual(bn.memoryFootprint(), sum([bn.cpt(x).memoryFootprint() for x, _ in bn]))


class TestLoadBN(BayesNetTestCase):
  def listen(self, percent):
    if not percent > 100:
      if percent % 10 == 0:
        self.bufferlisten += "#"

  def ecoute(self, pourcent):
    if pourcent > 100:
      self.bufferecoute += "FINI"

  def testSimpleBIFLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir("alarm.bif"), self.listen)
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testSimpleBIFLoadWithoutListener(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir("alarm.bif"))
    self.assertEqual(self.bufferlisten, "")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testListBIFLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir("alarm.bif"), [self.listen, self.ecoute])
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testTupleBIFLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir("alarm.bif"), (self.ecoute, self.listen))
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testSimpleNETLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadNET(self.agrumSrcDir("test1.net"), self.listen)
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

  def testSimpleDSLLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir("DSL/alarm.dsl"), self.listen)
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testSimpleDSLLoadWithoutListener(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir("DSL/alarm.dsl"))
    self.assertEqual(self.bufferlisten, "")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testListDSLLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir("DSL/alarm.dsl"), [self.listen, self.ecoute])
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testTupleDSLLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir("DSL/alarm.dsl"), (self.ecoute, self.listen))
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testO3PRMLoad(self):
    bn = gum.loadBN(
      self.agrumSrcDir("o3prm/Asia.o3prm"), [], verbose=False
    )  # verbose=False : don't want to see the warnings
    self.assertEqual(bn.size(), 8)

  def testUAILoad(self):
    bn = gum.loadBN(
      self.agrumSrcDir("uai/BNUAIReader_file2.uai"), [], verbose=False
    )  # verbose=False : don't want to see the warnings
    self.assertEqual(bn.size(), 5)

  def testBIFXMLLoad(self):
    bn = gum.loadBN(self.agrumSrcDir("dog.bifxml"), [], verbose=False)  # verbose=False : don't want to see the warnings
    self.assertEqual(bn.size(), 5)


class TestSaveBN(BayesNetTestCase):
  def testReadAfterWrite(self):
    bn = gum.BayesNet()
    bn.add(gum.RangeVariable("1", "", 0, 1))
    bn.add(gum.DiscretizedVariable("2", "").addTick(0.0).addTick(0.5).addTick(1.0))
    bn.add(gum.LabelizedVariable("3", "", 2))
    bn.add(gum.LabelizedVariable("4", "", 2))
    bn.add(gum.LabelizedVariable("5", "", 3))

    bn.addArc("1", "3")
    bn.addArc("1", "4")
    bn.addArc("3", "5")
    bn.addArc("4", "5")
    bn.addArc("2", "4")
    bn.addArc("2", "5")

    bn.cpt("1").fillWith([0.2, 0.8])
    bn.cpt("2").fillWith([0.3, 0.7])
    bn.cpt("3").fillWith([0.1, 0.9, 0.9, 0.1])
    bn.cpt("4").fillWith([0.4, 0.6, 0.5, 0.5, 0.5, 0.5, 1.0, 0.0])
    bn.cpt("5").fillWith(
      [
        0.3,
        0.6,
        0.1,
        0.5,
        0.5,
        0.0,
        0.5,
        0.5,
        0.0,
        1.0,
        0.0,
        0.0,
        0.4,
        0.6,
        0.0,
        0.5,
        0.5,
        0.0,
        0.5,
        0.5,
        0.0,
        0.0,
        0.0,
        1.0,
      ]
    )

    gum.saveBN(bn, self.agrumSrcDir("o3prm/BNO3PRMIO_file.o3prm"))

    bn2 = gum.loadBN(self.agrumSrcDir("o3prm/BNO3PRMIO_file.o3prm"), system="bayesnet")

    self.assertEqual(bn.dim(), bn2.dim())
    self.assertEqual(bn.log10DomainSize(), bn2.log10DomainSize())
    for n in bn.names():
      self.assertEqual(bn.variable(n).name(), bn2.variable(n).name())
      self.assertEqual(bn.variable(n).varType(), bn2.variable(n).varType())
      self.assertEqual(bn.variable(n).domainSize(), bn2.variable(n).domainSize())

  def testToFast(self):
    model = gum.fastBN("A->B->C<-D->E;F->G[3];H->I{a|b|c};X")
    model2 = gum.fastBN(model.toFast())
    self.assertEqual(model.names(), model2.names())
    for _, n in model:
      self.assertEqual(model.variable(n).toFast(), model2.variable(n).toFast())
      self.assertEqual(
        {model.variable(i).name() for i in model.parents(n)}, {model2.variable(i).name() for i in model2.parents(n)}
      )


class TestScore(BayesNetTestCase):
  def testSkeletonPrecision(self):
    true_bn = gum.fastBN("A->B->C")
    bn = gum.fastBN("A->B;C->B;A->C")
    precision = bvb.GraphicalBNComparator(true_bn, bn).skeletonScores()["precision"]
    self.assertEqual(2.0 / 3.0, precision)

  def testSkeletonRecall(self):
    true_bn = gum.fastBN("A->B->C")
    bn = gum.fastBN("A->B;C->B;A->C")
    recall = bvb.GraphicalBNComparator(true_bn, bn).skeletonScores()["recall"]
    self.assertEqual(1.0, recall)

  def testSkeletonFscore(self):
    true_bn = gum.fastBN("A->B->C")
    bn = gum.fastBN("A->B;C->B;A->C")
    fscore = bvb.GraphicalBNComparator(true_bn, bn).skeletonScores()["fscore"]
    self.assertEqual(4.0 / 5.0, fscore)

  def testPrecision(self):
    true_bn = gum.fastBN("A->B->C")
    bn = gum.fastBN("A->B;C->B;A->C")
    precision = bvb.GraphicalBNComparator(true_bn, bn).scores()["precision"]
    self.assertEqual(1.0 / 3.0, precision)

  def testRecall(self):
    true_bn = gum.fastBN("A->B->C")
    bn = gum.fastBN("A->B;C->B;A->C")
    recall = bvb.GraphicalBNComparator(true_bn, bn).scores()["recall"]
    self.assertEqual(1.0 / 2.0, recall)

  def testFscore(self):
    true_bn = gum.fastBN("A->B->C")
    bn = gum.fastBN("A->B;C->B;A->C")
    fscore = bvb.GraphicalBNComparator(true_bn, bn).scores()["fscore"]
    self.assertEqual(2.0 / 5.0, fscore)

  def testHamming(self):
    true_bn = gum.fastBN("A->B->C;B->D")
    bn = gum.fastBN("A->B;C->B;D->C")
    hamming = bvb.GraphicalBNComparator(true_bn, bn).hamming()["hamming"]
    self.assertEqual(2, hamming)

  def testStructuralHamming(self):
    true_bn = gum.fastBN("A->B->C;B->D")
    bn = gum.fastBN("A->B;C->B;D->C")
    shd = bvb.GraphicalBNComparator(true_bn, bn).hamming()["structural hamming"]
    self.assertEqual(4, shd)


class TestRandomBN(BayesNetTestCase):
  def testMaxMod(self):
    bn = gum.randomBN(n=16, names=[f"X{i}" for i in range(16)], ratio_arc=3, domain_size=2)
    for i in bn.nodes():
      self.assertEqual(bn.variable(i).domainSize(), 2)
    all2 = True

    bn = gum.randomBN(n=16, names=[f"X{i}" for i in range(16)], ratio_arc=3, domain_size=4)
    for i in bn.nodes():
      if bn.variable(i).domainSize() > 2:
        all2 = False
        break
    self.assertFalse(all2)


class TestGraphicalConcepts(BayesNetTestCase):
  def testChildren(self):
    bn = gum.fastBN("A->B->C<-D->E;D->F->G<-A")
    self.assertEqual(bn.children(1), {2})
    self.assertEqual(bn.children("A"), {1, 6})


ts = unittest.TestSuite()
addTests(ts, TestConstructors)
addTests(ts, TestInsertions)
addTests(ts, TestFeatures)
addTests(ts, TestLoadBN)
addTests(ts, TestSaveBN)
addTests(ts, TestRandomBN)
addTests(ts, TestScore)
addTests(ts, TestGraphicalConcepts)
