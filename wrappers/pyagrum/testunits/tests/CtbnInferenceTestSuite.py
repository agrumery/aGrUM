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

import numpy as np

import pyagrum as gum
import pyagrum.ctbn as ct

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


def _model():
  ctbn = ct.CTBN()
  ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
  ctbn.add(gum.LabelizedVariable("B", "B", ["b0", "b1"]))
  ctbn.addArc("A", "B")
  ctbn.CIM("A").fromMatrix(np.array([[-2.0, 2.0], [1.0, -1.0]]))
  ctbn.CIM("B")[{"A": "a0"}] = [[-1, 1], [2, -2]]
  ctbn.CIM("B")[{"A": "a1"}] = [[-3, 3], [4, -4]]
  return ctbn


class TestCtbnInference(pyAgrumTestCase):
  def testForwardSamplingIsReproducible(self):
    ctbn = _model()
    with gum.RandomGeneratorGuard():  # the seed of the run is given back at the end
      # the seed comes from the generator, so that the run stays driven by core.randomSeed (never 0, the clock)
      seed = gum.randomValue(2**31 - 1) + 1
      results = []
      for _ in range(2):
        gum.initRandom(seed)
        ie = ct.ForwardSamplingInference(ctbn)
        ie.makeInference(timeHorizon=50, burnIn=10)
        results.append(ie.posterior("B").toarray().tolist())
    self.assertEqual(results[0], results[1])

  def testParallelInferenceEqualsAverageInference(self):
    ctbn = _model()
    with gum.RandomGeneratorGuard():  # the seed of the run is given back at the end
      seed = gum.randomValue(2**31 - 1) + 1  # never 0 (the clock)
      gum.initRandom(seed)
      seq = ct.ForwardSamplingInference(ctbn)
      seq.averageInference(nbTrajectories=3, timeHorizon=50, burnIn=10)

      gum.initRandom(seed)
      par = ct.ForwardSamplingInference(ctbn)
      nbEvents = par.makeParallelInference(nbTrajectories=3, timeHorizon=50, burnIn=10)

    self.assertEqual(len(nbEvents), 3)
    self.assertTrue(all(n > 0 for n in nbEvents))
    for name in ctbn.names():
      self.assertAlmostEqual(par.posterior(name).sum(), 1.0)
      np.testing.assert_allclose(par.posterior(name).toarray(), seq.posterior(name).toarray(), rtol=1e-12)

  def testParallelInferenceApproximatesStationaryDistribution(self):
    ctbn = _model()
    ie = ct.ForwardSamplingInference(ctbn)
    ie.makeParallelInference(nbTrajectories=2, timeHorizon=2000, burnIn=10)
    # A is a 2-state chain (rates 2 and 1): P(A) = [1/3, 2/3]
    np.testing.assert_allclose(ie.posterior("A").toarray(), [1 / 3, 2 / 3], atol=0.05)

  def testSampleCountsOnlyTheTimeAfterTheBurnIn(self):
    ctbn = _model()
    ie = ct.ForwardSamplingInference(ctbn)
    posteriors = {name: gum.Tensor().add(ctbn.variable(name)) for name in ctbn.names()}
    ie.makeSample(posteriors, timeHorizon=0.5, burnIn=10)
    for name in ctbn.names():
      self.assertAlmostEqual(posteriors[name].sum(), 0.5)

  def testBurnInStopsInAnAbsorbingState(self):
    # a0 is absorbing: the process stays there after its first transition
    ctbn = ct.CTBN()
    ctbn.add(gum.LabelizedVariable("A", "A", ["a0", "a1"]))
    ctbn.CIM("A").fromMatrix(np.array([[0.0, 0.0], [1.0, -1.0]]))
    ie = ct.ForwardSamplingInference(ctbn)
    ie.makeInference(timeHorizon=10, burnIn=10)
    self.assertEqual(ie.posterior("A").toarray().tolist(), [1.0, 0.0])


ts = unittest.TestSuite()
addTests(ts, TestCtbnInference)
