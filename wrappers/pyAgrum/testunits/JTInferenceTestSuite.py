# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class JTInferenceTestCase(pyAgrumTestCase):

  def _getInference(self,bn):
    raise(NotImplementedError("This class is a generic class for JT Inference"))

  def setUp(self):
    self.bn = gum.BayesNet()

    self.c, self.r = \
      [self.bn.add(gum.LabelizedVariable(name, name, 2))
       for name in 'c r'.split()]
    self.s, self.w = \
      [self.bn.add(gum.LabelizedVariable(name, name, 0).addLabel('no').addLabel('yes'))
       for name in 's w'.split()]

    for link in [(self.c, self.s), (self.c, self.r),
                 (self.s, self.w), (self.r, self.w)]:
      self.bn.addArc(*link)

    self.bn.cpt(self.c)[:] = [0.5, 0.5]
    self.bn.cpt(self.s)[:] = [[0.5, 0.5],
                              [0.9, 0.1]]
    self.bn.cpt(self.r)[:] = [[0.8, 0.2],
                              [0.2, 0.8]]
    self.bn.cpt(self.w)[0, 0, :] = [1, 0]
    self.bn.cpt(self.w)[0, 1, :] = [0.1, 0.9]
    self.bn.cpt(self.w)[1, 0, :] = [0.1, 0.9]
    self.bn.cpt(self.w)[1, 1, :] = [0.01, 0.99]

    self.bni = gum.BayesNet()

    self.ci, self.si = \
      [self.bni.add(gum.LabelizedVariable(name, name, 2))
       for name in 'ci si'.split()]
    self.ri = self.bni.add(gum.RangeVariable('ri', '', 5, 6))

    vwi = gum.DiscretizedVariable('wi', '')
    vwi.addTick(0.2).addTick(0.4).addTick(0.6)
    self.wi = self.bni.add(vwi)

    for link in [(self.ci, self.si), (self.ci, self.ri),
                 (self.si, self.wi), (self.ri, self.wi)]:
      self.bni.addArc(*link)

    self.bni.cpt(self.ci)[:] = [0.5, 0.5]
    self.bni.cpt(self.si)[:] = [[0.5, 0.5],
                                [0.9, 0.1]]
    self.bni.cpt(self.ri)[:] = [[0.8, 0.2],
                                [0.2, 0.8]]
    self.bni.cpt(self.wi)[0, 0, :] = [1, 0]
    self.bni.cpt(self.wi)[0, 1, :] = [0.1, 0.9]
    self.bni.cpt(self.wi)[1, 0, :] = [0.1, 0.9]
    self.bni.cpt(self.wi)[1, 1, :] = [0.01, 0.99]

    self.bn2 = gum.BayesNet()

    self.s2, self.r2, self.w2 = \
      [self.bn2.add(gum.LabelizedVariable(name, name, 2))
       for name in 's2 r2 w2'.split()]

    for link in [(self.r2, self.s2), (self.s2, self.w2),
                 (self.r2, self.w2)]:
      self.bn2.addArc(*link)

    self.bn2.cpt(self.s2)[:] = [[0.6, 0.4],
                                [0.99, 0.01]]
    self.bn2.cpt(self.r2)[:] = [0.8, 0.2]
    self.bn2.cpt(self.w2)[0, 0, :] = [1, 0]
    self.bn2.cpt(self.w2)[0, 1, :] = [0.1, 0.9]
    self.bn2.cpt(self.w2)[1, 0, :] = [0.2, 0.8]
    self.bn2.cpt(self.w2)[1, 1, :] = [0.01, 0.99]


  def testSimpleInference(self):
    ie = self._getInference(self.bn)
    ie.makeInference()

    ie = self._getInference(self.bn)
    ie.setEvidence({self.s: [0, 1], self.w: (1, 0)})
    ie.makeInference()
    result = ie.posterior(self.r)

    self.assertListsAlmostEqual(result.tolist(), [0.95890411, 0.04109589])

  def testDictOfLabels(self):
    ie = self._getInference(self.bn)
    ie.setEvidence({'s': 0, 'w': 1})
    ie.makeInference()
    result = ie.posterior(self.r)

    ie2 = self._getInference(self.bn)
    ie2.setEvidence({'s': 'no', 'w': 'yes'})
    ie2.makeInference()
    result2 = ie2.posterior(self.r)

    self.assertListsAlmostEqual(result.tolist(), result2.tolist())

  def testDictOfLabelsWithId(self):
    ie = self._getInference(self.bn)
    ie.setEvidence({self.s: 0, self.w: 1})
    ie.makeInference()
    result = ie.posterior(self.r)

    ie2 = self._getInference(self.bn)
    ie2.setEvidence({self.s: 'no', self.w: 'yes'})
    ie2.makeInference()
    result2 = ie2.posterior(self.r)

    self.assertListsAlmostEqual(result.tolist(), result2.tolist())

  def testWithDifferentVariables(self):
    ie = self._getInference(self.bn)
    ie.setEvidence({'r': [0, 1], 'w': (1, 0)})
    ie.makeInference()
    result = ie.posterior(self.s).tolist()
    ie = self._getInference(self.bni)
    ie.setEvidence({'ri': [0, 1], 'wi': (1, 0)})
    ie.makeInference()
    result2 = ie.posterior(self.si).tolist()
    self.assertListsAlmostEqual(result, result2)

    ie = self._getInference(self.bn)
    ie.setEvidence({'r': 1, 'w': 0})
    ie.makeInference()
    result = ie.posterior(self.s).tolist()
    self.assertListsAlmostEqual(result, result2)

    ie = self._getInference(self.bni)
    ie.setEvidence({'ri': "6", 'wi': "0.33"})
    ie.makeInference()
    result = ie.posterior(self.si).tolist()
    self.assertListsAlmostEqual(result, result2)

  def testWithDifferentVariablesWithId(self):
    ie = self._getInference(self.bn)
    ie.setEvidence({self.r: [0, 1], self.w: (1, 0)})
    ie.makeInference()
    result = ie.posterior(self.s).tolist()
    ie = self._getInference(self.bni)
    ie.setEvidence({self.ri: [0, 1], self.wi: (1, 0)})
    ie.makeInference()
    result2 = ie.posterior(self.si).tolist()
    self.assertListsAlmostEqual(result, result2)

    ie = self._getInference(self.bn)
    ie.setEvidence({self.r: 1, self.w: 0})
    ie.makeInference()
    result = ie.posterior(self.s).tolist()
    self.assertListsAlmostEqual(result, result2)

    ie = self._getInference(self.bni)
    ie.setEvidence({self.ri: "6", self.wi: "0.33"})
    ie.makeInference()
    result2 = ie.posterior(self.si).tolist()
    self.assertListsAlmostEqual(result, result2)

  def testOpenBayesSiteExamples(self):
    ie = self._getInference(self.bn)
    ie.makeInference()
    result = ie.posterior(self.w)
    self.assertListsAlmostEqual(result.tolist(), [0.3529, 0.6471],
                                places=4)

    ie = self._getInference(self.bn)
    ie.setEvidence({'s': 1, 'c': 0})
    ie.makeInference()
    result = ie.posterior(self.w)
    self.assertListsAlmostEqual(result.tolist(), [0.082, 0.918], places=4)

  def testWikipediaExample(self):
    ie = self._getInference(self.bn2)
    ie.setEvidence({'w2': 1})
    ie.makeInference()
    result = ie.posterior(self.r2)
    expected = [1 - 0.3577, 0.3577]
    self.assertListsAlmostEqual(result.tolist(), expected, places=4)


class LazyPropagationTestCase(JTInferenceTestCase):
  def _getInference(self,bn):
    return gum.LazyPropagation(bn)

class ShaferShenoyTestCase(JTInferenceTestCase):
  def _getInference(self,bn):
    return gum.ShaferShenoyInference(bn)

class VariableEliminationTestCase(JTInferenceTestCase):
  def _getInference(self,bn):
    return gum.VariableElimination(bn)

ts = unittest.TestSuite()
addTests(ts, LazyPropagationTestCase)
addTests(ts, ShaferShenoyTestCase)
addTests(ts, VariableEliminationTestCase)
