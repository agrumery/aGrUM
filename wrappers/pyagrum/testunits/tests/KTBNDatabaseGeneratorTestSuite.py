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

import csv as csvmod
import math
import os
import tempfile
import unittest

import pyagrum.ktbn as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

AT = gum.KTBN.ATEMPORAL


class VarOrderMode:
  # SWIG exposes the nested enum as flat EnumName_MEMBER class attributes,
  # not a nested enum type -- this shim gives the tests a familiar spelling.
  RANDOM = gum.KTBNDatabaseGenerator.VarOrderMode_RANDOM
  TOPOLOGICAL = gum.KTBNDatabaseGenerator.VarOrderMode_TOPOLOGICAL
  ANTI_TOPOLOGICAL = gum.KTBNDatabaseGenerator.VarOrderMode_ANTI_TOPOLOGICAL


def _model():
  m = gum.KTBN(2)
  m.add(gum.LabelizedVariable("X", "", ["off", "on"]), True)
  m.add(gum.LabelizedVariable("C", "", ["lo", "hi"]), False)
  m.addArc("X", 0, "X", 1)
  m.addArc("C", AT, "X", 1)
  m.generateCPTs()
  return m


def _readCSV(path, sep=","):
  with open(path, newline="") as f:
    rows = list(csvmod.reader(f, delimiter=sep))
  return rows[0], rows[1:]


def _checkDrawSamples(tc, lls, directory, stem, expectedRows):
  tc.assertEqual(len(lls), len(expectedRows))
  for ll in lls:
    tc.assertLessEqual(ll, 0.0)
    tc.assertTrue(math.isfinite(ll))
  for i, n in enumerate(expectedRows):
    path = os.path.join(directory, f"{stem}{i + 1}.csv")
    tc.assertTrue(os.path.exists(path))
    _, rows = _readCSV(path)
    tc.assertEqual(len(rows), n)
  tc.assertFalse(os.path.exists(os.path.join(directory, f"{stem}0.csv")))


