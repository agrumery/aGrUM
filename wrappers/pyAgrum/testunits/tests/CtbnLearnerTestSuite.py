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

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum.ctbn as ct


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
    ctbn.CIM("cloudy?")[{"rain?": "0"}] = [[-.1, .1],
                                           [1, -1]]
    ctbn.CIM("cloudy?")[{"rain?": "1"}] = [[-.1, .1],
                                           [1, -1]]
    ctbn.CIM("rain?")[{"cloudy?": "0"}] = [[-0.5, 0.5],
                                           [1000, -1000]]
    ctbn.CIM("rain?")[{"cloudy?": "1"}] = [[-2, 2],
                                           [2, -2]]

    ie = ct.ForwardSamplingInference(ctbn)

    countRep = 0
    nbrep = 2
    minRep = 1
    for i in range(nbrep):
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_learn_1.csv'), n=1, timeHorizon=100, burnIn=500)
      learner = ct.Learner(self.agrumSrcDir('trajectory_learn_1.csv'))
      ctbnLearned = learner.learnCTBN(template=ctbn)
      if set([(ctbn.name(n), ctbn.name(m)) for n, m in ctbn.arcs()]) == set(
         [(ctbnLearned.name(n), ctbnLearned.name(m)) for n, m in ctbnLearned.arcs()]):
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

    countRep = 0
    nbrep = 2
    minRep = 1
    for i in range(nbrep):
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_learn_2.csv'), n=1, timeHorizon=100, burnIn=500)
      learner = ct.Learner(self.agrumSrcDir('trajectory_learn_2.csv'))
      ctbnLearned = learner.learnCTBN(template=ctbn)
      if set([(ctbn.name(n), ctbn.name(m)) for n, m in ctbn.arcs()]) == set(
         [(ctbnLearned.name(n), ctbnLearned.name(m)) for n, m in ctbnLearned.arcs()]):
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
      ie.writeTrajectoryCSV(self.agrumSrcDir('trajectory_learn_3.csv'), timeHorizon=10, burnIn=1)
      learner = ct.Learner(self.agrumSrcDir('trajectory_learn_3.csv'))
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


ts = unittest.TestSuite()
addTests(ts, TestCtbnLearner)
