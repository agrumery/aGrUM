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

import unittest

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum.ctbn as ct


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
    ctbn.CIM("cloudy?")[{"rain?": "0"}] = [[-.1, .1],
                                           [1, -1]]
    ctbn.CIM("cloudy?")[{"rain?": "1"}] = [[-.1, .1],
                                           [1, -1]]
    ctbn.CIM("rain?")[{"cloudy?": "0"}] = [[-0.5, 0.5],
                                           [1000, -1000]]
    ctbn.CIM("rain?")[{"cloudy?": "1"}] = [[-2, 2],
                                           [2, -2]]

    ie = ct.ForwardSamplingInference(ctbn)

    # independence tests
    countRep1 = 0
    countRep2 = 0
    nbRep = 2
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_indep_1.csv'), n=1, timeHorizon=100, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir('trajectory_indep_1.csv'), ctbn=ctbn)
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
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_indep_2.csv'), n=1, timeHorizon=50, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir('trajectory_indep_2.csv'), ctbn=ctbn)
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

    ctbn.CIM("A")[:] = [[-4, 4],
                        [2, -2]]
    ctbn.CIM("B")[{"A": "a"}] = [[-5, 5],
                                 [4, -4]]
    ctbn.CIM("B")[{"A": "b"}] = [[-1, 1],
                                 [1, -1]]
    ctbn.CIM("C")[{"B": "x"}] = [[-5, 5],
                                 [4, -4]]
    ctbn.CIM("C")[{"B": "y"}] = [[-1, 1],
                                 [1, -1]]

    # make sample
    ie = ct.ForwardSamplingInference(ctbn)

    countRep1 = 0
    countRep2 = 0
    countRep3 = 0
    countRep4 = 0
    nbRep = 2
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_indep_3.csv'), n=1, timeHorizon=100, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir('trajectory_indep_3.csv'), ctbn=ctbn)
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
    nbRep = 2
    minRep = 1
    for i in range(nbRep):
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_indep_4.csv'), n=1, timeHorizon=150, burnIn=500)
      traj = ct.Trajectory(self.agrumSrcDir('trajectory_indep_4.csv'), ctbn=ctbn)
      test = ct.FChi2Test(traj)
      if test.testIndep("X", "Z", []):
        countRep1 += 1
      if not test.testIndep("Y", "X", []):
        countRep2 += 1
      if not test.testIndep("Y", "Z", []):
        countRep3 += 1

    self.assertGreaterEqual(countRep1, minRep)
    self.assertGreaterEqual(countRep2, minRep)
    self.assertGreaterEqual(countRep3, minRep)


ts = unittest.TestSuite()
addTests(ts, TestCtbnIndependence)
