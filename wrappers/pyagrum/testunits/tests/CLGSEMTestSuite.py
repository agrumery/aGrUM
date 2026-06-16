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

import pickle
import pyagrum.clg as gclg

SEM1 = """
A=4.5 [0.3] # comments are allowed
F=7 [0.5]
B=3 + 1.2F [0.3]
C=9 +  2A + 1.5B [0.6]
D=9 + C + F[0.7]
E=9 + D [0.9]
"""


class CLGSEMTestCase(pyAgrumTestCase):
  def test_names(self):
    model = gclg.CLG()
    model.add(gclg.GaussianVariable("A", 1, 1))
    self.assertEqual(model.names(), ["A"])

  def test_names_from_sem(self):
    sem = SEM1
    model = gclg.SEM.toclg(sem)
    self.assertEqual(set(model.names()), {"A", "F", "B", "C", "D", "E"})

  def test_sem1_IO(self):
    sem = SEM1
    model = gclg.SEM.toclg(sem)

    sem_filename = self.agrumSrcDir("test_model.sem")
    gclg.SEM.saveCLG(model, sem_filename)
    model2 = gclg.SEM.loadCLG(sem_filename)
    self.assertEqual(model, model2)

  def test_clg_pickle(self):
    sem = SEM1
    model = gclg.SEM.toclg(sem)

    pkl_filename = self.agrumSrcDir("test_model.pkl")
    with open(pkl_filename, "bw") as f:
      pickle.dump(model, f)
    with open(pkl_filename, "br") as f:
      model2 = pickle.load(f)
    self.assertEqual(model, model2)

  def test_negative_coef_roundtrip(self):
    import warnings

    sem = """
A=4.5[0.3]
B=3-2.5A[0.5]
"""
    model = gclg.SEM.toclg(sem)
    sem_out = gclg.SEM.tosem(model)
    model2 = gclg.SEM.toclg(sem_out)
    self.assertEqual(model, model2)
    self.assertAlmostEqual(model2.coefArc("A", "B"), -2.5, delta=1e-9)

  def test_mu_zero_roundtrip(self):
    sem = """
A=0.0[0.3]
B=0.0+1.5A[0.5]
"""
    model = gclg.SEM.toclg(sem)
    sem_out = gclg.SEM.tosem(model)
    model2 = gclg.SEM.toclg(sem_out)
    self.assertEqual(model, model2)
    self.assertAlmostEqual(model2.variable("A").mu(), 0.0, delta=1e-9)
    self.assertAlmostEqual(model2.variable("B").mu(), 0.0, delta=1e-9)

  def test_sem_instantiation_warns(self):
    import warnings

    with warnings.catch_warnings(record=True) as w:
      warnings.simplefilter("always")
      gclg.SEM()
      self.assertTrue(len(w) > 0)

  def test_syntax_error_bad_line_raises(self):
    bad_sem = "X 4.5 [0.3]"
    with self.assertRaises(SyntaxError):
      gclg.SEM.toclg(bad_sem)

  def test_comments_ignored(self):
    sem = """
# full comment line
A=4.5[0.3]  # inline comment
B=3+1.2A[0.5]
"""
    model = gclg.SEM.toclg(sem)
    self.assertIn("A", model.names())
    self.assertIn("B", model.names())


ts = unittest.TestSuite()
addTests(ts, CLGSEMTestCase)
