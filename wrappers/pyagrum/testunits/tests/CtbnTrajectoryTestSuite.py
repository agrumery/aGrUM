############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.ctbn as ct


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

    cimA = ctbn.CIM("A").asTensor()
    cimB = ctbn.CIM("B").asTensor()
    cimC = ctbn.CIM("C").asTensor()

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
    count = count.sumOut(ct.CIM.varJ("A"))
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
    timeCloudy = time.sumIn(["cloudy?"])
    timeRain = time.sumIn([ct.CIM.varI("rain?")])

    # check bayes probability : the probability of raining if it is not cloudy should be low
    #                           the probability of cloudy weather should be high if it's raining
    proba1 = time[{"cloudy?": "0", ct.CIM.varI("rain?"): "1"}] / timeCloudy[{"cloudy?": "0"}]
    proba2 = time[{"cloudy?": "1", ct.CIM.varI("rain?"): "1"}] / timeRain[{ct.CIM.varI("rain?"): "1"}]
    self.assertAlmostEqual(proba1, 0.01, delta=0.01)
    self.assertAlmostEqual(proba2, 1, delta=0.05)


ts = unittest.TestSuite()
addTests(ts, TestCtbnTrajectory)
