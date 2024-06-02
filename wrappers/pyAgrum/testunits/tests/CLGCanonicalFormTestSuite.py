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

from numpy import pi
from numpy import log
from pyAgrum.clg.CanonicalForm import CanonicalForm


class CLGCanonicalFormTestCase(pyAgrumTestCase):
  def test_augmentation(self):
    phi = CanonicalForm([1, 3], [[1, 2], [3, 4]], [1, 2], 2)

    phi0 = phi.augment([0])
    phi2 = phi.augment([2])
    phi4 = phi.augment([4])
    phi31 = phi.augment([3, 1])

    self.assertEqual(phi0, CanonicalForm([0, 1, 3],
                                         [[0, 0, 0],
                                          [0, 1, 2],
                                          [0, 3, 4]],
                                         [0, 1, 2],
                                         2))

    self.assertEqual(phi2, CanonicalForm([1, 2, 3],
                                         [[1, 0, 2],
                                          [0, 0, 0],
                                          [3, 0, 4]],
                                         [1, 0, 2],
                                         2))

    self.assertEqual(phi4, CanonicalForm([1, 3, 4],
                                         [[1, 2, 0],
                                          [3, 4, 0],
                                          [0, 0, 0]],
                                         [1, 2, 0],
                                         2))

    self.assertEqual(phi, phi31)

  def test_multiplication_division(self):
    # Example 14.1 of Koller & Friedman

    phi1 = CanonicalForm([0, 1], [[1, -1], [-1, 1]], [1, -1], -3)
    phi2 = CanonicalForm([1, 2], [[3, -2], [-2, 4]], [5, -1], 1)

    product = CanonicalForm([0, 1, 2],
                            [[1, -1, 0], [-1, 4, -2], [0, -2, 4]],
                            [1, 4, -1],
                            -2)

    ratio = CanonicalForm([0, 1, 2],
                          [[1, -1, 0], [-1, -2, 2], [0, 2, -4]],
                          [1, -6, 1],
                          -4)

    inverse = CanonicalForm([0, 1, 2],
                            [[-1, 1, 0], [1, 2, -2], [0, -2, 4]],
                            [-1, 6, -1],
                            4)

    self.assertEqual(phi1 * phi2, product)
    self.assertEqual(phi2 * phi1, product)
    self.assertEqual(phi1 / phi2, ratio)
    self.assertEqual(phi2 / phi1, inverse)

  def test_fromCLG(self):
    cf1 = CanonicalForm.fromCLG(0, [], -4, 2, [])
    cf2 = CanonicalForm.fromCLG(0, [1], 3, 0.5, [-0.5])

    g1 = - 2 - log(8 * pi) / 2
    g2 = - 18 - log(pi / 2) / 2

    self.assertEqual(cf1, CanonicalForm([0], [[.25]], [-1], g1))
    self.assertEqual(cf2, CanonicalForm([0, 1], [[4, 2], [2, 1]], [12, 6], g2))

  def test_toGaussian(self):
    cf = CanonicalForm([0], [[.25]], [-1], -2 - log(8 * pi) / 2)
    scope, mu, gamma = cf.toGaussian()

    self.assertEqual(scope, [0])
    self.assertEqual(mu, -4)
    self.assertEqual(gamma, 4)

  def test_marginalize(self):
    # Example 4.1 from "Learning Bayesian networks, R. Neapolitan (2004)"
    cf1 = CanonicalForm.fromCLG(0, [], 40, 5, [])
    cf2 = CanonicalForm.fromCLG(1, [0], 0, 30, [10])

    cf = (cf1 * cf2).marginalize([0])
    scope, mu, gamma = cf.toGaussian()

    delta = 1e-7
    self.assertAlmostEqual(mu[0][0], 400., delta=delta)
    self.assertAlmostEqual(gamma[0][0], 3400., delta=delta)

  def test_reduce(self):
    # Example 4.2 from "Learning Bayesian networks, R. Neapolitan (2004)"
    cf1 = CanonicalForm.fromCLG(0, [], 40, 5, [])
    cf2 = CanonicalForm.fromCLG(1, [0], 0, 30, [10])

    scope, mu, gamma = (cf1 * cf2.reduce({1: 300})).toGaussian()

    delta = 1e-2
    self.assertAlmostEqual(mu[0][0], 32.65, delta=delta)
    self.assertAlmostEqual(gamma[0][0], 6.62, delta=delta)

    cf3 = cf1.reduce({4: 400})  # Reducing a variable not in scope

    self.assertEqual(cf1, cf3)


ts = unittest.TestSuite()
addTests(ts, CLGCanonicalFormTestCase)