class KTBNDatabaseGeneratorTestCase(pyAgrumTestCase):
  def setUp(self):
    self._dir = tempfile.mkdtemp()

  def testDrawSingleTrajectory(self):
    gum.initRandom(42)
    m = _model()
    T = 5
    gen = gum.KTBNDatabaseGenerator(m)

    lls = gen.drawSamples(1, T, self._dir, "kdbn_one", VarOrderMode.RANDOM, False)
    _checkDrawSamples(self, lls, self._dir, "kdbn_one", [T])

    header, rows = _readCSV(os.path.join(self._dir, "kdbn_one1.csv"))
    self.assertEqual(sorted(header), ["C", "X"])
    iC, iX = header.index("C"), header.index("X")
    c0 = rows[0][iC]
    for r in rows:
      self.assertLess(int(r[iX]), 2)
      self.assertLess(int(r[iC]), 2)
      self.assertEqual(r[iC], c0)  # atemporal C is constant along the trajectory

    gen.drawSamples(1, T, self._dir, "kdbn_one_lbl", VarOrderMode.RANDOM, True)
    hl, rl = _readCSV(os.path.join(self._dir, "kdbn_one_lbl1.csv"))
    iXl, iCl = hl.index("X"), hl.index("C")
    for r in rl:
      self.assertIn(r[iXl], ("off", "on"))
      self.assertIn(r[iCl], ("lo", "hi"))

    # boundary T == k: only Phase 1 runs
    gen.drawSamples(1, 2, self._dir, "kdbn_horizonk", VarOrderMode.RANDOM, False)
    hk, rk = _readCSV(os.path.join(self._dir, "kdbn_horizonk1.csv"))
    self.assertEqual(len(rk), 2)
    self.assertEqual(rk[0][hk.index("C")], rk[1][hk.index("C")])

    # custom separator
    gen.drawSamples(1, 2, self._dir, "kdbn_semi", VarOrderMode.RANDOM, False, ";")
    with open(os.path.join(self._dir, "kdbn_semi1.csv")) as f:
      content = f.read()
    self.assertIn(";", content)
    self.assertNotIn(",", content)

  def testDrawSamplesPerLength(self):
    gum.initRandom(7)
    m = _model()
    gen = gum.KTBNDatabaseGenerator(m)
    lengths = [3, 4, 5]

    lls = gen.drawSamples(lengths, self._dir, "toto")
    _checkDrawSamples(self, lls, self._dir, "toto", lengths)

  def testDrawSamplesFixedLength(self):
    gum.initRandom(11)
    m = _model()
    gen = gum.KTBNDatabaseGenerator(m)
    nbSamples, T = 4, 5

    lls = gen.drawSamples(nbSamples, T, self._dir, "fixed")
    _checkDrawSamples(self, lls, self._dir, "fixed", [T] * nbSamples)

  def testVarOrderModesAndNbVars(self):
    m = _model()
    gen = gum.KTBNDatabaseGenerator(m)
    self.assertEqual(gen.nbVars(), 2)

    for mode in (VarOrderMode.RANDOM, VarOrderMode.TOPOLOGICAL, VarOrderMode.ANTI_TOPOLOGICAL):
      gen.drawSamples(1, 2, self._dir, "kdbn_varorder", mode, False)
      header, _ = _readCSV(os.path.join(self._dir, "kdbn_varorder1.csv"))
      self.assertEqual(sorted(header), ["C", "X"])

  def testTopologicalVarOrder(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("C", AT, "X", 0)
    m.addArc("C", AT, "X", 1)
    m.addArc("X", 0, "X", 1)  # lagged: ignored for column ordering
    m.generateCPTs()

    gen = gum.KTBNDatabaseGenerator(m)
    gen.drawSamples(1, 2, self._dir, "kdbn_topo", VarOrderMode.TOPOLOGICAL)
    topo, _ = _readCSV(os.path.join(self._dir, "kdbn_topo1.csv"))
    self.assertEqual(topo, ["C", "X"])  # contemporaneous parent C before X

    gen.drawSamples(1, 2, self._dir, "kdbn_antitopo", VarOrderMode.ANTI_TOPOLOGICAL)
    anti, _ = _readCSV(os.path.join(self._dir, "kdbn_antitopo1.csv"))
    self.assertEqual(anti, ["X", "C"])
    self.assertEqual(list(reversed(anti)), topo)

    m2 = gum.KTBN(2)
    m2.add(gum.LabelizedVariable("A", "", 2), True)
    m2.add(gum.LabelizedVariable("B", "", 2), True)
    m2.add(gum.LabelizedVariable("D", "", 2), False)
    m2.addArc("A", 0, "B", 1)  # lagged: must NOT force A before B
    m2.addArc("D", AT, "A", 0)  # atemporal: D must precede A
    m2.generateCPTs()
    gen2 = gum.KTBNDatabaseGenerator(m2)

    gen2.drawSamples(1, 2, self._dir, "kdbn_topo2_", VarOrderMode.TOPOLOGICAL)
    t2, _ = _readCSV(os.path.join(self._dir, "kdbn_topo2_1.csv"))
    self.assertEqual(sorted(t2), ["A", "B", "D"])
    self.assertLess(t2.index("D"), t2.index("A"))

  def testDiscretizedLabelRendering(self):
    gum.initRandom(123)
    m = gum.KTBN(2)
    m.add(gum.DiscretizedVariable("D", "", [0.0, 1.0, 2.0, 3.0]), True)
    m.addArc("D", 0, "D", 1)
    m.generateCPTs()

    gen = gum.KTBNDatabaseGenerator(m)

    gen.setDiscretizedLabelModeInterval()
    gen.drawSamples(1, 4, self._dir, "kdbn_discr", VarOrderMode.RANDOM, True)
    h, r = _readCSV(os.path.join(self._dir, "kdbn_discr1.csv"))
    self.assertIn("[", r[0][h.index("D")])

    gen.setDiscretizedLabelModeMedian()
    gen.drawSamples(1, 4, self._dir, "kdbn_discr", VarOrderMode.RANDOM, True)
    h, r = _readCSV(os.path.join(self._dir, "kdbn_discr1.csv"))
    med = float(r[0][h.index("D")])
    self.assertGreaterEqual(med, 0.0)
    self.assertLessEqual(med, 3.0)

    gen.setDiscretizedLabelModeRandom()
    gen.drawSamples(1, 4, self._dir, "kdbn_discr", VarOrderMode.RANDOM, True)
    h, r = _readCSV(os.path.join(self._dir, "kdbn_discr1.csv"))
    rnd = float(r[0][h.index("D")])
    self.assertGreaterEqual(rnd, 0.0)
    self.assertLessEqual(rnd, 3.0)

  def testSampledFrequenciesMatchCPT(self):
    gum.initRandom(42)
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 1)
    m.cpt("X", 0).fillWith([0.7, 0.3])
    m.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8])

    gen = gum.KTBNDatabaseGenerator(m)
    gen.drawSamples(1, 60000, self._dir, "kdbn_stats", VarOrderMode.RANDOM, False)
    h, rows = _readCSV(os.path.join(self._dir, "kdbn_stats1.csv"))
    iX = h.index("X")

    byPrev = [0, 0]
    next0ByPrev = [0, 0]
    for t in range(1, len(rows)):
      prev = int(rows[t - 1][iX])
      cur = int(rows[t][iX])
      byPrev[prev] += 1
      if cur == 0:
        next0ByPrev[prev] += 1

    tol = 0.02
    self.assertLess(abs(next0ByPrev[0] / byPrev[0] - 0.9), tol)
    self.assertLess(abs(next0ByPrev[1] / byPrev[1] - 0.2), tol)

  def testHigherOrderLag(self):
    gum.initRandom(1)
    m = gum.KTBN(3)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.addArc("X", 0, "X", 2)  # lag 2: X[t] depends only on X[t-2]

    m.fillCPT("X", 0, {}, [1.0, 0.0])
    m.fillCPT("X", 1, {}, [0.0, 1.0])
    m.fillCPT("X", 2, {("X", 0): 0}, [1.0, 0.0])
    m.fillCPT("X", 2, {("X", 0): 1}, [0.0, 1.0])

    T = 7
    gen = gum.KTBNDatabaseGenerator(m)
    lls = gen.drawSamples(1, T, self._dir, "kdbn_lag", VarOrderMode.RANDOM, False)

    self.assertLess(abs(lls[0]), 1e-9)  # deterministic CPTs -> log2-likelihood 0

    h, rows = _readCSV(os.path.join(self._dir, "kdbn_lag1.csv"))
    iX = h.index("X")
    self.assertEqual(len(rows), T)
    for t in range(T):
      self.assertEqual(int(rows[t][iX]), t % 2)

  def testDrawSamplesErrors(self):
    m = _model()  # k = 2
    gen = gum.KTBNDatabaseGenerator(m)

    with self.assertRaises(gum.OperationNotAllowed):
      gen.drawSamples([3, 1, 4], self._dir, "err")
    with self.assertRaises(gum.OperationNotAllowed):
      gen.drawSamples(3, 1, self._dir, "errfix")
    with self.assertRaises(gum.InvalidArgument):
      gen.drawSamples(1, 3, self._dir, "errsep", VarOrderMode.RANDOM, False, "\n")


ts = unittest.TestSuite()
addTests(ts, KTBNDatabaseGeneratorTestCase)
