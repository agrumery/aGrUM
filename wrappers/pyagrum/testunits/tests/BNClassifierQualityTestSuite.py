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
import sklearn.datasets
import sklearn.metrics

from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.skbn as skbn


class BNClassifierQualityTestCase(pyAgrumTestCase):
  def testPredictAccuracyAboveChance(self):
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
    acc = sklearn.metrics.accuracy_score(y, clf.predict(X))
    self.assertGreater(acc, 0.6)

  def testPredictConfusionMatrixDiagonal(self):
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
    cm = sklearn.metrics.confusion_matrix(y, clf.predict(X))
    for i in range(len(cm)):
      self.assertEqual(cm[i, i], cm[i].max())


ts = unittest.TestSuite()
addTests(ts, BNClassifierQualityTestCase)
