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


class KTBNLearnerTestCase(pyAgrumTestCase):
  def setUp(self):
    self._dir = tempfile.mkdtemp()

  def _p(self, name):
    return os.path.join(self._dir, name)

  def _writeStd(self):
    # 4 trajectories over X,Y,C,D, lengths {6,5,7,6}. X alternates from
    # starts {0,1,0,1}, Y always equals X, D always equals C, C is {0,0,1,1}.
    lens = [6, 5, 7, 6]
    starts = [0, 1, 0, 1]
    cs = [0, 0, 1, 1]
    for i in range(4):
      rows = []
      x = starts[i]
      for _ in range(lens[i]):
        rows.append(f"{x},{x},{cs[i]},{cs[i]}")
        x = 1 - x
      _writeCSV(self._p(f"ktbnl_std{i + 1}.csv"), "X,Y,C,D", rows)
    return self._dir

  def _writeSolo(self):
    # 8 trajectories over X,C,D of length 10 covering every (C, X-start)
    # combination twice; X is independent of C,D.
    for i in range(8):
      c, start = i % 2, (i // 2) % 2
      rows = []
      x = start
      for _ in range(10):
        rows.append(f"{x},{c},{c}")
        x = 1 - x
      _writeCSV(self._p(f"ktbnl_solo{i + 1}.csv"), "X,C,D", rows)
    return self._dir

  def _std(self, k=2, atemporal=None):
    if atemporal is None:
      atemporal = {"C", "D"}
    self._writeStd()
    return gum.KTBNLearner(self._dir, "ktbnl_std", 4, k, _schema(["X", "Y", "C", "D"]), atemporal)

  def _solo(self):
    self._writeSolo()
    return gum.KTBNLearner(self._dir, "ktbnl_solo", 8, 2, _schema(["X", "C", "D"]), {"C", "D"})

  def _structure(self, arcs, atemporal=None):
    if atemporal is None:
      atemporal = {"C", "D"}
    s = gum.KTBN(2)
    for name in ("X", "Y", "C", "D"):
      s.add(gum.LabelizedVariable(name, "", ["0", "1"]), name not in atemporal)
    for tail, head in arcs:
      s.addArc(tail[0], tail[1], head[0], head[1])
    return s

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

  def _hasCDedge(self, m):
    return m.existsArc("C", AT, "D", AT) or m.existsArc("D", AT, "C", AT)

  # ---- constructors ----

  def testCSVConstructor(self):
    _writeCSV(self._p("ktbnl_csv1.csv"), "X,Y", ["0,1", "1,0", "0,1", "1,0", "0,1"])
    _writeCSV(self._p("ktbnl_csv2.csv"), "X,Y", ["1,0", "0,1", "1,0", "0,1"])
    _writeCSV(self._p("ktbnl_csv3.csv"), "X,Y", ["0,1", "1,0", "0,1", "1,0", "0,1", "1,0"])

    l = gum.KTBNLearner(self._dir, "ktbnl_csv", 3, 2)
    self.assertEqual(l.k(), 2)
    self.assertEqual(l.nbCols(), 2)
    self.assertEqual(l.nbSamples(), 3)
    self.assertEqual(tuple(l.nbRows()), (5, 4, 6))
    self.assertEqual(tuple(l.names()), ("X", "Y"))
    self.assertEqual(l.domainSize("X"), 2)

    self._checkTemporalValidity(self._learn(l))

    lraw = gum.KTBNLearner(self._dir, "ktbnl_csv", 3, 2, NO_ATEMPORAL, ["?"], False)
    self.assertEqual(lraw.domainSize("X"), 2)

    # LIMITATION (documented on purpose): domains come from trajectory 1 only
    _writeCSV(self._p("ktbnl_dom1.csv"), "X,Y", ["0,1", "1,0", "0,1"])
    _writeCSV(self._p("ktbnl_dom2.csv"), "X,Y", ["0,2", "1,0", "2,1"])
    with self.assertRaises(Exception):
      gum.KTBNLearner(self._dir, "ktbnl_dom", 2, 2)

  def testInferAtemporalVars(self):
    _writeCSV(self._p("ktbnl_infer1.csv"), "X,C", ["0,red", "1,red", "0,red", "1,red"])
    _writeCSV(self._p("ktbnl_infer2.csv"), "X,C", ["1,red", "0,red", "1,red", "0,red"])
    _writeCSV(self._p("ktbnl_infer3.csv"), "X,C", ["0,red", "0,red", "1,red", "1,red"])

    l = gum.KTBNLearner(self._dir, "ktbnl_infer", 3, 2)  # no atemporalVars -> infer
    m = self._learn(l)
    self.assertIn("C", m.atemporalVarNames())
    self.assertIn("X", m.temporalVarNames())
    self._checkTemporalValidity(m)

    # empty set forces C to stay temporal, where omitting it inferred atemporal
    lexp = gum.KTBNLearner(self._dir, "ktbnl_infer", 3, 2, NO_ATEMPORAL)
    mexp = self._learn(lexp)
    self.assertIn("C", mexp.temporalVarNames())
    self.assertNotIn("C", mexp.atemporalVarNames())

    _writeCSV(self._p("ktbnl_inferdom1.csv"), "X,C", ["0,red", "1,red", "0,red"])
    _writeCSV(self._p("ktbnl_inferdom2.csv"), "X,C", ["0,blue", "1,blue", "0,blue"])
    with self.assertRaises(gum.UnknownLabelInDatabase):
      gum.KTBNLearner(self._dir, "ktbnl_inferdom", 2, 2)

  def testConstructorErrors(self):
    self._writeStd()
    bn = _schema(["X", "Y", "C", "D"])

    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNLearner(self._dir, "ktbnl_std", 4, 1)  # k < 2
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNLearner(self._dir, "ktbnl_std", 4, 1, bn)
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNLearner(self._dir, "ktbnl_std", 0, 2)  # 0 trajectories
    with self.assertRaises(gum.IOError):
      gum.KTBNLearner(self._dir, "ktbnl_std", 5, 2, bn)  # trajectory 5 absent

    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNLearner(self._dir, "ktbnl_std", 4, 2, {"GHOST"})
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNLearner(self._dir, "ktbnl_std", 4, 2, bn, {"GHOST"})

    ghost = _schema(["X", "Y", "C", "D", "GHOST"])
    with self.assertRaises(gum.MissingVariableInDatabase):
      gum.KTBNLearner(self._dir, "ktbnl_std", 4, 2, ghost, {"C", "D"})

    _writeCSV(self._p("ktbnl_badhdr1.csv"), "X,Y", ["0,1", "1,0"])
    _writeCSV(self._p("ktbnl_badhdr2.csv"), "Y,X", ["0,1", "1,0"])
    with self.assertRaises(gum.InvalidArgument):
      gum.KTBNLearner(self._dir, "ktbnl_badhdr", 2, 2)

    _writeCSV(self._p("ktbnl_tiny1.csv"), "X,Y", ["0,1", "1,0"])
    with self.assertRaises(gum.OperationNotAllowed):
      gum.KTBNLearner(self._dir, "ktbnl_tiny", 1, 3)  # trajectory shorter than k

  def testShapeAccessors(self):
    l = self._std()
    self.assertEqual(l.k(), 2)
    self.assertEqual(l.nbCols(), 4)
    self.assertEqual(l.nbSamples(), 4)
    self.assertEqual(tuple(l.nbRows()), (6, 5, 7, 6))
    self.assertEqual(tuple(l.names()), ("X", "Y", "C", "D"))
    self.assertEqual(tuple(l.domainSizes()), (2, 2, 2, 2))
    self.assertEqual(l.domainSize("X"), 2)
    self.assertEqual(l.domainSize("C"), 2)
    with self.assertRaises(gum.MissingVariableInDatabase):
      l.domainSize("GHOST")
    self.assertFalse(l.hasMissingValues())

    l3 = self._std(3)
    self.assertEqual(l3.k(), 3)
    self.assertEqual(l3.nbCols(), 4)

  def testMissingValues(self):
    _writeCSV(self._p("ktbnl_miss1.csv"), "X,Y", ["0,1", "1,0", "0,1"])
    _writeCSV(self._p("ktbnl_miss2.csv"), "X,Y", ["0,1", "?,0", "1,1"])

    lok = gum.KTBNLearner(self._dir, "ktbnl_miss", 1, 2)
    self.assertFalse(lok.hasMissingValues())

    l = gum.KTBNLearner(self._dir, "ktbnl_miss", 2, 2)
    self.assertTrue(l.hasMissingValues())
    self.assertEqual(l.nbDroppedRows(), 0)

    ldrop = gum.KTBNLearner(self._dir, "ktbnl_miss", 2, 2, NO_ATEMPORAL, ["?"], True, True)
    self.assertFalse(ldrop.hasMissingValues())
    self.assertGreater(ldrop.nbDroppedRows(), 0)
    self.assertTrue(ldrop.isIgnoringMissingSymbols())

  def testMissingValuesDoNotBlockLearning(self):
    for s in range(1, 5):
      x = s % 2
      rows = []
      for t in range(12):
        rows.append("?" if t in (3, 7) else str(x))
        x = 1 - x
      _writeCSV(self._p(f"ktbnl_gap{s}.csv"), "X", rows)

    refuse = gum.KTBNLearner(self._dir, "ktbnl_gap", 4, 2)
    refuse.useScoreBIC().useGreedyHillClimbing().useSmoothingPrior(1.0)
    with self.assertRaises(gum.MissingValueInDatabase):
      refuse.learnKTBN()

    l = gum.KTBNLearner(self._dir, "ktbnl_gap", 4, 2, NO_ATEMPORAL, ["?"], True, True)
    l.useScoreBIC().useGreedyHillClimbing().useSmoothingPrior(1.0)
    gum.initRandom(1)
    l.learnKTBN()
    self.assertGreater(l.nbDroppedRows(), 0)
    self.assertFalse(l.hasMissingValues())

  def testExactStructureRecovery(self):
    l = self._solo()
    m = self._learn(l)

    self.assertEqual(m.k(), 2)
    self.assertEqual(m.temporalVarNames(), frozenset({"X"}))
    self.assertEqual(m.atemporalVarNames(), frozenset({"C", "D"}))
    self._checkTemporalValidity(m)

    self.assertTrue(m.existsArc("X", 0, "X", 1))
    self.assertTrue(self._hasCDedge(m))
    self.assertEqual(m.sizeArcs(), 2)

    m2 = self._learn(l)  # relearning from the same learner is stable
    self.assertTrue(m2.existsArc("X", 0, "X", 1))
    self.assertEqual(m2.sizeArcs(), 2)

  def testWithoutAtemporalLearner(self):
    mono = self._std(2, {"C"})
    self.assertNotIn("Atemporal learner", mono.toString())
    mono.addNoParentNode("C")  # silent no-op
    mono.eraseNoParentNode("C")

    m = self._learn(mono)
    self.assertEqual(m.atemporalVarNames(), frozenset({"C"}))
    self.assertEqual(m.parents("C", AT), ())
    self._checkTemporalValidity(m)

    none = self._std(2, set())
    self.assertNotIn("Atemporal learner", none.toString())
    self.assertEqual(none.nbCols(), 4)
    m0 = self._learn(none)
    self.assertEqual(m0.nbAtemporalVars(), 0)
    self._checkTemporalValidity(m0)

  def testLearnParameters(self):
    l = self._std()
    s = self._structure(
      [
        (("C", AT), ("D", AT)),
        (("C", AT), ("X", 0)),
        (("X", 0), ("Y", 0)),
        (("X", 0), ("X", 1)),
        (("X", 1), ("Y", 1)),
        (("C", AT), ("Y", 1)),
      ]
    )

    m = l.learnParameters(s, False)  # exact ML, no score prior

    self.assertEqual(m.sizeArcs(), 6)
    for tail, head in s.arcs():
      self.assertTrue(m.existsArc(tail[0], tail[1], head[0], head[1]))

    md = l.learnParameters(s)  # score-aware variant also succeeds
    self.assertEqual(md.sizeArcs(), 6)
    self.assertLess(abs(md.cpt("X", 1).sum() - 2.0), 1e-9)

  def testLearnParametersSmoothing(self):
    l = self._std()
    self.assertIs(l.useSmoothingPrior(1.0), l)
    self.assertEqual(self._stateValue(l, "Prior"), "Smoothing")

    m = l.learnParameters(self._structure([(("X", 0), ("X", 1))]), False)
    p = m.cpt("X", 1)
    inst = gum.Instantiation(p)
    inst.setFirst()
    while not inst.end():
      flip = inst.val(p.variable("X[1]")) != inst.val(p.variable("X[0]"))
      self.assertLess(abs(p[inst] - (11.0 / 12.0 if flip else 1.0 / 12.0)), 1e-9)
      inst.inc()

  def testScoreAndPriorSelection(self):
    l = self._std()
    l.useGreedyHillClimbing()

    self.assertIs(l.useScoreAIC(), l)
    self.assertEqual(self._stateValue(l, "Score"), "AIC")
    self.assertEqual(self._stateValue(l.useScoreBD(), "Score"), "BD")
    self.assertEqual(self._stateValue(l.useScoreBDeu(), "Score"), "BDeu")
    self.assertEqual(self._stateValue(l.useScoreBIC(), "Score"), "BIC")
    self.assertEqual(self._stateValue(l.useScoreLog2Likelihood(), "Score"), "Log2Likelihood")
    self.assertEqual(self._stateValue(l.useScoreMDL(), "Score"), "MDL")
    l.useScorefNML()
    self.assertEqual(self._stateValue(l, "Score"), "fNML")

    l.useScoreBIC()
    self.assertEqual(l.checkScorePriorCompatibility(), "")
    l.useScoreBDeu().useSmoothingPrior(1.0)
    self.assertNotEqual(l.checkScorePriorCompatibility(), "")

  def testAlgorithmSelection(self):
    l = self._std()

    self.assertIs(l.useGreedyHillClimbing(), l)
    self.assertEqual(self._stateValue(l, "Algorithm"), "Greedy Hill Climbing")
    self.assertTrue(l.isScoreBased())
    self.assertFalse(l.isConstraintBased())

    l.useExtendedGreedyHillClimbing()
    self.assertEqual(self._stateValue(l, "Algorithm"), "Extended Greedy Hill Climbing")

    l.useLocalSearchWithTabuList(50, 3)
    self.assertEqual(self._stateValue(l, "Algorithm"), "Local Search with Tabu List")

    l.useMIIC()
    self.assertEqual(self._stateValue(l, "Algorithm"), "MIIC")
    self.assertTrue(l.isConstraintBased())
    self.assertFalse(l.isScoreBased())

  def testMIICCorrectionsAndLatentVariables(self):
    l = self._solo()

    l.useGreedyHillClimbing()
    self.assertEqual(l.latentVariables(), ())

    l.useMIIC()
    self.assertEqual(self._stateValue(l.useMDLCorrection(), "Correction"), "MDL")
    self.assertEqual(self._stateValue(l.useNMLCorrection(), "Correction"), "NML")
    self.assertEqual(self._stateValue(l.useNoCorrection(), "Correction"), "No correction")

    l.useMDLCorrection()
    gum.initRandom(42)
    self._checkTemporalValidity(l.learnKTBN())
    l.latentVariables()  # must not throw

  def testForbiddenArcs(self):
    l = self._solo()

    l.addForbiddenArc("X", 0, "X", 1)
    self.assertFalse(self._learn(l).existsArc("X", 0, "X", 1))

    self.assertIs(l.eraseForbiddenArc("X[0]", "X[1]"), l)
    self.assertTrue(self._learn(l).existsArc("X", 0, "X", 1))

    l.addForbiddenArc("X", 1, "X", 0)  # backward arc: accepted no-op
    self.assertTrue(self._learn(l).existsArc("X", 0, "X", 1))

    l.addForbiddenArc("C", AT, "D", AT).addForbiddenArc("D", AT, "C", AT)
    m = self._learn(l)
    self.assertFalse(m.existsArc("C", AT, "D", AT))
    self.assertFalse(m.existsArc("D", AT, "C", AT))
    l.eraseForbiddenArc("C", AT, "D", AT).eraseForbiddenArc("D", AT, "C", AT)
    self.assertTrue(self._hasCDedge(self._learn(l)))

    with self.assertRaises(gum.InvalidArgument):
      l.eraseForbiddenArc("X", 0, "C", AT)  # temporal -> atemporal
    with self.assertRaises(gum.InvalidArgument):
      l.eraseForbiddenArc("X", 1, "X", 0)  # backward in time

  def testMandatoryArcs(self):
    l = self._solo()

    l.addMandatoryArc("C", AT, "X", 1)
    l.addMandatoryArc("C", AT, "X", 0)
    l.addMandatoryArc("D", AT, "C", AT)
    m = self._learn(l)
    self.assertTrue(m.existsArc("C", AT, "X", 1))
    self.assertTrue(m.existsArc("C", AT, "X", 0))
    self.assertTrue(m.existsArc("D", AT, "C", AT))

    l.eraseMandatoryArc("C", AT, "X", 1)
    l.eraseMandatoryArc("C", "X[0]")
    l.eraseMandatoryArc("D", AT, "C", AT)
    m = self._learn(l)
    self.assertFalse(m.existsArc("C", AT, "X", 1))
    self.assertFalse(m.existsArc("C", AT, "X", 0))

    with self.assertRaises(gum.InvalidArgument):
      l.addMandatoryArc("X", 0, "C", AT)
    with self.assertRaises(gum.InvalidArgument):
      l.addMandatoryArc("X", 1, "X", 0)
    l.eraseMandatoryArc("X", 1, "X", 0)  # silent no-op

    l3 = self._std(3)
    l3.addMandatoryArc("C", AT, "X", 1)
    self.assertTrue(self._learn(l3).existsArc("C", AT, "X", 1))

  def testIntraSliceArcs(self):
    l = self._std()
    l.addForbiddenArc("X", 0, "Y", 1).addForbiddenArc("Y", 0, "Y", 1)
    l.addForbiddenIntraSliceArc("X", "Y").addForbiddenIntraSliceArc("Y", "X")
    m = self._learn(l)
    for t in range(2):
      self.assertFalse(m.existsArc("X", t, "Y", t))
      self.assertFalse(m.existsArc("Y", t, "X", t))

    l.eraseForbiddenIntraSliceArc("X", "Y")
    self.assertTrue(self._learn(l).existsArc("X", 1, "Y", 1))

  def testIntraSliceArcsRejectNonTemporalBase(self):
    l = self._std()  # C, D atemporal
    before = l.toString()

    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenIntraSliceArc("C", "X")
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenIntraSliceArc("X", "C")
    with self.assertRaises(gum.InvalidArgument):
      l.eraseForbiddenIntraSliceArc("C", "X")
    with self.assertRaises(gum.InvalidArgument):
      l.addForbiddenIntraSliceArc("GHOST", "X")

    self.assertEqual(l.toString(), before)

  def testForbiddenArcAllSlices(self):
    def hasXYarc(m, k):
      for ts in range(k):
        for hs in range(ts, k):
          if m.existsArc("X", ts, "Y", hs):
            return True
      return False

    l3 = self._std(3)
    l3.addForbiddenArcAllSlices("X", "Y")
    self.assertFalse(hasXYarc(self._learn(l3), 3))

    l3.eraseForbiddenArcAllSlices("X", "Y")
    l3.addMandatoryArc("X", 0, "Y", 1)
    self.assertTrue(self._learn(l3).existsArc("X", 0, "Y", 1))

    l2 = self._std()
    l2.addForbiddenArcAllSlices("C", "X")
    mForbidden = self._learn(l2)
    self.assertFalse(mForbidden.existsArc("C", AT, "X", 0))
    self.assertFalse(mForbidden.existsArc("C", AT, "X", 1))

    l2.eraseForbiddenArcAllSlices("C", "X")
    l2.addMandatoryArc("C", AT, "X", 0)
    self.assertTrue(self._learn(l2).existsArc("C", AT, "X", 0))

    l4 = self._solo()
    l4.addForbiddenArcAllSlices("X", "C")  # already impossible: harmless no-op
    l4.eraseForbiddenArcAllSlices("X", "C")

    l4.addForbiddenArcAllSlices("C", "D").addForbiddenArcAllSlices("D", "C")
    mAt = self._learn(l4)
    self.assertFalse(mAt.existsArc("C", AT, "D", AT))
    self.assertFalse(mAt.existsArc("D", AT, "C", AT))

  def testNoParentNodes(self):
    l = self._solo()

    l.addNoParentNode("X", 1)
    m = self._learn(l)
    self.assertEqual(m.parents("X", 1), ())
    self.assertTrue(self._hasCDedge(m))

    l.eraseNoParentNode("X[1]")
    self.assertTrue(self._learn(l).existsArc("X", 0, "X", 1))

    l.addNoParentNode("D")
    m = self._learn(l)
    self.assertEqual(m.parents("D", AT), ())
    self.assertTrue(m.existsArc("D", AT, "C", AT))

    l.eraseNoParentNode("D", AT)
    self.assertTrue(self._hasCDedge(self._learn(l)))

  def testNoChildrenNodes(self):
    l = self._solo()

    l.addNoChildrenNode("X", 0)
    m = self._learn(l)
    self.assertEqual(m.children("X", 0), ())

    l.eraseNoChildrenNode("X[0]")
    self.assertTrue(self._learn(l).existsArc("X", 0, "X", 1))

    l.addNoChildrenNode("C")
    m = self._learn(l)
    self.assertEqual(m.children("C", AT), ())
    self.assertTrue(m.existsArc("D", AT, "C", AT))

    l.eraseNoChildrenNode("C", AT)
    self.assertTrue(self._hasCDedge(self._learn(l)))

  def testPossibleEdges(self):
    l = self._solo()

    l.addPossibleEdge("C", AT, "X", 1)
    m = self._learn(l)
    self.assertFalse(m.existsArc("X", 0, "X", 1))
    self.assertFalse(m.existsArc("C", AT, "D", AT))
    self.assertFalse(m.existsArc("D", AT, "C", AT))

    l.erasePossibleEdge("C", AT, "X", 1)
    self.assertTrue(self._learn(l).existsArc("X", 0, "X", 1))

    l.addPossibleEdge("C", "D")
    m = self._learn(l)
    self.assertFalse(m.existsArc("X", 0, "X", 1))
    self.assertTrue(self._hasCDedge(m))
    self.assertIs(l.erasePossibleEdge("C", "D"), l)

  def testDiagnostics(self):
    l = self._std()

    s = l.toString()
    self.assertIn("2 temporal, 2 atemporal", s)
    self.assertIn("Transition learner", s)
    self.assertIn("Initial learner", s)
    self.assertIn("Atemporal learner", s)

    l.useScoreAIC().useLocalSearchWithTabuList()
    self.assertEqual(self._stateValue(l, "Algorithm"), "Local Search with Tabu List")
    self.assertEqual(self._stateValue(l, "Score"), "AIC")

  def testCopyState(self):
    src = self._solo()
    src.useScoreAIC().useSmoothingPrior(0.5).useLocalSearchWithTabuList()
    src.addForbiddenArc("X", 0, "X", 1)

    dst = self._solo()
    dst.copyState(src)
    self.assertEqual(self._stateValue(dst, "Score"), "AIC")
    self.assertEqual(self._stateValue(dst, "Prior"), "Smoothing")
    self.assertEqual(self._stateValue(dst, "Algorithm"), "Local Search with Tabu List")
    self.assertFalse(self._learn(dst).existsArc("X", 0, "X", 1))

    mono = self._std(2, {"C"})
    mono.copyState(src)  # safe no-op for unknown names, must not throw


ts = unittest.TestSuite()
addTests(ts, KTBNLearnerTestCase)
