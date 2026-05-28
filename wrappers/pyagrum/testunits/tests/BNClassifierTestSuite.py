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

import numpy as np
import pandas as pd
import pickle

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.skbn as skbn
import sklearn

from sklearn.exceptions import NotFittedError

import tempfile
import os


def _make_binary(n_samples=80, n_features=4):
  # Generate a binary classification dataset (float X, int y in {0, 1}).
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
  # Generate a multi-class classification dataset (float X, int y in {0, ..., n_classes-1})
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


def _normalizeDiscretizerAudit(discretizer_audit):
  # Convert NumPy arrays in a dictionary to lists for reliable comparison
  return {
    k: {kk: (vv.tolist() if isinstance(vv, np.ndarray) else vv) for kk, vv in v.items()}
    for k, v in discretizer_audit.items()
  }


class BNCLassifierTestCase(pyAgrumTestCase):
  def testFitFromCsv(self):
    csvfile = self.agrumSrcDir("miniasia.csv")

    asia_target_column = "lung_cancer"

    classif1 = skbn.createBNClassifier()
    classif1.fitFromTabular(csvfile, asia_target_column)

    self.assertEqual(classif1.bn_.size(), 8)
    self.assertEqual(classif1.target_, asia_target_column)
    self.assertTrue(classif1.threshold_ <= 1)

    self.assertGreater(classif1.MarkovBlanket_.size(), 0)

  def testClassifierFromDf(self):
    csvfile = self.agrumSrcDir("miniasia.csv")

    df_asia = pd.read_csv(csvfile)
    asia_target_column = "lung_cancer"

    x_train_asia = df_asia[:9000].drop(asia_target_column, axis=1)
    y_train_asia = df_asia[:9000][asia_target_column]

    x_test_asia = df_asia[-1000:].drop(asia_target_column, axis=1)
    _ = df_asia[-1000:][asia_target_column]

    classif2 = skbn.createBNClassifier()
    classif2.fit(x_train_asia, y_train_asia)

    self.assertEqual(classif2.bn_.size(), 8)

    self.assertEqual(classif2.target_, asia_target_column)
    self.assertTrue(classif2.threshold_ <= 1)

    yproba = classif2.predict_proba(x_test_asia)
    self.assertEqual(yproba.shape, (299, 2))
    self.assertEqual(yproba[0].sum(), 1)

    ypred = classif2.predict(x_test_asia)
    self.assertEqual(ypred.shape, (299,))
    self.assertIn(ypred[0], [0, 1])

    self.assertGreater(classif2.MarkovBlanket_.size(), 0)

    classif3 = skbn.createBNClassifier()
    classif3.fitFromTabular(csvfile, "lung_cancer")

    self.assertEqual(classif3.bn_.size(), 8)

    self.assertEqual(classif3.target_, asia_target_column)
    self.assertTrue(classif3.threshold_ <= 1)

    df = pd.read_csv(csvfile)
    classif4 = skbn.createBNClassifier()
    classif4.fitFromTabular(df, "lung_cancer")

    self.assertEqual(classif4.bn_.size(), 8)

    self.assertEqual(classif4.target_, asia_target_column)
    self.assertTrue(classif4.threshold_ <= 1)

    # some instantiation of parents are missing : No prior should lead to division by 0
    classif3 = skbn.createBNClassifier(prior="NoPrior")
    with self.assertRaises(gum.DatabaseError):
      classif3.fit(x_train_asia, y_train_asia)

  def testPickling(self):
    bn = gum.fastBN("X1{A|B}->X2{a|b|c}->Y{y0|y1}<-X1;X3[3]->Y<-X4{A|B}")
    dftrain, _ = gum.generateSample(bn, 500)
    dftest, _ = gum.generateSample(bn, 300)
    bnc = skbn.createBNClassifier()

    bnc.fitFromTabular(dftrain, "Y")

    smodel = pickle.dumps(bnc)
    bnc2 = pickle.loads(smodel)

    bnc2.fitFromTabular(dftrain, "Y")

    self.assertDictEqual(
      _normalizeDiscretizerAudit(bnc.type_processor.audit(dftrain)),
      _normalizeDiscretizerAudit(bnc2.type_processor.audit(dftrain)),
    )
    params1 = {k: v for k, v in bnc.get_params().items() if k != "type_processor"}
    params2 = {k: v for k, v in bnc2.get_params().items() if k != "type_processor"}
    self.assertDictEqual(params1, params2)
    self.assertEqual(bnc.threshold_, bnc2.threshold_)
    feature_cols = [c for c in dftrain.columns if c != "Y"]
    dftest_X = dftest[feature_cols]
    self.assertEqual((bnc.predict_proba(dftest_X) - bnc2.predict_proba(dftest_X)).max(), 0)
    self.assertEqual((bnc.predict_proba(dftest_X) - bnc2.predict_proba(dftest_X)).min(), 0)

    self.assertTrue(all(bnc.predict(dftest_X) == bnc2.predict(dftest_X)))


