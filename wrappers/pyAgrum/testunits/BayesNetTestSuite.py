# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet, LabelizedVar, InvalidNode, InvalidCircuit
from pyAgrumTestSuite import pyAgrumTestCase


class BayesNetTestCase(pyAgrumTestCase):

    def setUp(self):
        self.var1 = LabelizedVar("var1", "1", 2)
        self.var2 = LabelizedVar( "var2", "2", 2 )
        self.var3 = LabelizedVar( "var3", "3", 2 )
        self.var4 = LabelizedVar( "var4", "4", 2 )
        self.var5 = LabelizedVar( "var5", "(gum::Size) 5", 3 )
        self.bufferlisten = ""
        self.bufferecoute = ""


    def fillTopo(self, bn, idList ):
        idList.append(bn.add(self.var1));
        idList.append(bn.add(self.var2));
        idList.append(bn.add(self.var3));
        idList.append(bn.add(self.var4));
        idList.append(bn.add(self.var5));

        bn.insertArc(idList[0], idList[2]);
        bn.insertArc(idList[2], idList[4]);
        bn.insertArc(idList[1], idList[3]);
        bn.insertArc(idList[0], idList[3]);
        bn.insertArc(idList[3], idList[4]);
        bn.insertArc(idList[1], idList[4]);



class TestConstructors(BayesNetTestCase):

    def testConstructor(self):
        bn = BayesNet()
        bn = BayesNet('French Touch Network')


    def testCopyConstructor(self):
        bn1 = BayesNet()
        bn2 = BayesNet(bn1)



class TestInsertions(BayesNetTestCase):

    def testVariables(self):
        bn = BayesNet()
        idList = []

        idList.append(bn.add(self.var1))
        idList.append(bn.add(self.var2))
        idList.append(bn.add(self.var3))
        idList.append(bn.add(self.var4))
        idList.append(bn.add(self.var5))

        self.assertEqual(len(bn), 5)

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
        bn = BayesNet()
        idList = []

        idList.append(bn.add(self.var1))
        idList.append(bn.add(self.var2))
        idList.append(bn.add(self.var3))
        idList.append(bn.add(self.var4))
        idList.append(bn.add(self.var5))

        bn.insertArc(idList[0], idList[2])
        bn.insertArc(idList[2], idList[4])
        bn.insertArc(idList[1], idList[3])
        bn.insertArc(idList[0], idList[3])
        bn.insertArc(idList[3], idList[4])
        bn.insertArc(idList[1], idList[4])

        self.assertRaises(InvalidCircuit, bn.insertArc,idList[4],idList[0])
        self.assertRaises(InvalidNode, bn.insertArc,42,666)


    def testEraseArcs(self):
        bn = BayesNet()
        idList = []

        self.assertTrue(bn.empty())

        self.fillTopo(bn, idList)

        self.assertFalse(bn.empty())
        self.assertEqual(len(bn), 5)

        bn.eraseArc(idList[0], idList[2])
        bn.eraseArc(idList[2], idList[4])
        bn.eraseArc(idList[1], idList[3])
        bn.eraseArc(idList[0], idList[3])
        bn.eraseArc(idList[3], idList[4])
        bn.eraseArc(idList[1], idList[4])

        self.assertFalse(bn.empty())



class TestFeatures(BayesNetTestCase):

    def testMoralGraph(self):
        bn = BayesNet()
        idList = []

        self.fillTopo(bn, idList)
        graph = bn.moralGraph()


    def testTopologicalOrder(self):
        bn = BayesNet()
        idList = []

        self.fillTopo(bn, idList)
        topoOrder = bn.getTopologicalOrder()
        self.assertEqual(topoOrder.size(), 5)



class TestBIF(BayesNetTestCase):

    def listen(self, percent):
        if not percent>100:
            if (10*(percent/10)==percent):
                self.bufferlisten += "#"


    def ecoute(self, pourcent):
        if pourcent > 100:
            self.bufferecoute += "FINI"


    def testSimpleBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadBIF('../../../src/testunits/ressources/alarm.bif', self.listen)
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testSimpleBIFLoadWithoutListener(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadBIF('../../../src/testunits/ressources/alarm.bif')
        self.assertEqual(self.bufferlisten, "")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testListBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadBIF('../../../src/testunits/ressources/alarm.bif', [self.listen, self.ecoute])
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testTupleBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadBIF('../../../src/testunits/ressources/alarm.bif', (self.ecoute, self.listen))
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testSimpleNETLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadDSL('../../../src/testunits/ressources/test1.net', self.listen)
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res)

    def testSimpleDSLLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadDSL('../../../src/testunits/ressources/DSL/alarm.dsl', self.listen)
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testSimpleDSLLoadWithoutListener(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadDSL('../../../src/testunits/ressources/DSL/alarm.dsl')
        self.assertEqual(self.bufferlisten, "")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)

    def testListDSLLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadDSL('../../../src/testunits/ressources/DSL/alarm.dsl', [self.listen, self.ecoute])
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)


    def testTupleDSLLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet()
        res = bn.loadDSL('../../../src/testunits/ressources/DSL/alarm.dsl', (self.ecoute, self.listen))
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res)
        
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("KINKEDTUBE")).tolist(), 
																		[0.04, 0.96],places=4)
        self.assertListsAlmostEqual(bn.cpt(bn.idFromName("HR")).tolist(), 
																		[[0.05, 0.9, 0.05], [0.01, 0.09, 0.9]],places=4)



ts = unittest.TestSuite()
ts.addTest(TestConstructors('testConstructor'))
ts.addTest(TestConstructors('testCopyConstructor'))
ts.addTest(TestInsertions('testVariables'))
ts.addTest(TestInsertions('testArcs'))
ts.addTest(TestInsertions('testEraseArcs'))
ts.addTest(TestFeatures('testMoralGraph'))
ts.addTest(TestFeatures('testTopologicalOrder'))
ts.addTest(TestBIF('testSimpleBIFLoad'))
ts.addTest(TestBIF('testSimpleBIFLoadWithoutListener'))
ts.addTest(TestBIF('testListBIFLoad'))
ts.addTest(TestBIF('testTupleBIFLoad'))
ts.addTest(TestBIF('testSimpleDSLLoad'))
ts.addTest(TestBIF('testSimpleDSLLoadWithoutListener'))
ts.addTest(TestBIF('testListDSLLoad'))
ts.addTest(TestBIF('testTupleDSLLoad'))
