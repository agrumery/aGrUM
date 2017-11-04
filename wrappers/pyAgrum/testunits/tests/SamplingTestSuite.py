# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class SamplingTestCase(pyAgrumTestCase):
  def iterTest(self, goalPotential, inferenceEngine, target, evs, seuil=0.1, nbr=10):
    min = 1000

    for i in range(nbr):
      inferenceEngine.eraseAllEvidence()
      inferenceEngine.setEvidence(evs)
      inferenceEngine.makeInference()
      result = inferenceEngine.posterior(target)
      diff = (goalPotential - result).abs().max()
      if diff <= seuil:
        return None
      else:
        print("!", end="")
      if min > diff:
        min = diff

    return "Approximations always bad : on {} tests, {:1.7}>{:1.7}".format(nbr, min, seuil)

  def unsharpen(bn):
    for nod in bn.ids():
      bn.cpt(nod).translate(bn.maxParam() / 2).normalizeAsCPT()

  def setUp(self):
    self.bn = gum.fastBN("c->s{no|yes}->w{no|yes};c->r->w")
    SamplingTestCase.unsharpen(self.bn)
    self.c, self.s, self.w, self.r = [self.bn.idFromName(s) for s in "cswr"]

    self.bn2 = gum.fastBN("r2->s2->w2;r2->w2")
    SamplingTestCase.unsharpen(self.bn2)
    self.r2, self.s2, self.w2 = [self.bn2.idFromName(s) for s in ["s2", "w2", "r2"]]


class TestDictFeature(SamplingTestCase):
  def testDictOfSequences(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence('s', 1)
    protoie.addEvidence('w', 0)
    protoie.makeInference()
    proto = protoie.posterior(self.r)

    ie = gum.LoopyImportanceSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, {'s': [0, 1], 'w': (1, 0)})
    if msg is not None:
      self.fail(msg)

    ie = gum.LoopyImportanceSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, ({'s': 1, 'w': 0}))
    if msg is not None:
      self.fail(msg)

  def testDictOfLabels(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence('s', 0)
    protoie.addEvidence('w', 1)
    protoie.makeInference()
    proto = protoie.posterior(self.r)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, {'s': 0, 'w': 1})
    if msg is not None:
      self.fail(msg)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, {'s': 'no', 'w': 'yes'})
    if msg is not None:
      self.fail(msg)

  def testDictOfLabelsWithId(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence('s', 0)
    protoie.addEvidence('w', 1)
    protoie.makeInference()
    proto = protoie.posterior(self.r)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.01)
    msg = self.iterTest(proto, ie, self.r, {self.s: 0, self.w: 1})
    if msg is not None:
      self.fail(msg)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.01)
    msg = self.iterTest(proto, ie, self.r, {self.s: 'no', self.w: 'yes'})
    if msg is not None:
      self.fail(msg)

  def testWithDifferentVariables(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence('s', 0)
    protoie.addEvidence('w', 1)
    protoie.makeInference()
    proto = protoie.posterior(self.s)

    ie = gum.LoopyWeightedSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.1)
    ie.setMinEpsilonRate(0.01)
    # msg = self.iterTest(proto, ie, self.s, {'r': [0, 1], 'w': (1, 0)})
    # if msg is not None:
    #   self.fail(msg)

    ie2 = gum.LoopyGibbsSampling(self.bn)
    ie2.setVerbosity(False)
    ie2.setEpsilon(0.1)
    ie2.setMinEpsilonRate(0.01)
    ie2.setEvidence({'r': 1, 'w': 0})
    ie2.makeInference()
    print(ie.posterior('s'))

    msg = self.iterTest(proto, ie2, self.s, {'r': 1, 'w': 0})
    if msg is not None:
      self.fail(msg)


class TestInferenceResults(SamplingTestCase):
  def testOpenBayesSiteExamples(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.makeInference()
    proto = protoie.posterior(self.w)

    ie = gum.LoopyWeightedSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.1)
    ie.setMinEpsilonRate(0.01)
    result = ie.posterior(self.w)
    self.assertGreaterEqual(0.1, (proto - result).abs().max())

    protoie = gum.LazyPropagation(self.bn)
    protoie.makeInference()
    ie.setEvidence({'s': 1, 'c': 0})
    proto = protoie.posterior(self.w)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.1)
    ie.setMinEpsilonRate(0.01)
    ie.setEvidence({'s': 1, 'c': 0})
    ie.makeInference()
    result = ie.posterior(self.w)
    self.assertGreaterEqual(0.1, (proto - result).abs().max())

  def WikipediaExample(self):
    ie = gum.LoopyGibbsSampling(self.bn2)
    ie.setVerbosity(False)
    ie.setEpsilon(0.1)
    ie.setMinEpsilonRate(0.001)
    ie.setEvidence({'w2': 1})
    ie.makeInference()
    result = ie.posterior(self.r2)
    expected = [1 - 0.3577, 0.3577]
    self.assertDelta(result.tolist(), expected)


ts = unittest.TestSuite()
addTests(ts, TestDictFeature)
addTests(ts, TestInferenceResults)