class BNClassifierFitTestCase(pyAgrumTestCase):
  # Tests for fit(X, y), fitFromTabular and learning methods.#

  def testFitNumpyFloat(self):
    # fit() with a float numpy array sets classes_ and n_features_in_
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "classes_"))
    self.assertTrue(hasattr(clf, "n_features_in_"))
    self.assertEqual(clf.n_features_in_, X.shape[1])

  def testFitNumpyInt(self):
    # fit() with an int numpy array sets classes_
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X.astype(int), y)
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitNumpyObject(self):
    # fit() with an object numpy array sets classes_
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X.astype(object), y)
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitYInt(self):
    # fit() with integer y produces classes_ == {0, 1}
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y.astype(int))
    self.assertEqual(set(clf.classes_), {0, 1})

  def testFitYString(self):
    # fit() with string y produces classes_ == {'cat', 'dog'}
    X, y = _make_binary()
    y_str = np.where(y == 0, "cat", "dog")
    clf = skbn.createBNClassifier()
    clf.fit(X, y_str)
    self.assertEqual(set(clf.classes_), {"cat", "dog"})

  def testFitYBool(self):
    # fit() with boolean y does not raise
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y.astype(bool))
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitYList(self):
    # fit() with a Python list as y does not raise
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, list(y))
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitYSeriesWithName(self):
    # it() with a named Series uses the series name as target_
    X, y = _make_binary()
    y_s = pd.Series(y, name="label")
    clf = skbn.createBNClassifier()
    clf.fit(X, y_s)
    self.assertEqual(clf.target_, "label")

  def testFitYSeriesNameNone(self):
    # fit() with Series name=None falls back to 'y' as target_
    X, y = _make_binary()
    y_s = pd.Series(y, name=None)
    clf = skbn.createBNClassifier()
    clf.fit(X, y_s)
    self.assertEqual(clf.target_, "y")

  def testFitXDataframe(self):
    # fit() with a DataFrame uses column names as variable names
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"feat_{i}" for i in range(X.shape[1])])
    clf = skbn.createBNClassifier()
    clf.fit(df, y)
    self.assertIn("feat_0", clf.variableNameIndexDictionary_)

  def testFitXDataframeYSeries(self):
    # fit() with DataFrame X and named Series y sets target_ correctly
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"f{i}" for i in range(X.shape[1])])
    y_s = pd.Series(y, name="cible")
    clf = skbn.createBNClassifier()
    clf.fit(df, y_s)
    self.assertEqual(clf.target_, "cible")

  def testFitMulticlass(self):
    # fit() on a 3-class problem sets len(classes_) == 3
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    self.assertEqual(len(clf.classes_), 3)

  def testFitReturnsSelf(self):
    # fit() returns the classifier instance (sklearn convention)
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    result = clf.fit(X, y)
    self.assertIs(result, clf)

  def testFitTwice(self):
    # Calling fit() twice in a row does not raise
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitClassesAttribute(self):
    # classes_ matches exactly the unique values in y
    X, y = _make_binary()
    clf = skbn.createBNClassifier()
    clf.fit(X, y)
    self.assertTrue(np.array_equal(np.sort(clf.classes_), np.sort(np.unique(y))))

  def testfitFromTabularframe(self):
    # fitFromTabular() with a DataFrame sets classes_
    X, y = _make_binary()
    df = pd.DataFrame(X, columns=[f"f{i}" for i in range(X.shape[1])])
    df["target"] = y
    clf = skbn.createBNClassifier()
    clf.fitFromTabular(df, "target")
    self.assertTrue(hasattr(clf, "classes_"))

  def testFitFromCsvTemp(self):
    # fitFromTabular() with a CSV file path sets classes_
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
    # NaiveBayes learning method sets bn_ after fit()
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))

  def testLearningTAN(self):
    # TAN learning method sets bn_ after fit()
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="TAN")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))

  def testLearningChowLiu(self):
    # Chow-Liu learning method sets bn_ after fit()
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="Chow-Liu")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))

  def testLearningMIIC(self):
    # MIIC learning method sets bn_ after fit()
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="MIIC")
    clf.fit(X, y)
    self.assertTrue(hasattr(clf, "bn_"))


