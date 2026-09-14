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

# Correctness of the forward interventional engine, judged against an
# INDEPENDENT path: the KTBN unrolled to a plain BayesNet, do-mutilated
# (intervened nodes: parents cut, CPT set to a point mass), then solved by
# LazyPropagation. The forward roll and the join tree share no code, so
# agreement is meaningful -- not a tautology.

import unittest

import pyagrum.ktbn as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

AT = gum.KTBN.ATEMPORAL
TOL = 1e-9


def _nodeName(base, slice_):
  return base if slice_ == AT else f"{base}[{slice_}]"


def _toVector(tensor):
  v = [0.0] * tensor.variable(0).domainSize()
  for i in range(len(v)):
    inst = gum.Instantiation(tensor)
    inst.chgVal(tensor.variable(0), i)
    v[i] = tensor[inst]
  return v


def _oracleMarginals(ktbn, dos, evs, T):
  # dos/evs: list of (base, slice, value)
  bn = ktbn.unroll(T)

  for base, slice_, value in dos:
    n = _nodeName(base, slice_)
    parents = [bn.variable(p).name() for p in bn.parents(n)]
    for p in parents:
      bn.eraseArc(p, n)
    bn.cpt(n).fillWith(gum.Tensor.deterministicTensor(bn.variable(n), value))

  lp = gum.LazyPropagation(bn)
  for base, slice_, value in evs:
    lp.addEvidence(_nodeName(base, slice_), value)

  out = {}
  for n in bn.nodes():
    out[bn.variable(n).name()] = _toVector(lp.posterior(n))
  return out, lp.evidenceProbability()


def _checkAgainstOracle(tc, ktbn, dos, targets, T, evs=None):
  if evs is None:
    evs = []
  ie = gum.KTBNInference(ktbn)
  for base, slice_, value in dos:
    ie.addIntervention(base, slice_, value)
  for base, slice_, value in evs:
    ie.addObservation(base, slice_, value)
  for t in targets:
    ie.addTarget(t)
  ie.makeInference(T)

  oracle, pe = _oracleMarginals(ktbn, dos, evs, T)
  wanted = lambda b: (not targets) or b in targets

  for b in ktbn.temporalVarNames():
    if wanted(b):
      for t in range(T):
        got = _toVector(ie.posterior(b, t))
        exp = oracle[_nodeName(b, t)]
        for x, y in zip(got, exp):
          tc.assertLess(abs(x - y), TOL)
  for b in ktbn.atemporalVarNames():
    if wanted(b):
      got = _toVector(ie.posterior(b, AT))
      exp = oracle[b]
      for x, y in zip(got, exp):
        tc.assertLess(abs(x - y), TOL)

  tc.assertLess(abs(ie.observationProbability() - pe), 1e-9)


