# -*- coding: utf-8 -*-
# (c) Copyright 2022-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)
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

# __author__ = "Pierre-Henri Wuillemin, Zhe Huang"

# python3 gumTest.py quick_clg
import unittest

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

from pyAgrum.clg.CLG import CLG
from pyAgrum.clg.GaussianVariable import GaussianVariable
from pyAgrum.clg.learning import CLGLearner


class CLGLearningTestCase(pyAgrumTestCase):
  def test_TestIndep(self):
    # clg is a CLG model      
    clg = CLG()
    # 4 variables
    A = GaussianVariable(mu=4.5, sigma=0.1, name='A')
    B = GaussianVariable(mu=3, sigma=0.1, name='B')
    C = GaussianVariable(mu=9, sigma=0.2, name='C')
    D = GaussianVariable(mu=9, sigma=0.2, name='D')
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
    A = GaussianVariable(mu=4.5, sigma=0.1, name='A')
    B = GaussianVariable(mu=3, sigma=0.1, name='B')
    C = GaussianVariable(mu=9, sigma=0.2, name='C')
    D = GaussianVariable(mu=9, sigma=0.2, name='D')
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
    A = GaussianVariable(mu=4.5, sigma=0.3, name='A')
    B = GaussianVariable(mu=3, sigma=0.3, name='B')
    C = GaussianVariable(mu=9, sigma=0.6, name='C')
    D = GaussianVariable(mu=9, sigma=0.7, name='D')
    E = GaussianVariable(mu=9, sigma=0.9, name='E')
    F = GaussianVariable(mu=7, sigma=0.5, name='F')
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
    A = GaussianVariable(mu=4.5, sigma=0.1, name='A')
    B = GaussianVariable(mu=3, sigma=0.1, name='B')
    C = GaussianVariable(mu=9, sigma=0.2, name='C')
    D = GaussianVariable(mu=9, sigma=0.2, name='D')
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
