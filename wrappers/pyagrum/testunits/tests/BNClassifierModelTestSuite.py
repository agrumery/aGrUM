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

import sklearn.datasets

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.skbn as skbn


def _make_binary(n_samples=80, n_features=4):
  X, y = sklearn.datasets.make_classification(
    n_samples=n_samples,
    n_features=n_features,
    n_informative=n_features,
    n_redundant=0,
    n_classes=2,
    n_clusters_per_class=1,
    random_state=42,
  )
  return X.astype(float), y.astype(int)


class BNClassifierModelTestCase(pyAgrumTestCase):
  def setUp(self):
    X, y = _make_binary()
    self.clf = skbn.createBNClassifier()
    self.clf.fit(X, y)

  def testModelReturnsBayesNet(self):
    self.assertIsInstance(self.clf.model(), gum.BayesNet)

  def testModelIsCopy(self):
    self.assertIsNot(self.clf.model(), self.clf.bn_)

  def testModelNodesMatchBn(self):
    self.assertEqual(sorted(self.clf.model().names()), sorted(self.clf.bn_.names()))

  def testModelMutationIsolated(self):
    m = self.clf.model()
    old_names = sorted(self.clf.bn_.names())
    first = next(iter(m.names()))
    m.changeVariableName(first, "__renamed__")
    self.assertEqual(sorted(self.clf.bn_.names()), old_names)

  def testModelFromTrainedModel(self):
    bn = gum.BayesNet()
    for name in ["X0", "X1", "X2", "y"]:
      bn.add(gum.LabelizedVariable(name, name, 2))
    bn.addArc("X0", "y")
    bn.addArc("X1", "y")
    bn.addArc("X2", "y")
    bn.generateCPTs()
    clf = skbn.createBNClassifier()
    clf.fromTrainedModel(bn, "y")
    m = clf.model()
    self.assertIsInstance(m, gum.BayesNet)
    self.assertIsNot(m, clf.bn_)
    self.assertEqual(sorted(m.names()), sorted(bn.names()))


ts = unittest.TestSuite()
addTests(ts, BNClassifierModelTestCase)
