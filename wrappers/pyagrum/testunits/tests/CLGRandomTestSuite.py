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

from .pyAgrumTestSuite import pyAgrumTestCase, addTests

from pyagrum.clg import CLG
from pyagrum.clg import GaussianVariable
from pyagrum.clg.CLG import randomCLG


class CLGRandomTestCase(pyAgrumTestCase):
  NAMES = ["A", "B", "C", "D", "E"]
  N = len(NAMES)

  # ------------------------------------------------------------------ #
  # Basic structure                                                      #
  # ------------------------------------------------------------------ #

  def test_returns_clg(self):
    clg = randomCLG(self.N, self.NAMES)
    self.assertIsInstance(clg, CLG)

  def test_node_count(self):
    clg = randomCLG(self.N, self.NAMES)
    self.assertEqual(len(clg.nodes()), self.N)

  def test_node_names(self):
    clg = randomCLG(self.N, self.NAMES)
    self.assertEqual(set(clg.names()), set(self.NAMES))

  # ------------------------------------------------------------------ #
  # Parameter ranges                                                     #
  # ------------------------------------------------------------------ #

  def test_sigma_strictly_positive(self):
    for _ in range(10):
      clg = randomCLG(self.N, self.NAMES, SigmaMin=0.5, SigmaMax=3.0)
      for node in clg.nodes():
        self.assertGreater(clg.variable(node).sigma(), 0)

  def test_sigma_in_bounds(self):
    lo, hi = 2.0, 4.0
    for _ in range(10):
      clg = randomCLG(self.N, self.NAMES, SigmaMin=lo, SigmaMax=hi)
      for node in clg.nodes():
        s = clg.variable(node).sigma()
        self.assertGreaterEqual(s, lo)
        self.assertLessEqual(s, hi)

  def test_mu_in_bounds(self):
    lo, hi = -2.0, 2.0
    for _ in range(10):
      clg = randomCLG(self.N, self.NAMES, MuMin=lo, MuMax=hi)
      for node in clg.nodes():
        m = clg.variable(node).mu()
        self.assertGreaterEqual(m, lo)
        self.assertLessEqual(m, hi)

  def test_arc_coef_absolute_in_bounds(self):
    lo, hi = 2.0, 5.0
    for _ in range(10):
      clg = randomCLG(self.N, self.NAMES, ArcCoefMin=lo, ArcCoefMax=hi)
      for arc in clg.arcs():
        c = abs(clg.coefArc(arc[0], arc[1]))
        self.assertGreaterEqual(c, lo)
        self.assertLessEqual(c, hi)

  def test_arc_coef_nonzero(self):
    for _ in range(10):
      clg = randomCLG(self.N, self.NAMES)
      for arc in clg.arcs():
        self.assertNotEqual(clg.coefArc(arc[0], arc[1]), 0)

  # ------------------------------------------------------------------ #
  # max_parents                                                          #
  # ------------------------------------------------------------------ #

  def test_max_parents_none_no_constraint(self):
    clg = randomCLG(self.N, self.NAMES, max_parents=None)
    self.assertIsInstance(clg, CLG)

  def test_max_parents_respected(self):
    for max_p in (1, 2):
      for _ in range(10):
        clg = randomCLG(self.N, self.NAMES, max_parents=max_p)
        for node in clg.nodes():
          self.assertLessEqual(len(clg.parents(node)), max_p)

  # ------------------------------------------------------------------ #
  # ratio_arc                                                            #
  # ------------------------------------------------------------------ #

  def test_ratio_arc_increases_density(self):
    # aGrUM uses an internal C++ RNG so Python seeding is ineffective.
    # Compare average arc counts over many samples with n=10.
    big_names = [f"X{i}" for i in range(10)]
    n = len(big_names)
    SAMPLES = 30
    avg_sparse = sum(len(randomCLG(n, big_names, ratio_arc=1.2).arcs()) for _ in range(SAMPLES)) / SAMPLES
    avg_dense = sum(len(randomCLG(n, big_names, ratio_arc=2.0).arcs()) for _ in range(SAMPLES)) / SAMPLES
    self.assertLess(avg_sparse, avg_dense)

  # ------------------------------------------------------------------ #
  # Validation errors                                                    #
  # ------------------------------------------------------------------ #

  def test_error_names_length_mismatch(self):
    with self.assertRaises(ValueError):
      randomCLG(6, self.NAMES)

  def test_error_nb_variables_too_small(self):
    with self.assertRaises(ValueError):
      randomCLG(3, ["A", "B", "C"])

  def test_error_sigma_min_zero(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, SigmaMin=0)

  def test_error_sigma_min_negative(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, SigmaMin=-1)

  def test_error_sigma_min_greater_than_max(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, SigmaMin=5, SigmaMax=2)

  def test_error_mu_min_greater_than_max(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, MuMin=10, MuMax=0)

  def test_error_coef_min_zero(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, ArcCoefMin=0)

  def test_error_coef_min_greater_than_max(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, ArcCoefMin=8, ArcCoefMax=3)

  def test_error_max_parents_zero(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, max_parents=0)

  def test_error_ratio_arc_zero(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, ratio_arc=0)

  def test_error_ratio_arc_negative(self):
    with self.assertRaises(ValueError):
      randomCLG(self.N, self.NAMES, ratio_arc=-1.0)

  # ------------------------------------------------------------------ #
  # GaussianVariable sigma validation                                   #
  # ------------------------------------------------------------------ #

  def test_gaussian_variable_sigma_zero_allowed(self):
    v = GaussianVariable(name="X", mu=0, sigma=0)
    self.assertEqual(v.sigma(), 0)

  def test_gaussian_variable_sigma_negative_raises(self):
    with self.assertRaises(ValueError):
      GaussianVariable(name="X", mu=0, sigma=-1)

  def test_gaussian_variable_set_sigma_zero_allowed(self):
    v = GaussianVariable(name="X", mu=0, sigma=1)
    v.setSigma(0)
    self.assertEqual(v.sigma(), 0)

  def test_gaussian_variable_set_sigma_negative_raises(self):
    v = GaussianVariable(name="X", mu=0, sigma=1)
    with self.assertRaises(ValueError):
      v.setSigma(-2)


ts = unittest.TestSuite()
addTests(ts, CLGRandomTestCase)
