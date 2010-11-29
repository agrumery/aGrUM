# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet, LabelizedVar, RangeVar, DiscretizedVar, Potential
from pyAgrum import ListPotentials, LazyPropagation
from pyAgrumTestSuite import pyAgrumTestCase


class LazyPropagationTestCase(pyAgrumTestCase):

    def setUp(self):
        self.bn = BayesNet()

        self.c, self.r = \
            [self.bn.add(LabelizedVar(name, name, 2))
             for name in 'c r'.split()]
        self.s, self.w = \
            [self.bn.add(LabelizedVar(name, name, 0).addLabel('no')\
                                                    .addLabel('yes'))
             for name in 's w'.split()]

        for link in [(self.c, self.s), (self.c, self.r),
                     (self.s, self.w), (self.r, self.w)]:
            self.bn.insertArc(*link)

        self.bn.cpt(self.c)[:] = [0.5, 0.5]
        self.bn.cpt(self.s)[:] = [ [0.5, 0.5],
                                   [0.9, 0.1] ]
        self.bn.cpt(self.r)[:] = [ [0.8, 0.2],
                                   [0.2, 0.8] ]
        self.bn.cpt(self.w)[0,0,:] = [1, 0]
        self.bn.cpt(self.w)[0,1,:] = [0.1, 0.9]
        self.bn.cpt(self.w)[1,0,:] = [0.1, 0.9]
        self.bn.cpt(self.w)[1,1,:] = [0.01, 0.99]

        self.bni = BayesNet()

        self.ci, self.si = \
            [self.bni.add(LabelizedVar(name, name, 2))
             for name in 'ci si'.split()]
        self.ri = self.bni.add(RangeVar('ri', '', 5, 6))

        vwi = DiscretizedVar('wi', '')
        vwi.addTick(0.2).addTick(0.4).addTick(0.6)
        self.wi = self.bni.add(vwi)

        for link in [(self.ci, self.si), (self.ci, self.ri),
                     (self.si, self.wi), (self.ri, self.wi)]:
            self.bni.insertArc(*link)

        self.bni.cpt(self.ci)[:] = [0.5, 0.5]
        self.bni.cpt(self.si)[:] = [ [0.5, 0.5],
                                     [0.9, 0.1] ]
        self.bni.cpt(self.ri)[:] = [ [0.8, 0.2],
                                     [0.2, 0.8] ]
        self.bni.cpt(self.wi)[0,0,:] = [1, 0]
        self.bni.cpt(self.wi)[0,1,:] = [0.1, 0.9]
        self.bni.cpt(self.wi)[1,0,:] = [0.1, 0.9]
        self.bni.cpt(self.wi)[1,1,:] = [0.01, 0.99]

        self.bn2 = BayesNet()

        self.s2, self.r2, self.w2 = \
            [self.bn2.add(LabelizedVar(name, name, 2))
             for name in 's2 r2 w2'.split()]

        for link in [(self.r2, self.s2), (self.s2, self.w2),
                     (self.r2, self.w2)]:
            self.bn2.insertArc(*link)

        self.bn2.cpt(self.s2)[:] = [ [0.6,  0.4 ],
                                     [0.99, 0.01] ]
        self.bn2.cpt(self.r2)[:] = [0.8, 0.2]
        self.bn2.cpt(self.w2)[0,0,:] = [1, 0]
        self.bn2.cpt(self.w2)[0,1,:] = [0.1, 0.9]
        self.bn2.cpt(self.w2)[1,0,:] = [0.2, 0.8]
        self.bn2.cpt(self.w2)[1,1,:] = [0.01, 0.99]



