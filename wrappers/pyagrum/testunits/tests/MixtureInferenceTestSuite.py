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
############################################################################

import unittest

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.bnmixture as BNM


class TestMixtureInference(pyAgrumTestCase):
  def testBNMixtureInference_posterior_noEvidence(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=1.0)
    ie = BNM.BNMixtureInference(bnm)
    ie.makeInference()
    p = ie.posterior("A")
    self.assertAlmostEqual(sum(p.tolist()), 1.0, places=5)
    self.assertIn("A", p.names)

  def testBNMixtureInference_posterior_withEvidence(self):
    bn0 = gum.fastBN("A->C<-B")
    bn0.cpt("A").fillWith([0.3, 0.7])
    bn0.cpt("B").fillWith([0.6, 0.4])
    bn0.cpt("C")[{"A": 0, "B": 0}] = [0.9, 0.1]
    bn0.cpt("C")[{"A": 0, "B": 1}] = [0.4, 0.6]
    bn0.cpt("C")[{"A": 1, "B": 0}] = [0.2, 0.8]
    bn0.cpt("C")[{"A": 1, "B": 1}] = [0.7, 0.3]
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn1.cpt("B")[{"A": 0}] = [0.7, 0.3]
    bn1.cpt("B")[{"A": 1}] = [0.5, 0.5]
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=1.0)
    bnm.add("bn1", bn1, w=2.0)
    ie = BNM.BNMixtureInference(bnm)
    ie.setEvidence({"A": 0})
    ie.makeInference()
    p_mix = ie.posterior("C")
    # posterior = normalize(Σ wᵢ · P_BNᵢ(C | evs))
    p0 = gum.getPosterior(bn0, target="C", evs={"A": 0})
    p1 = gum.getPosterior(bn1, target="C", evs={"A": 0})
    p_manual = (p0 + 2 * p1.toVarsIn(bn0)).normalize()
    for v, v_ref in zip(p_mix.tolist(), p_manual.tolist()):
      self.assertAlmostEqual(v, v_ref, places=5)

  def testBootstrapMixtureInference_posterior(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn2 = gum.BayesNet(bn0)
    bn2.addArc("B", "A")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.0)
    bnb.add("s2", bn2, w=1.0)
    ie = BNM.BootstrapMixtureInference(bnb)
    ie.makeInference()
    p = ie.posterior("A")
    self.assertAlmostEqual(sum(p.tolist()), 1.0, places=5)
    self.assertIn("A", p.names)

  def testBootstrapMixtureInference_posterior_isRefBNOnly(self):
    bn0 = gum.fastBN("A->C<-B")
    bn0.cpt("A").fillWith([0.3, 0.7])
    bn0.cpt("C")[{"A": 0, "B": 0}] = [0.9, 0.1]
    bn0.cpt("C")[{"A": 0, "B": 1}] = [0.2, 0.8]
    bn0.cpt("C")[{"A": 1, "B": 0}] = [0.4, 0.6]
    bn0.cpt("C")[{"A": 1, "B": 1}] = [0.7, 0.3]
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn1.cpt("B")[{"A": 0}] = [0.99, 0.01]
    bn1.cpt("B")[{"A": 1}] = [0.01, 0.99]
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.0)
    ie = BNM.BootstrapMixtureInference(bnb)
    ie.makeInference()
    p_mix = ie.posterior("A")
    p_ref = gum.getPosterior(bn0, target="A")
    for v, v_ref in zip(p_mix.tolist(), p_ref.tolist()):
      self.assertAlmostEqual(v, v_ref, places=5)

  def testBootstrapMixtureInference_withEvidence(self):
    bn0 = gum.fastBN("A->C<-B")
    bn0.cpt("A").fillWith([0.3, 0.7])
    bn0.cpt("C")[{"A": 0, "B": 0}] = [0.9, 0.1]
    bn0.cpt("C")[{"A": 0, "B": 1}] = [0.2, 0.8]
    bn0.cpt("C")[{"A": 1, "B": 0}] = [0.4, 0.6]
    bn0.cpt("C")[{"A": 1, "B": 1}] = [0.7, 0.3]
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.0)
    ie = BNM.BootstrapMixtureInference(bnb)
    ie.setEvidence({"A": 0})
    ie.makeInference()
    p = ie.posterior("C")
    self.assertAlmostEqual(sum(p.tolist()), 1.0, places=5)
    # l'évidence doit modifier le posterior par rapport au prior
    ie2 = BNM.BootstrapMixtureInference(bnb)
    ie2.makeInference()
    p_prior = ie2.posterior("C")
    self.assertFalse(all(abs(a - b) < 1e-6 for a, b in zip(p.tolist(), p_prior.tolist())))

  def testBootstrapMixtureInference_quantiles(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bn2 = gum.BayesNet(bn0)
    bn2.addArc("B", "A")
    bnb = BNM.BootstrapMixture("ref", bn0)
    bnb.add("s1", bn1, w=1.0)
    bnb.add("s2", bn2, w=1.0)
    ie = BNM.BootstrapMixtureInference(bnb)
    ie.makeInference()
    q1, q2 = ie.quantiles("A")
    self.assertIn("A", q1.names)
    self.assertIn("A", q2.names)
    for v1, v2 in zip(q1.tolist(), q2.tolist()):
      self.assertLessEqual(v1, v2)

  def testBootstrapMixtureInference_quantiles_emptyRaises(self):
    bn0 = gum.fastBN("A->C<-B")
    bnb = BNM.BootstrapMixture("ref", bn0)
    ie = BNM.BootstrapMixtureInference(bnb)
    ie.makeInference()
    with self.assertRaises(gum.SizeError):
      ie.quantiles("A")

  def testBug4_stats_diracVarianceIsZero(self):
    from pyagrum.bnmixture import notebook as bnm_nb

    bn = gum.BayesNet()
    v = gum.RangeVariable("X", "", 0, 4)
    bn.add(v)
    bn.cpt("X").fillWith([0.0, 0.0, 0.0, 1.0, 0.0])
    mu, std = bnm_nb._stats(bn.cpt("X"))
    self.assertAlmostEqual(mu, 3.0)
    self.assertAlmostEqual(std, 0.0)

  def testBNMixtureInference_emptyMixtureRaises(self):
    bnm = BNM.BNMixture()
    ie = BNM.BNMixtureInference(bnm)
    ie.makeInference()
    with self.assertRaises(gum.InvalidArgument):
      ie.posterior("A")

  def testBNMixtureInference_nonNormalizedWeights(self):
    bn0 = gum.fastBN("A->C<-B")
    bn1 = gum.BayesNet(bn0)
    bn1.addArc("A", "B")
    bnm = BNM.BNMixture()
    bnm.add("bn0", bn0, w=3.0)
    bnm.add("bn1", bn1, w=7.0)
    ie = BNM.BNMixtureInference(bnm)
    ie.makeInference()
    p = ie.posterior("A")
    self.assertAlmostEqual(sum(p.tolist()), 1.0, places=6)


ts = unittest.TestSuite()
addTests(ts, TestMixtureInference)
