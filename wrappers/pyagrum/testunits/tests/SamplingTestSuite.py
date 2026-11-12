############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import pyagrum as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class SamplingTestCase(pyAgrumTestCase):
  def iterTest(self, goalTensor, inferenceEngine, target, evs, *, seuil=0.1, nbr=60):
    min = 1000

    for i in range(nbr):
      inferenceEngine.eraseAllEvidence()
      inferenceEngine.setEvidence(evs)
      inferenceEngine.makeInference()
      result = inferenceEngine.posterior(target)
      diff = (goalTensor - result).abs().max()

      if diff <= seuil:
        return None

      if min > diff:
        min = diff

    return f"Approximations always bad : on {nbr} tests, {min:1.7}>{seuil:1.7}"

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
    protoie.addEvidence("s", 1)
    protoie.addEvidence("w", 0)
    protoie.makeInference()
    proto = protoie.posterior(self.r)

    ie = gum.LoopyImportanceSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, {"s": [0, 1], "w": (1, 0)})
    if msg is not None:
      self.fail(msg)

    ie = gum.LoopyImportanceSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, ({"s": 1, "w": 0}))
    if msg is not None:
      self.fail(msg)

  def testDictOfLabels(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence("s", 0)
    protoie.addEvidence("w", 1)
    protoie.makeInference()
    proto = protoie.posterior(self.r)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, {"s": 0, "w": 1})
    if msg is not None:
      self.fail(msg)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.05)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.r, {"s": "no", "w": "yes"})
    if msg is not None:
      self.fail(msg)

  def testDictOfLabelsWithId(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence("s", 0)
    protoie.addEvidence("w", 1)
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
    msg = self.iterTest(proto, ie, self.r, {self.s: "no", self.w: "yes"})
    if msg is not None:
      self.fail(msg)

  def testWithDifferentVariables(self):
    protoie = gum.LazyPropagation(self.bn)
    protoie.addEvidence("r", 1)
    protoie.addEvidence("w", 0)
    protoie.makeInference()
    proto = protoie.posterior(self.s)

    ie = gum.LoopyWeightedSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.1)
    ie.setMinEpsilonRate(0.01)

    msg = self.iterTest(proto, ie, self.s, {"r": [0, 1], "w": (1, 0)})
    if msg is not None:
      self.fail(msg)

    ie2 = gum.LoopyGibbsSampling(self.bn)
    ie2.setVerbosity(False)
    ie2.setEpsilon(0.1)
    ie2.setMinEpsilonRate(0.01)
    ie2.setEvidence({"r": 1, "w": 0})
    ie2.makeInference()

    msg = self.iterTest(proto, ie2, self.s, {"r": 1, "w": 0})
    if msg is not None:
      self.fail(msg)

    ie3 = gum.LoopyMonteCarloSampling(self.bn)
    ie3.setVerbosity(False)
    ie3.setEpsilon(0.1)
    ie3.setMinEpsilonRate(0.01)

    msg = self.iterTest(proto, ie3, self.s, {"r": [0, 1], "w": (1, 0)})
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
    protoie.setEvidence({"s": 0, "c": 0})
    protoie.makeInference()
    proto = protoie.posterior(self.w)

    ie = gum.LoopyGibbsSampling(self.bn)
    ie.setVerbosity(False)
    ie.setEpsilon(0.02)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, self.w, {"s": 0, "c": 0}, seuil=0.15)
    if msg is not None:
      self.fail(msg)

  def WikipediaExample(self):
    protoie = gum.LazyPropagation(self.bn2)
    protoie.makeInference()
    proto = protoie.posterior("w2")

    ie = gum.LoopyWeightedSampling(self.bn2)
    ie.setVerbosity(False)
    ie.setEpsilon(0.01)
    ie.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie, "w2", {})
    if msg is not None:
      self.fail(msg)

    ie2 = gum.LoopyMonteCarloSampling(self.bn2)
    ie2.setVerbosity(False)
    ie2.setEpsilon(0.01)
    ie2.setMinEpsilonRate(0.001)
    msg = self.iterTest(proto, ie2, "w2", {})
    if msg is not None:
      self.fail(msg)


