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

import pyagrum.credal_net as gum

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


class CredalNetTestCase(pyAgrumTestCase):
  def _manualCredalNet(self):
    cn = gum.CredalNet()
    a = cn.addVariable("A", 2)
    b = cn.addVariable("B", 2)
    cn.addArc(a, b)
    cn.setCPTs(a, [[[0.2, 0.8], [0.6, 0.4]]])
    cn.setCPTs(b, [[[0.1, 0.9]], [[0.3, 0.7], [0.5, 0.5]]])
    return cn, a, b

  def testSetCPTsAndSrcCpt(self):
    cn, a, b = self._manualCredalNet()
    src = cn.credalNet_srcCpt()
    self.assertIsInstance(src, dict)
    self.assertEqual(set(src.keys()), {a, b})
    self.assertEqual(src[a], [[[0.2, 0.8], [0.6, 0.4]]])
    self.assertEqual(src[b], [[[0.1, 0.9]], [[0.3, 0.7], [0.5, 0.5]]])

  def testSetCPTsAcceptsTuples(self):
    cn = gum.CredalNet()
    a = cn.addVariable("A", 2)
    cn.setCPTs(a, (((0.25, 0.75),),))
    self.assertEqual(cn.credalNet_srcCpt()[a], [[[0.25, 0.75]]])

  def testSetCPTsInvalidArguments(self):
    for bad in ("abc", 3, [[["x", "y"]]]):
      cn = gum.CredalNet()
      a = cn.addVariable("A", 2)
      with self.assertRaises(TypeError):
        cn.setCPTs(a, bad)

    cn = gum.CredalNet()
    a = cn.addVariable("A", 2)
    with self.assertRaises(gum.CPTError):
      cn.setCPTs(a, [[[0.5, 0.6]]])
    with self.assertRaises(gum.SizeError):
      cn.setCPTs(a, [[[0.2, 0.3, 0.5]]])

  def testCurrentCptFromBNs(self):
    bn = gum.fastBN("A->B")
    cn = gum.CredalNet(bn, bn)
    cn.intervalToCredal()
    a, b = bn.idFromName("A"), bn.idFromName("B")
    cur = cn.credalNet_currentCpt()
    self.assertEqual(set(cur.keys()), {a, b})
    # bnmin == bnmax: each credal set is the single vertex given by the CPT
    for v, p in zip(cur[a][0][0], bn.cpt(a).tolist()):
      self.assertAlmostEqual(v, p)
    for parent_state in range(2):
      self.assertEqual(len(cur[b][parent_state]), 1)
      for v, p in zip(cur[b][parent_state][0], bn.cpt(b)[{"A": parent_state}].tolist()):
        self.assertAlmostEqual(v, p)

  def testBinaryCPTMinMax(self):
    bn = gum.fastBN("A->B")
    cn = gum.CredalNet(bn, bn)
    cn.intervalToCredal()
    cn.computeBinaryCPTMinMax()
    pmin, pmax = cn.get_binaryCPT_min(), cn.get_binaryCPT_max()
    self.assertEqual([len(x) for x in pmin], [1, 2])
    a, b = bn.idFromName("A"), bn.idFromName("B")
    # bnmin == bnmax: lower and upper probabilities of the "true" modality are the CPT values
    self.assertAlmostEqual(pmin[a][0], bn.cpt(a)[1])
    self.assertAlmostEqual(pmax[a][0], bn.cpt(a)[1])
    for parent_state in range(2):
      self.assertAlmostEqual(pmin[b][parent_state], bn.cpt(b)[{"A": parent_state, "B": 1}])
      self.assertAlmostEqual(pmax[b][parent_state], bn.cpt(b)[{"A": parent_state, "B": 1}])


ts = unittest.TestSuite()
addTests(ts, CredalNetTestCase)
