# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import pyAgrum as gum 

import unittest
from pyAgrumTestSuite import pyAgrumTestCase


class PythonBNListenerTestCase(pyAgrumTestCase):

    def setUp(self):
        self.bn = gum.BayesNet()
        self.buffer0 = ""
        self.buffer1 = ""
        self.var1 = gum.LabelizedVariable("a", "le noeud a", 2)
        self.var2 = gum.LabelizedVariable("b", "le noeud b", 2)
        self.var3 = gum.LabelizedVariable("c", "le noeud c", 3)


    def bufferize(self, str):
        self.buffer0 = str
        self.buffer1 += str



class TestConstructors(PythonBNListenerTestCase):

    def testConstructor(self):
        listen1 = gum.PythonBNListener(self.bn.dag(), self.bn.variableNodeMap())



class TestMessagesSetting(PythonBNListenerTestCase):

    def testMessagesInsertion(self):
        listen1 = gum.PythonBNListener(self.bn.dag(), self.bn.variableNodeMap())
        listen1.setWhenNodeAdded(
            lambda i,s: self.bufferize("ajout d'un noeud %s (%d)\n"%(s,i)))
        listen1.setWhenArcAdded(
            lambda i,j: self.bufferize("ajout d'un arc (%d->%d)\n"%(i,j)))
        listen1.setWhenNodeDeleted(
            lambda i: self.bufferize("suppression du noeud %d\n"%i))
        listen1.setWhenArcDeleted(
            lambda i,j: self.bufferize(
                "suppression de l'arc (%d->%d)\n"%(i,j)))


    def testMessagesChange(self):
        listen1 = gum.PythonBNListener(self.bn.dag(), self.bn.variableNodeMap())
        listen1.setWhenNodeAdded(
            lambda i,s: self.bufferize("ajout d'un noeud %s (%d)\n"%(s,i)))
        listen1.setWhenNodeAdded(
            lambda i,s: self.bufferize("noeud++ %s (%d) ... OK\n"%(s,i)))



class TestListeners(PythonBNListenerTestCase):

    def test1Listener(self):
        listen1 = gum.PythonBNListener(self.bn.dag(), self.bn.variableNodeMap())
        listen1.setWhenNodeAdded(
            lambda i,s: self.bufferize("Noeud++ %s (%d)\n"%(s,i)))
        listen1.setWhenArcAdded(
            lambda i,j: self.bufferize("Arc++ (%d->%d)\n"%(i,j)))
        listen1.setWhenNodeDeleted(
            lambda i: self.bufferize("Noeud-- %d\n"%i))
        listen1.setWhenArcDeleted(
            lambda i,j: self.bufferize("Arc-- (%d->%d)\n"%(i,j)))

        a = self.bn.add(self.var1)
        self.assertEqual(self.buffer0, "Noeud++ a (%d)\n"%a)
        b = self.bn.add(self.var2)
        self.assertEqual(self.buffer0, "Noeud++ b (%d)\n"%b)
        c = self.bn.add(self.var3)
        self.assertEqual(self.buffer0, "Noeud++ c (%d)\n"%c)

        self.bn.addArc(a, b)
        self.assertEqual(self.buffer0, "Arc++ (%d->%d)\n"%(a, b))
        self.bn.addArc(b, c)
        self.assertEqual(self.buffer0, "Arc++ (%d->%d)\n"%(b, c))

        self.assertRaises(gum.InvalidDirectedCycle, self.bn.addArc,c,a)

        self.buffer1 = ""
        self.bn.erase(b)
        self.assertEqual(self.buffer1,
            "Arc-- (%d->%d)\nArc-- (%d->%d)\nNoeud-- %d\n"%(a, b, b, c, b))


    def test2Listeners(self):
        listen1 = gum.PythonBNListener(self.bn.dag(), self.bn.variableNodeMap())
        listen2 = gum.PythonBNListener(self.bn.dag(), self.bn.variableNodeMap())
        listen1.setWhenNodeAdded(
            lambda i,s: self.bufferize("Node++ %s (%d)\n"%(s,i)))
        listen2.setWhenNodeAdded(
            lambda i,s: self.bufferize("Noeud++ %s (%d)\n"%(s,i)))

        self.buffer1 = ""
        a = self.bn.add(self.var1)
        self.assertTrue("Node++ a (%d)\n"%a in self.buffer1)
        self.assertTrue("Noeud++ a (%d)\n"%a in self.buffer1)



ts = unittest.TestSuite()
ts.addTest(TestConstructors('testConstructor'))
ts.addTest(TestMessagesSetting('testMessagesInsertion'))
ts.addTest(TestMessagesSetting('testMessagesChange'))
ts.addTest(TestListeners('test1Listener'))
ts.addTest(TestListeners('test2Listeners'))
