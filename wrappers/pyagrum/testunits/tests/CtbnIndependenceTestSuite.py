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
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.ctbn as ct


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
    ctbn.CIM("cloudy?")[{"rain?": "0"}] = [[-0.1, 0.1], [1, -1]]
    ctbn.CIM("cloudy?")[{"rain?": "1"}] = [[-0.1, 0.1], [1, -1]]
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


ts = unittest.TestSuite()
addTests(ts, TestCtbnIndependence)
