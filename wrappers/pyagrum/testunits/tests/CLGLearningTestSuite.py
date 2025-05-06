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

from .pyAgrumTestSuite import pyAgrumTestCase, addTests

from pyagrum.clg.CLG import CLG
from pyagrum.clg.GaussianVariable import GaussianVariable
from pyagrum.clg.learning import CLGLearner


class CLGLearningTestCase(pyAgrumTestCase):
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
    A = GaussianVariable(mu=4.5, sigma=0.3, name="A")
    B = GaussianVariable(mu=3, sigma=0.3, name="B")
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


ts = unittest.TestSuite()
addTests(ts, CLGLearningTestCase)
