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

import pickle
import unittest

import numpy as np
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


def _make_multiclass(n_classes=3, n_samples=100, n_features=4):
  X, y = sklearn.datasets.make_classification(
    n_samples=n_samples,
    n_features=n_features,
    n_informative=n_features,
    n_redundant=0,
    n_classes=n_classes,
    n_clusters_per_class=1,
    random_state=42,
  )
  return X.astype(float), y.astype(int)


class BNClassifierPickleTestCase(pyAgrumTestCase):
  def testPicklePredictIdentical(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertTrue(np.array_equal(clf.predict(X), clf2.predict(X)))

  def testPicklePredictProbaIdentical(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertTrue(np.allclose(clf.predict_proba(X), clf2.predict_proba(X)))

  def testPickleClassesPreserved(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertTrue(np.array_equal(clf.classes_, clf2.classes_))

  def testPickleNFeaturesPreserved(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertEqual(clf.n_features_in_, clf2.n_features_in_)

  def testPickleMulticlass(self):
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertEqual(clf2.predict(X).shape, (len(X),))

  def testPickleFromTrainedModel(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    clf3 = pickle.loads(pickle.dumps(clf2))
    self.assertTrue(hasattr(clf3, "classes_"))
    self.assertTrue(np.array_equal(clf2.classes_, clf3.classes_))


ts = unittest.TestSuite()
addTests(ts, BNClassifierPickleTestCase)
