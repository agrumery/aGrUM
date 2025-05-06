############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
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


def _normalizeDiscretizerAudit(discretizer_audit):
  """Convert NumPy arrays in a dictionary to lists for reliable comparison."""
  return {
    k: {kk: (vv.tolist() if isinstance(vv, np.ndarray) else vv) for kk, vv in v.items()}
    for k, v in discretizer_audit.items()
  }


class BNCLassifierTestCase(pyAgrumTestCase):
  def testFitFromCsv(self):
    csvfile = self.agrumSrcDir("miniasia.csv")

    asia_target_column = "lung_cancer"

    classif1 = skbn.BNClassifier()
    classif1.fit(data=csvfile, targetName=asia_target_column)

    self.assertEqual(classif1.bn.size(), 8)
    self.assertEqual(classif1.target, asia_target_column)
    self.assertTrue(classif1.threshold <= 1)

    self.assertGreater(classif1.MarkovBlanket.size(), 0)

  def testClassifierFromDf(self):
    csvfile = self.agrumSrcDir("miniasia.csv")

    df_asia = pd.read_csv(csvfile)
    asia_target_column = "lung_cancer"

    x_train_asia = df_asia[:9000].drop(asia_target_column, axis=1)
    y_train_asia = df_asia[:9000][asia_target_column]

    x_test_asia = df_asia[-1000:].drop(asia_target_column, axis=1)
    _ = df_asia[-1000:][asia_target_column]

    classif2 = skbn.BNClassifier()
    classif2.fit(x_train_asia, y_train_asia)

    self.assertEqual(classif2.bn.size(), 8)

    self.assertEqual(classif2.target, asia_target_column)
    self.assertTrue(classif2.threshold <= 1)

    yproba = classif2.predict_proba(x_test_asia)
    self.assertEqual(yproba.shape, (299, 2))
    self.assertEqual(yproba[0].sum(), 1)

    ypred = classif2.predict(x_test_asia)
    self.assertEqual(ypred.shape, (299,))
    self.assertIn(ypred[0], [0, 1])

    self.assertGreater(classif2.MarkovBlanket.size(), 0)

    classif3 = skbn.BNClassifier()
    classif3.fit(data=csvfile, targetName="lung_cancer")

    self.assertEqual(classif3.bn.size(), 8)

    self.assertEqual(classif3.target, asia_target_column)
    self.assertTrue(classif3.threshold <= 1)

    df = pd.read_csv(csvfile)
    classif4 = skbn.BNClassifier()
    classif4.fit(data=df, targetName="lung_cancer")

    self.assertEqual(classif4.bn.size(), 8)

    self.assertEqual(classif4.target, asia_target_column)
    self.assertTrue(classif4.threshold <= 1)

    # some instantiation of parents are missing : No prior should lead to division by 0
    classif3 = skbn.BNClassifier(prior="NoPrior")
    with self.assertRaises(gum.DatabaseError):
      classif3.fit(x_train_asia, y_train_asia)

  def testPickling(self):
    bn = gum.fastBN("X1->X2{a|b|c}->Y<-X1;X3[3]->Y<-X4")
    dftrain, _ = gum.generateSample(bn, 500)
    dftest, _ = gum.generateSample(bn, 300)
    bnc = skbn.BNClassifier()

    bnc.fit(data=dftrain, targetName="Y")

    smodel = pickle.dumps(bnc)
    bnc2 = pickle.loads(smodel)

    bnc2.fit(data=dftrain, targetName="Y")

    self.assertDictEqual(
      _normalizeDiscretizerAudit(bnc.type_processor.audit(dftrain)),
      _normalizeDiscretizerAudit(bnc2.type_processor.audit(dftrain)),
    )
    self.assertDictEqual(bnc.get_params(), bnc2.get_params())
    self.assertEqual(bnc.threshold, bnc2.threshold)
    self.assertEqual((bnc.predict_proba(dftest) - bnc2.predict_proba(dftest)).max(), 0)
    self.assertEqual((bnc.predict_proba(dftest) - bnc2.predict_proba(dftest)).min(), 0)

    self.assertTrue(all(bnc.predict(dftest) == bnc2.predict(dftest)))


ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
