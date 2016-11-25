# -*- encoding: UTF-8 -*-
import pyAgrum as gum
import scipy

import unittest
from pyAgrumTestSuite import pyAgrumTestCase,addTests


class BayesNetTestCase(pyAgrumTestCase):

    def setUp(self):
        self.var1 = gum.LabelizedVariable("var1", "1", 2)
        self.var2 = gum.LabelizedVariable( "var2", "2", 2 )
        self.var3 = gum.LabelizedVariable( "var3", "3", 2 )
        self.var4 = gum.LabelizedVariable( "var4", "4", 2 )
        self.var5 = gum.LabelizedVariable( "var5", "(gum::Size) 5", 3 )
        self.bufferlisten = ""
        self.bufferecoute = ""


    def fillTopo(self, bn, idList ):
        idList.append(bn.add(self.var1));
        idList.append(bn.add(self.var2));
        idList.append(bn.add(self.var3));
        idList.append(bn.add(self.var4));
        idList.append(bn.add(self.var5));

        bn.addArc(idList[0], idList[2]);
        bn.addArc(idList[2], idList[4]);
        bn.addArc(idList[1], idList[3]);
        bn.addArc(idList[0], idList[3]);
        bn.addArc(idList[3], idList[4]);
        bn.addArc(idList[1], idList[4]);

    def fill(self,bn,idList):
        self.fillTopo(bn,idList)

        bn.cpt( idList[0] ).fillWith( [0.2, 0.8] )

        bn.cpt( idList[1] ).fillWith( [0.3, 0.7] )

        bn.cpt( idList[2] ).fillWith( [0.1, 0.9, 0.9, 0.1] )

        bn.cpt( idList[3] ).fillWith( [0.4, 0.6,
                                       0.5, 0.5,
                                       0.5, 0.5,
                                       1.0, 0.0])

        bn.cpt( idList[4] ).fillWith([0.3,0.6,0.1,
                                      0.5,0.5,0.0,
                                      0.5,0.5,0.0,
                                      1.0,0.0,0.0,
                                      0.4,0.6,0.0,
                                      0.5,0.5,0.0,
                                      0.5,0.5,0.0,
                                      0.0,0.0,1.0])

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

        self.assertRaises(gum.InvalidDirectedCycle, bn.addArc,idList[4],idList[0])
        self.assertRaises(gum.InvalidNode, bn.addArc,42,666)


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

        self.assertEqual(str(bn.variable(0)),"HISTORY<TRUE,FALSE>")
        bn.variable(0).toLabelizedVar().changeLabel(0,"toto")
        self.assertEqual(str(bn.variable(0)),"HISTORY<toto,FALSE>")

    def testStringAccessors(self):
      bn=gum.BayesNet()
      for s in "ABC":
        bn.add(gum.LabelizedVariable(s,"",2))
      for arc in [("A","C"),("B","C")]:
        bn.addArc(*arc)

      self.assertEqual(str(bn),"BN{nodes: 3, arcs: 2, domainSize: 8, parameters: 12, compression ratio: -50% }")

      bn.cpt("A").fillWith(1).normalize()
      bn.generateCPT("B")
      bn.generateCPT("C")
      with self.assertRaises(IndexError):
        bn.cpt("XX")

      bn.reverseArc("A","C")
      self.assertEqual(str(bn),"BN{nodes: 3, arcs: 3, domainSize: 8, parameters: 14, compression ratio: -75% }")

      with self.assertRaises(gum.InvalidArc):
        bn.reverseArc("A","C")
      with self.assertRaises(gum.GraphError):
        bn.reverseArc("A","C")
      with self.assertRaises(IndexError):
        bn.reverseArc("A","X")

      bn.erase("A")
      self.assertEqual(str(bn),"BN{nodes: 2, arcs: 1, domainSize: 4, parameters: 6, compression ratio: -50% }")

      with self.assertRaises(IndexError):
        bn.erase("A")

      bn.eraseArc("B","C")
      self.assertEqual(str(bn),"BN{nodes: 2, arcs: 0, domainSize: 4, parameters: 4, compression ratio: 0% }")

      with self.assertRaises(IndexError):
        bn.eraseArc("B","C")

    def testShortcutAdd(self):
        bn=gum.BayesNet()
        a=bn.add("A",2)
        b=bn.add("B",3)
        self.assertEqual(a,0)
        self.assertEqual(b,1)

        with self.assertRaises(gum.DuplicateLabel):
            bn.add("A",5)
        with self.assertRaises(gum.OperationNotAllowed):
            bn.add("C",0)

        bn.addArc("A","B")
        self.assertEqual(bn.log10DomainSize(),scipy.log10(6.0))
        self.assertEqual(bn.dim(),5)


    def testSomeFunctions(self):
      bn=gum.BayesNet()
      idList=[]
      self.fill(bn,idList)

      self.assertEquals(bn.maxVarDomainSize(),3)
      self.assertEquals(bn.minParam(),0.0)
      self.assertEquals(bn.maxParam(),1.0)
      self.assertEquals(bn.minNonZeroParam(),0.1)
      self.assertEquals(bn.maxNonOneParam(),0.9)


class TestLoadBN(BayesNetTestCase):

    def listen(self, percent):
        if not percent>100:
            if (percent %10 == 0):
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
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testSimpleBIFLoadWithoutListener(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'))
        self.assertEqual(self.bufferlisten, "")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testListBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'), [self.listen, self.ecoute])
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testTupleBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadBIF(self.agrumSrcDir('src/testunits/ressources/alarm.bif'), (self.ecoute, self.listen))
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testSimpleNETLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/test1.net'), self.listen)
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res=="")

    def testSimpleDSLLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'), self.listen)
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testSimpleDSLLoadWithoutListener(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'))
        self.assertEqual(self.bufferlisten, "")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testListDSLLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'), [self.listen, self.ecoute])
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)


    def testTupleDSLLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = gum.BayesNet()
        res = bn.loadDSL(self.agrumSrcDir('src/testunits/ressources/DSL/alarm.dsl'), (self.ecoute, self.listen))
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res=="")

        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(),[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(),[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testO3PRMLoad(self):
        bn=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/o3prm/Asia.o3prm'),[],verbose=False) # verbose=False : don't want to see the warnings
        self.assertEqual(bn.size(),8)

    def testUAILoad(self):
        bn=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/uai/BNUAIReader_file2.uai'),[],verbose=False) # verbose=False : don't want to see the warnings
        self.assertEqual(bn.size(),5)

    def testBIFXMLLoad(self):
        bn=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/dog.bifxml'),[],verbose=False) # verbose=False : don't want to see the warnings
        self.assertEqual(bn.size(),5)


ts = unittest.TestSuite()
addTests(ts,TestConstructors)
addTests(ts,TestLoadBN)