class BNClassifierPredictTestCase(pyAgrumTestCase):
  # Tests for predict() and predict_proba()

  def testPredictShape(self):
    # predict() returns an array of shape (n_samples,)
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    preds = clf.predict(X)
    self.assertEqual(preds.shape, (len(X),))

  def testPredictValuesInClasses(self):
    # All predicted values belong to classes_
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    preds = clf.predict(X)
    self.assertTrue(set(preds).issubset(set(clf.classes_)))

  def testPredictMulticlassShape(self):
    # predict() on a multi-class problem returns shape (n_samples,)
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    preds = clf.predict(X)
    self.assertEqual(preds.shape, (len(X),))

  def testPredictDataframeInput(self):
    # predict() accepts a DataFrame with the correct column names
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    col_names = list(clf.variableNameIndexDictionary_.keys())
    X_df = pd.DataFrame(X, columns=col_names)
    preds = clf.predict(X_df)
    self.assertEqual(preds.shape, (len(X),))

  def testPredictConsistentWithProba(self):
    # argmax(predict_proba(X)) must equal predict(X) (sklearn consistency)
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    preds = clf.predict(X)
    proba = clf.predict_proba(X)
    argmax_preds = clf.classes_[np.argmax(proba, axis=1)]
    self.assertTrue(np.array_equal(preds, argmax_preds))

  def testPredictProbaShapeBinary(self):
    # predict_proba() returns shape (n_samples, 2) for a binary problem
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertEqual(proba.shape, (len(X), 2))

  def testPredictProbaSumToOne(self):
    # Each row of predict_proba() sums to 1
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertTrue(np.allclose(proba.sum(axis=1), 1.0))

  def testPredictProbaBetween0And1(self):
    # All probabilities returned by predict_proba() are in [0, 1]
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertTrue((proba >= 0).all() and (proba <= 1).all())

  def testPredictProbaMulticlassShape(self):
    # predict_proba() returns (n_samples, n_classes) for a multi-class problem
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertEqual(proba.shape, (len(X), 3))

  def testPredictProbaMulticlassSumToOne(self):
    # Multi-class probabilities sum to 1 per row
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertTrue(np.allclose(proba.sum(axis=1), 1.0))

  def testPredictProbaMulticlassBetween0And1(self):
    # All multi-class probabilities are in [0, 1]
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    proba = clf.predict_proba(X)
    self.assertTrue(np.all(proba >= 0.0) and np.all(proba <= 1.0))


class BNClassifierErrorTestCase(pyAgrumTestCase):
  # Tests for expected errors (ValueError, NotFittedError)

  def testErrorFitYNone(self):
    # fit(X, None) raises ValueError mentioning 'y' or 'none'
    X, _ = _make_binary()
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError) as ctx:
      clf.fit(X, None)
    msg = str(ctx.exception).lower()
    self.assertTrue("y" in msg or "none" in msg)

  def testErrorFitSingleClass(self):
    # fit() with a single-class y raises ValueError
    X, _ = _make_binary()
    y_single = np.zeros(len(X), dtype=int)
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fit(X, y_single)

  def testErrorFitContinuousY(self):
    # fit() with continuous float y raises ValueError
    X, _ = _make_binary()
    y_cont = np.random.rand(len(X))
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fit(X, y_cont)

  def testErrorPredictNotFitted(self):
    # predict() before fit() raises NotFittedError
    X, _ = _make_binary()
    clf = skbn.createBNClassifier()
    with self.assertRaises(NotFittedError):
      clf.predict(X)

  def testErrorPredictProbaNotFitted(self):
    # predict_proba() before fit() raises NotFittedError
    X, _ = _make_binary()
    clf = skbn.createBNClassifier()
    with self.assertRaises(NotFittedError):
      clf.predict_proba(X)

  def testErrorPredictWrongNFeatures(self):
    # predict() with a wrong number of features raises ValueError
    X, y = _make_binary(n_features=4)
    clf = skbn.createBNClassifier().fit(X, y)
    X_wrong = np.random.rand(10, 7)
    with self.assertRaises(ValueError):
      clf.predict(X_wrong)

  def testErrorPredictWrongFeaturesFromTrainedModel(self):
    # predict() after fromTrainedModel() with wrong feature count raises ValueError
    X, y = _make_binary(n_features=4)
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    X_wrong = np.random.rand(10, 7)
    with self.assertRaises(ValueError):
      clf2.predict(X_wrong)

  def testErrorfitFromTabularNoTargetName(self):
    # fitFromTabular() without a target name raises ValueError
    X, y = _make_binary()
    df = pd.DataFrame(X)
    df["target"] = y
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fitFromTabular(df, None)

  def testErrorfitFromTabularNoData(self):
    # fitFromTabular() without data raises ValueError
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fitFromTabular(None, "target")


