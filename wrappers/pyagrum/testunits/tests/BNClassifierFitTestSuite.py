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

import pyagrum
from pyagrum import skbn

from .pyAgrumTestSuite import addTests, pyAgrumTestCase


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
  def testFitWithTheFNMLScore(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="GHC", scoringType="fNML")
    clf.fit(X, y)
    self.assertEqual(clf.learner_.state()["Score"][0], "fNML")

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

  @staticmethod
  def _tanData():
    # X0 and X1 depend on Y and are strongly dependent given Y, X2 and X3 are independent noise
    rng = np.random.default_rng(0)
    n = 400
    y = rng.integers(0, 2, n)
    x0 = y + rng.integers(0, 2, n)
    x1 = (x0 + (rng.random(n) < 0.1)) % 3
    x2 = rng.integers(0, 3, n)
    x3 = rng.integers(0, 3, n)
    return np.column_stack([x0, x1, x2, x3]), y

  @staticmethod
  def _featureArcs(clf):
    target = clf.bn_.idFromName(clf.target_)
    return target, {(t, h) for t, h in clf.bn_.arcs() if t != target}

  def testTANIsASpanningTreeOverTheFeatures(self):
    # with a significance of 1, every feature and every pair is kept (TAN of Friedman et al.): the pairs
    # with almost no conditional mutual information must still be used to span the features
    X, y = self._tanData()
    clf = skbn.createBNClassifier(learningMethod="TAN", TANSignificance=1)
    clf.fit(X, y)
    target, featureArcs = self._featureArcs(clf)
    features = set(clf.bn_.nodes()) - {target}
    self.assertEqual(clf.bn_.children(target), features)
    self.assertEqual(len(featureArcs), len(features) - 1)
    self.assertTrue(all(len(clf.bn_.parents(f) - {target}) <= 1 for f in features))

  def testSelectiveTANDropsTheFeaturesIndependentOfTheClass(self):
    X, y = self._tanData()
    clf = skbn.createBNClassifier(learningMethod="TAN")
    clf.fit(X, y)
    target, featureArcs = self._featureArcs(clf)
    x0, x1, x2, x3 = (clf.bn_.idFromName(f"x{i}") for i in range(4))
    self.assertEqual(clf.bn_.children(target), {x0, x1})
    self.assertEqual({frozenset(a) for a in featureArcs}, {frozenset((x0, x1))})
    for x in (x2, x3):
      self.assertEqual(clf.bn_.parents(x) | clf.bn_.children(x), set())
    self.assertEqual(len(clf.predict(X[:5])), 5)

  def testSelectiveTANIsAForest(self):
    # two blocks (X0, X1) and (X2, X3), each one dependent on Y, independent of each other given Y
    rng = np.random.default_rng(1)
    n = 400
    y = rng.integers(0, 2, n)
    x0 = y + rng.integers(0, 2, n)
    x1 = (x0 + (rng.random(n) < 0.1)) % 3
    x2 = 2 * y - rng.integers(0, 2, n)
    x3 = (x2 + (rng.random(n) < 0.1)) % 3
    X = np.column_stack([x0, x1, x2, x3])
    clf = skbn.createBNClassifier(learningMethod="TAN")
    clf.fit(X, y)
    target, featureArcs = self._featureArcs(clf)
    x0, x1, x2, x3 = (clf.bn_.idFromName(f"x{i}") for i in range(4))
    self.assertEqual(clf.bn_.children(target), {x0, x1, x2, x3})
    self.assertEqual({frozenset(a) for a in featureArcs}, {frozenset((x0, x1)), frozenset((x2, x3))})

  def testSelectiveTANWithoutSignificantFeatureWarns(self):
    X, y = self._tanData()
    noise = X[:, 2:]
    clf = skbn.createBNClassifier(learningMethod="TAN")
    with self.assertWarns(UserWarning):
      clf.fit(noise, y)
    self.assertEqual(clf.bn_.sizeArcs(), 0)
    # the features are ignored: the same prediction for every row
    proba = clf.predict_proba(noise)
    self.assertTrue(np.allclose(proba, proba[0]))
    self.assertEqual(len(set(clf.predict(noise))), 1)

  def testTANSignificanceComesFromTheConfiguration(self):
    X, y = self._tanData()
    clf = skbn.createBNClassifier(learningMethod="TAN")
    self.assertIsNone(clf.get_params()["TANSignificance"])
    pyagrum.config.push()
    try:
      pyagrum.config["skbn", "tanSignificance"] = 1
      clf.fit(X, y)
    finally:
      pyagrum.config.pop()
    target, featureArcs = self._featureArcs(clf)
    self.assertEqual(len(clf.bn_.children(target)), 4)
    self.assertEqual(len(featureArcs), 3)

  def testTANSignificanceIsInZeroOne(self):
    X, y = self._tanData()
    for significance in [0, -0.1, 1.5]:
      with self.assertRaises(ValueError):
        pyagrum.config["skbn", "tanSignificance"] = significance
      # scikit-learn estimators do not raise in __init__ or set_params: the value is checked by fit
      clf = skbn.createBNClassifier(learningMethod="TAN", TANSignificance=significance)
      with self.assertRaises(ValueError):
        clf.fit(X, y)

  def testTANWithASingleFeatureIsANaiveBayes(self):
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="TAN", TANSignificance=1)
    clf.fit(X[:, :1], y)
    self.assertIsNotNone(clf.bn_)
    target = clf.bn_.idFromName(clf.target_)
    self.assertEqual(clf.bn_.size(), 2)
    self.assertEqual(clf.bn_.sizeArcs(), 1)
    self.assertEqual(clf.bn_.children(target), set(clf.bn_.nodes()) - {target})
    self.assertEqual(len(clf.predict(X[:5, :1])), 5)

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

  def testPriorWeightComesFromTheConfiguration(self):
    X, y = _make_binary()

    # the CPTs of the roots are uniform whatever the prior (balanced classes, quantile
    # discretization): the prior is seen on the largest deviation from uniform among all the CPTs
    def maxDeviationFromUniform(bn):
      return max(np.abs(bn.cpt(i).toarray() - 1 / bn.variable(i).domainSize()).max() for i in bn.nodes())

    clf = skbn.createBNClassifier(prior="Smoothing")
    self.assertIsNone(clf.get_params()["priorWeight"])
    clf.fit(X, y)
    self.assertGreater(maxDeviationFromUniform(clf.bn_), 1e-3)

    # a huge weight of the prior makes all the CPTs almost uniform
    pyagrum.config.push()
    try:
      pyagrum.config["learning", "priorWeight"] = 1e6
      heavy = skbn.createBNClassifier(prior="Smoothing")
      heavy.fit(X, y)
      self.assertLess(maxDeviationFromUniform(heavy.bn_), 1e-3)
    finally:
      pyagrum.config.pop()

    # an explicit weight wins over the configuration
    explicit = skbn.createBNClassifier(prior="Smoothing", priorWeight=1)
    explicit.fit(X, y)
    for name in clf.bn_.names():
      self.assertTrue(np.allclose(explicit.bn_.cpt(name).toarray(), clf.bn_.cpt(name).toarray()))


ts = unittest.TestSuite()
addTests(ts, BNClassifierFitTestCase)
