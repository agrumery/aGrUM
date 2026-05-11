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

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class StructuralMetricsCompareBNTestCase(pyAgrumTestCase):
  def _assertPerfectMetrics(self, comp):
    for fn in [comp.precision_skeleton, comp.recall_skeleton, comp.f_score_skeleton,
               comp.precision, comp.recall, comp.f_score]:
      self.assertAlmostEqual(fn(), 1.0)
    self.assertEqual(int(comp.shd_skeleton()), 0)
    self.assertEqual(int(comp.shd()), 0)

  def testCompareBN(self):
    # compare(BayesNet, BayesNet) overload — values from C++ test_bn
    # Both BNs matched by variable name, comparison runs on essential graphs.
    # bn1 essential graph: edges 0-1, 0-2 (no v-structure)
    # bn2 essential graph: edges 0-1, 0-2, 1-2 (triangle, no v-structure)
    bn1 = gum.fastBN("0->1;0->2")
    bn2 = gum.fastBN("0->1->2;0->2")
    comp = gum.StructuralMetrics()
    comp.compare(bn1, bn2)
    self.assertAlmostEqual(comp.precision_skeleton(), 2.0 / 3.0, places=3)
    self.assertAlmostEqual(comp.recall_skeleton(), 1.0, places=3)
    self.assertAlmostEqual(comp.f_score_skeleton(), 0.8, places=3)
    self.assertEqual(int(comp.shd_skeleton()), 1)
    self.assertAlmostEqual(comp.precision(), 2.0 / 3.0, places=3)
    self.assertAlmostEqual(comp.recall(), 1.0, places=3)
    self.assertAlmostEqual(comp.f_score(), 0.8, places=3)
    self.assertEqual(int(comp.shd()), 1)

  def testCompareBNNameAlignment(self):
    # Same arc structure, different variable-appearance order in fastBN =>
    # NodeIds differ but names match. All metrics must be 1 / shd must be 0.
    # g1: D->A, B->A, C->B  (v-structure at A: D and B not adjacent)
    # g2: C->B, B->A, D->A  (same arcs, different id assignment)
    g1 = gum.fastBN("D->A<-B<-C")
    g2 = gum.fastBN("C->B->A<-D")
    comp = gum.StructuralMetrics()
    comp.compare(g1, g2)
    self._assertPerfectMetrics(comp)


class StructuralMetricsSIDTestCase(pyAgrumTestCase):
  def testSidBNSelfIsZero(self):
    bn = gum.fastBN("A->B->C")
    comp = gum.StructuralMetrics()
    self.assertEqual(int(comp.sid(bn, bn)), 0)

  def testSidBNNameAlignment(self):
    # Same structure, different variable order => sid = 0
    g1 = gum.fastBN("D->A<-B<-C")
    g2 = gum.fastBN("C->B->A<-D")
    comp = gum.StructuralMetrics()
    self.assertEqual(int(comp.sid(g1, g2)), 0)

  def testSidBNReversedArc(self):
    # Peters & Bühlmann 2015, Figure 2 (via C++ test_sid_bn_overload):
    # G has 0->1; h2 is G with that arc reversed (1->0). sid(G, h2) = 8.
    g  = gum.fastBN("0->1;0->2;1->2;0->3;1->3;0->4;1->4")
    h2 = gum.fastBN("1->0;0->2;1->2;0->3;1->3;0->4;1->4")
    comp = gum.StructuralMetrics()
    self.assertEqual(int(comp.sid(g, h2)), 8)

  def testSidBNSubgraphZero(self):
    # Proposition 7 (Peters & Bühlmann): pa_G(j) ⊆ pa_H(j) for all j => sid(G, H) = 0.
    # G: A->B, A->C   H: A->B, A->C, B->C  (G ⊆ H)
    g = gum.fastBN("A->B;A->C")
    h = gum.fastBN("A->B;A->C;B->C")
    comp = gum.StructuralMetrics()
    self.assertEqual(int(comp.sid(g, h)), 0)


ts = unittest.TestSuite()
addTests(ts, StructuralMetricsCompareBNTestCase)
addTests(ts, StructuralMetricsSIDTestCase)
