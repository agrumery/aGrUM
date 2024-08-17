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


class TestCtbnTrajectory(pyAgrumTestCase):
  def testTrajectory1(self):

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

    cimA = ctbn.CIM("A").asPotential()
    cimB = ctbn.CIM("B").asPotential()
    cimC = ctbn.CIM("C").asPotential()

    ie = ct.ForwardSamplingInference(ctbn)

    ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_traj_1.csv'), n=1, timeHorizon=400, burnIn=100)

    traj = ct.Trajectory(self.agrumSrcDir('trajectory_traj_1.csv'))
    traj.computeAllCIMs()

    # checks if CIMs are correctly learned (using _pot to avoid the copy)
    newCimA = ctbn.CIM("A")._pot
    newCimB = ctbn.CIM("B")._pot
    newCimC = ctbn.CIM("C")._pot
    I1 = gum.Instantiation(newCimA)
    I2 = gum.Instantiation(newCimB)
    I3 = gum.Instantiation(newCimC)
    I1.setFirst()
    I2.setFirst()
    I3.setFirst()
    while not I1.end():
      self.assertAlmostEqual(cimA.get(I1), newCimA.get(I1), delta=.5)
      I1.inc()
    while not I2.end():
      self.assertAlmostEqual(cimB.get(I2), newCimB.get(I2), delta=.5)
      I2.inc()
    while not I3.end():
      self.assertAlmostEqual(cimC.get(I3), newCimC.get(I3), delta=.5)
      I3.inc()

  def testTrajectory2(self):
    # Create ctbn
    ctbn = ct.CTBN()
    A = gum.LabelizedVariable("A", "A", ["a0", "a1", "a2"])
    ctbn.add(A)
    ctbn.CIM("A")[:] = [[-6, 3, 3],
                        [3, -6, 3],
                        [3, 3, -6]]

    # make sample using forward sampling
    ie = ct.ForwardSamplingInference(ctbn)
    ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_traj_2.csv'), n=1, timeHorizon=400, burnIn=100)
    traj = ct.Trajectory(self.agrumSrcDir('trajectory_traj_2.csv'))

    # check number of transitions
    time, count = traj.computeStats("A", [])
    count = count.normalize()
    count = count.margSumOut(ct.CIM.varJ("A"))
    I = gum.Instantiation(count)
    I.setFirst()
    while not I.end():
      self.assertAlmostEqual(count.get(I), 1 / 3, delta=0.1)
      I.inc()

    # check average time spent
    time = time.normalize()
    I = gum.Instantiation(time)
    I.setFirst()
    while not I.end():
      self.assertAlmostEqual(time.get(I), 1 / 3, delta=0.1)
      I.inc()

  def testTrajectory3(self):
    # Create ctbn
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
    ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_traj_3.csv'), n=1, timeHorizon=400, burnIn=100)
    traj = ct.Trajectory(self.agrumSrcDir('trajectory_traj_3.csv'))

    time, count = traj.computeStats("rain?", ["cloudy?"])
    timeCloudy = time.margSumIn(["cloudy?"])
    timeRain = time.margSumIn([ct.CIM.varI("rain?")])

    # check bayes probability : the probability of raining if it is not cloudy should be low
    #                           the probability of cloudy weather should be high if it's raining
    proba1 = time[{"cloudy?": "0", ct.CIM.varI("rain?"): "1"}] / timeCloudy[{"cloudy?": "0"}]
    proba2 = time[{"cloudy?": "1", ct.CIM.varI("rain?"): "1"}] / timeRain[{ct.CIM.varI("rain?"): "1"}]
    self.assertAlmostEqual(proba1, 0.01, delta=0.01)
    self.assertAlmostEqual(proba2, 1, delta=0.05)


ts = unittest.TestSuite()
addTests(ts, TestCtbnTrajectory)
