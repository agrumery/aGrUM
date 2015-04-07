# -*- encoding: UTF-8 -*-

import pyAgrum as gum 

import unittest
from pyAgrumTestSuite import pyAgrumTestCase



class VariablesTestCase(pyAgrumTestCase):

    def setUp(self):
        self.varL1 = gum.LabelizedVariable("var1", "1")
        self.varL2 = gum.LabelizedVariable( "var2", "2", 5 )


    def tearDown(self):
        self.varL1 = self.varL2 = object()



class TestDiscreteVariable(VariablesTestCase):

    def testLabelizedVarCopy(self):
        self.varL1.addLabel("coucou")
        self.varL1.addLabel("super")
        self.varD = self.varL1.clone()
        self.assertFalse(self.varD.empty())
        self.assertEqual(self.varD.label(3), "super")



class TestLabelizedVariable(VariablesTestCase):

    def testCopyConstructor(self):
        var = gum.LabelizedVariable(self.varL1)


    def testPythonListComprehension(self):
        c, s, r, w = [ gum.LabelizedVariable(name, name, 2) \
                       for name in 'c s r w'.split() ]
        for var, name in zip([c, s, r, w], 'c s r w'.split()):
            self.assertEqual(var.name(), name)
            self.assertEqual(var.description(), name)
            self.assertEqual(var.domainSize(), 2)


    def testLabels(self):
        gum.LabelizedVariable('a', '', 0).addLabel('a1').addLabel('a2')\
                                .addLabel('a3').addLabel('a4')\
                                .addLabel('a5').addLabel('a6')
        self.assertEqual(self.varL2.domainSize(), 5)
        self.varL1.addLabel("coucou")
        self.varL1.addLabel("super")
        self.assertEqual(self.varL1.domainSize(), 4)
        self.varL1.isLabel("coucou")
        self.assertEqual(self.varL1["super"], 3)
        self.assertEqual(self.varL1.label(3), "super")


    def testEraseLabels(self):
        self.varL2.eraseLabels()
        self.assertEqual(self.varL2.domainSize(), 0)



class TestRangeVariable(VariablesTestCase):

    def testCopyConstructor(self):
        var1 = gum.RangeVariable("var 1", "this is var 1")
        var2 = gum.RangeVariable("var 2", "this is var 2", 1, 4)

        var3 = gum.RangeVariable(var1)
        self.assertEqual(var3.minVal(), var1.minVal())
        self.assertEqual(var3.maxVal(), var1.maxVal())
        self.assertNotEqual(var1.maxVal(), var2.maxVal())


    def testLabels(self):
        var1 = gum.RangeVariable("var 1", "this is var 1")
        self.assertEqual(var1.domainSize(), 2)
        self.assertFalse(var1.empty())

        var1.setMinVal(1)
        var1.setMaxVal(0)
        self.assertTrue(var1.empty())

        var1.setMaxVal(9)
        self.assertFalse(var1.empty())
        self.assertEqual(var1.domainSize(), 9)
        self.assertTrue(var1.belongs(3))
        self.assertFalse(var1.belongs(0))
        self.assertFalse(var1.belongs(10))

        self.assertEqual(var1.label(1), "2")
        self.assertEqual(var1["2"], 1)


    def testEraseLabels(self):
        self.varL2.eraseLabels()
        self.assertEqual(self.varL2.domainSize(), 0)



class TestDiscretizedVariable(VariablesTestCase):

    def testAddTicks(self):
        gum.DiscretizedVariable('a', '').addTick(0.5).addTick(5.9).addTick(5.99).addTick(0.1).addTick(0.23).addTick(12)

        var = gum.DiscretizedVariable("var", "test var")

        self.assertTrue(var.empty())
        self.assertEqual(var.domainSize(), 0)
        var.addTick(0.2)
        self.assertTrue(var.empty())
        self.assertEqual(var.domainSize(), 0)
        var.addTick(3)
        self.assertTrue(var.empty())
        self.assertEqual(var.domainSize(), 1)
        var.addTick(2)
        self.assertFalse(var.empty())
        self.assertEqual(var.domainSize(), 2)

        self.assertRaises(gum.DefaultInLabel, var.addTick,2)
        self.assertEqual(str(var), "var<[0.2;2[,[2;3]>")


    def testNormalLimits(self):
        var = gum.DiscretizedVariable("var", "test var")

        var.addTick(2.1).addTick(2.5).addTick(2.3).addTick(2.7)
        self.assertEqual(str(var), "var<[2.1;2.3[,[2.3;2.5[,[2.5;2.7]>")

        self.assertRaises(gum.OutOfLowerBound, var.index,0)
        self.assertEqual(var.index(2.1), 0)
        self.assertEqual(var.index(2.2), 0)
        self.assertEqual(var.index(2.3), 1)
        self.assertEqual(var.index(2.4), 1)
        self.assertEqual(var.index(2.5), 2)
        self.assertEqual(var.index(2.6), 2)
        self.assertEqual(var.index(2.7), 2)
        self.assertRaises(gum.OutOfUpperBound, var.index,8)


    def toto(self, i, j, k, l, m, n):
        var = gum.DiscretizedVariable("var", "test var")
        var.addTick(i).addTick(j).addTick(k).addTick(l).addTick(m).addTick(n)
        self.assertEqual(var.domainSize(), 5);
        self.assertEqual(str(var), "var<[1;2[,[2;3[,[3;4[,[4;5[,[5;6]>")


    def testOrderTicks(self):
        for i in range(1, 7):
            for j in range(1, 7):
                for k in range(1, 7):
                    for l in range(1, 7):
                        for m in range(1, 7):
                            for n in range(1, 7):
                                if (i+j+k+l+m+n==21) & (i*j*k*l*m*n==720):
                                    self.toto(i, j, k, l, m, n)



ts = unittest.TestSuite()
ts.addTest(TestDiscreteVariable('testLabelizedVarCopy'))
ts.addTest(TestLabelizedVariable('testCopyConstructor'))
ts.addTest(TestLabelizedVariable('testPythonListComprehension'))
ts.addTest(TestLabelizedVariable('testLabels'))
ts.addTest(TestLabelizedVariable('testEraseLabels'))
ts.addTest(TestRangeVariable('testCopyConstructor'))
ts.addTest(TestRangeVariable('testLabels'))
ts.addTest(TestRangeVariable('testEraseLabels'))
ts.addTest(TestDiscretizedVariable('testAddTicks'))
ts.addTest(TestDiscretizedVariable('testNormalLimits'))
ts.addTest(TestDiscretizedVariable('testOrderTicks'))
