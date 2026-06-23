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
import pandas as pd

import pyagrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyagrum.skbn as skbn


def _normalizeDiscretizerAudit(discretizer_audit):
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


ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
