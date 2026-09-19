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

import os
import tempfile
import unittest

import pyagrum.ktbn as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

AT = gum.KTBN.ATEMPORAL


def _buildK3Model():
  m = gum.KTBN(3)
  m.add(gum.LabelizedVariable("X", "", 2), True)
  m.add(gum.LabelizedVariable("Y", "", 2), True)
  m.add(gum.LabelizedVariable("C", "", 2), False)
  m.addArc("X", 0, "X", 2)
  m.addArc("X", 1, "X", 2)
  m.addArc("X", 2, "Y", 2)
  m.addArc("C", AT, "X", 2)
  m.generateCPTs()
  return m


def _flatCPT(cpt):
  inst = gum.Instantiation(cpt)
  vals = []
  inst.setFirst()
  while not inst.end():
    vals.append(cpt[inst])
    inst.inc()
  return vals


def _cptsAlmostEqual(a, b, tc, places=7):
  va, vb = _flatCPT(a), _flatCPT(b)
  tc.assertEqual(len(va), len(vb))
  for x, y in zip(va, vb):
    tc.assertAlmostEqual(x, y, places=places)


class KTBNPersistenceTestCase(pyAgrumTestCase):
  def setUp(self):
    self._dir = tempfile.mkdtemp()

  def _path(self, name):
    return os.path.join(self._dir, name)

  def _checkK3RoundTrip(self, orig, m2):
    self.assertEqual(m2.k(), 3)
    self.assertEqual(m2.nbTemporalVars(), 2)
    self.assertEqual(m2.nbAtemporalVars(), 1)
    self.assertTrue(m2.existsArc("X", 0, "X", 2))
    self.assertTrue(m2.existsArc("X", 1, "X", 2))
    self.assertTrue(m2.existsArc("X", 2, "Y", 2))
    self.assertTrue(m2.existsArc("C", AT, "X", 2))
    for base, slice_ in (("X", 0), ("X", 1), ("X", 2), ("Y", 0), ("Y", 1), ("Y", 2)):
      _cptsAlmostEqual(m2.cpt(base, slice_), orig.cpt(base, slice_), self)
    _cptsAlmostEqual(m2.cpt("C"), orig.cpt("C"), self)

  def testSaveLoadRoundTrip(self):
    m = _buildK3Model()

    fn = self._path("roundtrip.jgum")
    m.save(fn)
    self._checkK3RoundTrip(m, gum.KTBN.load(fn))

    fnb = self._path("roundtrip.bgum")
    m.save(fnb)
    self._checkK3RoundTrip(m, gum.KTBN.load(fnb))

  def testSaveLoadCPTValues(self):
    m = gum.KTBN(2)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("X", 0, "X", 1)
    m.addArc("C", AT, "X", 1)

    m.cpt("X", 0).fillWith([0.7, 0.3])
    m.cpt("C").fillWith([0.4, 0.6])
    m.cpt("X", 1).fillWith([0.9, 0.1, 0.2, 0.8, 0.6, 0.4, 0.3, 0.7])

    def check(loaded):
      self.assertEqual(loaded.k(), 2)
      self.assertTrue(loaded.exists("X"))
      self.assertTrue(loaded.exists("C"))
      self.assertTrue(loaded.existsArc("X", 0, "X", 1))
      self.assertTrue(loaded.existsArc("C", AT, "X", 1))
      _cptsAlmostEqual(loaded.cpt("X", 0), m.cpt("X", 0), self)
      _cptsAlmostEqual(loaded.cpt("C"), m.cpt("C"), self)
      _cptsAlmostEqual(loaded.cpt("X", 1), m.cpt("X", 1), self)

    fnj = self._path("cptvalues.jgum")
    fnb = self._path("cptvalues.bgum")
    m.save(fnj)
    check(gum.KTBN.load(fnj))
    m.save(fnb)
    check(gum.KTBN.load(fnb))

  def testExtensionResolution(self):
    m = _buildK3Model()
    stem = self._path("noext")

    m.save(stem)  # no extension -> append ".bgum"
    self.assertTrue(os.path.exists(stem + ".bgum"))
    self._checkK3RoundTrip(m, gum.KTBN.load(stem))

  def testLoadFallbackFromBN(self):
    bn = gum.BayesNet()
    bn.add(gum.LabelizedVariable("X[0]", "", 2))
    bn.add(gum.LabelizedVariable("X[1]", "", 2))
    bn.add(gum.LabelizedVariable("C", "", 2))
    bn.addArc(bn.idFromName("X[0]"), bn.idFromName("X[1]"))
    bn.addArc(bn.idFromName("C"), bn.idFromName("X[1]"))
    bn.generateCPTs()

    fn = self._path("plainbn.jgum")
    bn.saveGUM(fn)  # plain BN writer: NO KTBN.* properties in the file

    loaded = gum.KTBN.load(fn)
    self.assertEqual(loaded.k(), 2)
    self.assertEqual(loaded.nbTemporalVars(), 1)
    self.assertEqual(loaded.nbAtemporalVars(), 1)
    self.assertTrue(loaded.existsArc("X", 0, "X", 1))
    self.assertTrue(loaded.existsArc("C", AT, "X", 1))
    _cptsAlmostEqual(loaded.cpt("X", 1), bn.cpt(bn.idFromName("X[1]")), self)

  def testK1SaveLoad(self):
    m = gum.KTBN(1)
    m.add(gum.LabelizedVariable("X", "", 2), True)
    m.add(gum.LabelizedVariable("C", "", 2), False)
    m.addArc("C", AT, "X", 0)
    m.cpt("C").fillWith([0.4, 0.6])
    m.cpt("X", 0).fillWith([0.3, 0.7, 0.8, 0.2])  # P(X[0] | C)

    def check(loaded):
      self.assertEqual(loaded.k(), 1)
      self.assertEqual(loaded.nbTemporalVars(), 1)
      self.assertEqual(loaded.nbAtemporalVars(), 1)
      self.assertTrue(loaded.existsArc("C", AT, "X", 0))
      _cptsAlmostEqual(loaded.cpt("X", 0), m.cpt("X", 0), self)
      _cptsAlmostEqual(loaded.cpt("C"), m.cpt("C"), self)

    fnj = self._path("k1.jgum")
    fnb = self._path("k1.bgum")
    m.save(fnj)
    check(gum.KTBN.load(fnj))
    m.save(fnb)
    check(gum.KTBN.load(fnb))

  def testSaveKTBNFreeFunction(self):
    # gum.saveKTBN/gum.loadKTBN mirror KTBN.save()/KTBN.load() (like
    # saveBN/loadBN, saveMRF/loadMRF).
    m = _buildK3Model()
    fn = self._path("free_functions.jgum")
    gum.saveKTBN(m, fn)
    self._checkK3RoundTrip(m, gum.loadKTBN(fn))


ts = unittest.TestSuite()
addTests(ts, KTBNPersistenceTestCase)
