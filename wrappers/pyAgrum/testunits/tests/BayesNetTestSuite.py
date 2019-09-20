# -*- encoding: UTF-8 -*-
import unittest

import numpy
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


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

    bn.cpt(idList[3]).fillWith([0.4, 0.6,
                                0.5, 0.5,
                                0.5, 0.5,
                                1.0, 0.0])

    bn.cpt(idList[4]).fillWith([0.3, 0.6, 0.1,
                                0.5, 0.5, 0.0,
                                0.5, 0.5, 0.0,
                                1.0, 0.0, 0.0,
                                0.4, 0.6, 0.0,
                                0.5, 0.5, 0.0,
                                0.5, 0.5, 0.0,
                                0.0, 0.0, 1.0])


class TestConstructors(BayesNetTestCase):
  def testConstructor(self):
    bn = gum.BayesNet()
    bn = gum.BayesNet('French Touch Network')

  def testCopyConstructor(self):
    bn1 = gum.BayesNet()
    bn2 = gum.BayesNet(bn1)


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
      for id_b in idList[i:len(idList)]:
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


class TestFeatures(BayesNetTestCase):
  def testMoralGraph(self):
    bn = gum.BayesNet()
    idList = []

    self.fillTopo(bn, idList)
    graph = bn.moralGraph()

  def testTopologicalOrder(self):
    bn = gum.BayesNet()
    idList = []

    self.fillTopo(bn, idList)
    topoOrder = bn.topologicalOrder()
    self.assertEqual(len(topoOrder), 5)

  def testChangeLabel(self):
    bn = gum.BayesNet()

    res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'))

    self.assertEqual(str(bn.variable(0)), "HISTORY<TRUE,FALSE>")
    bn.variable(0).toLabelizedVar().changeLabel(0, "toto")
    self.assertNotEqual(str(bn.variable(0)), "HISTORY<toto,FALSE>")
    bn.changeVariableLabel(0, "TRUE", "toto")
    self.assertEqual(str(bn.variable(0)), "HISTORY<toto,FALSE>")

  def testStringAccessors(self):
    bn = gum.BayesNet()
    for s in "ABC":
      bn.add(gum.LabelizedVariable(s, "", 2))
    for arc in [("A", "C"), ("B", "C")]:
      bn.addArc(*arc)

    with self.assertRaises(gum.DuplicateElement):
      bn.addArc("A","C")

    self.assertEqual(str(bn), "BN{nodes: 3, arcs: 2, domainSize: 8, dim: 12}")

    bn.cpt("A").fillWith(1).normalize()
    bn.generateCPT("B")
    bn.generateCPT("C")
    with self.assertRaises(gum.NotFound):
      bn.cpt("XX")

    bn.reverseArc("A", "C")
    self.assertEqual(str(bn), "BN{nodes: 3, arcs: 3, domainSize: 8, dim: 14}")

    with self.assertRaises(gum.InvalidArc):
      bn.reverseArc("A", "C")
    with self.assertRaises(gum.GraphError):
      bn.reverseArc("A", "C")
    with self.assertRaises(gum.NotFound):
      bn.reverseArc("A", "X")

    bn.erase("A")
    self.assertEqual(str(bn), "BN{nodes: 2, arcs: 1, domainSize: 4, dim: 6}")

    self.assertEqual(bn.parents(bn.idFromName("C")),bn.parents("C"))

    with self.assertRaises(gum.NotFound):
      bn.erase("A")

    bn.eraseArc("B", "C")
    self.assertEqual(str(bn), "BN{nodes: 2, arcs: 0, domainSize: 4, dim: 4}")

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
    with self.assertRaises(gum.OperationNotAllowed):
      bn.add("C", 0)

    bn.addArc("A", "B")
    self.assertEqual(bn.log10DomainSize(), numpy.log10(6.0))
    self.assertEqual(bn.dim(), 5)

  def testSomeFunctions(self):
    bn = gum.BayesNet()
    idList = []
    self.fillBN(bn, idList)

    self.assertEquals(bn.maxVarDomainSize(), 3)
    self.assertEquals(bn.minParam(), 0.0)
    self.assertEquals(bn.maxParam(), 1.0)
    self.assertEquals(bn.minNonZeroParam(), 0.1)
    self.assertEquals(bn.maxNonOneParam(), 0.9)

  def test_fastBN(self):
    bn = gum.fastBN("a->b->c;a->c")
    self.assertEquals(bn.size(), 3)
    self.assertEquals(bn.sizeArcs(), 3)
    self.assertEquals(len(bn.arcs()),3)
    self.assertEquals(bn.dim(), (2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1)))

    bn = gum.fastBN("a->b->c;a->c", 3)
    self.assertEquals(bn.size(), 3)
    self.assertEquals(bn.sizeArcs(), 3)
    self.assertEquals(bn.dim(), (3 - 1) + (3 * (3 - 1)) + (3 * 3 * (3 - 1)))

    bn = gum.fastBN("a->b[5]->c;a->c")
    self.assertEquals(bn.size(), 3)
    self.assertEquals(bn.sizeArcs(), 3)
    self.assertEquals(bn.dim(), (2 - 1) + (2 * (5 - 1)) + (2 * 5 * (2 - 1)))

    bn = gum.fastBN("a->b->c;a[1000]->c")
    self.assertEquals(bn.size(), 3)
    self.assertEquals(bn.sizeArcs(), 3)
    self.assertEquals(bn.dim(), (2 - 1) + (2 * (2 - 1)) + (2 * 2 * (2 - 1)))

    with self.assertRaises(gum.InvalidDirectedCycle):
      bn = gum.fastBN("a->b->c->a")

    bn = gum.fastBN("a{yes|maybe|no}->b->c;a->c")
    self.assertEquals(bn.size(), 3)
    self.assertEquals(bn.sizeArcs(), 3)
    self.assertEquals(bn.dim(),
                      (3 - 1) + (3 * (2 - 1)) + (3 * 2 * (2 - 1)))

    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a{yes}->b->c;a->c")

    with self.assertRaises(gum.InvalidArgument):
      bn = gum.fastBN("a{yes|no|yes}->b->c;a->c")

    bn = gum.fastBN("a->b->c->d->e->f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      1 + 5 * 2)

    bn = gum.fastBN("a<-b<-c<-d<-e<-f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      1 + 5 * 2)

    bn = gum.fastBN("a<-b->c<-d->e<-f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b<-c->d<-e->f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b<-c;c->d<-e->f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b<-c->d;d<-e->f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      3 * 1 + 2 + 2 * 4)

    bn = gum.fastBN("a->b;b<-c;c->d;d<-e;e->f")
    self.assertEquals(bn.size(), 6)
    self.assertEquals(bn.sizeArcs(), 5)
    self.assertEquals(bn.dim(),
                      3 * 1 + 2 + 2 * 4)
    
  def testFastPrototypeVarType(self):
    bn = gum.fastBN("a")
    self.assertEquals(bn.variable("a").__str__(), "a[0,1]")

    bn = gum.fastBN("a[0,1]")
    self.assertEquals(bn.variable("a").__str__(), "a[0,1]")

    with self.assertRaises(gum.InvalidArgument):
      bn=gum.fastBN("a[0,0]")
    with self.assertRaises(gum.InvalidArgument):
      bn=gum.fastBN("a[1,0]")
    with self.assertRaises(gum.InvalidArgument):
      bn=gum.fastBN("a[1,1]")

    bn = gum.fastBN("a[5]")
    self.assertEquals(bn.variable("a").__str__(), "a[0,4]")

    bn = gum.fastBN("a[2,5]")
    self.assertEquals(bn.variable("a").__str__(), "a[2,5]")

    bn = gum.fastBN("a[-2,2]")
    self.assertEquals(bn.variable("a").__str__(), "a[-2,2]")

    bn = gum.fastBN("a[-0.4,0.1,0.5,3.14,10]")
    self.assertEquals(bn.variable("a").__str__(), "a<[-0.4;0.1[,[0.1;0.5[,[0.5;3.14[,[3.14;10]>")
    
  def test_minimalCondSet(self):
    bn = gum.fastBN("A->C->E->F->G;B->C;B->D->F;H->E")
    iA, iB, iC, iD, iE, iF, iG, iH = [bn.idFromName(s) for s in
                                      "ABCDEFGH"]
    tous = set([iA, iB, iC, iD, iE, iF, iG, iH])

    r = bn.minimalCondSet(iA, tous)
    self.assertEqual(r, set([iA]))

    r = bn.minimalCondSet(iA, tous - set([iA]))
    self.assertEqual(r, set([iB, iC]))

    r = bn.minimalCondSet(iA, set([iE, iF, iG]))
    self.assertEqual(r, set([iE, iF]))

    r = bn.minimalCondSet(iA, set([iB, iC, iE, iF, iG]))
    self.assertEqual(r, set([iB, iC]))

    r = bn.minimalCondSet(iA, set([iC, iE, iF, iG]))
    self.assertEqual(r, set([iC, iE, iF]))

    r = bn.minimalCondSet(iC, tous)
    self.assertEqual(r, set([iC]))

    r = bn.minimalCondSet(iC, tous - set([iC]))
    self.assertEqual(r, set([iA, iB, iH, iE]))

    r = bn.minimalCondSet(iC, set([iE, iF, iG]))
    self.assertEqual(r, set([iE, iF]))

    r = bn.minimalCondSet(iC, set([iB, iE, iF, iG]))
    self.assertEqual(r, set([iE, iB]))

    r = bn.minimalCondSet(iC, set([iC, iE, iF, iG]))
    self.assertEqual(r, set([iC]))

    # for set of targets
    tous = set([iA, iB, iC, iD, iE, iF, iG, iH])

    r = bn.minimalCondSet([iE, iD], tous)
    self.assertEqual(r, set([iE, iD]))

    r = bn.minimalCondSet([iE, iD], tous - set([iE]))
    self.assertEqual(r, set([iC, iD, iH, iF]))

    r = bn.minimalCondSet([iE, iD], tous - set([iE, iD]))
    self.assertEqual(r, set([iB, iC, iH, iF]))


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
    res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'), self.listen)
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testSimpleBIFLoadWithoutListener(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'))
    self.assertEqual(self.bufferlisten, "")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testListBIFLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'), [self.listen, self.ecoute])
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testTupleBIFLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'), (self.ecoute, self.listen))
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testSimpleNETLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadNET(self.agrumSrcDir('src/testunits/ressources/test1.net'), self.listen)
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

  def testSimpleDSLLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'), self.listen)
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testSimpleDSLLoadWithoutListener(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'))
    self.assertEqual(self.bufferlisten, "")
    self.assertEqual(self.bufferecoute, "")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testListDSLLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'), [self.listen, self.ecoute])
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testTupleDSLLoad(self):
    self.bufferlisten = ""
    self.bufferecoute = ""
    bn = gum.BayesNet()
    res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'), (self.ecoute, self.listen))
    self.assertEqual(self.bufferlisten, "##########")
    self.assertEqual(self.bufferecoute, "FINI")
    self.assertTrue(res == "")

    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), [0.04, 0.96], places=4)
    self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), [[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]], places=4)

  def testO3PRMLoad(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm'), [],
                    verbose=False)  # verbose=False : don't want to see the warnings
    self.assertEqual(bn.size(), 8)

  def testUAILoad(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/uai/BNUAIReader_file2.uai'), [],
                    verbose=False)  # verbose=False : don't want to see the warnings
    self.assertEqual(bn.size(), 5)

  def testBIFXMLLoad(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/dog.bifxml'), [],
                    verbose=False)  # verbose=False : don't want to see the warnings
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
    bn.cpt("5").fillWith([0.3, 0.6, 0.1, 0.5, 0.5, 0.0, 0.5, 0.5,
                          0.0, 1.0, 0.0, 0.0, 0.4, 0.6, 0.0, 0.5,
                          0.5, 0.0, 0.5, 0.5, 0.0, 0.0, 0.0, 1.0])

    gum.saveBN(bn, self.agrumSrcDir("src/testunits/ressources/o3prm/BNO3PRMIO_file.o3prm"))

    bn2 = gum.loadBN(self.agrumSrcDir("src/testunits/ressources/o3prm/BNO3PRMIO_file.o3prm"), system="BayesNet")

    self.assertEquals(bn.dim(), bn2.dim())
    self.assertEquals(bn.log10DomainSize(), bn2.log10DomainSize())
    for n in bn.names():
      self.assertEquals(bn.variable(n).name(), bn2.variable(n).name())
      self.assertEquals(bn.variable(n).varType(), bn2.variable(n).varType())
      self.assertEquals(bn.variable(n).domainSize(), bn2.variable(n).domainSize())

import pyAgrum.lib.bn_vs_bn as bvb

class TestScore(BayesNetTestCase):
    def testPrecision(self):
        true_bn = gum.fastBN("A->B->C")
        bn = gum.fastBN("A->B;C->B;A->C")
        precision = bvb.GraphicalBNComparator(true_bn, bn).scores()['precision']
        self.assertEquals(1./3., precision)

    def testRecall(self):
        true_bn = gum.fastBN("A->B->C")
        bn = gum.fastBN("A->B;C->B;A->C")
        recall = bvb.GraphicalBNComparator(true_bn, bn).scores()['recall']
        self.assertEquals(1./2., recall)

    def testFscore(self):
        true_bn = gum.fastBN("A->B->C")
        bn = gum.fastBN("A->B;C->B;A->C")
        fscore = bvb.GraphicalBNComparator(true_bn, bn).scores()['fscore']
        self.assertEquals(2./5., fscore)
    
    def testHamming(self):
        true_bn = gum.fastBN("A->B->C;B->D")
        bn = gum.fastBN("A->B;C->B;D->C")
        hamming = bvb.GraphicalBNComparator(true_bn, bn).hamming()['hamming']
        self.assertEquals(2, hamming)
    
    def testStructuralHamming(self):
        true_bn = gum.fastBN("A->B->C;B->D")
        bn = gum.fastBN("A->B;C->B;D->C")
        shd = bvb.GraphicalBNComparator(true_bn, bn).hamming()['structural hamming']
        self.assertEquals(4, shd)




ts = unittest.TestSuite()
addTests(ts, TestConstructors)
addTests(ts, TestInsertions)
addTests(ts, TestFeatures)
addTests(ts, TestLoadBN)
addTests(ts, TestSaveBN)
addTests(ts, TestScore)
