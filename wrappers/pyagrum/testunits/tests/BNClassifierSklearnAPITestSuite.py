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

import sklearn
import sklearn.base
import sklearn.datasets

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


class BNClassifierSklearnAPITestCase(pyAgrumTestCase):
  def testGetParams(self):
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes", discretizationNbBins=3)
    params = clf.get_params()
    self.assertEqual(params["learningMethod"], "NaiveBayes")
    self.assertEqual(clf.type_processor.defaultParamDiscretizationMethod, 3)

  def testSetParams(self):
    clf = skbn.createBNClassifier()
    clf.set_params(learningMethod="TAN")
    self.assertEqual(clf.learningMethod, "TAN")

  def testCloneNoFittedAttrs(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes").fit(X, y)
    clf2 = sklearn.base.clone(clf)
    self.assertFalse(hasattr(clf2, "bn_"))
    self.assertEqual(clf2.learningMethod, "NaiveBayes")

  def testCloneThenFit(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes").fit(X, y)
    clf2 = sklearn.base.clone(clf)
    clf2.fit(X, y)
    self.assertTrue(hasattr(clf2, "classes_"))


ts = unittest.TestSuite()
addTests(ts, BNClassifierSklearnAPITestCase)
