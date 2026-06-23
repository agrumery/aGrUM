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

import numpy as np
import pandas as pd
import sklearn
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


class BNClassifierFitTestCase(pyAgrumTestCase):
  def testFitNumpyFloat(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "classes_"))
    self.assertTrue(hasattr(clf, "n_features_in_"))
    self.assertEqual(clf.n_features_in_, X.shape[1])

  def testFitNumpyInt(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X.astype(int), y)
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitNumpyObject(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X.astype(object), y)
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitYInt(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y.astype(int))
    self.assertEqual(set(clf.classes_), {0, 1})

  def testFitYString(self):
    X, y = _make_binary()
    y_str = np.where(y == 0, "cat", "dog")
    clf = skbn.createBNClassifier()
    clf.fit(X, y_str)
    self.assertEqual(set(clf.classes_), {"cat", "dog"})

  def testFitYBool(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y.astype(bool))
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitYList(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, list(y))
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitYSeriesWithName(self):
    X, y = _make_binary()
    y_s = pd.Series(y, name="label")
    clf = skbn.createBNClassifier()
    clf.fit(X, y_s)
    self.assertEqual(clf.target_, "label")

  def testFitYSeriesNameNone(self):
    X, y = _make_binary()
    y_s = pd.Series(y, name=None)
    clf = skbn.createBNClassifier()
    clf.fit(X, y_s)
    self.assertEqual(clf.target_, "y")

  def testFitXDataframe(self):
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"feat_{i}" for i in range(X.shape[1])])
    clf = skbn.createBNClassifier()
    clf.fit(df, y)
    self.assertIn("feat_0", clf.variableNameIndexDictionary_)

  def testFitXDataframeYSeries(self):
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"f{i}" for i in range(X.shape[1])])
    y_s = pd.Series(y, name="cible")
    clf = skbn.createBNClassifier()
    clf.fit(df, y_s)
    self.assertEqual(clf.target_, "cible")

  def testFitMulticlass(self):
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    self.assertEqual(len(clf.classes_), 3)

  def testFitReturnsSelf(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    result = clf.fit(X, y)
    self.assertIs(result, clf)

  def testFitTwice(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitClassesAttribute(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    self.assertTrue(np.array_equal(np.sort(clf.classes_), np.sort(np.unique(y))))

  def testfitFromTabularframe(self):
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"f{i}" for i in range(X.shape[1])])
    df["target"] = y
    clf = skbn.createBNClassifier()
    clf.fitFromTabular(df, "target")
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitFromCsvTemp(self):
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"f{i}" for i in range(X.shape[1])])
    df["target"] = y
    with tempfile.NamedTemporaryFile(suffix=".csv", delete=False, mode="w") as f:
      df.to_csv(f, index=False)
      fname = f.name
    try:
      clf = skbn.createBNClassifier()
      clf.fitFromTabular(fname, "target")
      self.assertTrue(hasattr(clf, "classes_"))
    finally:
      os.remove(fname)

  def testLearningNaiveBayes(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))

  def testLearningTAN(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="TAN")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))

  def testLearningChowLiu(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="Chow-Liu")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))

  def testLearningMIIC(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="MIIC")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))


ts = unittest.TestSuite()
addTests(ts, BNClassifierFitTestCase)
