# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
# (pierre-henri.wuillemin@lip6.fr)

# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.

# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!
import unittest

import numpy
import pandas as pd
import pyAgrum as gum
from .pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum.skbn as bnc


class BNCLassifierTestCase(pyAgrumTestCase):
  def testFitFromCsv(self):
    csvfile = self.agrumSrcDir('miniasia.csv')

    asia_target_column = 'lung_cancer'

    classif1 = bnc.BNClassifier()
    classif1.fit(filename=csvfile, targetName=asia_target_column)

    self.assertEqual(classif1.bn.size(), 8)
    self.assertEqual(classif1.target, asia_target_column)
    self.assertTrue(classif1.threshold <= 1)

    self.assertGreater(classif1.MarkovBlanket.size(), 0)

  def testClassifierFromDf(self):
    csvfile = self.agrumSrcDir('miniasia.csv')

    df_asia = pd.read_csv(csvfile)
    asia_target_column = 'lung_cancer'

    x_train_asia = df_asia[:9000].drop(asia_target_column, axis=1)
    y_train_asia = df_asia[:9000][asia_target_column]

    x_test_asia = df_asia[-1000:].drop(asia_target_column, axis=1)
    y_test_asia = df_asia[-1000:][asia_target_column]

    classif2 = bnc.BNClassifier()
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

    # some instantiation of parents are missing : No prior should lead to division by 0
    classif3 = bnc.BNClassifier(aPriori="NoPrior")
    with self.assertRaises(gum.DatabaseError):
      classif3.fit(x_train_asia, y_train_asia)


ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
