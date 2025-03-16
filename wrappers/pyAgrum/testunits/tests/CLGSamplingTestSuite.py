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

from numpy import allclose

import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

from pyAgrum.clg import CLG
from pyAgrum.clg import GaussianVariable
from pyAgrum.clg import ForwardSampling
from pyAgrum.clg import CLGVariableElimination


class CLGSamplingTestCase(pyAgrumTestCase):
  def test_simple(self):
    # clg is a CLG model
    clg = CLG()
    # prog=«sigma=2;X=N(5);Y=N(3);Z=X+Y»
    # we have 3 variables
    X = GaussianVariable(mu=4.5, sigma=2, name='X')
    Y = GaussianVariable(mu=3, sigma=2, name='Y')
    Z = GaussianVariable(mu=9, sigma=4, name='Z')

    idY = clg.add(Y)
    idX = clg.add(X)
    idZ = clg.add(Z)
    # X and Y are the parents of Z
    clg.addArc(idX, idZ, 1)
    clg.addArc(idY, idZ, 1.5)

    fs = ForwardSampling(clg)
    fs.makeSample(10000)

    DELTA = 2e-1
    self.assertAlmostEqual(X.mu(), fs.mean_sample("X"), delta=DELTA)
    self.assertAlmostEqual(Y.mu(), fs.mean_sample("Y"), delta=DELTA)
    self.assertAlmostEqual(1 * X.mu() + 1.5 * Y.mu() + Z.mu(), fs.mean_sample('Z'), delta=DELTA)

  def test_compare(self):
    X = GaussianVariable(mu=0.5, sigma=1, name='X')
    Y = GaussianVariable(mu=0, sigma=2, name='Y')

    clg = CLG()
    idX = clg.add(X)
    idY = clg.add(Y)

    clg.addArc(idX, idY, -1)

    ie = CLGVariableElimination(clg)
    vx = ie.posterior('X')
    vy = ie.posterior('Y')

    fs = ForwardSampling(clg)
    fs.makeSample(10000)

    DELTA = 1e-1
    self.assertAlmostEqual(vx.mu(), fs.mean_sample("X"), delta=DELTA)
    self.assertAlmostEqual(vy.mu(), fs.mean_sample("Y"), delta=DELTA)
    self.assertAlmostEqual(vx.sigma(), fs.stddev_sample("X"), delta=DELTA)
    self.assertAlmostEqual(vy.sigma(), fs.stddev_sample("Y"), delta=DELTA)


ts = unittest.TestSuite()
addTests(ts, CLGSamplingTestCase)
