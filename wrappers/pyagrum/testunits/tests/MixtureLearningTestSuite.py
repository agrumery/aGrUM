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


class TestMixtureLearning(pyAgrumTestCase):
  def _gen_df(self, bn, n=300):
    gen = gum.BNDatabaseGenerator(bn)
    gen.drawSamples(n)
    return gen.to_pandas()

  # --- BNMLearner ---

  def testBNMLearner_dimensionMismatch(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    with self.assertRaises(gum.ArgumentError):
      BNM.BNMLearner([1.0, 2.0], [df])

  def testBNMLearner_unknownAlgorithm(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    lrn_bn = BNM.BNMLearner([1.0], [df])
    learner = gum.BNLearner(df)
    with self.assertRaises(gum.ArgumentError):
      lrn_bn.updateState(learner, algorithm="foo")

  def testBNMLearner_K2_withoutOrder(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    lrn_bn = BNM.BNMLearner([1.0], [df])
    learner = gum.BNLearner(df)
    with self.assertRaises(gum.ArgumentError):
      lrn_bn.updateState(learner, algorithm="K2")

  def testBNMLearner_updateState_noKwargs_withRefLearner(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=100)
    lrn_bn = BNM.BNMLearner([1.0], [df])
    lrn_bn._ref_learner = gum.BNLearner(df)
    target = gum.BNLearner(df)
    lrn_bn.updateState(target)
    self.assertIsInstance(target.learnBN(), gum.BayesNet)

  def testBNMLearner_updateState_allScoreTypes(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=100)
    lrn = BNM.BNMLearner([1.0], [df])
    for score in ("K2", "AIC", "BIC", "BDeu", "Log2Likelihood"):
      learner = gum.BNLearner(df)
      lrn.updateState(learner, score=score)
      self.assertIsInstance(learner.learnBN(), gum.BayesNet, msg=f"score={score}")
    learner = gum.BNLearner(df)
    lrn.updateState(learner, score="BD", prior="Smoothing")
    self.assertIsInstance(learner.learnBN(), gum.BayesNet, msg="score=BD")

  def testBNMLearner_updateState_allCorrectionTypes(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=100)
    lrn = BNM.BNMLearner([1.0], [df])
    for correction in ("MDL", "NML", "No correction"):
      learner = gum.BNLearner(df)
      lrn.updateState(learner, algorithm="MIIC", correction=correction)
      self.assertIsInstance(learner.learnBN(), gum.BayesNet, msg=f"correction={correction}")

  def testBNMLearner_updateState_unknownCorrectionRaises(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    lrn = BNM.BNMLearner([1.0], [df])
    learner = gum.BNLearner(df)
    with self.assertRaises(gum.ArgumentError):
      lrn.updateState(learner, correction="bad")

  def testBNMLearner_updateState_dirichletWithoutSourceRaises(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    lrn = BNM.BNMLearner([1.0], [df])
    learner = gum.BNLearner(df)
    with self.assertRaises(gum.ArgumentError):
      lrn.updateState(learner, prior="Dirichlet")

  def testBug5_unknownPriorRaisesArgumentError(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    lrn_bn = BNM.BNMLearner([1.0], [df])
    learner = gum.BNLearner(df)
    with self.assertRaises(gum.ArgumentError):
      lrn_bn.updateState(learner, prior="BadPrior")

  def testBNMLearner_learnBNM_basic(self):
    bn_ref = gum.fastBN("A->C<-B")
    df0 = self._gen_df(bn_ref)
    df1 = self._gen_df(bn_ref)
    learner = BNM.BNMLearner([1.0, 2.0], [df0, df1], template=bn_ref)
    bnm = learner.learnBNM()
    self.assertIsInstance(bnm, BNM.BNMixture)
    self.assertEqual(bnm.size(), 2)
    # source de poids max (index 1, w=2.0) → "bn0"
    self.assertAlmostEqual(bnm.weight("bn0"), 2.0)
    self.assertAlmostEqual(bnm.weight("bn1"), 1.0)
    self.assertAlmostEqual(sum(bnm.weights().values()), 3.0)

  def testBNMLearner_add(self):
    bn_ref = gum.fastBN("A->C<-B")
    df0 = self._gen_df(bn_ref)
    df1 = self._gen_df(bn_ref)
    df2 = self._gen_df(bn_ref)
    learner = BNM.BNMLearner([1.0, 1.0], [df0, df1], template=bn_ref)
    learner.add(df2, 1.5)
    bnm = learner.learnBNM()
    self.assertIsInstance(bnm, BNM.BNMixture)
    self.assertAlmostEqual(bnm.weight("bn0"), 1.5)
    self.assertGreaterEqual(bnm.size(), 1)
    self.assertLessEqual(bnm.size(), 3)

  def testBNMLearner_withCSVSource(self):
    import tempfile, os

    bn_ref = gum.fastBN("A->C<-B")
    df1 = self._gen_df(bn_ref, n=200)
    df2 = self._gen_df(bn_ref, n=300)
    with tempfile.TemporaryDirectory() as d:
      csv_path = os.path.join(d, "data.csv")
      df1.to_csv(csv_path, index=False)
      learner = BNM.BNMLearner([1.0, 0.5], [csv_path, df2], template=bn_ref)
      bnm = learner.learnBNM()
      self.assertIsInstance(bnm, BNM.BNMixture)
      self.assertGreaterEqual(bnm.size(), 1)

  def testBNMLearner_invalidSourceTypeRaises(self):
    with self.assertRaises(gum.ArgumentError):
      BNM.BNMLearner([1.0], [42]).learnBNM()

  # --- BNMBootstrapLearner ---

  def testBNMBootstrapLearner_updateState_raises(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=50)
    boot_learner = BNM.BNMBootstrapLearner(df, template=bn_ref)
    learner = gum.BNLearner(df)
    with self.assertRaises(NotImplementedError):
      boot_learner.updateState(learner)

  def testBNMBootstrapLearner_learnBNM_basic(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=300)
    boot_learner = BNM.BNMBootstrapLearner(df, template=bn_ref)
    boot_learner.useIter(4)
    bnm = boot_learner.learnBNM()
    self.assertIsInstance(bnm, BNM.BootstrapMixture)
    self.assertEqual(bnm.refName, "bn0")
    self.assertGreaterEqual(bnm.size(), 1)
    self.assertLessEqual(bnm.size(), 4)
    for w in bnm.weights().values():
      self.assertAlmostEqual(w, 1.0)

  def testBNMBootstrapLearner_useIter(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=300)
    boot_learner = BNM.BNMBootstrapLearner(df, template=bn_ref)
    boot_learner.useIter(3)
    bnm = boot_learner.learnBNM()
    self.assertIsInstance(bnm, BNM.BootstrapMixture)
    self.assertGreaterEqual(bnm.size(), 1)
    self.assertLessEqual(bnm.size(), 3)

  def testBNMBootstrapLearner_consecutiveLearnBNM(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=200)
    boot_learner = BNM.BNMBootstrapLearner(df, template=bn_ref)
    boot_learner.useIter(3)
    bnm1 = boot_learner.learnBNM()
    bnm2 = boot_learner.learnBNM()
    self.assertIsInstance(bnm1, BNM.BootstrapMixture)
    self.assertIsInstance(bnm2, BNM.BootstrapMixture)
    # les deux refBNs doivent être identiques (poids réinitialisés entre appels)
    self.assertEqual(bnm1.refBN, bnm2.refBN)

  def testBNMBootstrapLearner_invalidSourceTypeRaises(self):
    with self.assertRaises(gum.ArgumentError):
      BNM.BNMBootstrapLearner(42)

  def testBNMLearner_statesEmptyList(self):
    bn_ref = gum.fastBN("A->C<-B")
    df = self._gen_df(bn_ref, n=100)
    lrn = BNM.BNMLearner([1.0], [df], states=[])
    self.assertEqual(lrn._states, [{}])


ts = unittest.TestSuite()
addTests(ts, TestMixtureLearning)