class KTBNInferenceTestCase(pyAgrumTestCase):
  def testRingStressAllLags(self):
    gum.initRandom(1)
    m = gum.KTBN(3)
    m.addTemporal("P", 2)
    m.addArc("P", 0, "P", 1)
    m.addArc("P", 1, "P", 2)
    m.addArc("P", 0, "P", 2)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 9)
    _checkAgainstOracle(self, m, [("P", 4, 1)], [], 9)

  def testOrderOne(self):
    gum.initRandom(2)
    m = gum.KTBN(1)
    m.addTemporal("P", 3)
    m.addAtemporal("C", 2)
    m.addArc("C", AT, "P", 0)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 5)
    _checkAgainstOracle(self, m, [("C", AT, 1)], [], 5)

  def testIntraSliceArc(self):
    gum.initRandom(3)
    m = gum.KTBN(2)
    m.addTemporal("A", 2)
    m.addTemporal("Z", 2)
    m.addArc("A", 0, "A", 1)
    m.addArc("Z", 0, "Z", 1)
    m.addArc("Z", 1, "A", 1)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 8)

  def testDeepReachBack(self):
    gum.initRandom(4)
    m = gum.KTBN(4)
    m.addTemporal("X", 2)
    m.addTemporal("Y", 2)
    for s in range(1, 4):
      m.addArc("X", s - 1, "X", s)
      m.addArc("Y", s - 1, "Y", s)
    m.addArc("X", 0, "Y", 3)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 12)

  def testInterventionSemantics(self):
    gum.initRandom(6)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    base = gum.KTBNInference(m)
    base.makeInference(3)
    priorX0 = _toVector(base.posterior("X", 0))

    cut = gum.KTBNInference(m)
    cut.addIntervention("X", 1, 0)
    cut.makeInference(3)

    # upstream of the intervention is UNCHANGED (conditioning would move it)
    for x, y in zip(_toVector(cut.posterior("X", 0)), priorX0):
      self.assertLess(abs(x - y), TOL)
    # the intervened node is a point mass
    for x, y in zip(_toVector(cut.posterior("X", 1)), [1.0, 0.0]):
      self.assertLess(abs(x - y), TOL)

    _checkAgainstOracle(self, m, [("X", 1, 0)], [], 3)

  def testHorizonConsistency(self):
    gum.initRandom(7)
    m = gum.KTBN(3)
    m.addTemporal("X", 2)
    m.addAtemporal("C", 2)
    m.addArc("X", 0, "X", 1)
    m.addArc("X", 1, "X", 2)
    m.addArc("C", AT, "X", 2)
    m.generateCPTs()

    big = gum.KTBNInference(m)
    big.makeInference(8)
    ref = [_toVector(big.posterior("X", t)) for t in range(8)]

    for T in range(1, 9):
      ie = gum.KTBNInference(m)
      ie.makeInference(T)
      for x, y in zip(_toVector(ie.posterior("X", T - 1)), ref[T - 1]):
        self.assertLess(abs(x - y), TOL)

  def testTargetModes(self):
    gum.initRandom(8)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addTemporal("Y", 2)
    m.addArc("X", 0, "X", 1)
    m.addArc("Y", 0, "Y", 1)
    m.generateCPTs()

    ie = gum.KTBNInference(m)
    self.assertFalse(ie.isInTargetMode())

    ie.addTarget("X")
    self.assertTrue(ie.isInTargetMode())
    self.assertTrue(ie.isTarget("X"))
    self.assertFalse(ie.isTarget("Y"))

    ie.makeInference(4)
    with self.assertRaises(gum.UndefinedElement):
      ie.posterior("Y", 0)

    ie.eraseTarget("X")
    self.assertFalse(ie.isInTargetMode())

  def testExceptions(self):
    gum.initRandom(9)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addAtemporal("C", 2)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNInference(None)

    ie = gum.KTBNInference(m)
    with self.assertRaises(gum.InvalidArgument):
      ie.makeInference(0)
    with self.assertRaises(gum.NotFound):
      ie.addTarget("nope")
    with self.assertRaises(gum.OutOfBounds):
      ie.addIntervention("X", 0, 99)
    with self.assertRaises(gum.OperationNotAllowed):
      ie.posterior("X", 0)  # before run

    ie2 = gum.KTBNInference(m)
    ie2.makeInference(4)
    with self.assertRaises(gum.OutOfBounds):
      ie2.posterior("X", 4)  # slice >= horizon

  def testNameApiAndStaleness(self):
    gum.initRandom(10)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    ie = gum.KTBNInference(m)
    ie.makeInference(5)

    for x, y in zip(_toVector(ie.posterior("X", 2)), _toVector(ie.posterior("X[2]"))):
      self.assertLess(abs(x - y), TOL)

    s3 = _toVector(ie.posterior("X", 3))
    ie.makeInference(5)  # idempotence
    for x, y in zip(_toVector(ie.posterior("X", 3)), s3):
      self.assertLess(abs(x - y), TOL)

    ie.makeInference(9)  # grow horizon
    for x, y in zip(_toVector(ie.posterior("X", 3)), s3):
      self.assertLess(abs(x - y), TOL)
    ie.posterior("X", 8)  # must not throw

    ie.addIntervention("X", 1, 0)
    ie.posterior("X", 4)  # state change invalidates the cache; must not throw

  def testInterventionByLabelAndBatch(self):
    m = gum.KTBN(2)
    m.addTemporal(gum.LabelizedVariable("X", "", ["no", "yes"]))
    m.addTemporal(gum.LabelizedVariable("Z", "", ["off", "on"]))
    m.addAtemporal(gum.LabelizedVariable("C", "", ["low", "high"]))
    m.addArc("X", 0, "X", 1)
    m.addArc("Z", 0, "Z", 1)
    m.addArc("C", AT, "X", 1)
    gum.initRandom(5)
    m.generateCPTs()

    byIndex, byLabel = gum.KTBNInference(m), gum.KTBNInference(m)
    byIndex.addIntervention("X", 0, 1)
    byLabel.addIntervention("X", 0, "yes")
    byIndex.makeInference(4)
    byLabel.makeInference(4)
    for x, y in zip(_toVector(byLabel.posterior("X", 2)), _toVector(byIndex.posterior("X", 2))):
      self.assertLess(abs(x - y), TOL)

    oneByOne, batched = gum.KTBNInference(m), gum.KTBNInference(m)
    oneByOne.addIntervention("X", 0, 1)
    oneByOne.addIntervention("Z", 0, 0)
    oneByOne.addIntervention("C", AT, 1)
    batched.addIntervention({"X[0]": "yes", ("Z", 0): 0, "C": "high"})
    self.assertTrue(batched.hasIntervention("X", 0))
    self.assertTrue(batched.hasIntervention("Z", 0))
    self.assertTrue(batched.hasIntervention("C", AT))
    oneByOne.makeInference(4)
    batched.makeInference(4)
    for x, y in zip(_toVector(batched.posterior("X", 3)), _toVector(oneByOne.posterior("X", 3))):
      self.assertLess(abs(x - y), TOL)

    # within a batch, the last value for a repeated node wins
    repeated, last = gum.KTBNInference(m), gum.KTBNInference(m)
    repeated.addIntervention({"X[0]": 0})  # dict key uniqueness: last write wins
    repeated.addIntervention({"X[0]": "yes"})
    last.addIntervention("X", 0, 1)
    repeated.makeInference(3)
    last.makeInference(3)
    for x, y in zip(_toVector(repeated.posterior("X", 2)), _toVector(last.posterior("X", 2))):
      self.assertLess(abs(x - y), TOL)

    # a bad entry aborts the whole batch: nothing is recorded
    atomic = gum.KTBNInference(m)
    with self.assertRaises(gum.OutOfBounds):
      atomic.addIntervention({"X[0]": 0, "Z[0]": "nope"})
    self.assertFalse(atomic.hasIntervention("X", 0))
    self.assertFalse(atomic.hasIntervention("Z", 0))

    with self.assertRaises(gum.OutOfBounds):
      byLabel.addIntervention("X", 0, "maybe")
    with self.assertRaises(gum.NotFound):
      atomic.addIntervention({"nope": 0})

  def testWiderModel(self):
    gum.initRandom(11)
    m = gum.KTBN(3)
    m.addTemporal("A", 2)
    m.addTemporal("B", 2)
    m.addTemporal("C", 2)
    m.addAtemporal("W", 2)
    m.addArc("A", 0, "A", 1)
    m.addArc("A", 1, "A", 2)
    m.addArc("B", 0, "B", 1)
    m.addArc("B", 1, "B", 2)
    m.addArc("A", 0, "B", 1)
    m.addArc("B", 0, "C", 1)
    m.addArc("C", 0, "C", 1)
    m.addArc("C", 1, "C", 2)
    m.addArc("W", AT, "A", 0)
    m.addArc("W", AT, "C", 2)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 10)
    _checkAgainstOracle(self, m, [("B", 3, 0)], [], 10)

  # -- observations: an observation must revise the WHOLE network --

  def testObservationSmoothingBackwards(self):
    gum.initRandom(20)
    m = gum.KTBN(2)
    m.addTemporal("X", 3)
    m.addTemporal("Y", 2)
    m.addArc("X", 0, "X", 1)
    m.addArc("X", 1, "Y", 1)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 7, evs=[("Y", 6, 1)])
    _checkAgainstOracle(self, m, [], [], 7, evs=[("X", 0, 2), ("Y", 6, 0)])

  def testObservationRevisesAtemporal(self):
    gum.initRandom(21)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addAtemporal("C", 3)
    m.addArc("C", AT, "X", 0)
    m.addArc("C", AT, "X", 1)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [], [], 6, evs=[("X", 5, 1)])
    _checkAgainstOracle(self, m, [], [], 6, evs=[("X", 1, 0), ("X", 5, 1)])

  def testObservationAndInterventionTogether(self):
    gum.initRandom(22)
    m = gum.KTBN(3)
    m.addTemporal("A", 2)
    m.addTemporal("B", 3)
    m.addAtemporal("W", 2)
    m.addArc("A", 0, "A", 1)
    m.addArc("A", 1, "A", 2)
    m.addArc("A", 0, "B", 2)
    m.addArc("B", 1, "B", 2)
    m.addArc("W", AT, "A", 0)
    m.generateCPTs()
    _checkAgainstOracle(self, m, [("A", 4, 1)], [], 9, evs=[("B", 7, 2)])
    _checkAgainstOracle(self, m, [("B", 3, 0)], [], 9, evs=[("A", 8, 1), ("B", 6, 1)])

  def testSoftObservation(self):
    gum.initRandom(27)
    m = gum.KTBN(2)
    m.addTemporal("X", 3)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    soft = gum.KTBNInference(m)
    soft.addObservation("X", 3, [0.0, 1.0, 0.0])
    soft.makeInference(5)

    hard = gum.KTBNInference(m)
    hard.addObservation("X", 3, 1)
    hard.makeInference(5)

    for t in range(5):
      for x, y in zip(_toVector(soft.posterior("X", t)), _toVector(hard.posterior("X", t))):
        self.assertLess(abs(x - y), TOL)

    mid = gum.KTBNInference(m)
    mid.addObservation("X", 3, [0.5, 0.5, 0.0])
    mid.makeInference(5)
    p = _toVector(mid.posterior("X", 3))
    self.assertLess(p[2], 1e-12)
    self.assertLess(abs(p[0] + p[1] - 1.0), 1e-9)

  def testObservationApiContract(self):
    gum.initRandom(28)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addAtemporal("C", 2)
    m.addArc("X", 0, "X", 1)
    m.generateCPTs()

    ie = gum.KTBNInference(m)
    self.assertFalse(ie.hasObservation())
    ie.addObservation("X", 2, 1)
    self.assertTrue(ie.hasObservation())
    self.assertTrue(ie.hasObservation("X", 2))
    self.assertTrue(ie.hasObservation("X[2]"))
    self.assertFalse(ie.hasObservation("X", 3))

    ie.eraseObservation("X", 2)
    self.assertFalse(ie.hasObservation())

    ie.addObservation("X[1]", 0)
    self.assertTrue(ie.hasObservation("X", 1))
    ie.clearObservation()
    self.assertFalse(ie.hasObservation())

    ie.makeInference(4)
    self.assertLess(abs(ie.observationProbability() - 1.0), 1e-9)
    self.assertLess(abs(ie.logObservationProbability()), 1e-9)

    with self.assertRaises(gum.NotFound):
      ie.addObservation("nope", 0, 0)
    with self.assertRaises(gum.InvalidArgument):
      ie.addObservation("C", 0, 0)
    with self.assertRaises(gum.OutOfBounds):
      ie.addObservation("X", 1, 7)
    with self.assertRaises(gum.InvalidArgument):
      ie.addObservation("X", 1, [1.0])
    with self.assertRaises(gum.InvalidArgument):
      ie.addObservation("X", 1, [0.0, 0.0])
    with self.assertRaises(gum.InvalidArgument):
      ie.addObservation("X", 1, [-1.0, 2.0])

  def testImpossibleObservation(self):
    gum.initRandom(29)
    m = gum.KTBN(2)
    m.addTemporal("X", 2)
    m.addArc("X", 0, "X", 1)
    m.fillCPT("X", 0, {}, [1.0, 0.0])
    m.fillCPT("X", 1, {("X", 0): 0}, [1.0, 0.0])
    m.fillCPT("X", 1, {("X", 0): 1}, [0.0, 1.0])

    ie = gum.KTBNInference(m)
    ie.addObservation("X", 3, 1)  # impossible
    with self.assertRaises(gum.FatalError):
      ie.makeInference(5)

  def testWindowIntrospection(self):
    gum.initRandom(30)
    m = gum.KTBN(3)
    m.addTemporal("X", 2)
    m.addArc("X", 0, "X", 1)
    m.addArc("X", 0, "X", 2)
    m.generateCPTs()

    ie = gum.KTBNInference(m)
    iface = ie.interfaceSize()
    nbClq = ie.windowJunctionTree().size()
    self.assertEqual(iface, 2)
    self.assertGreater(nbClq, 0)
    ie.makeInference(5)
    ie.makeInference(50)
    self.assertEqual(ie.interfaceSize(), iface)
    self.assertEqual(ie.windowJunctionTree().size(), nbClq)


ts = unittest.TestSuite()
addTests(ts, KTBNInferenceTestCase)
