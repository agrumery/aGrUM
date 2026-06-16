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

import pyagrum.bnmixture as BNM


class TestMixtureModel(pyAgrumTestCase):
  def testBNMixture_1(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn2 = gum.BayesNet(bn0)
    bn3 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn2.addArc("B", "A")
    bn3.eraseArc("B", "C")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=2.0)
    bnm.add("bn2", bn2, w=3.0)

    self.assertSetEqual(set(bnm.names()), {"bn0", "bn1", "bn2"})
    self.assertEqual(bnm.size(), 3)
    self.assertDictEqual({"bn0": 1.0, "bn1": 2.0, "bn2": 3.0}, bnm.weights())
    bnm.setWeight("bn2", 4.5)
    self.assertEqual(bnm.weight("bn2"), 4.5)

    with self.assertRaises(gum.InvalidArgument):
      bnm.add("bn3", bn3, -3)
    with self.assertRaises(gum.InvalidArgument):
      bnm.add("refBN", bn3, 2)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.BayesNet(bn0)
      bn4.changeVariableName("A", "D")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.fastBN("A{0|1|2|3|4|5}->C<-B")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.ArgumentError):
      bnm.add("bn1", bn4, 1)

    bnm.add("bn3", bn3, 2)
    self.assertSetEqual(set(bnm.names()), {"bn0", "bn1", "bn2", "bn3"})
    bnm.remove("bn3")
    self.assertSetEqual(set(bnm.names()), {"bn0", "bn1", "bn2"})
    with self.assertRaises(gum.NotFound):
      bnm.remove("bn42")

    self.assertFalse(bnm.isNormalized())
    bnm.normalize()
    self.assertTrue(bnm.isNormalized())
    self.assertTrue(bnm.isValid())
    bnm.setWeight("bn0", 0)
    bnm.setWeight("bn1", 0)
    bnm.setWeight("bn2", 0)
    self.assertFalse(bnm.isValid())
    self.assertEqual(bnm.BN("bn0"), bn0)
    self.assertEqual(bnm.BN("bn1"), bn1)
    self.assertEqual(bnm.BN("bn2"), bn2)
    self.assertSetEqual(bnm.zeroBNs(), {"bn0", "bn1", "bn2"})
    self.assertEqual(bnm.existsArc("A", "C"), 3)
    self.assertEqual(bnm.existsArc("A", "B"), 1)
    self.assertEqual(bnm.existsArc("B", "A"), 1)
    self.assertEqual(bnm.existsArc("C", "A"), 0)
    self.assertEqual(bnm.existsArc("C", "B"), 0)
    self.assertEqual(bnm.variable("A"), bn0.variable("A"))
    bnm.setWeight("bn0", 1)
    bnm.setWeight("bn1", 2)
    bnm.setWeight("bn2", 3)
    bnm.updateRef()
    self.assertEqual(bnm._refBN, bnm.BN("bn2"))

  def testBootstrapMixture_1(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn2 = gum.BayesNet(bn0)
    bn3 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn2.addArc("B", "A")
    bn3.eraseArc("B", "C")
    bnm = BNM.BootstrapMixture("bn0", bn0)
    bnm.add("bn1", bn1, w=2.0)
    bnm.add("bn2", bn2, w=3.0)

    self.assertSetEqual(set(bnm.names()), {"bn1", "bn2"})
    self.assertEqual(bnm.size(), 2)
    self.assertDictEqual({"bn1": 2.0, "bn2": 3.0}, bnm.weights())
    bnm.setWeight("bn2", 4.5)
    self.assertEqual(bnm.weight("bn2"), 4.5)

    with self.assertRaises(gum.InvalidArgument):
      bnm.add("bn3", bn3, -3)
    with self.assertRaises(gum.InvalidArgument):
      bnm.add("bn0", bn3, 2)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.BayesNet(bn0)
      bn4.changeVariableName("A", "D")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.InvalidArgument):
      bn4 = gum.fastBN("A{0|1|2|3|4|5}->C<-B")
      bnm.add("bn4", bn4, 1)
    with self.assertRaises(gum.ArgumentError):
      bnm.add("bn1", bn3, 1)

    bnm.add("bn3", bn3, 2)
    self.assertSetEqual(set(bnm.names()), {"bn1", "bn2", "bn3"})
    bnm.remove("bn3")
    self.assertSetEqual(set(bnm.names()), {"bn1", "bn2"})
    with self.assertRaises(gum.NotFound):
      bnm.remove("bn42")

    self.assertFalse(bnm.isNormalized())
    bnm.normalize()
    self.assertTrue(bnm.isNormalized())
    self.assertTrue(bnm.isValid())
    bnm.setWeight("bn1", 0)
    bnm.setWeight("bn2", 0)
    self.assertFalse(bnm.isValid())
    self.assertEqual(bnm.BN("bn1"), bn1)
    self.assertEqual(bnm.BN("bn2"), bn2)
    self.assertSetEqual(bnm.zeroBNs(), {"bn1", "bn2"})
    self.assertEqual(bnm.existsArc("A", "C"), 2)
    self.assertEqual(bnm.existsArc("A", "B"), 1)
    self.assertEqual(bnm.existsArc("B", "A"), 1)
    self.assertEqual(bnm.existsArc("C", "A"), 0)
    self.assertEqual(bnm.existsArc("C", "B"), 0)
    self.assertEqual(bnm.variable("A"), bn0.variable("A"))

  def testBNMixture_saveLoad_jgum(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=3.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "mix.bnm")
      BNM.saveBNM(bnm, path)
      self.assertTrue(os.path.exists(path))
      bnm2 = BNM.loadBNM(path)
      self.assertEqual(set(bnm2.names()), {"bn0", "bn1"})
      self.assertAlmostEqual(bnm2.weight("bn0"), 1.0)
      self.assertAlmostEqual(bnm2.weight("bn1"), 3.0)
      self.assertEqual(bnm2.BN("bn0"), bn0)
      self.assertEqual(bnm2.BN("bn1"), bn1)

  def testBNMixture_saveLoad_bgum(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=2.0)
    bnm.add("bn1", bn1, w=1.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "mix.bnm")
      BNM.saveBNM(bnm, path, fmt="bgum")
      bnm2 = BNM.loadBNM(path)
      self.assertEqual(set(bnm2.names()), {"bn0", "bn1"})
      self.assertAlmostEqual(bnm2.weight("bn0"), 2.0)
      self.assertAlmostEqual(bnm2.weight("bn1"), 1.0)

  def testBNMixture_save_badFormat(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    with tempfile.TemporaryDirectory() as d:
      with self.assertRaises(gum.InvalidArgument):
        BNM.saveBNM(bnm, os.path.join(d, "mix.bnm"), fmt="bif")

  def testBootstrapMixture_saveLoad(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.5)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "boot.bnb")
      BNM.saveBNM(bnb, path)
      bnb2 = BNM.loadBNM(path)
      self.assertEqual(set(bnb2.names()), {"s1"})
      self.assertAlmostEqual(bnb2.weight("s1"), 1.5)
      self.assertEqual(bnb2._refName, "ref")
      self.assertEqual(bnb2.BN("s1"), bn1)

  def testBootstrapMixture_saveLoad_bgum(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.5)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "boot_bgum.bnb")
      BNM.saveBNM(bnb, path, fmt="bgum")
      bnb2 = BNM.loadBNM(path)
      self.assertEqual(set(bnb2.names()), {"s1"})
      self.assertAlmostEqual(bnb2.weight("s1"), 1.5)
      self.assertEqual(bnb2.refName, "ref")
      self.assertEqual(bnb2.BN("s1"), bn1)

  def testBootstrapMixture_saveLoad_refNameCorrect(self):
    import tempfile, os

    bn0 = gum.fastBN("X->Y")
    bn1 = gum.BayesNet(bn0)
    bn1.eraseArc("X", "Y")
    bnb = BNM.BootstrapMixture("myref", bn0)
    bnb.add("s1", bn1, w=1.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "boot2.bnb")
      BNM.saveBNM(bnb, path)
      bnb2 = BNM.loadBNM(path)
      self.assertEqual(bnb2.refName, "myref")
      with self.assertRaises(gum.InvalidArgument):
        bnb2.add("myref", bn0, w=1.0)

  def testSaveBNM_loadBNM_BNMixture(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=2.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "test.bnm")
      BNM.saveBNM(bnm, path)
      self.assertTrue(os.path.isfile(path))
      self.assertFalse(os.path.isfile(path + ".zip"))
      bnm2 = BNM.loadBNM(path)
      self.assertIsInstance(bnm2, BNM.BNMixture)
      self.assertEqual(set(bnm2.names()), {"bn0", "bn1"})
      self.assertAlmostEqual(bnm2.weight("bn0"), 1.0)
      self.assertAlmostEqual(bnm2.weight("bn1"), 2.0)

  def testSaveBNM_loadBNM_BootstrapMixture(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.5)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "test.bnb")
      BNM.saveBNM(bnb, path)
      self.assertTrue(os.path.isfile(path))
      bnb2 = BNM.loadBNM(path)
      self.assertIsInstance(bnb2, BNM.BootstrapMixture)
      self.assertEqual(bnb2.refName, "ref")
      self.assertEqual(set(bnb2.names()), {"s1"})
      self.assertAlmostEqual(bnb2.weight("s1"), 1.5)

  def testBNMixture_reprEmpty(self):
    bnm = BNM.BNMixture()
    self.assertIn("<none>", repr(bnm))

  def testBNMixture_reprNonEmpty(self):
    bn0 = gum.fastBN("A->B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    r = repr(bnm)
    self.assertIn("bn0", r)
    self.assertIn("refBN", r)

  def testBootstrapMixture_repr(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnb = BNM.BootstrapMixture("myref", bn0)
    self.assertIn("myref", repr(bnb))
    bnb.add("s1", bn1, w=1.0)
    r2 = repr(bnb)
    self.assertIn("myref", r2)
    self.assertIn("s1", r2)

  def testBNMixture_normalizeRaisesOnZeroWeights(self):
    bn0 = gum.fastBN("A->B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=0.0)
    with self.assertRaises(gum.InvalidArgument):
      bnm.normalize()

  def testBootstrapMixture_normalizeIsNormalized(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn2 = gum.BayesNet(bn0)
    bn2.addArc("B", "A")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=2.0)
    bnb.add("s2", bn2, w=3.0)
    self.assertFalse(bnb.isNormalized())
    bnb.normalize()
    self.assertTrue(bnb.isNormalized())

  def testBNMixture_properties(self):
    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    self.assertIsNone(bnm.refBN)
    self.assertEqual(bnm.refName, "refBN")
    bnm.add("bn0", bn0, w=1.0)
    self.assertIsNotNone(bnm.refBN)
    with self.assertRaises(AttributeError):
      bnm.refBN = bn0
    with self.assertRaises(AttributeError):
      bnm.refName = "x"

  def testBootstrapMixture_properties(self):
    bn0 = gum.fastBN("A->C<-B")
    bnb = BNM.BootstrapMixture("myref", bn0)
    self.assertEqual(bnb.refName, "myref")
    self.assertIsNotNone(bnb.refBN)
    with self.assertRaises(AttributeError):
      bnb.refBN = bn0

  def testBNMixture_BNisCopy(self):
    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    copy = bnm.BN("bn0")
    copy.addArc("A", "B")
    self.assertFalse(bnm.BN("bn0").existsArc("A", "B"))

  def testBNMixture_variableOnEmptyRaises(self):
    bnm = BNM.BNMixture()
    with self.assertRaises(gum.NotFound):
      bnm.variable("A")

  def testIMixture_setWeightNegativeRaises(self):
    bn0 = gum.fastBN("A->B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    with self.assertRaises(gum.InvalidArgument):
      bnm.setWeight("bn0", -0.5)

  def testIMixture_weightUnknownRaises(self):
    bn0 = gum.fastBN("A->B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    with self.assertRaises(gum.NotFound):
      bnm.weight("unknown")

  def testBNMixture_addSameBNSameName(self):
    bn0 = gum.fastBN("A->B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn0", bn0, w=2.0)
    self.assertAlmostEqual(bnm.weight("bn0"), 3.0)
    self.assertEqual(bnm.size(), 1)

  def testBNMixture_addSameBNDifferentName(self):
    bn0 = gum.fastBN("A->B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn0, w=2.0)
    self.assertAlmostEqual(bnm.weight("bn0"), 3.0)
    self.assertEqual(bnm.size(), 1)

  def testBNMixture_BNs(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=2.0)
    bns = bnm.BNs()
    self.assertEqual(len(bns), 2)
    self.assertTrue(any(b == bn0 for b in bns))
    self.assertTrue(any(b == bn1 for b in bns))

  def testBNMixture_saveLoad_multiDotFilename(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.fastBN("A->B->C")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=2.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "my.mix.file.bnm")
      BNM.saveBNM(bnm, path)
      bnm2 = BNM.loadBNM(path)
      self.assertEqual(set(bnm2.names()), {"bn0", "bn1"})

  def testBug1_loadBNM_nameWithColon(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("fold:1", bn0, w=1.0)
    bnm.add("fold:2", bn1, w=2.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "mix_colon.bnm")
      BNM.saveBNM(bnm, path)
      bnm2 = BNM.loadBNM(path)
      self.assertEqual(set(bnm2.names()), {"fold:1", "fold:2"})
      self.assertAlmostEqual(bnm2.weight("fold:1"), 1.0)
      self.assertAlmostEqual(bnm2.weight("fold:2"), 2.0)

  def testBug2_addBNWithSameVarsInDifferentInsertionOrder(self):
    bn_ref = gum.fastBN("A->B->C")
    bn_diff_order = gum.BayesNet()
    bn_diff_order.add(bn_ref.variable("C"))
    bn_diff_order.add(bn_ref.variable("B"))
    bn_diff_order.add(bn_ref.variable("A"))
    bn_diff_order.addArc("A", "B")
    bn_diff_order.addArc("C", "B")
    bnm = BNM.BNMixture()
    bnm.add("ref", bn_ref, w=1.0)
    bnm.add("diff", bn_diff_order, w=1.0)
    self.assertIn("diff", bnm.names())

  def testBug3_loadRetroCompatibleBNM_bootstrapWithoutRefFileRaises(self):
    import tempfile, os, zipfile

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    with tempfile.TemporaryDirectory() as d:
      bn_path = os.path.join(d, "s1.jgum")
      gum.saveBN(bn1, bn_path)
      bad_zip = os.path.join(d, "bad.bnb")
      with zipfile.ZipFile(bad_zip, "w") as zf:
        zf.writestr("type.txt", "BootstrapMixture\n")
        zf.writestr("weights.txt", "s1:1.0\n")
        zf.write(bn_path, "s1.jgum")
      with self.assertRaises(gum.InvalidArgument):
        BNM.loadRetroCompatibleBNM(bad_zip)

  def testBug6_updateRefOnEmptyMixtureRaises(self):
    bnm = BNM.BNMixture()
    with self.assertRaises(gum.InvalidArgument):
      bnm.updateRef()

  def testIMixture_existsArcWithIntegerIds(self):
    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    id_A = bn0.idFromName("A")
    id_C = bn0.idFromName("C")
    id_B = bn0.idFromName("B")
    self.assertEqual(bnm.existsArc(id_A, id_C), 1)
    self.assertEqual(bnm.existsArc(id_B, id_C), 1)
    self.assertEqual(bnm.existsArc(id_A, id_B), 0)

  def testNormalizedArcsWeight_zeroWeightsRaises(self):
    from pyagrum.bnmixture import notebook as bnm_nb

    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=0.0)
    with self.assertRaises(gum.InvalidArgument):
      bnm_nb._normalizedArcsWeight(bnm)

  def testBNMixture_isNormalizedAfterNormalize(self):
    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    for i in range(20):
      bni = gum.BayesNet(bn0)
      bnm.add(f"bn{i}", bni, w=float(i + 1) * 1.1)
    bnm.normalize()
    self.assertTrue(bnm.isNormalized())

  def testSaveBNM_manifestPresent(self):
    import tempfile, os, zipfile, json

    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "mix.bnm")
      BNM.saveBNM(bnm, path)
      with zipfile.ZipFile(path) as zf:
        self.assertIn("manifest.json", zf.namelist())
        manifest = json.loads(zf.read("manifest.json"))
        self.assertIn("ref_file", manifest)
        self.assertEqual(manifest["type"], "BNMixture")
        self.assertAlmostEqual(manifest["weights"]["bn0"], 1.0)
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", gum.BayesNet(bn0), w=1.0)
    with tempfile.TemporaryDirectory() as d:
      path = os.path.join(d, "boot.bnb")
      BNM.saveBNM(bnb, path)
      with zipfile.ZipFile(path) as zf:
        manifest = json.loads(zf.read("manifest.json"))
        self.assertEqual(manifest["type"], "BootstrapMixture")

  def testBug_saveBNM_memberNameCollidesWithRefRaises(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bnm = BNM.BNMixture()
    bnm.add("ref_refBN", bn0, w=1.0)
    with tempfile.TemporaryDirectory() as d:
      with self.assertRaises(gum.InvalidArgument):
        BNM.saveBNM(bnm, os.path.join(d, "bad.bnm"))

  def testBug_saveBNM_bootstrapMemberNameCollidesWithRefRaises(self):
    import tempfile, os

    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnb = BNM.BootstrapMixture("myref", bn0)
    bnb.add("ref_myref", bn1, w=1.0)
    with tempfile.TemporaryDirectory() as d:
      with self.assertRaises(gum.InvalidArgument):
        BNM.saveBNM(bnb, os.path.join(d, "bad.bnb"))

  def testLoadBNM_noManifestRaises(self):
    import tempfile, os, zipfile

    bn0 = gum.fastBN("A->C<-B")
    with tempfile.TemporaryDirectory() as d:
      bn0_path = os.path.join(d, "bn0.jgum")
      ref_path = os.path.join(d, "ref_refBN.jgum")
      gum.saveBN(bn0, bn0_path)
      gum.saveBN(bn0, ref_path)
      old_zip = os.path.join(d, "old.bnm")
      with zipfile.ZipFile(old_zip, "w") as zf:
        zf.writestr("type.txt", "BNMixture")
        zf.writestr("weights.txt", "bn0:1.0\n")
        zf.write(bn0_path, "bn0.jgum")
        zf.write(ref_path, "ref_refBN.jgum")
      with self.assertRaises(gum.InvalidArgument):
        BNM.loadBNM(old_zip)

  def testLoadRetroCompatibleBNM_oldArchive(self):
    import tempfile, os, zipfile

    bn0 = gum.fastBN("A->C<-B")
    with tempfile.TemporaryDirectory() as d:
      bn0_path = os.path.join(d, "bn0.jgum")
      ref_path = os.path.join(d, "ref_refBN.jgum")
      gum.saveBN(bn0, bn0_path)
      gum.saveBN(bn0, ref_path)
      old_zip = os.path.join(d, "old.bnm")
      with zipfile.ZipFile(old_zip, "w") as zf:
        zf.writestr("type.txt", "BNMixture")
        zf.writestr("weights.txt", "bn0:1.0\n")
        zf.write(bn0_path, "bn0.jgum")
        zf.write(ref_path, "ref_refBN.jgum")
      bnm2 = BNM.loadRetroCompatibleBNM(old_zip)
      self.assertIsInstance(bnm2, BNM.BNMixture)
      self.assertEqual(set(bnm2.names()), {"bn0"})


ts = unittest.TestSuite()
addTests(ts, TestMixtureModel)