class BNClassifierPickleTestCase(pyAgrumTestCase):
  # Tests for pickle/unpickle round-trip

  def testPicklePredictIdentical(self):
    # predict() gives identical results after a pickle/unpickle round-trip
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    preds_before = clf.predict(X)
    clf2 = pickle.loads(pickle.dumps(clf))
    preds_after = clf2.predict(X)
    self.assertTrue(np.array_equal(preds_before, preds_after))

  def testPicklePredictProbaIdentical(self):
    # predict_proba() gives identical results after a pickle/unpickle round-trip
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    proba_before = clf.predict_proba(X)
    clf2 = pickle.loads(pickle.dumps(clf))
    proba_after = clf2.predict_proba(X)
    self.assertTrue(np.allclose(proba_before, proba_after))

  def testPickleClassesPreserved(self):
    # classes_ is correctly restored after pickle
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertTrue(np.array_equal(clf.classes_, clf2.classes_))

  def testPickleNFeaturesPreserved(self):
    # n_features_in_ is correctly restored after pickle
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    self.assertEqual(clf.n_features_in_, clf2.n_features_in_)

  def testPickleMulticlass(self):
    # Pickle round-trip works for a multi-class classifier
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = pickle.loads(pickle.dumps(clf))
    preds = clf2.predict(X)
    self.assertEqual(preds.shape, (len(X),))

  def testPickleFromTrainedModel(self):
    # Pickle round-trip preserves classes_ after fromTrainedModel()
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    clf3 = pickle.loads(pickle.dumps(clf2))
    self.assertTrue(hasattr(clf3, "classes_"))
    self.assertTrue(np.array_equal(clf2.classes_, clf3.classes_))


class BNClassifierFromTrainedModelTestCase(pyAgrumTestCase):
  # Tests for fromTrainedModel() and its dtype parameter

  def testFromTrainedModelHasClasses(self):
    # fromTrainedModel() sets classes_
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    self.assertTrue(hasattr(clf2, "classes_"))

  def testFromTrainedModelClassesAreStrings(self):
    # classes_ from fromTrainedModel() are strings by default
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    self.assertTrue(all(isinstance(c, str) for c in clf2.classes_))

  def testFromTrainedModelBinary(self):
    # fromTrainedModel() works for a binary classifier (len(classes_) == 2)
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    self.assertEqual(len(clf2.classes_), 2)

  def testFromTrainedModelMulticlass(self):
    # fromTrainedModel() works for a multi-class classifier
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    self.assertEqual(len(clf2.classes_), 3)

  def testFtmPredictShape(self):
    # predict() after fromTrainedModel() returns shape (n_samples,)
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    preds = clf2.predict(X)
    self.assertEqual(preds.shape, (len(X),))

  def testFtmPredictValuesInClasses(self):
    # Predictions after fromTrainedModel() are contained in classes_
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    preds = clf2.predict(X)
    self.assertTrue(set(preds).issubset(set(clf2.classes_)))

  def testFtmPredictProbaShape(self):
    # predict_proba() after fromTrainedModel() returns shape (n_samples, 2)
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    proba = clf2.predict_proba(X)
    self.assertEqual(proba.shape, (len(X), 2))

  def testFtmPredictProbaSumToOne(self):
    # Probabilities sum to 1 per row after fromTrainedModel()
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    proba = clf2.predict_proba(X)
    self.assertTrue(np.allclose(proba.sum(axis=1), 1.0))

  def testFtmPredictConsistentWithProba(self):
    # argmax(predict_proba) == predict() after fromTrainedModel()
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    preds = clf2.predict(X)
    proba = clf2.predict_proba(X)
    argmax_preds = clf2.classes_[np.argmax(proba, axis=1)]
    self.assertTrue(np.array_equal(preds, argmax_preds))

  def testFtmPredictMulticlass(self):
    # predict() after fromTrainedModel() works for a multi-class problem
    X, y = _make_multiclass(n_classes=3)
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    preds = clf2.predict(X)
    self.assertEqual(preds.shape, (len(X),))
    self.assertTrue(set(preds).issubset(set(clf2.classes_)))

  def testFtmDtypeStrDefault(self):
    # dtype=str (default): classes_ are strings
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    self.assertTrue(all(isinstance(c, str) for c in clf2.classes_))

  def testFtmDtypeInt(self):
    # dtype=int: classes_ are converted to int and equal {0, 1}
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_, dtype=int)
    self.assertTrue(all(isinstance(c, (int, np.integer)) for c in clf2.classes_))
    self.assertEqual(set(clf2.classes_), {0, 1})

  def testFtmDtypeNumpyInt32(self):
    # dtype=np.int32: classes_.dtype is np.int32
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_, dtype=np.int32)
    self.assertEqual(clf2.classes_.dtype, np.int32)

  def testFtmDtypeNumpyInt64(self):
    # dtype=np.int64: classes_.dtype is np.int64
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_, dtype=np.int64)
    self.assertEqual(clf2.classes_.dtype, np.int64)

  def testFtmDtypeBoolRaises(self):
    # dtype=bool raises ValueError
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf2.fromTrainedModel(clf.bn_, clf.target_, dtype=bool)

  def testFtmDtypeFloatRaises(self):
    # dtype=float raises ValueError (continuous classes are not allowed)
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf2.fromTrainedModel(clf.bn_, clf.target_, dtype=float)

  def testFtmDtypeInvalidRaises(self):
    # dtype=list (invalid type) raises ValueError
    X, y = _make_binary()
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf2.fromTrainedModel(clf.bn_, clf.target_, dtype=list)


