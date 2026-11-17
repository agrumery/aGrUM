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
import unittest.mock

import pyagrum as gum
import pyagrum.ctbn as ct

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class TestCtbnLearner(pyAgrumTestCase):
  def testLearner1(self):
    # create ctbn
    ctbn = ct.CTBN()
    A = gum.LabelizedVariable("cloudy?", "cloudy?", ["0", "1"])
    B = gum.LabelizedVariable("rain?", "rain?", ["0", "1"])
    ctbn.add(A)
    ctbn.add(B)
    ctbn.addArc("cloudy?", "rain?")
    ctbn.addArc("rain?", "cloudy?")
    ctbn.CIM("cloudy?")[{"rain?": "0"}] = [[-0.1, 0.1], [1, -1]]
    ctbn.CIM("cloudy?")[{"rain?": "1"}] = [[-0.1, 0.1], [1, -1]]
    ctbn.CIM("rain?")[{"cloudy?": "0"}] = [[-0.5, 0.5], [1000, -1000]]
    ctbn.CIM("rain?")[{"cloudy?": "1"}] = [[-2, 2], [2, -2]]

    ie = ct.ForwardSamplingInference(ctbn)

    countRep = 0
    nbrep = 2
    minRep = 1
    for i in range(nbrep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_learn_1.csv"), n=1, timeHorizon=100, burnIn=500)
      learner = ct.Learner(self.agrumSrcDir("trajectory_learn_1.csv"))
      ctbnLearned = learner.learnCTBN(template=ctbn)
      if set([(ctbn.name(n), ctbn.name(m)) for n, m in ctbn.arcs()]) == set(
        [(ctbnLearned.name(n), ctbnLearned.name(m)) for n, m in ctbnLearned.arcs()]
      ):
        countRep += 1

    self.assertGreaterEqual(countRep, minRep)

  def testLearner2(self):
    # create ctbn
    ctbn = ct.CTBN()
    A = gum.LabelizedVariable("A", "A", ["a", "b"])
    B = gum.LabelizedVariable("B", "B", ["x", "y"])
    C = gum.LabelizedVariable("C", "C", ["m", "n"])
    ctbn.add(A)
    ctbn.add(B)
    ctbn.add(C)

    ctbn.addArc("A", "B")
    ctbn.addArc("B", "C")

    ctbn.CIM("A")[:] = [[-4, 4], [2, -2]]
    ctbn.CIM("B")[{"A": "a"}] = [[-5, 5], [4, -4]]
    ctbn.CIM("B")[{"A": "b"}] = [[-1, 1], [1, -1]]
    ctbn.CIM("C")[{"B": "x"}] = [[-5, 5], [4, -4]]
    ctbn.CIM("C")[{"B": "y"}] = [[-1, 1], [1, -1]]

    # make sample
    ie = ct.ForwardSamplingInference(ctbn)

    countRep = 0
    nbrep = 2
    minRep = 1
    for i in range(nbrep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_learn_2.csv"), n=1, timeHorizon=100, burnIn=500)
      learner = ct.Learner(self.agrumSrcDir("trajectory_learn_2.csv"))
      ctbnLearned = learner.learnCTBN(template=ctbn)
      if set([(ctbn.name(n), ctbn.name(m)) for n, m in ctbn.arcs()]) == set(
        [(ctbnLearned.name(n), ctbnLearned.name(m)) for n, m in ctbnLearned.arcs()]
      ):
        countRep += 1

    self.assertGreaterEqual(countRep, minRep)

  def testLearner3(self):
    # create random ctbn with 5 nodes with domainsize=3 and 2 parents at mot
    ctbn = ct.randomCTBN((0.5, 0.6), n=4, parMax=2, modal=3)
    ie = ct.ForwardSamplingInference(ctbn)

    countRep1 = 0
    countRep2 = 0
    countRep3 = 0
    nbrep = 2
    minRep = 1
    for i in range(nbrep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_learn_3.csv"), timeHorizon=10, burnIn=1)
      learner = ct.Learner(self.agrumSrcDir("trajectory_learn_3.csv"))
      ctbnLearned = learner.learnCTBN()
      if len(ctbn.variables()) == len(ctbnLearned.variables()):
        countRep1 += 1
      if set(ctbn.names()) == set(ctbnLearned.names()):
        countRep2 += 1
      flag = True
      for v in ctbn.variables():
        if set(ctbn.labels(v.name())) != set(ctbnLearned.labels(v.name())):
          flag = False
          break
      if flag:
        countRep3 += 1

    self.assertGreaterEqual(countRep1, minRep)
    self.assertGreaterEqual(countRep2, minRep)
    self.assertGreaterEqual(countRep3, minRep)

  def testTrajectoriesOfDifferentDurations(self):
    # X jumps at every time unit, during 10 time units in a trajectory and 20 in the other: q_0 = 15 jumps / 15
    # time units, q_1 = 13 / 15, whatever the order of the trajectories
    # (Y never changes)
    def trajectory(horizon):
      # a row (t, X, x) is a jump of X out of x at time t; the last rows give the states at the horizon
      traj = [(0.0, "X", "0"), (0.0, "Y", "0")]
      traj += [(float(t), "X", str((t - 1) % 2)) for t in range(1, horizon)]
      traj += [(float(horizon), "X", str((horizon - 1) % 2)), (float(horizon), "Y", "0")]
      return traj

    for data in [{0: trajectory(10), 1: trajectory(20)}, {0: trajectory(20), 1: trajectory(10)}]:
      ctbn = ct.CTBN()
      ctbn.add(gum.LabelizedVariable("X", "X", 2))
      ctbn.add(gum.LabelizedVariable("Y", "Y", 2))
      ct.Learner(data).fitParameters(ctbn)
      self.assertAlmostEqual(ctbn.CIM("X")[{"X#i": 0, "X#j": 1}], 1.0)
      self.assertAlmostEqual(ctbn.CIM("X")[{"X#i": 1, "X#j": 0}], 13 / 15)

      # the counts of the independence tests
      test = ct.FChi2Test(ct.Trajectory(data, ctbn=ctbn))
      test.testIndep("X", "Y", [])
      self.assertEqual(test.r2.toarray().tolist(), [15.0, 13.0])

  def testCTPCTestsEachConditioningSetOnce(self):
    # the independence tests are replaced by a rule (X is independent of A only): with the two parents A and B of X,
    # A is removed with the empty set (b=0), then no set of size 1 remains for B
    ctbn = ct.CTBN()
    for name in ["X", "A", "B"]:
      ctbn.add(gum.LabelizedVariable(name, name, 2))
    data = {0: [(0.0, "X", "0"), (0.0, "A", "0"), (0.0, "B", "0"), (1.0, "X", "0"), (1.0, "A", "0"), (1.0, "B", "0")]}

    calls = []

    def testIndep(self, X, Y, U):
      calls.append((X, Y, tuple(sorted(U))))
      return (X, Y) == ("X", "A")

    with unittest.mock.patch.object(ct.FChi2Test, "testIndep", testIndep):
      learned = ct.Learner(data).learnCTBN(template=ctbn)

    self.assertEqual(len(calls), len(set(calls)))
    arcs = {(learned.name(a), learned.name(b)) for a, b in learned.arcs()}
    self.assertIn(("B", "X"), arcs)
    self.assertNotIn(("A", "X"), arcs)

  def testSmallRatesAreNotRounded(self):
    # X leaves 0 at t=4000 and 12000, leaves 1 at t=8000, horizon 16000: q_0 = 2 / 8000, q_1 = 1 / 8000
    data = {0: [(0.0, "X", "0"), (4000.0, "X", "0"), (8000.0, "X", "1"), (12000.0, "X", "0"), (16000.0, "X", "1")]}
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("X", "X", 2))
    ct.Learner(data).fitParameters(ctbn)
    cim = ctbn.CIM("X")
    self.assertAlmostEqual(cim[{"X#i": 0, "X#j": 1}], 2 / 8000, places=15)
    self.assertAlmostEqual(cim[{"X#i": 1, "X#j": 0}], 1 / 8000, places=15)
    for i in range(2):
      self.assertEqual(cim[{"X#i": i, "X#j": 0}] + cim[{"X#i": i, "X#j": 1}], 0.0)


ts = unittest.TestSuite()
addTests(ts, TestCtbnLearner)
