# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet_float, LabelizedVar, RangeVar, DiscretizedVar, Potential_float
from pyAgrum import ListPotentials_float, Gibbs_float
from pyAgrumTestSuite import pyAgrumTestCase


class GibbsTestCase(pyAgrumTestCase):

    def setUp(self):
        self.bn = BayesNet_float()

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

        self.bni = BayesNet_float()

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

        self.bn2 = BayesNet_float()

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



class TestDictFeature(GibbsTestCase):
    def testDictOfSequences(self):
        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'s': [0, 1], 'w': (1, 0)})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        list_pot = ListPotentials_float()
        pot = Potential_float()
        pot.add(self.bn.variable(self.s))
        pot[:] = [0, 1]
        list_pot.append(pot)
        pot = Potential_float()
        pot.add(self.bn.variable(self.w))
        pot[:] = [1, 0]
        list_pot.append(pot)

        ie.insertEvidence(list_pot)
        ie.makeInference()
        result2 = ie.marginal(self.r)

        self.assertDelta(result.tolist(), result2.tolist())


    def testDictOfNumbers(self):
        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'s': 1, 'w': 0})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        list_pot = ListPotentials_float()
        pot = Potential_float()
        pot.add(self.bn.variable(self.s))
        pot[:] = [0, 1]
        list_pot.append(pot)
        pot = Potential_float()
        pot.add(self.bn.variable(self.w))
        pot[:] = [1, 0]
        list_pot.append(pot)

        ie.insertEvidence(list_pot)
        ie.makeInference()
        result2 = ie.marginal(self.r)

        self.assertDelta(result.tolist(), result2.tolist())


    def testDictOfLabels(self):
        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'s': 0, 'w': 1})
        ie.makeInference()
        result = ie.marginal(self.r)

        ie2 = GibbsInference_float(self.bn)
        ie2.setVerbosity(False)
        ie2.setEpsilon(0.0001)
        ie2.setMinEpsilonRate(0.0001)
        ie2.setEvidence({'s': 'no', 'w': 'yes'})
        ie2.makeInference()
        result2 = ie2.marginal(self.r)

        self.assertListsAlmostEqual(result.tolist(), result2.tolist())


    def testWithDifferentVariables(self):
        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'r': [0, 1], 'w': (1, 0)})
        ie.makeInference()
        result = ie.marginal(self.s)

        ie = GibbsInference_float(self.bni)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'ri': [0, 1], 'wi': (1, 0)})
        ie.makeInference()
        result2 = ie.marginal(self.si)
        self.assertDelta(result.tolist(), result2.tolist())

        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'r': 1, 'w': 0})
        ie.makeInference()
        result = ie.marginal(self.s)

        ie = GibbsInference_float(self.bni)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'ri': 6, 'wi': 0.33})
        ie.makeInference()
        result2 = ie.marginal(self.si)
        self.assertDelta(result.tolist(), result2.tolist())



class TestInferenceResults(GibbsTestCase):

    def testOpenBayesSiteExamples(self):
        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        result = ie.marginal(self.w)
        self.assertDelta(result.tolist(), [0.3529, 0.6471])

        ie = GibbsInference_float(self.bn)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'s': 1, 'c': 0})
        ie.makeInference()
        result = ie.marginal(self.w)
        self.assertDelta(result.tolist(), [0.082, 0.918])


    def testWikipediaExample(self):
        ie = GibbsInference_float(self.bn2)
        ie.setVerbosity(False)
        ie.setEpsilon(0.0001)
        ie.setMinEpsilonRate(0.0001)
        ie.setEvidence({'w2': 1})
        ie.makeInference()
        result = ie.marginal(self.r2)
        expected = [1-0.3577, 0.3577]
        self.assertDelta(result.tolist(), expected)



ts = unittest.TestSuite()
ts.addTest(TestDictFeature('testDictOfSequences'))
ts.addTest(TestDictFeature('testDictOfNumbers'))
ts.addTest(TestDictFeature('testDictOfLabels'))
ts.addTest(TestDictFeature('testWithDifferentVariables'))
ts.addTest(TestInferenceResults('testOpenBayesSiteExamples'))
ts.addTest(TestInferenceResults('testWikipediaExample'))
