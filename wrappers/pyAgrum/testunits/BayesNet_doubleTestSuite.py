# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet_double, LabelizedVar, InvalidNode, InvalidCircuit
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
        bn = BayesNet_double()
        bn = BayesNet_double('French Touch Network')


    def testCopyConstructor(self):
        bn1 = BayesNet_double()
        bn2 = BayesNet_double(bn1)



class TestInsertions(BayesNetTestCase):

    def testVariables(self):
        bn = BayesNet_double()
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
        bn = BayesNet_double()
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
        bn = BayesNet_double()
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
        bn = BayesNet_double()
        idList = []

        self.fillTopo(bn, idList)
        graph = bn.moralGraph()


    def testTopologicalOrder(self):
        bn = BayesNet_double()
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
        bn = BayesNet_double()
        res = bn.loadBIF('../ressources/alarm.bif', self.listen)
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "")
        self.assertTrue(res)


    def testListBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet_double()
        res = bn.loadBIF('../ressources/alarm.bif', [self.listen, self.ecoute])
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res)


    def testTupleBIFLoad(self):
        self.bufferlisten = ""
        self.bufferecoute = ""
        bn = BayesNet_double()
        res = bn.loadBIF('../ressources/alarm.bif', (self.ecoute, self.listen))
        self.assertEqual(self.bufferlisten, "##########")
        self.assertEqual(self.bufferecoute, "FINI")
        self.assertTrue(res)



ts = unittest.TestSuite()
ts.addTest(TestConstructors('testConstructor'))
ts.addTest(TestConstructors('testCopyConstructor'))
ts.addTest(TestInsertions('testVariables'))
ts.addTest(TestInsertions('testArcs'))
ts.addTest(TestInsertions('testEraseArcs'))
ts.addTest(TestFeatures('testMoralGraph'))
ts.addTest(TestFeatures('testTopologicalOrder'))
ts.addTest(TestBIF('testSimpleBIFLoad'))
ts.addTest(TestBIF('testListBIFLoad'))
ts.addTest(TestBIF('testTupleBIFLoad'))
