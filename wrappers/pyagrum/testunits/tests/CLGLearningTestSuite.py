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
import pandas as pd

import pyagrum as gum
import pyagrum.clg as gclg
from pyagrum.clg.CLG import CLG
from pyagrum.clg.GaussianVariable import GaussianVariable
from pyagrum.clg.learning import CLGLearner

from .pyAgrumTestSuite import addTests, pyAgrumTestCase

SEM_FIT = """
A = 4.5[0.1]
B = 3[0.1]
C = 9 + 2A + 1.5B[0.2]
D = 9 + C[0.2]
"""


class CLGLearningTestCase(pyAgrumTestCase):
  def test_learnerAcceptsADataFrame(self):
    filename = self.agrumSrcDir("CLGLearningTestSuite2.csv")
    df = pd.read_csv(filename)
    from_file = gclg.SEM.toclg(SEM_FIT)
    CLGLearner(filename).fitParameters(from_file)
    from_df = gclg.SEM.toclg(SEM_FIT)
    CLGLearner(df).fitParameters(from_df)
    self.assertEqual(gclg.SEM.tosem(from_df), gclg.SEM.tosem(from_file))

  def test_learnerNeedsExactlyOneSourceOfData(self):
    with self.assertRaises(gum.ArgumentError):
      CLGLearner()

  def test_learnerFilenameIsDeprecated(self):
    # transitory: remove with the keyword argument filename (deprecated after 3.3.2)
    filename = self.agrumSrcDir("CLGLearningTestSuite2.csv")
    with self.assertWarns(DeprecationWarning):
      learner = CLGLearner(filename=filename)
    expected = gclg.SEM.toclg(SEM_FIT)
    CLGLearner(data=filename).fitParameters(expected)
    fitted = gclg.SEM.toclg(SEM_FIT)
    learner.fitParameters(fitted)
    self.assertEqual(gclg.SEM.tosem(fitted), gclg.SEM.tosem(expected))
    with self.assertRaises(gum.ArgumentError):
      CLGLearner(filename, filename=filename)

  def test_TestIndep(self):
    # clg is a CLG model
    clg = CLG()
    # 4 variables
    A = GaussianVariable(mu=4.5, sigma=0.1, name="A")
    B = GaussianVariable(mu=3, sigma=0.1, name="B")
    C = GaussianVariable(mu=9, sigma=0.2, name="C")
    D = GaussianVariable(mu=9, sigma=0.2, name="D")
    idA = clg.add(A)
    idB = clg.add(B)
    idC = clg.add(C)
    idD = clg.add(D)
    # A and B are the parents of C while C is the parent of D
    clg.addArc(idA, idC, 2)
    clg.addArc(idB, idC, 1.5)
    clg.addArc(idC, idD, 1)

    # learning
    learner = CLGLearner(self.agrumSrcDir("CLGLearningTestSuite2.csv"))
    learner.supremum_deviation(n_sample=10, fwer_delta=0.05)

    # Assert
    self.assertEqual(learner.test_indep(idA, idB, set()), True)
    self.assertEqual(learner.test_indep(idA, idB, {idC}), False)
    self.assertEqual(learner.test_indep(idA, idD, set()), False)
    self.assertEqual(learner.test_indep(idA, idD, {idC}), True)

  def test_SupremumDeviationReproducibleFromTheSeedOfAgrum(self):
    def sd(seed):
      gum.initRandom(seed)
      return CLGLearner(self.agrumSrcDir("CLGLearningTestSuite2.csv")).supremum_deviation(n_sample=10, fwer_delta=0.05)

    with gum.RandomGeneratorGuard():  # the seed of the run is given back at the end
      # the seed comes from the generator, so that the run stays driven by core.randomSeed (never 0, the clock)
      seed = gum.randomValue(2**30) + 1
      self.assertEqual(sd(seed), sd(seed))

  def test_SupremumDeviationIsTheBoundOfTheTheorem(self):
    delta = 0.05
    n = 10
    with gum.RandomGeneratorGuard():  # the seed of the run is given back at the end
      seed = gum.randomValue(2**30) + 1
      gum.initRandom(seed)
      learner = CLGLearner(self.agrumSrcDir("CLGLearningTestSuite1.csv"))
      sd = learner.supremum_deviation(n_sample=n, fwer_delta=delta)
      # the same random signs as the ones drawn by supremum_deviation
      gum.initRandom(seed)
      terms = np.array(list(learner.r_XYZ.values()))
      K = terms.shape[1]
      sigma = gum.randomNumpyGenerator().choice([-1.0, 1.0], size=(n, K))

    # each term of the r-centered statistic lies in [-1, 1]
    self.assertLessEqual(np.max(np.abs(terms)), 1.0)

    # Theorem 3.1 of Pellegrina et al. with z = 1, c = 2 and ln(4/delta), whatever the number of tests
    z = 1.0
    c = 2.0
    lam = np.log(4 / delta)
    R = np.mean(np.max(sigma @ terms.T / K, axis=1))
    Rt = R + 2 * z * np.sqrt(lam / (2 * n * K))
    expected = 2 * Rt + np.sqrt(c * (4 * K * Rt + c * lam) * lam) / K + c * lam / K + c * np.sqrt(lam / (2 * K))
    self.assertAlmostEqual(sd, expected, places=10)

  def test_PCMB(self):
    # clg is a CLG model
    clg = CLG()
    # 4 variables
    A = GaussianVariable(mu=4.5, sigma=0.1, name="A")
    B = GaussianVariable(mu=3, sigma=0.1, name="B")
    C = GaussianVariable(mu=9, sigma=0.2, name="C")
    D = GaussianVariable(mu=9, sigma=0.2, name="D")
    idA = clg.add(A)
    idB = clg.add(B)
    idC = clg.add(C)
    idD = clg.add(D)
    # A and B are the parents of C while C is the parent of D
    clg.addArc(idA, idC, 2)
    clg.addArc(idB, idC, 1.5)
    clg.addArc(idC, idD, 1)

    # learning
    learner = CLGLearner(self.agrumSrcDir("CLGLearningTestSuite2.csv"))
    learner.supremum_deviation(n_sample=10, fwer_delta=0.05)

    # Assert
    self.assertEqual(learner.RAveL_PC(idA), {idC})
    self.assertEqual(learner.RAveL_PC(idC), {idA, idB, idD})
    self.assertEqual(learner.RAveL_MB(idA), {idB, idC})

  def test_PCalgo(self):
    # clg is a CLG model
    clg = CLG()
    # 6 variables
    # F - B is oriented by the variances only (no v-structure, no Meek rule): F -> B needs Var(F) < Var(B), here
    # 0.25 < 0.8^2 * 0.25 + 0.5^2 = 0.41, far from the sampling noise (CLGLearningTestSuite1.csv: 20000 records)
    A = GaussianVariable(mu=4.5, sigma=0.3, name="A")
    B = GaussianVariable(mu=3, sigma=0.5, name="B")
    C = GaussianVariable(mu=9, sigma=0.6, name="C")
    D = GaussianVariable(mu=9, sigma=0.7, name="D")
    E = GaussianVariable(mu=9, sigma=0.9, name="E")
    F = GaussianVariable(mu=7, sigma=0.5, name="F")
    idA = clg.add(A)
    idB = clg.add(B)
    idC = clg.add(C)
    idD = clg.add(D)
    idE = clg.add(E)
    idF = clg.add(F)
    # A and B are the parents of C while C is the parent of D
    clg.addArc(idA, idC, 2)
    clg.addArc(idB, idC, 1.5)
    clg.addArc(idC, idD, 1)
    clg.addArc(idD, idE, 1)
    clg.addArc(idF, idB, 0.8)

    # learning
    learner = CLGLearner(self.agrumSrcDir("CLGLearningTestSuite1.csv"))
    learner.supremum_deviation(n_sample=10, fwer_delta=0.05)
    # PC algorithm
    C = learner.PC_algorithm(order=clg.nodes())

    # Assert
    self.assertEqual(C, {0: {2}, 1: {2}, 2: {3}, 3: {4}, 4: set(), 5: {1}})

  def test_ParameterEstimation(self):
    # clg is a CLG model
    clg = CLG()
    # 4 variables
    A = GaussianVariable(mu=4.5, sigma=0.1, name="A")
    B = GaussianVariable(mu=3, sigma=0.1, name="B")
    C = GaussianVariable(mu=9, sigma=0.2, name="C")
    D = GaussianVariable(mu=9, sigma=0.2, name="D")
    idA = clg.add(A)
    idB = clg.add(B)
    idC = clg.add(C)
    idD = clg.add(D)
    # A and B are the parents of C while C is the parent of D
    clg.addArc(idA, idC, 2)
    clg.addArc(idB, idC, 1.5)
    clg.addArc(idC, idD, 1)

    # learning
    learner = CLGLearner(self.agrumSrcDir("CLGLearningTestSuite2.csv"))
    learner.supremum_deviation(n_sample=10, fwer_delta=0.05)
    # PC algorithm
    C = learner.PC_algorithm(order=clg.nodes())
    # Parameter estimation
    id2mu, id2sigma, arc2coef = learner.estimate_parameters(C)

    delta = 1
    # Assert
    for node in clg.nodes():
      self.assertAlmostEqual(id2mu[node], clg.variable(node)._mu, delta=delta)
      self.assertAlmostEqual(id2sigma[node], clg.variable(node)._sigma, delta=delta)

    for arc in clg.arcs():
      self.assertAlmostEqual(arc2coef[arc], clg.coefArc(*arc), delta=delta)

  def test_Rule3NeedsNonAdjacentParents(self):
    # C[x] holds the nodes y with x - y (y in C[x] and x in C[y]) or x -> y (y in C[x] only)
    learner = CLGLearner(self.agrumSrcDir("CLGLearningTestSuite2.csv"))
    # 0 - 2 -> 1, 0 - 3 -> 1, 0 - 1 and 2 - 3: Meek's rule 3 does not apply, as 2 and 3 are adjacent
    C = {0: {1, 2, 3}, 1: {0}, 2: {0, 1, 3}, 3: {0, 1, 2}}
    self.assertEqual(learner.three_rules(C), {0: {1, 2, 3}, 1: {0}, 2: {0, 1, 3}, 3: {0, 1, 2}})
    # without 2 - 3, rule 3 orients 0 -> 1
    C = {0: {1, 2, 3}, 1: {0}, 2: {0, 1}, 3: {0, 1}}
    self.assertEqual(learner.three_rules(C), {0: {1, 2, 3}, 1: set(), 2: {0, 1}, 3: {0, 1}})


ts = unittest.TestSuite()
addTests(ts, CLGLearningTestCase)
