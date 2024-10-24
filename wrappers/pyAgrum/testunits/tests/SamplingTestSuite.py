# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
import time
import unittest

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class SamplingTestCase(pyAgrumTestCase):
  def iterTest(self, goalPotential, inferenceEngine, target, evs, *, seuil=0.1, nbr=20):
    min = 1000

    for i in range(nbr):
      inferenceEngine.eraseAllEvidence()
      inferenceEngine.setEvidence(evs)
      inferenceEngine.makeInference()
      result = inferenceEngine.posterior(target)
      diff = (goalPotential - result).abs().max()

      if diff <= seuil:
        return None

      if min > diff:
        min = diff

    return "Approximations always bad : on {} tests, {:1.7}>{:1.7}".format(nbr, min, seuil)

  def unsharpen(self, bn):
    for nod in bn.nodes():
      bn.cpt(nod).translate(bn.maxParam() / 2).normalizeAsCPT()

  def setUp(self):
    self.bn = gum.fastBN("c->s{no|yes}->w{no|yes};c->r->w")
    self.unsharpen(self.bn)
    self.c, self.s, self.w, self.r = [self.bn.idFromName(s) for s in "cswr"]

    self.bn2 = gum.fastBN("r2->s2->w2;r2->w2")
    self.unsharpen(self.bn2)
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
    protoie.addEvidence('r', 1)
    protoie.addEvidence('w', 0)
    protoie.makeInference()
    proto = protoie.posterior(self.s)

    ie = gum.LoopyWeightedSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.1)
    ie.setMinEpsilonRate(0.01)

    msg = self.iterTest(proto, ie, self.s, {'r': [0, 1], 'w': (1, 0)})
    if msg is not None:
      self.fail(msg)

    ie2 = gum.LoopyGibbsSampling(self.bn)
    ie2.setVerbosity(False)
    ie2.setEpsilon(0.1)
    ie2.setMinEpsilonRate(0.01)
    ie2.setEvidence({'r': 1, 'w': 0})
    ie2.makeInference()

    msg = self.iterTest(proto, ie2, self.s, {'r': 1, 'w': 0})
    if msg is not None:
      self.fail(msg)

    ie3 = gum.LoopyMonteCarloSampling(self.bn)
    ie3.setVerbosity(False)
    ie3.setEpsilon(0.1)
    ie3.setMinEpsilonRate(0.01)

    msg = self.iterTest(proto, ie3, self.s, {'r': [0, 1], 'w': (1, 0)})
    if msg is not None:
      self.fail(msg)


class TestInferenceResults(SamplingTestCase):
  def testOpenBayesSiteExamples(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.makeInference()
    proto = protoie.posterior(self.w)

    ie = gum.LoopyWeightedSampling(self.bn)
    ie.setVerbosity(True)
    ie.setEpsilon(0.02)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.w, {})
    if msg is not None:
      self.fail(msg)

    protoie = gum.LazyPropagation(self.bn)
    protoie.makeInference()
    ie.setEvidence({'s': 0, 'c': 0})
    proto = protoie.posterior(self.w)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.02)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.w, {'s': 0, 'c': 0}, seuil=0.15)
    if msg is not None:
      self.fail(msg)

  def WikipediaExample(self):
    protoie = gum.LazyPropagation(self.bn2)
    protoie.makeInference()
    proto = protoie.posterior('w2')

    ie = gum.LoopyWeightedSampling(self.bn2)
    ie.setVerbosity(False)
    ie.setEpsilon(0.01)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, 'w2', {})
    if msg is not None:
      self.fail(msg)

    ie2 = gum.LoopyMonteCarloSampling(self.bn2)
    ie2.setVerbosity(False)
    ie2.setEpsilon(0.01)
    ie2.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie2, 'w2', {})
    if msg is not None:
      self.fail(msg)


ts = unittest.TestSuite()
addTests(ts, TestDictFeature)
addTests(ts, TestInferenceResults)