class BNClassifierSklearnAPITestCase(pyAgrumTestCase):
  # Tests for sklearn-compatible API (get_params, set_params, clone)

  def testGetParams(self):
    # get_params() returns the correct hyperparameters
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes", discretizationNbBins=3)
    params = clf.get_params()
    self.assertEqual(params["learningMethod"], "NaiveBayes")
    self.assertEqual(clf.type_processor.defaultParamDiscretizationMethod, 3)

  def testSetParams(self):
    # set_params() updates the hyperparameters in place
    clf = skbn.createBNClassifier()
    clf.set_params(learningMethod="TAN")
    self.assertEqual(clf.learningMethod, "TAN")

  def testCloneNoFittedAttrs(self):
    # clone() creates an unfitted estimator with the same hyperparameters
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes").fit(X, y)
    clf2 = sklearn.base.clone(clf)
    self.assertFalse(hasattr(clf2, "bn_"))
    self.assertEqual(clf2.learningMethod, "NaiveBayes")

  def testCloneThenFit(self):
    # clone() followed by fit() works correctly
    X, y = _make_binary()
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes").fit(X, y)
    clf2 = sklearn.base.clone(clf)
    clf2.fit(X, y)
    self.assertTrue(hasattr(clf2, "classes_"))


class BNClassifierQualityTestCase(pyAgrumTestCase):
  # Tests for prediction quality on simple datasets

  def testPredictAccuracyAboveChance(self):
    # Accuracy on training data must exceed 0.6 with NaiveBayes
    X, y = sklearn.datasets.make_classification(
      n_samples=200,
      n_features=4,
      n_informative=4,
      n_redundant=0,
      n_classes=2,
      n_clusters_per_class=1,
      random_state=0,
    )
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes").fit(X, y)
    preds = clf.predict(X)
    acc = sklearn.metrics.accuracy_score(y, preds)
    self.assertGreater(acc, 0.6)

  def testPredictConfusionMatrixDiagonal(self):
    # The diagonal of the confusion matrix must dominate each row
    X, y = sklearn.datasets.make_classification(
      n_samples=200,
      n_features=4,
      n_informative=4,
      n_redundant=0,
      n_classes=2,
      n_clusters_per_class=1,
      random_state=0,
    )
    clf = skbn.createBNClassifier(learningMethod="NaiveBayes").fit(X, y)
    preds = clf.predict(X)
    cm = sklearn.metrics.confusion_matrix(y, preds)
    for i in range(len(cm)):
      self.assertEqual(cm[i, i], cm[i].max())


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
addTests(ts, BNCLassifierTestCase)
addTests(ts, BNClassifierFitTestCase)
addTests(ts, BNClassifierPredictTestCase)
addTests(ts, BNClassifierErrorTestCase)
addTests(ts, BNClassifierPickleTestCase)
addTests(ts, BNClassifierFromTrainedModelTestCase)
addTests(ts, BNClassifierSklearnAPITestCase)
addTests(ts, BNClassifierQualityTestCase)
