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
import pyagrum.ctbn as ct
from pyagrum.ctbn.constants import FAIL_TO_REJECT

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class TestCtbnIndependence(pyAgrumTestCase):
  def testIndep1(self):
    # create ctbn
    ctbn = ct.CTBN()
    A = gum.LabelizedVariable("cloudy?", "cloudy?", ["0", "1"])
    B = gum.LabelizedVariable("rain?", "rain?", ["0", "1"])
    ctbn.add(A)
    ctbn.add(B)
    ctbn.addArc("cloudy?", "rain?")
    ctbn.addArc("rain?", "cloudy?")
    # the clouds go away faster when it rains: cloudy? depends on rain?
    ctbn.CIM("cloudy?")[{"rain?": "0"}] = [[-1, 1], [0.5, -0.5]]
    ctbn.CIM("cloudy?")[{"rain?": "1"}] = [[-1, 1], [10, -10]]
    ctbn.CIM("rain?")[{"cloudy?": "0"}] = [[-0.5, 0.5], [1000, -1000]]
    ctbn.CIM("rain?")[{"cloudy?": "1"}] = [[-2, 2], [2, -2]]

    ie = ct.ForwardSamplingInference(ctbn)

    # independence tests
    countRep1 = 0
    countRep2 = 0
    nbRep = 2
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_indep_1.csv"), n=1, timeHorizon=100, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir("trajectory_indep_1.csv"), ctbn=ctbn)
      test = ct.FChi2Test(traj)
      if not test.testIndep("rain?", "cloudy?", []):
        countRep1 += 1
      if not test.testIndep("cloudy?", "rain?", []):
        countRep2 += 1

    self.assertGreaterEqual(countRep1, minRep)
    self.assertGreaterEqual(countRep2, minRep)

  def testIndep2(self):
    # create ctbn
    ctbn = ct.CTBN()
    coinA = gum.LabelizedVariable("coinA", "coinA", ["head", "tail"])
    coinB = gum.LabelizedVariable("coinB", "coinB", ["head", "tail"])
    ctbn.add(coinA)
    ctbn.add(coinB)

    ctbn.CIM("coinA")[:] = [[-2, 2], [2, -2]]
    ctbn.CIM("coinB")[:] = [[-2, 2], [2, -2]]

    # generate sample
    ie = ct.ForwardSamplingInference(ctbn)

    countRep1 = 0
    countRep2 = 0
    nbRep = 2
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_indep_2.csv"), n=1, timeHorizon=50, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir("trajectory_indep_2.csv"), ctbn=ctbn)
      test = ct.FChi2Test(traj)
      if test.testIndep("coinA", "coinB", []):
        countRep1 += 1
      if test.testIndep("coinB", "coinA", []):
        countRep2 += 1

    self.assertGreaterEqual(countRep1, minRep)
    self.assertGreaterEqual(countRep2, minRep)

  def testIndep3(self):
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

    countRep1 = 0
    countRep2 = 0
    countRep3 = 0
    countRep4 = 0
    nbRep = 2
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_indep_3.csv"), n=1, timeHorizon=100, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir("trajectory_indep_3.csv"), ctbn=ctbn)
      test = ct.FChi2Test(traj)
      if not test.testIndep("B", "A", []):
        countRep1 += 1
      if not test.testIndep("C", "B", []):
        countRep2 += 1
      if test.testIndep("A", "B", []):
        countRep3 += 1
      if test.testIndep("C", "A", []):
        countRep4 += 1

    self.assertGreaterEqual(countRep1, minRep)
    self.assertGreaterEqual(countRep2, minRep)
    self.assertGreaterEqual(countRep3, minRep)
    self.assertGreaterEqual(countRep4, minRep)

  def testIndep4(self):
    # create ctbn
    ctbn = ct.CTBN()
    X = gum.LabelizedVariable("X", "X", ["x0", "x1"])
    Y = gum.LabelizedVariable("Y", "Y", ["y0", "y1"])
    Z = gum.LabelizedVariable("Z", "Z", ["z0", "z1"])
    ctbn.add(X)
    ctbn.add(Y)
    ctbn.add(Z)
    ctbn.addArc("X", "Y")
    ctbn.addArc("Z", "Y")
    ctbn.CIM("X")[:] = [[-2, 2], [2, -2]]
    ctbn.CIM("Z")[:] = [[-2, 2], [2, -2]]
    ctbn.CIM("Y")[{"X": "x0", "Z": "z0"}] = [[-1, 1], [1, -1]]
    ctbn.CIM("Y")[{"X": "x1", "Z": "z0"}] = [[-10, 10], [10, -10]]
    ctbn.CIM("Y")[{"X": "x0", "Z": "z1"}] = [[-25, 25], [25, -25]]
    ctbn.CIM("Y")[{"X": "x1", "Z": "z1"}] = [[-8, 8], [8, -8]]

    # make sample
    ie = ct.ForwardSamplingInference(ctbn)

    countRep1 = 0
    countRep2 = 0
    countRep3 = 0
    nbRep = 3
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_indep_4.csv"), n=1, timeHorizon=150, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir("trajectory_indep_4.csv"), ctbn=ctbn)
      test = ct.FChi2Test(traj)
      if countRep1 < minRep:
        if test.testIndep("X", "Z", []):
          countRep1 += 1
      if countRep2 < minRep:
        if not test.testIndep("Y", "X", []):
          countRep2 += 1
      if countRep3 < minRep:
        if not test.testIndep("Y", "Z", []):
          countRep3 += 1

    self.assertGreaterEqual(countRep1, minRep)
    self.assertGreaterEqual(countRep2, minRep)
    self.assertGreaterEqual(countRep3, minRep)

  def testDegreesOfFreedomAreTheTotalCounts(self):
    # The degrees of freedom of the F test are the transition counts over all the trajectories (Bregoli et al.
    # 2020, eq. 18): two copies of a trajectory double them.
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("X", "X", 2))
    ctbn.add(gum.LabelizedVariable("Y", "Y", 2))
    ctbn.addArc("Y", "X")
    ctbn.CIM("X")[{"Y": 0}] = [[-1, 1], [1, -1]]
    ctbn.CIM("X")[{"Y": 1}] = [[-3, 3], [3, -3]]
    ctbn.CIM("Y")[:] = [[-1, 1], [1, -1]]

    ie = ct.ForwardSamplingInference(ctbn)
    ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_indep_dof.csv"), n=1, timeHorizon=20, burnIn=10)
    once = ct.Trajectory(self.agrumSrcDir("trajectory_indep_dof.csv"), ctbn=ctbn)
    twice = ct.Trajectory({0: once.data[0], 1: once.data[0]}, ctbn=ctbn)

    test1 = ct.FChi2Test(once)
    test1.testIndep("X", "Y", [])
    test2 = ct.FChi2Test(twice)
    test2.testIndep("X", "Y", [])

    for i in range(2):
      self.assertGreater(test1.r2[{"X#i": i}], 0)
      self.assertEqual(test2.r1[{"X#i": i, "Y": 0}], 2 * test1.r1[{"X#i": i, "Y": 0}])
      self.assertEqual(test2.r2[{"X#i": i}], 2 * test1.r2[{"X#i": i}])

  def testDependenceIsFoundWithManyTrajectories(self):
    # X jumps 1.5 times faster when Y=1: with 100 trajectories, about 2500 transitions of X, the dependence is
    # far beyond the acceptance interval of the F test.
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("X", "X", 2))
    ctbn.add(gum.LabelizedVariable("Y", "Y", 2))
    ctbn.addArc("Y", "X")
    ctbn.CIM("X")[{"Y": 0}] = [[-1, 1], [1, -1]]
    ctbn.CIM("X")[{"Y": 1}] = [[-1.5, 1.5], [1.5, -1.5]]
    ctbn.CIM("Y")[:] = [[-1, 1], [1, -1]]

    ie = ct.ForwardSamplingInference(ctbn)
    ie.writeTrajectoryCSV(self.agrumSrcDir("trajectory_indep_many.csv"), n=100, timeHorizon=20, burnIn=10)
    traj = ct.Trajectory(self.agrumSrcDir("trajectory_indep_many.csv"), ctbn=ctbn)
    test = ct.FChi2Test(traj)
    self.assertFalse(test.testIndep("X", "Y", []))

  def testFTestComparesOnlyTheExitRates(self):
    # X leaves each state at rate 1 whatever Y: only its destination depends on Y (0->1->2 when Y=0, 0->2->1 when
    # Y=1). The time to transition test compares the exit rates only (Bregoli et al. 2020, eq. 18): it accepts,
    # and the state to state test finds the dependence.
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("X", "X", 3))
    ctbn.add(gum.LabelizedVariable("Y", "Y", 2))
    ctbn.addArc("Y", "X")

    def trajectory(y, cycle):
      # a row (t, X, x) is a jump of X out of x at time t; the last rows give the states at the horizon
      traj = [(0.0, "X", cycle[0]), (0.0, "Y", y)]
      traj += [(float(t), "X", cycle[(t - 1) % 3]) for t in range(1, 60)]
      traj += [(60.0, "X", cycle[59 % 3]), (60.0, "Y", y)]
      return traj

    traj = ct.Trajectory({0: trajectory("0", "012"), 1: trajectory("1", "102")}, ctbn=ctbn)
    test = ct.FChi2Test(traj)
    test.addVariables("X", "Y", [])
    self.assertEqual(test.nullTimeToTransitionHypothesisF("X", "Y", []), FAIL_TO_REJECT)
    self.assertFalse(test.testIndep("X", "Y", []))

  def testFTestIgnoresAConfigurationWithoutTransition(self):
    # X jumps every time unit whatever Y; Y only moves to 1 at 59.95, and X does not jump while Y=1: no exit of X is
    # seen given Y=1 (r1 = 0), so this configuration brings no evidence against the independence.
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("X", "X", 2))
    ctbn.add(gum.LabelizedVariable("Y", "Y", 2))
    ctbn.addArc("Y", "X")

    # a row (t, X, x) is a jump of X out of x at time t; the last rows give the states at the horizon
    traj = [(0.0, "X", "0"), (0.0, "Y", "0")]
    traj += [(float(t), "X", str((t - 1) % 2)) for t in range(1, 60)]
    traj += [(59.95, "Y", "0"), (60.0, "X", str(59 % 2)), (60.0, "Y", "1")]

    test = ct.FChi2Test(ct.Trajectory({0: traj}, ctbn=ctbn))
    self.assertTrue(test.testIndep("X", "Y", []))

  def testSignificanceLevelsComeFromTheConfiguration(self):
    # X jumps every time unit when Y=0 and every 1.44 time units when Y=1 (600 time units each): the F test rejects
    # the independence at 10% but not at 5% (the default).
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("X", "X", 2))
    ctbn.add(gum.LabelizedVariable("Y", "Y", 2))
    ctbn.addArc("Y", "X")

    def trajectory(y, dwell):
      # a row (t, X, x) is a jump of X out of x at time t; the last rows give the states at the horizon
      n = round(600 / dwell)
      traj = [(0.0, "X", "0"), (0.0, "Y", y)]
      traj += [(dwell * t, "X", str((t - 1) % 2)) for t in range(1, n)]
      traj += [(600.0, "X", str((n - 1) % 2)), (600.0, "Y", y)]
      return traj

    traj = ct.Trajectory({0: trajectory("0", 1.0), 1: trajectory("1", 1.44)}, ctbn=ctbn)

    test = ct.FChi2Test(traj)
    self.assertEqual(test.fSignificance, 0.05)
    self.assertEqual(test.chi2Significance, 0.05)
    self.assertTrue(test.testIndep("X", "Y", []))

    gum.config.push()
    try:
      gum.config["ctbn", "fSignificance"] = 0.1
      gum.config["ctbn", "chi2Significance"] = 0.2
      test10 = ct.FChi2Test(traj)
    finally:
      gum.config.pop()
    self.assertEqual(test10.fSignificance, 0.1)
    self.assertEqual(test10.chi2Significance, 0.2)
    self.assertFalse(test10.testIndep("X", "Y", []))
    # the levels are read when the test is created
    self.assertTrue(test.testIndep("X", "Y", []))

  def testSignificanceLevelsAreInZeroOne(self):
    for key in ["fSignificance", "chi2Significance"]:
      for significance in [0, -0.1, 1, 1.5]:
        with self.assertRaises(ValueError):
          gum.config["ctbn", key] = significance


ts = unittest.TestSuite()
addTests(ts, TestCtbnIndependence)
