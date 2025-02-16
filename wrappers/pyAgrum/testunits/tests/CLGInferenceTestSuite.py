############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
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
#      or (at your option) any later version.                              #
#    - the MIT license (MIT)                                               #
#    - or both in dual license, as here                                    #
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
import math

from numpy import allclose

from pyAgrum.clg.CLG import CLG
from pyAgrum.clg.GaussianVariable import GaussianVariable
from pyAgrum.clg.variableElimination import CLGVariableElimination

from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class CLGInferenceTestCase(pyAgrumTestCase):
    def test_simple(self):
        # This test uses the fact that a multivariate gaussian can be converted
        # into a CLG and conversely. The relation between the parameters of the
        # multivariate gaussian and those of a CLG is given by:
        # If Sigma = [[nu_x, nu_xy], [nu_xy, nu_y]] and mu = [mu_x, mu_y]
        # then nu_0 = nu_x, nu_1 = nu_y - nu_xy^2/nu_x, b_21 = nu_xy/nu_x
        # mu_1 = mu_x and mu_2 = mu_y - nu_xy/nu_x * mu_x.

        true_cov_mat = [[1, -1], [-1, 5]]
        true_mean_vec = [[0.5], [-0.5]]

        clg = CLG()

        # Defining the CLG corresponding to the multivariate gaussian
        X = GaussianVariable(mu=0.5, sigma=1, name='X')
        Y = GaussianVariable(mu=0, sigma=2, name='Y')

        idX = clg.add(X)
        idY = clg.add(Y)

        clg.addArc(idX, idY, -1)

        ie = CLGVariableElimination(clg)

        scope, mu, Sigma = ie.canonicalPosterior(['X', 'Y']).toGaussian()

        self.assertEqual(allclose(true_mean_vec, mu, 1e-7), True)
        self.assertEqual(allclose(true_cov_mat, Sigma, 1e-7), True)

    def test_examples_4_12(self):
        # Examples 4.1 an 4.2 from "Learning Bayesian Networks, R. Neapolitan (2004)"
        clg = CLG()

        X = GaussianVariable(mu=40, sigma=5, name='X')
        Y = GaussianVariable(mu=0, sigma=30, name='Y')

        idX = clg.add(X)
        idY = clg.add(Y)

        # X is the parent of Y
        clg.addArc(idX, idY, 10)

        ie = CLGVariableElimination(clg)

        vY = ie.posterior('Y')

        ie.updateEvidence({'Y': 300})

        vX_y = ie.posterior('X')

        self.assertEqual(ie.hasEvidence('Y'), True)
        self.assertEqual(ie.nbrEvidence(), 1)

        delta = 1e-7
        self.assertAlmostEqual(vY.mu(), 400., delta=delta)
        self.assertAlmostEqual(vY.sigma(), math.sqrt(3400.), delta=delta)

        delta = 1e-2
        self.assertAlmostEqual(vX_y.mu(), 32.65, delta=delta)
        self.assertAlmostEqual(vX_y.sigma(), math.sqrt(6.62), delta=delta)

    def test_example_4_5(self):
        # Examples 4.5 from "Learning Bayesian Networks, R. Neapolitan (2004)"
        clg = CLG()

        # Replace Infinity by a high value (1e7)
        P = GaussianVariable(mu=0, sigma=1e7, name='P')  # No information on P
        D = GaussianVariable(mu=0, sigma=300, name='D')

        idP = clg.add(P)
        idD = clg.add(D)

        # P is the parent of D
        clg.addArc(idP, idD, 1)

        ie = CLGVariableElimination(clg)
        ie.updateEvidence({'D': 8000})

        posteriorP = ie.posterior('P')

        delta = 1e-2
        self.assertAlmostEqual(posteriorP.mu(), 8000., delta=delta)
        self.assertAlmostEqual(
            posteriorP.sigma(), math.sqrt(90000.), delta=delta)

    def test_examples_4_67(self):
        # Examples 4.6 and 4.7 from "Learning Bayesian Networks, R. Neapolitan (2004)"
        clg = CLG()

        P = GaussianVariable(mu=0, sigma=1e7, name='P')
        M = GaussianVariable(mu=0, sigma=1e7, name='M')
        D = GaussianVariable(mu=0, sigma=300, name='D')

        idP = clg.add(P)
        idM = clg.add(M)
        idD = clg.add(D)

        # P is the parent of D
        clg.addArc(idP, idD, 1)
        clg.addArc(idM, idD, 1)

        ie = CLGVariableElimination(clg)
        ie.updateEvidence({'D': 8000})

        posteriorP = ie.canonicalPosterior(['P'])
        scopeP, muP, gammaP = posteriorP.toGaussian()

        delta = 1e-2
        self.assertAlmostEqual(muP[0][0], 4000., delta=delta)
        self.assertGreaterEqual(gammaP[0][0], 1e7)

        ie.updateEvidence({'M': 1000})

        posteriorP = ie.canonicalPosterior(['P'])
        scopeP, muP, gammaP = posteriorP.toGaussian()

        delta = 1e-2
        self.assertAlmostEqual(muP[0][0], 7000., delta=delta)
        self.assertAlmostEqual(gammaP[0][0], 90000., delta=delta)


ts = unittest.TestSuite()
addTests(ts, CLGInferenceTestCase)