class TestIncompatibleEvidence(pyAgrumTestCase):
  """Evidence with a null probability: the samplers and the database generator raise instead of looping forever."""

  engines = ("MonteCarloSampling", "WeightedSampling", "ImportanceSampling")

  @staticmethod
  def deterministic(bn, node, f):
    for a in range(2):
      for b in range(2):
        cpt = [0.0, 0.0]
        cpt[f(a, b)] = 1.0
        bn.cpt(node)[{"A": a, "B": b}] = cpt

  def andBN(self):
    # C = A and B: the evidence {C:1, A:0} is detected by the removal of impossible values
    bn = gum.fastBN("A->C<-B")
    self.deterministic(bn, "C", lambda a, b: a & b)
    return bn

  def xorBN(self):
    # Y = A xor B and Z = not(A xor B): {Y:1, Z:1} is impossible, but each CPT alone allows every value
    bn = gum.fastBN("A->Y<-B;A->Z<-B")
    self.deterministic(bn, "Y", lambda a, b: a ^ b)
    self.deterministic(bn, "Z", lambda a, b: 1 - (a ^ b))
    return bn

  def testProvedIncompatibility(self):
    bn = self.andBN()
    for name in self.engines:
      with self.subTest(engine=name):
        ie = getattr(gum, name)(bn)
        ie.setEvidence({"C": 1, "A": 0})
        with self.assertRaisesRegex(gum.IncompatibleEvidence, "has a null probability"):
          ie.makeInference()

  def testRejectionBound(self):
    bn = self.xorBN()
    gum.config.push()
    try:
      gum.config["sampling", "maxConsecutiveRejections"] = 1000
      for name in self.engines:
        with self.subTest(engine=name):
          ie = getattr(gum, name)(bn)
          ie.setEvidence({"Y": 1, "Z": 1})
          with self.assertRaisesRegex(gum.IncompatibleEvidence, "consecutive draws rejected"):
            ie.makeInference()
    finally:
      gum.config.pop()

  def testTimeLimit(self):
    bn = self.xorBN()
    gum.config.push()
    try:
      gum.config["sampling", "maxConsecutiveRejections"] = 10**12
      for name in self.engines:
        with self.subTest(engine=name):
          ie = getattr(gum, name)(bn)
          ie.setEvidence({"Y": 1, "Z": 1})
          ie.setMaxTime(0.2)
          with self.assertRaisesRegex(gum.IncompatibleEvidence, "within the time limit"):
            ie.makeInference()
    finally:
      gum.config.pop()

  def testCompatibleEvidence(self):
    bn = self.xorBN()
    exact = gum.LazyPropagation(bn)
    exact.setEvidence({"Y": 1, "Z": 0})
    exact.makeInference()
    for name in self.engines:
      with self.subTest(engine=name):
        ie = getattr(gum, name)(bn)
        ie.setEvidence({"Y": 1, "Z": 0})
        ie.setEpsilon(1e-2)
        ie.makeInference()
        self.assertAlmostEqual(ie.posterior("A")[1], exact.posterior("A")[1], delta=0.1)

  def testDatabaseGenerator(self):
    generator = gum.BNDatabaseGenerator(self.andBN())
    with self.assertRaisesRegex(gum.IncompatibleEvidence, "has a null probability"):
      generator.drawSamples(10, {"C": 1, "A": 0})

    gum.config.push()
    try:
      gum.config["sampling", "maxConsecutiveRejections"] = 1000
      generator = gum.BNDatabaseGenerator(self.xorBN())
      with self.assertRaisesRegex(gum.IncompatibleEvidence, "consecutive draws rejected"):
        generator.drawSamples(10, {"Y": 1, "Z": 1})
    finally:
      gum.config.pop()


ts = unittest.TestSuite()
addTests(ts, TestDictFeature)
addTests(ts, TestInferenceResults)
addTests(ts, TestIncompatibleEvidence)
