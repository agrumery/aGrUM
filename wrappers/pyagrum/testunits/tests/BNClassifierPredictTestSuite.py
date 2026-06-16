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

import numpy as np
import pandas as pd
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


class BNClassifierPredictTestCase(pyAgrumTestCase):
  def testPredictShape(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertEqual(clf.predict(X).shape, (len(X),))

  def testPredictValuesInClasses(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertTrue(set(clf.predict(X)).issubset(set(clf.classes_)))

  def testPredictMulticlassShape(self):
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertEqual(clf.predict(X).shape, (len(X),))

  def testPredictDataframeInput(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    col_names = list(clf.variableNameIndexDictionary_.keys())
    X_df = pd.DataFrame(X, columns=col_names)
    self.assertEqual(clf.predict(X_df).shape, (len(X),))

  def testPredictConsistentWithProba(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    preds = clf.predict(X)
    proba = clf.predict_proba(X)
    argmax_preds = clf.classes_[np.argmax(proba, axis=1)]
    self.assertTrue(np.array_equal(preds, argmax_preds))

  def testPredictProbaShapeBinary(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertEqual(clf.predict_proba(X).shape, (len(X), 2))

  def testPredictProbaSumToOne(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertTrue(np.allclose(clf.predict_proba(X).sum(axis=1), 1.0))

  def testPredictProbaBetween0And1(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertTrue((proba >= 0).all() and (proba <= 1).all())

  def testPredictProbaMulticlassShape(self):
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertEqual(clf.predict_proba(X).shape, (len(X), 3))

  def testPredictProbaMulticlassSumToOne(self):
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    self.assertTrue(np.allclose(clf.predict_proba(X).sum(axis=1), 1.0))

  def testPredictProbaMulticlassBetween0And1(self):
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertTrue(np.all(proba >= 0.0) and np.all(proba <= 1.0))


ts = unittest.TestSuite()
addTests(ts, BNClassifierPredictTestCase)
