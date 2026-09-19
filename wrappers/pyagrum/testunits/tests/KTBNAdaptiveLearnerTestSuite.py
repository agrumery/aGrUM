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

import os
import tempfile
import unittest

import pyagrum.ktbn as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

AT = gum.KTBN.ATEMPORAL
NO_ATEMPORAL = set()


def _writeCSV(path, header, rows):
  with open(path, "w") as f:
    f.write(header + "\n")
    for r in rows:
      f.write(r + "\n")


def _schema(names):
  bn = gum.BayesNet()
  for n in names:
    bn.add(gum.LabelizedVariable(n, "", ["0", "1"]))
  return bn


class KTBNAdaptiveLearnerTestCase(pyAgrumTestCase):
  def setUp(self):
    self._dir = tempfile.mkdtemp()

  def _p(self, name):
    return os.path.join(self._dir, name)

  # single column "X" alternating (X[t+1] = 1 - X[t]) from a per-trajectory
  # start; one trajectory per (start, length) spec.
  def _writeAlt(self, base, specs):
    for i, (start, length) in enumerate(specs):
      rows = []
      x = start
      for _ in range(length):
        rows.append(str(x))
        x = 1 - x
      _writeCSV(self._p(f"{base}{i + 1}.csv"), "X", rows)
    return self._dir

  # single column "X" following a pure lag-2 rule: the period-4 pattern
  # S = 0,1,1,0 phase-shifted per trajectory. Only a k >= 3 model captures it.
  def _writeLag2(self, base, nbTraj, length):
    S = [0, 1, 1, 0]
    for i in range(nbTraj):
      rows = [str(S[(i + t) % 4]) for t in range(length)]
      _writeCSV(self._p(f"{base}{i + 1}.csv"), "X", rows)
    return self._dir

  # 8 trajectories over X,C,D of length 10 covering every (C, X-start)
  # combination twice; X alternates, D = C, X independent of C and D.
  def _writeSolo(self):
    for i in range(8):
      c, start = i % 2, (i // 2) % 2
      rows = []
      x = start
      for _ in range(10):
        rows.append(f"{x},{c},{c}")
        x = 1 - x
      _writeCSV(self._p(f"kad_solo{i + 1}.csv"), "X,C,D", rows)
    return self._dir

  def _alt(self, kMax=3):
    self._writeAlt("kad_alt", [(0, 8), (1, 8), (0, 8), (1, 8)])
    return gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 4, kMax)

  def _solo(self, kMax=3):
    self._writeSolo()
    return gum.KTBNAdaptiveLearner(self._dir, "kad_solo", 8, kMax, _schema(["X", "C", "D"]), {"C", "D"})

  # same files but nothing declared atemporal, so X, C and D are all temporal
  # bases: lets the per-slice / base-pair constraint setters address C and D too.
  def _multi(self, kMax=3):
    self._writeSolo()
    return gum.KTBNAdaptiveLearner(self._dir, "kad_solo", 8, kMax, NO_ATEMPORAL)

  # the deterministic learning baseline: BIC structure score + greedy hill
  # climbing, fixed seed (default algorithm is MIIC, non-deterministic).
  def _learn(self, l):
    l.useScoreBIC().useGreedyHillClimbing()
    gum.initRandom(42)
    return l.learnKTBN()

  def _stateValue(self, l, key):
    for k, v, _comment in l.state():
      if k == key:
        return v
    return ""

  def _checkTemporalValidity(self, m):
    for tail, head in m.arcs():
      if head[1] == AT:
        self.assertEqual(tail[1], AT)
      elif tail[1] != AT:
        self.assertLessEqual(tail[1], head[1])

  # ---- constructors ----

  def testConstructorErrors(self):
    self._writeAlt("kad_alt", [(0, 8), (1, 8), (0, 8), (1, 8)])
    bn = _schema(["X"])

    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 4, 1)  # kMax < 2
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 4, 1, bn)
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 0, 3)  # 0 trajectories
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 0, 3, bn)

    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 4, 3, {"GHOST"})
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 4, 3, bn, {"GHOST"})

    # explicit-atemporal constructor: a missing trajectory file is not seen
    # at construction (only trajectory 1's header is read), only at learnKTBN().
    missing = gum.KTBNAdaptiveLearner(self._dir, "kad_alt", 5, 3, NO_ATEMPORAL)
    missing.kMax()
    with self.assertRaises(gum.IOError):
      missing.learnKTBN()

    # inferring constructor, general case: a missing trajectory is normally
    # caught at construction, since testing constancy means opening every file.
    with self.assertRaises(gum.IOError):
      gum.KTBNAdaptiveLearner(self._writeSolo(), "kad_solo", 9, 2)

  def testInferAtemporalVars(self):
    _writeCSV(self._p("kad_infer1.csv"), "X,C", ["0,red", "1,red", "0,red", "1,red"])
    _writeCSV(self._p("kad_infer2.csv"), "X,C", ["1,red", "0,red", "1,red", "0,red"])
    _writeCSV(self._p("kad_infer3.csv"), "X,C", ["0,red", "0,red", "1,red", "1,red"])

    lexp = gum.KTBNAdaptiveLearner(self._dir, "kad_infer", 3, 2, NO_ATEMPORAL)
    mexp = self._learn(lexp)
    self.assertIn("C", mexp.temporalVarNames())
    self.assertNotIn("C", mexp.atemporalVarNames())

    linfer = gum.KTBNAdaptiveLearner(self._dir, "kad_infer", 3, 2)  # no atemporalVars -> infer
    minfer = self._learn(linfer)
    self.assertIn("C", minfer.atemporalVarNames())
    self.assertIn("X", minfer.temporalVarNames())
    self._checkTemporalValidity(minfer)

  def testBNSchemaConstructor(self):
    self._writeSolo()

    # CSV constructor: C and D are constant within trajectory 1, so a value
    # first appearing later is rejected while learning.
    csv = gum.KTBNAdaptiveLearner(self._dir, "kad_solo", 8, 2)
    with self.assertRaises(gum.UnknownLabelInDatabase):
      self._learn(csv)

    # BN-schema constructor: domains fixed up front, learning succeeds.
    bnl = self._solo(2)
    m = self._learn(bnl)
    self.assertEqual(m.atemporalVarNames(), frozenset({"C", "D"}))
    self._checkTemporalValidity(m)

  def testShortTrajectoryBoundary(self):
    self._writeAlt("kad_short", [(0, 5), (1, 2)])  # shortest length 2
    l = gum.KTBNAdaptiveLearner(self._dir, "kad_short", 2, 3)
    with self.assertRaises(gum.OperationNotAllowed):
      l.learnKTBN()  # fails at k = 3
    with self.assertRaises(gum.OperationNotAllowed):
      l.bestK()  # nothing selected

  # ---- validation ----

  def testValidationUnknownBase(self):
    l = self._solo()
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenArc("GHOST", 0, "X", 1)
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenArc("GHOST[0]", "X[1]")
    with self.assertRaises(gum.InvalidArgument):
      l.addMandatoryArc("X", 0, "GHOST", 1)
    with self.assertRaises(gum.InvalidArgument):
      l.addNoParentNode("GHOST", 0)
    with self.assertRaises(gum.InvalidArgument):
      l.addNoChildrenNode("GHOST[1]")
    with self.assertRaises(gum.InvalidArgument):
      l.addPossibleEdge("X", 0, "GHOST", 1)
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenIntraSliceArc("GHOST", "X")
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenArcAllSlices("X", "GHOST")

  def testValidationSliceRange(self):
    l = self._solo(3)  # candidate slices span 0..2

    l.addForbiddenArc("X", 2, "X", 2)  # kMax-1 fits
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenArc("X", 3, "X", 2)  # kMax is out of range

    l.addForbiddenArc("C", AT, "X", 0)  # atemporal: only ATEMPORAL is valid
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenArc("C", 1, "X", 0)

    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenIntraSliceArc("C", "X")
    with self.assertRaises(gum.InvalidArgument):
      l.eraseForbiddenIntraSliceArc("X", "C")

    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenArc("X", -2, "X", 1)
    with self.assertRaises(gum.InvalidArgument):
      l.addMandatoryArc("X", 0, "X", -2)
    with self.assertRaises(gum.InvalidArgument):
      l.addNoParentNode("X", -2)
    with self.assertRaises(gum.InvalidArgument):
      l.addPossibleEdge("X", -2, "X", 1)

    l.addForbiddenArcAllSlices("X", "X")  # ATEMPORAL passed via base-name-only setter

  # ---- configuration & constraints ----

  def testStructureLearningConfig(self):
    l = self._solo()
    l.useGreedyHillClimbing()

    self.assertEqual(self._stateValue(l, "Structure score"), "BDeu")  # default
    self.assertIs(l.useScoreAIC(), l)
    self.assertEqual(self._stateValue(l, "Structure score"), "AIC")
    l.useScoreBD()
    self.assertEqual(self._stateValue(l, "Structure score"), "BD")
    l.useScoreLog2Likelihood()
    self.assertEqual(self._stateValue(l, "Structure score"), "Log2Likelihood")
    l.useScoreMDL()
    self.assertEqual(self._stateValue(l, "Structure score"), "MDL")
    l.useScorefNML()  # void return, not chainable
    self.assertEqual(self._stateValue(l, "Structure score"), "fNML")

    l.useExtendedGreedyHillClimbing()
    self.assertEqual(self._stateValue(l, "Algorithm"), "Extended Greedy Hill Climbing")
    l.useLocalSearchWithTabuList(50, 3)
    self.assertEqual(self._stateValue(l, "Algorithm"), "Local Search with Tabu List")
    self.assertEqual(self._stateValue(l, "Tabu list size"), str(50))
    self.assertEqual(self._stateValue(l, "Tabu nb decrease"), str(3))

    l.useMIIC().useNMLCorrection()
    self.assertEqual(self._stateValue(l, "Correction"), "NML")
    self.assertEqual(self._stateValue(l, "Structure score"), "")  # no score under MIIC
    l.useNoCorrection()
    self.assertEqual(self._stateValue(l, "Correction"), "No correction")

    l.useSmoothingPrior(0.5)
    self.assertEqual(self._stateValue(l, "Prior"), "Smoothing")
    self.assertEqual(self._stateValue(l, "Prior weight"), "0.500000")  # state() renders via std::to_string(double)

    l.allowArcAdditions(False).allowArcDeletions(False).allowArcReversals(False).setMaxIndegree(2)
    self.assertEqual(self._stateValue(l, "Arc additions"), "forbidden")
    self.assertEqual(self._stateValue(l, "Arc deletions"), "forbidden")
    self.assertEqual(self._stateValue(l, "Arc reversals"), "forbidden")
    self.assertEqual(self._stateValue(l, "Max in-degree"), str(2))

    s = l.toString()
    self.assertIn("Candidate orders", s)
    self.assertIn("Max in-degree", s)

    l2 = self._solo(4)
    l2.useMIIC().useSmoothingPrior(1.0)
    self.assertEqual(l2.checkScorePriorCompatibility(), "")
    l2.useGreedyHillClimbing().useScoreBDeu().useSmoothingPrior(1.0)
    self.assertNotEqual(l2.checkScorePriorCompatibility(), "")
    l2.useScoreBIC()
    self.assertEqual(l2.checkScorePriorCompatibility(), "")

  def testConstraintRecording(self):
    l = self._multi()  # X, C, D all temporal

    l.addMandatoryArc("X", 0, "X", 1)
    self.assertEqual(self._stateValue(l, "Mandatory arcs"), "{X[0]->X[1]}")
    l.eraseMandatoryArc("X", 0, "X", 1)
    self.assertEqual(self._stateValue(l, "Mandatory arcs"), "")
    l.eraseMandatoryArc("X", 0, "X", 1)  # silent no-op

    l.addForbiddenArc("X", 0, "C", 1)
    self.assertEqual(self._stateValue(l, "Forbidden arcs"), "{X[0]->C[1]}")
    l.eraseForbiddenArc("X", 0, "C", 1)
    self.assertEqual(self._stateValue(l, "Forbidden arcs"), "")

    l.addNoParentNode("X", 1)
    l.addNoChildrenNode("C", 0)
    self.assertEqual(self._stateValue(l, "No-parent nodes"), "{X[1]}")
    self.assertEqual(self._stateValue(l, "No-children nodes"), "{C[0]}")
    l.eraseNoParentNode("X", 1)
    l.eraseNoChildrenNode("C", 0)
    self.assertEqual(self._stateValue(l, "No-parent nodes"), "")
    self.assertEqual(self._stateValue(l, "No-children nodes"), "")

    # possible edges are undirected: (A,B) and (B,A) are the same record
    l.addPossibleEdge("X[0]", "X[1]")
    self.assertIn("X[0]->X[1]", self._stateValue(l, "Possible edges"))
    l.addPossibleEdge("X[1]", "X[0]")  # reverse orientation: still one record
    e = self._stateValue(l, "Possible edges")
    self.assertNotIn("X[1]->X[0]", e)
    self.assertEqual(e, "{X[0]->X[1]}")
    l.erasePossibleEdge("X[1]", "X[0]")  # erase via the reverse orientation
    self.assertEqual(self._stateValue(l, "Possible edges"), "")

    l.addForbiddenIntraSliceArc("X", "C")
    l.addForbiddenArcAllSlices("X", "D")
    self.assertEqual(self._stateValue(l, "Forbidden intra-slice arcs"), "{X->C}")
    self.assertEqual(self._stateValue(l, "Forbidden all-slices arcs"), "{X->D}")
    l.eraseForbiddenIntraSliceArc("X", "C")
    l.eraseForbiddenArcAllSlices("X", "D")
    self.assertEqual(self._stateValue(l, "Forbidden intra-slice arcs"), "")
    self.assertEqual(self._stateValue(l, "Forbidden all-slices arcs"), "")

  def testAccessorsBeforeLearnKTBN(self):
    l = self._alt()
    with self.assertRaises(gum.OperationNotAllowed):
      l.bestK()
    with self.assertRaises(gum.OperationNotAllowed):
      l.latentVariables()
    with self.assertRaises(gum.OperationNotAllowed):
      l.scorePerCandidateK()
    self.assertEqual(self._stateValue(l, "Selected k"), "not learned yet")

    self._learn(l)
    l.bestK()
    l.scorePerCandidateK()
    self.assertEqual(self._stateValue(l, "Selected k"), str(l.bestK()))

  def testDiagnostics(self):
    l = self._solo(4)
    self.assertEqual(l.kMax(), 4)
    self.assertEqual(self._stateValue(l, "Candidate orders"), "2..4")

  # ---- order-score selection ----

  def testOrderScoreSelection(self):
    l = self._solo()
    self.assertIs(l.useOrderScoreAIC(), l)
    self.assertEqual(self._stateValue(l, "Order selection score"), "AIC")
    self.assertIs(l.useOrderScorefNML(), l)
    self.assertEqual(self._stateValue(l, "Order selection score"), "fNML")
    self.assertIs(l.useOrderScoreBIC(), l)
    self.assertEqual(self._stateValue(l, "Order selection score"), "BIC")

    # kMax == 2: degenerate single-candidate loop
    l2 = self._alt(2)
    m2 = self._learn(l2)
    self.assertEqual(l2.bestK(), 2)
    self.assertEqual(m2.k(), 2)
    self.assertEqual(len(l2.scorePerCandidateK()), 1)
    self.assertEqual(l2.scorePerCandidateK()[0][0], 2)

    # scorePerCandidateK coherent with bestK: ascending k, argmax = bestK
    l4 = self._alt(4)
    self._learn(l4)
    sc = l4.scorePerCandidateK()
    self.assertEqual(len(sc), 3)  # k = 2, 3, 4
    argmaxK, best = 0, 0.0
    for i, (k, score) in enumerate(sc):
      self.assertEqual(k, i + 2)  # strictly ascending 2,3,4
      if argmaxK == 0 or score > best:
        best, argmaxK = score, k
    self.assertEqual(argmaxK, l4.bestK())

    # BIC trades off fit vs complexity: lag-1 picks k=2, lag-2 needs k=3
    lAlt = self._alt(3)
    self._learn(lAlt)
    self.assertEqual(lAlt.bestK(), 2)  # extra order would only add parameters

    self._writeLag2("kad_lag2", 4, 16)
    lLag2 = gum.KTBNAdaptiveLearner(self._dir, "kad_lag2", 4, 3)
    self._learn(lLag2)
    self.assertEqual(lLag2.bestK(), 3)  # lag-2 signal needs the wider window

    # a second learnKTBN() reproduces the first, doesn't accumulate
    l = self._alt(4)
    self._learn(l)
    k1 = l.bestK()
    s1 = l.scorePerCandidateK()
    self._learn(l)
    self.assertEqual(l.bestK(), k1)
    self.assertEqual(len(l.scorePerCandidateK()), len(s1))  # cleared, not appended
    self.assertAlmostEqual(l.scorePerCandidateK()[0][1], s1[0][1])

  def testConstraintReplay(self):
    # the strongest arc in the alternating data is X[0]->X[1]: unconstrained
    # it is learned, so forbidding it must remove it -- proving replay.
    base = self._alt(2)
    self.assertTrue(self._learn(base).existsArc("X", 0, "X", 1))

    l = self._alt(2)
    l.addForbiddenArc("X", 0, "X", 1)
    m = self._learn(l)
    self.assertFalse(m.existsArc("X", 0, "X", 1))

    # a constraint naming a slice no small candidate can host raises kMin, so
    # that candidate is skipped instead of learned with the constraint dropped.
    l3 = self._alt(3)
    l3.addForbiddenArc("X", 2, "X", 2)  # vacuous arc, but names slice 2
    self._learn(l3)
    self.assertEqual(len(l3.scorePerCandidateK()), 1)
    self.assertEqual(l3.scorePerCandidateK()[0][0], 3)

  def testKernelArcConstraints(self):
    l = self._solo()  # kMax=3, X temporal, C/D atemporal
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenKernelArc("GHOST", 0, "X")
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenKernelArc("X", -1, "X")
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenKernelArc("X", 3, "X")  # lag == kMax
    l.addForbiddenKernelArc("X", 2, "X")  # lag == kMax-1
    l.eraseForbiddenKernelArc("X", 2, "X")
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenKernelArc("C", 0, "X")  # atemporal endpoint
    with self.assertRaises(gum.InvalidArgument):
      l.addMandatoryKernelArc("X", 0, "C")

    l.addForbiddenKernelArc("X", 2, "X")
    self.assertEqual(self._stateValue(l, "Forbidden kernel arcs"), "{X->X (lag 2)}")
    l.addMandatoryKernelArc("X", 1, "X")
    self.assertEqual(self._stateValue(l, "Mandatory kernel arcs"), "{X->X (lag 1)}")
    l.eraseForbiddenKernelArc("X", 2, "X")
    l.eraseMandatoryKernelArc("X", 1, "X")
    self.assertEqual(self._stateValue(l, "Forbidden kernel arcs"), "")
    self.assertEqual(self._stateValue(l, "Mandatory kernel arcs"), "")
    l.eraseMandatoryKernelArc("X", 1, "X")  # silent no-op

    l5 = self._alt(5)
    l5.addForbiddenKernelArc("X", 2, "X")  # lag=2 -> kMin=3
    self.assertEqual(self._stateValue(l5, "Candidate orders"), "3..5")
    l5.eraseForbiddenKernelArc("X", 2, "X")
    self.assertEqual(self._stateValue(l5, "Candidate orders"), "2..5")

    # end-to-end replay: kMax=2 pins the kernel slice to 1, so lag=1 means
    # tailSlice=0 -- must match addForbiddenArc("X",0,"X",1)'s effect.
    forb = self._alt(2)
    forb.addForbiddenKernelArc("X", 1, "X")
    self.assertFalse(self._learn(forb).existsArc("X", 0, "X", 1))

    mand = self._alt(2)
    mand.addMandatoryKernelArc("X", 1, "X")
    self.assertTrue(self._learn(mand).existsArc("X", 0, "X", 1))

  def testLatentVariablesAccessor(self):
    l = self._solo()
    self._learn(l)  # greedy hill climbing
    self.assertEqual(l.latentVariables(), ())

    lm = self._solo()
    lm.useMIIC().useMDLCorrection()
    gum.initRandom(42)
    lm.learnKTBN()
    lm.latentVariables()  # must not throw


ts = unittest.TestSuite()
addTests(ts, KTBNAdaptiveLearnerTestCase)
