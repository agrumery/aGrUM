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
