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
import sklearn.datasets
from sklearn.exceptions import NotFittedError

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


class BNClassifierErrorTestCase(pyAgrumTestCase):
  def testErrorFitYNone(self):
    X, _ = _make_binary()
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError) as ctx:
      clf.fit(X, None)
    msg = str(ctx.exception).lower()
    self.assertTrue("y" in msg or "none" in msg)

  def testErrorFitSingleClass(self):
    X, _ = _make_binary()
    y_single = np.zeros(len(X), dtype=int)
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fit(X, y_single)

  def testErrorFitContinuousY(self):
    X, _ = _make_binary()
    y_cont = np.random.rand(len(X))
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fit(X, y_cont)

  def testErrorPredictNotFitted(self):
    X, _ = _make_binary()
    clf = skbn.createBNClassifier()
    with self.assertRaises(NotFittedError):
      clf.predict(X)

  def testErrorPredictProbaNotFitted(self):
    X, _ = _make_binary()
    clf = skbn.createBNClassifier()
    with self.assertRaises(NotFittedError):
      clf.predict_proba(X)

  def testErrorPredictWrongNFeatures(self):
    X, y = _make_binary(n_features=4)
    clf = skbn.createBNClassifier().fit(X, y)
    X_wrong = np.random.rand(10, 7)
    with self.assertRaises(ValueError):
      clf.predict(X_wrong)

  def testErrorPredictWrongFeaturesFromTrainedModel(self):
    X, y = _make_binary(n_features=4)
    clf = skbn.createBNClassifier().fit(X, y)
    clf2 = skbn.createBNClassifier()
    clf2.fromTrainedModel(clf.bn_, clf.target_)
    X_wrong = np.random.rand(10, 7)
    with self.assertRaises(ValueError):
      clf2.predict(X_wrong)

  def testErrorfitFromTabularNoTargetName(self):
    X, y = _make_binary()
    df = pd.DataFrame(X)
    df["target"] = y
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fitFromTabular(df, None)

  def testErrorfitFromTabularNoData(self):
    clf = skbn.createBNClassifier()
    with self.assertRaises(ValueError):
      clf.fitFromTabular(None, "target")


ts = unittest.TestSuite()
addTests(ts, BNClassifierErrorTestCase)
