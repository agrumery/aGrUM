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
#   useful, but WITHOUT ANY WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,      #
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

import unittest

from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum
import pyagrum.clg as gclg
from pyagrum.clg import CLG, GaussianVariable

SEM_SIMPLE = """
A=4.5[0.3]
B=3+1.2A[0.5]
C=9+2A+1.5B[0.6]
"""


class CLGModelTestCase(pyAgrumTestCase):

  # --- add ---

  def test_add_returns_id(self):
    clg = CLG()
    nid = clg.add(GaussianVariable("X", 0, 1))
    self.assertIsInstance(nid, int)

  def test_add_none_raises(self):
    clg = CLG()
    with self.assertRaises(ValueError):
      clg.add(None)

  def test_add_empty_name_raises(self):
    clg = CLG()
    with self.assertRaises(NameError):
      clg.add(GaussianVariable("", 0, 1))

  def test_add_duplicate_name_raises(self):
    clg = CLG()
    clg.add(GaussianVariable("X", 0, 1))
    with self.assertRaises(NameError):
      clg.add(GaussianVariable("X", 1, 2))

  # --- addArc ---

  def test_addArc_zero_coef_raises(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    with self.assertRaises(ValueError):
      clg.addArc(idX, idY, 0)

  def test_addArc_by_name(self):
    clg = CLG()
    clg.add(GaussianVariable("X", 0, 1))
    clg.add(GaussianVariable("Y", 0, 1))
    clg.addArc("X", "Y", 2.5)
    self.assertTrue(clg.existsArc("X", "Y"))
    self.assertEqual(clg.coefArc("X", "Y"), 2.5)

  # --- eraseArc ---

  def test_eraseArc_removes_from_arc2coef(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    clg.addArc(idX, idY, 2.0)
    clg.eraseArc(idX, idY)
    self.assertFalse(clg.existsArc(idX, idY))
    self.assertNotIn((idX, idY), clg._arc2coef)

  def test_eraseArc_then_readd(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    clg.addArc(idX, idY, 2.0)
    clg.eraseArc(idX, idY)
    clg.addArc(idX, idY, 3.0)
    self.assertEqual(clg.coefArc(idX, idY), 3.0)

  def test_eraseArc_eq_consistent(self):
    clg1 = gclg.SEM.toclg(SEM_SIMPLE)
    clg2 = gclg.SEM.toclg(SEM_SIMPLE)
    idA = clg1.idFromName("A")
    idB = clg1.idFromName("B")
    clg1.eraseArc(idA, idB)
    self.assertNotEqual(clg1, clg2)

  # --- setCoef ---

  def test_setCoef_nonexistent_arc_raises(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    with self.assertRaises(ValueError):
      clg.setCoef(idX, idY, 1.0)

  def test_setCoef_zero_raises(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    clg.addArc(idX, idY, 1.0)
    with self.assertRaises(ValueError):
      clg.setCoef(idX, idY, 0)

  def test_setCoef_updates_value(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    clg.addArc(idX, idY, 1.0)
    clg.setCoef(idX, idY, -3.5)
    self.assertEqual(clg.coefArc(idX, idY), -3.5)

  # --- idFromName / variable ---

  def test_idFromName_unknown_raises(self):
    clg = CLG()
    with self.assertRaises(pyagrum.NotFound):
      clg.idFromName("nonexistent")

  def test_idFromName_known_roundtrip(self):
    clg = CLG()
    nid = clg.add(GaussianVariable("X", 0, 1))
    self.assertEqual(clg.idFromName("X"), nid)

  # --- coefArc ---

  def test_coefArc_nonexistent_raises(self):
    clg = CLG()
    idX = clg.add(GaussianVariable("X", 0, 1))
    idY = clg.add(GaussianVariable("Y", 0, 1))
    with self.assertRaises(pyagrum.NotFound):
      clg.coefArc(idX, idY)

  # --- copy ---

  def test_copy_independence_mu(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    clg2 = CLG(clg)
    clg2.setMu(clg2.idFromName("A"), 999.0)
    self.assertNotEqual(clg.variable("A").mu(), 999.0)

  def test_copy_independence_sigma(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    clg2 = CLG(clg)
    clg2.setSigma(clg2.idFromName("A"), 99.0)
    self.assertNotEqual(clg.variable("A").sigma(), 99.0)

  def test_copy_same_structure(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    clg2 = CLG(clg)
    self.assertEqual(clg, clg2)

  def test_copy_preserves_node_ids(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    clg2 = CLG(clg)
    self.assertEqual(sorted(clg.nodes()), sorted(clg2.nodes()))
    for nid in clg.nodes():
      self.assertEqual(clg.variable(nid).name(), clg2.variable(nid).name())
      self.assertEqual(clg.variable(nid).mu(), clg2.variable(nid).mu())
      self.assertEqual(clg.variable(nid).sigma(), clg2.variable(nid).sigma())

  def test_copy_preserves_arc_ids(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    clg2 = CLG(clg)
    self.assertEqual(clg.arcs(), clg2.arcs())
    for arc in clg.arcs():
      self.assertEqual(clg.coefArc(arc[0], arc[1]), clg2.coefArc(arc[0], arc[1]))

  # --- __eq__ ---

  def test_eq_different_coef(self):
    clg1 = CLG()
    idX = clg1.add(GaussianVariable("X", 0, 1))
    idY = clg1.add(GaussianVariable("Y", 0, 1))
    clg1.addArc(idX, idY, 1.0)
    clg2 = CLG()
    idX2 = clg2.add(GaussianVariable("X", 0, 1))
    idY2 = clg2.add(GaussianVariable("Y", 0, 1))
    clg2.addArc(idX2, idY2, 2.0)
    self.assertNotEqual(clg1, clg2)

  def test_eq_different_structure(self):
    clg1 = CLG()
    clg1.add(GaussianVariable("X", 0, 1))
    clg1.add(GaussianVariable("Y", 0, 1))
    clg2 = CLG()
    idX2 = clg2.add(GaussianVariable("X", 0, 1))
    idY2 = clg2.add(GaussianVariable("Y", 0, 1))
    clg2.addArc(idX2, idY2, 1.0)
    self.assertNotEqual(clg1, clg2)

  def test_eq_non_clg_returns_not_implemented(self):
    clg = CLG()
    self.assertIs(clg.__eq__("not a CLG"), NotImplemented)

  # --- asDiscreteBN ---

  def test_asDiscreteBN_node_names(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    bn = clg.asDiscreteBN()
    self.assertEqual(set(bn.names()), set(clg.names()))

  def test_asDiscreteBN_arcs(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    bn = clg.asDiscreteBN()
    for arc in clg.arcs():
      pname = clg.variable(arc[0]).name()
      cname = clg.variable(arc[1]).name()
      self.assertTrue(bn.existsArc(pname, cname))

  # --- parent_names / children_names ---

  def test_parent_names(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    self.assertIn("A", clg.parent_names("B"))

  def test_children_names(self):
    clg = gclg.SEM.toclg(SEM_SIMPLE)
    self.assertIn("B", clg.children_names("A"))
    self.assertIn("C", clg.children_names("A"))


ts = unittest.TestSuite()
addTests(ts, CLGModelTestCase)
