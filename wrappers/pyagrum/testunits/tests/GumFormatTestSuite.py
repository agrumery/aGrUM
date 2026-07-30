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
import pickle
import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests


class TestGumFormat(pyAgrumTestCase):
  # ── BayesNet ──────────────────────────────────────────────────────────────

  def testBN_jgum_roundtrip(self):
    bn = gum.fastBN("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A")
    filename = self.agrumSrcDir("test_bn.jgum")
    gum.saveBN(bn, filename)
    bn2 = gum.loadBN(filename)
    self.assertEqual(bn, bn2)

  def testBN_bgum_roundtrip(self):
    bn = gum.fastBN("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A")
    filename = self.agrumSrcDir("test_bn.bgum")
    gum.saveBN(bn, filename)
    bn2 = gum.loadBN(filename)
    self.assertEqual(bn, bn2)

  def testBN_saveGUMstring_roundtrip(self):
    bn = gum.fastBN("A{Yes|Maybe|No}->B[1,5,10,100]->C<-A")
    s = bn.saveGUMstring()
    self.assertIsInstance(s, str)
    self.assertGreater(len(s), 0)

    bn2 = gum.BayesNet()
    bn2.loadGUMstring(s)
    self.assertEqual(bn, bn2)

  def testBN_empty_roundtrip(self):
    bn = gum.BayesNet()
    s = bn.saveGUMstring()
    bn2 = gum.BayesNet()
    bn2.loadGUMstring(s)
    self.assertEqual(bn2.size(), 0)

  def testBN_properties_preserved(self):
    bn = gum.fastBN("A->B->C")
    bn.setProperty("author", "test")
    filename = self.agrumSrcDir("test_bn_props.jgum")
    gum.saveBN(bn, filename)
    bn2 = gum.loadBN(filename)
    self.assertEqual(bn2.property("author"), "test")

  def testBN_pickle_via_jgum(self):
    bn = gum.fastBN("A->B->C")
    bn.setProperty("tag", "pickle_test")
    data = pickle.dumps(bn)
    bn2 = pickle.loads(data)
    self.assertEqual(bn, bn2)
    self.assertEqual(bn2.property("tag"), "pickle_test")

  def _labelizedVar(self, name, labels):
    v = gum.LabelizedVariable(name, "", 0)
    for label in labels:
      v.addLabel(label)
    return v

  def testBN_aggregatorsAndICI_jgum_roundtrip(self):
    labels = ["no", "yes"]
    bn = gum.BayesNet()
    idP1 = bn.add(self._labelizedVar("P1", labels))
    idP2 = bn.add(self._labelizedVar("P2", labels))

    idAnd = bn.addAND(self._labelizedVar("And", labels))
    idOr = bn.addOR(self._labelizedVar("Or", labels))
    idForall = bn.addFORALL(self._labelizedVar("Forall", labels), 1)
    idSum = bn.addSUM(self._labelizedVar("Sum", labels))
    idNoisyOR = bn.addNoisyOR(self._labelizedVar("NoisyOR", labels), 0.1)
    idLogit = bn.addLogit(self._labelizedVar("Logit", labels), 0.1)

    for nid in (idAnd, idOr, idForall, idSum):
      bn.addArc(idP1, nid)
      bn.addArc(idP2, nid)
    for nid in (idNoisyOR, idLogit):
      bn.addWeightedArc(idP1, nid, 0.3)
      bn.addWeightedArc(idP2, nid, 0.6)

    filename = self.agrumSrcDir("test_bn_aggregators.jgum")
    gum.saveBN(bn, filename)
    bn2 = gum.loadBN(filename)
    self.assertEqual(bn, bn2)

  def testBN_aggregatorsAndICI_bgum_roundtrip(self):
    labels = ["no", "yes"]
    bn = gum.BayesNet()
    idP1 = bn.add(self._labelizedVar("P1", labels))
    idP2 = bn.add(self._labelizedVar("P2", labels))
    idForall = bn.addFORALL(self._labelizedVar("Forall", labels), 1)
    idNoisyOR = bn.addNoisyOR(self._labelizedVar("NoisyOR", labels), 0.1)
    bn.addArc(idP1, idForall)
    bn.addArc(idP2, idForall)
    bn.addWeightedArc(idP1, idNoisyOR, 0.3)
    bn.addWeightedArc(idP2, idNoisyOR, 0.6)

    filename = self.agrumSrcDir("test_bn_aggregators.bgum")
    gum.saveBN(bn, filename)
    bn2 = gum.loadBN(filename)
    self.assertEqual(bn, bn2)

  def testBN_aggregator_compactEncoding(self):
    # a dense encoding of a 4-parent, 10-modality aggregator CPT would need 10^4
    # doubles (~80KB in JSON); the compact "kind":"aggregator" encoding keeps the
    # whole file well under 1KB.
    labels = [str(i) for i in range(10)]
    bn = gum.BayesNet()
    idP1 = bn.add(self._labelizedVar("P1", labels))
    idP2 = bn.add(self._labelizedVar("P2", labels))
    idP3 = bn.add(self._labelizedVar("P3", labels))
    idP4 = bn.add(self._labelizedVar("P4", labels))
    idSum = bn.addSUM(self._labelizedVar("Sum", labels))
    for pid in (idP1, idP2, idP3, idP4):
      bn.addArc(pid, idSum)

    filename = self.agrumSrcDir("test_bn_aggregator_compact.jgum")
    gum.saveBN(bn, filename)
    self.assertLess(os.path.getsize(filename), 1000)

  # ── InfluenceDiagram ──────────────────────────────────────────────────────

  def testID_jgum_roundtrip(self):
    id_ = gum.fastID("A->C->*D->$U;A->$U")
    filename = self.agrumSrcDir("test_id.jgum")
    gum.saveID(id_, filename)
    id2 = gum.loadID(filename)
    self.assertEqual(id_, id2)

  def testID_bgum_roundtrip(self):
    id_ = gum.fastID("A->C->*D->$U;A->$U")
    filename = self.agrumSrcDir("test_id.bgum")
    gum.saveID(id_, filename)
    id2 = gum.loadID(filename)
    self.assertEqual(id_, id2)

  def testID_saveGUMstring_roundtrip(self):
    id_ = gum.fastID("A->C->*D->$U;A->$U")
    s = id_.saveGUMstring()
    self.assertIsInstance(s, str)
    self.assertGreater(len(s), 0)

    id2 = gum.InfluenceDiagram()
    id2.loadGUMstring(s)
    self.assertEqual(id_, id2)

  def testID_properties_preserved(self):
    id_ = gum.fastID("A->*D->$U")
    id_.setProperty("author", "test")
    filename = self.agrumSrcDir("test_id_props.jgum")
    gum.saveID(id_, filename)
    id2 = gum.loadID(filename)
    self.assertEqual(id2.property("author"), "test")

  def testID_pickle_via_jgum(self):
    id_ = gum.fastID("A->*D->$U")
    id_.setProperty("tag", "pickle_test")
    data = pickle.dumps(id_)
    id2 = pickle.loads(data)
    self.assertEqual(id_, id2)
    self.assertEqual(id2.property("tag"), "pickle_test")

  # ── MarkovRandomField ─────────────────────────────────────────────────────

  def testMRF_jgum_roundtrip(self):
    mrf = gum.fastMRF("A--B--C;D--A")
    filename = self.agrumSrcDir("test_mrf.jgum")
    gum.saveMRF(mrf, filename)
    mrf2 = gum.loadMRF(filename)
    self.assertEqual(mrf, mrf2)

  def testMRF_bgum_roundtrip(self):
    mrf = gum.fastMRF("A--B--C;D--A")
    filename = self.agrumSrcDir("test_mrf.bgum")
    gum.saveMRF(mrf, filename)
    mrf2 = gum.loadMRF(filename)
    self.assertEqual(mrf, mrf2)

  def testMRF_saveGUMstring_roundtrip(self):
    mrf = gum.fastMRF("A--B--C;D--A")
    s = mrf.saveGUMstring()
    self.assertIsInstance(s, str)
    self.assertGreater(len(s), 0)

    mrf2 = gum.MarkovRandomField()
    mrf2.loadGUMstring(s)
    self.assertEqual(mrf, mrf2)

  def testMRF_no_factors_roundtrip(self):
    # MRF with nodes but zero factors — "factors" section must still exist in JSON
    mrf = gum.MarkovRandomField()
    mrf.add(gum.RangeVariable("A", "", 0, 1))
    mrf.add(gum.RangeVariable("B", "", 0, 1))
    filename = self.agrumSrcDir("test_mrf_nofactors.jgum")
    gum.saveMRF(mrf, filename)
    mrf2 = gum.loadMRF(filename)
    self.assertEqual(mrf2.size(), 2)
    self.assertEqual(len(mrf2.factors()), 0)

  def testMRF_properties_preserved(self):
    mrf = gum.fastMRF("A--B--C")
    mrf.setProperty("author", "test")
    filename = self.agrumSrcDir("test_mrf_props.jgum")
    gum.saveMRF(mrf, filename)
    mrf2 = gum.loadMRF(filename)
    self.assertEqual(mrf2.property("author"), "test")

  def testMRF_pickle_via_jgum(self):
    mrf = gum.fastMRF("A--B--C")
    mrf.setProperty("tag", "pickle_test")
    data = pickle.dumps(mrf)
    mrf2 = pickle.loads(data)
    self.assertEqual(mrf, mrf2)
    self.assertEqual(mrf2.property("tag"), "pickle_test")

  # ── Error handling ────────────────────────────────────────────────────────

  def testBN_loadGUMstring_invalid_json(self):
    bn = gum.BayesNet()
    with self.assertRaises(Exception) as ctx:
      bn.loadGUMstring("not valid json {{{")
    self.assertIn("Invalid JSON", str(ctx.exception))

  def testID_loadGUMstring_invalid_json(self):
    id_ = gum.InfluenceDiagram()
    with self.assertRaises(Exception) as ctx:
      id_.loadGUMstring("not valid json {{{")
    self.assertIn("Invalid JSON", str(ctx.exception))

  def testMRF_loadGUMstring_invalid_json(self):
    mrf = gum.MarkovRandomField()
    with self.assertRaises(Exception) as ctx:
      mrf.loadGUMstring("not valid json {{{")
    self.assertIn("Invalid JSON", str(ctx.exception))

  def testBN_loadGUM_file_not_found(self):
    with self.assertRaises(Exception) as ctx:
      gum.loadBN("/no/such/file.jgum")
    self.assertIn("No such file", str(ctx.exception))

  def testID_loadGUM_file_not_found(self):
    with self.assertRaises(Exception) as ctx:
      gum.loadID("/no/such/file.jgum")
    self.assertIn("No such file", str(ctx.exception))

  def testMRF_loadGUM_file_not_found(self):
    with self.assertRaises(Exception) as ctx:
      gum.loadMRF("/no/such/file.jgum")
    self.assertIn("No such file", str(ctx.exception))

  def testBN_loadGUMstring_wrong_type(self):
    id_ = gum.fastID("A->*D->$U")
    s = id_.saveGUMstring()  # produces type="ID"
    bn = gum.BayesNet()
    with self.assertRaises(Exception) as ctx:
      bn.loadGUMstring(s)
    self.assertIn("expected 'BN'", str(ctx.exception))
    self.assertIn("got 'ID'", str(ctx.exception))

  def testBN_loadGUM_wrong_type(self):
    filename = self.agrumSrcDir("jgum/check_id.jgum")
    with self.assertRaises(Exception) as ctx:
      gum.loadBN(filename)
    self.assertIn("expected 'BN'", str(ctx.exception))
    self.assertIn("got 'ID'", str(ctx.exception))

  def testID_loadGUMstring_wrong_type(self):
    bn = gum.fastBN("A->B->C")
    s = bn.saveGUMstring()  # produces type="BN"
    id_ = gum.InfluenceDiagram()
    with self.assertRaises(Exception) as ctx:
      id_.loadGUMstring(s)
    self.assertIn("expected 'ID'", str(ctx.exception))
    self.assertIn("got 'BN'", str(ctx.exception))

  def testID_loadGUM_wrong_type(self):
    filename = self.agrumSrcDir("jgum/check_bn.jgum")
    with self.assertRaises(Exception) as ctx:
      gum.loadID(filename)
    self.assertIn("expected 'ID'", str(ctx.exception))
    self.assertIn("got 'BN'", str(ctx.exception))

  def testMRF_loadGUMstring_wrong_type(self):
    bn = gum.fastBN("A->B->C")
    s = bn.saveGUMstring()  # produces type="BN"
    mrf = gum.MarkovRandomField()
    with self.assertRaises(Exception) as ctx:
      mrf.loadGUMstring(s)
    self.assertIn("expected 'MRF'", str(ctx.exception))
    self.assertIn("got 'BN'", str(ctx.exception))

  def testMRF_loadGUM_wrong_type(self):
    filename = self.agrumSrcDir("jgum/check_bn.jgum")
    with self.assertRaises(Exception) as ctx:
      gum.loadMRF(filename)
    self.assertIn("expected 'MRF'", str(ctx.exception))
    self.assertIn("got 'BN'", str(ctx.exception))


ts = unittest.TestSuite()
addTests(ts, TestGumFormat)
