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