class TestDictFeature(LazyPropagationTestCase):

    def testDictOfSequences(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({self.s: [0, 1], self.w: (1, 0)})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie = LazyPropagation(self.bn)
        list_pot = ListPotentials()
        pot = Potential()
        pot.add(self.bn.variable(self.s))
        pot[:] = [0, 1]
        list_pot.append(pot)
        pot = Potential()
        pot.add(self.bn.variable(self.w))
        pot[:] = [1, 0]
        list_pot.append(pot)

        ie.insertEvidence(list_pot)
        ie.makeInference()
        result2 = ie.marginal(self.r)

        result.empty()
        self.assertListsAlmostEqual(result.tolist(), result2.tolist())
        
    def testDictOfSequencesWithId(self):
				
        ie = LazyPropagation(self.bn)
        ie.setEvidence({'s': [0, 1], 'w': (1, 0)})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie = LazyPropagation(self.bn)
        list_pot = ListPotentials()
        pot = Potential()
        pot.add(self.bn.variable(self.s))
        pot[:] = [0, 1]
        list_pot.append(pot)
        pot = Potential()
        pot.add(self.bn.variable(self.w))
        pot[:] = [1, 0]
        list_pot.append(pot)

        ie.insertEvidence(list_pot)
        ie.makeInference()
        result2 = ie.marginal(self.r)

        result.empty()
        self.assertListsAlmostEqual(result.tolist(), result2.tolist())


    def testDictOfNumbers(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({'w': 0, 's': 1})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie = LazyPropagation(self.bn)
        list_pot = ListPotentials()
        pot = Potential()
        pot.add(self.bn.variable(self.s))
        pot[:] = [0, 1]
        list_pot.append(pot)
        pot = Potential()
        pot.add(self.bn.variable(self.w))
        pot[:] = [1, 0]
        list_pot.append(pot)

        ie.insertEvidence(list_pot)
        ie.makeInference()
        result2 = ie.marginal(self.r)

        self.assertListsAlmostEqual(result.tolist(), result2.tolist())

    def testDictOfNumbersWithId(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({self.w: 0, self.s: 1})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie = LazyPropagation(self.bn)
        list_pot = ListPotentials()
        pot = Potential()
        pot.add(self.bn.variable(self.s))
        pot[:] = [0, 1]
        list_pot.append(pot)
        pot = Potential()
        pot.add(self.bn.variable(self.w))
        pot[:] = [1, 0]
        list_pot.append(pot)

        ie.insertEvidence(list_pot)
        ie.makeInference()
        result2 = ie.marginal(self.r)

        self.assertListsAlmostEqual(result.tolist(), result2.tolist())

    def testDictOfLabels(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({'s': 0, 'w': 1})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie2 = LazyPropagation(self.bn)
        ie2.setEvidence({'s': 'no', 'w': 'yes'})
        ie2.makeInference()
        result2 = ie2.marginal(self.r)

        self.assertListsAlmostEqual(result.tolist(), result2.tolist())
        
    def testDictOfLabelsWithId(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({self.s: 0, self.w: 1})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie2 = LazyPropagation(self.bn)
        ie2.setEvidence({self.s: 'no', self.w: 'yes'})
        ie2.makeInference()
        result2 = ie2.marginal(self.r)

        self.assertListsAlmostEqual(result.tolist(), result2.tolist())


    def testWithDifferentVariables(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({'r': [0, 1], 'w': (1, 0)})
        ie.makeInference()
        result = ie.marginal(self.s)
        ie = LazyPropagation(self.bni)
        ie.setEvidence({'ri': [0, 1], 'wi': (1, 0)})
        ie.makeInference()
        result2 = ie.marginal(self.si)
        self.assertListsAlmostEqual(result.tolist(), result2.tolist())

        ie = LazyPropagation(self.bn)
        ie.setEvidence({'r': 1, 'w': 0})
        ie.makeInference()
        result = ie.marginal(self.s)
        ie = LazyPropagation(self.bni)
        ie.setEvidence({'ri': 6, 'wi': 0.33})
        ie.makeInference()
        result2 = ie.marginal(self.si)
        self.assertListsAlmostEqual(result.tolist(), result2.tolist())
        
    def testWithDifferentVariablesWithId(self):
        ie = LazyPropagation(self.bn)
        ie.setEvidence({self.r: [0, 1], self.w: (1, 0)})
        ie.makeInference()
        result = ie.marginal(self.s)
        ie = LazyPropagation(self.bni)
        ie.setEvidence({self.ri: [0, 1], self.wi: (1, 0)})
        ie.makeInference()
        result2 = ie.marginal(self.si)
        self.assertListsAlmostEqual(result.tolist(), result2.tolist())

        ie = LazyPropagation(self.bn)
        ie.setEvidence({self.r: 1, self.w: 0})
        ie.makeInference()
        result = ie.marginal(self.s)
        ie = LazyPropagation(self.bni)
        ie.setEvidence({self.ri: 6, self.wi: 0.33})
        ie.makeInference()
        result2 = ie.marginal(self.si)
        self.assertListsAlmostEqual(result.tolist(), result2.tolist())



class TestInferenceResults(LazyPropagationTestCase):

    def testOpenBayesSiteExamples(self):
        ie = LazyPropagation(self.bn)
        ie.makeInference()
        result = ie.marginal(self.w)
        self.assertListsAlmostEqual(result.tolist(), [0.3529, 0.6471],
                                    places=4)

        ie = LazyPropagation(self.bn)
        ie.setEvidence({'s': 1, 'c': 0})
        ie.makeInference()
        result = ie.marginal(self.w)
        self.assertListsAlmostEqual(result.tolist(), [0.082, 0.918], places=4)


    def testWikipediaExample(self):
        ie = LazyPropagation(self.bn2)
        ie.setEvidence({'w2': 1})
        ie.makeInference()
        result = ie.marginal(self.r2)
        expected = [1-0.3577, 0.3577]
        self.assertListsAlmostEqual(result.tolist(), expected, places=4)



ts = unittest.TestSuite()
ts.addTest(TestDictFeature('testDictOfSequences'))
ts.addTest(TestDictFeature('testDictOfNumbers'))
ts.addTest(TestDictFeature('testDictOfLabels'))
ts.addTest(TestDictFeature('testWithDifferentVariables'))
ts.addTest(TestInferenceResults('testOpenBayesSiteExamples'))
ts.addTest(TestInferenceResults('testWikipediaExample'))
