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

import pyAgrum.skbn as skbn


class BNCLassifierTestCase(pyAgrumTestCase):
  def testFitFromCsv(self):
    csvfile = self.agrumSrcDir('miniasia.csv')

    asia_target_column = 'lung_cancer'

    classif1 = skbn.BNClassifier()
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

    # some instantiation of parents are missing : No prior should lead to division by 0
    classif3 = skbn.BNClassifier(aPriori="NoPrior")
    with self.assertRaises(gum.DatabaseError):
      classif3.fit(x_train_asia, y_train_asia)

  def testCreateVariableFromDf(self):
    X = pd.DataFrame.from_dict({
      'var1': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 1, 2, 3],
      'var2': ['a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n'],
      'var3': [1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2, 5, 1, 2],
      'var4': [1.11, 2.213, 3.33, 4.23, 5.42, 6.6, 7.5, 8.9, 9.19, 10.11, 11.12, 12.21, 13.3, 14.5],
      'var5': [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 1]
    })

    discretizer = skbn.BNDiscretizer()
    self.assertEquals(str(discretizer.createVariable('var1', X['var1'])),
                      "var1:Range([1,11])")
    self.assertEquals(str(discretizer.createVariable('var2', X['var2'])),
                      "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})")
    self.assertEquals(str(discretizer.createVariable('var3', X['var3'])),
                      "var3:Integer({1|2|5})")
    self.assertEquals(str(discretizer.createVariable('var4', X['var4'])),
                      "var4:Labelized({1.11|2.213|3.33|4.23|5.42|6.6|7.5|8.9|9.19|10.11|11.12|12.21|13.3|14.5})")
    self.assertEquals(str(discretizer.createVariable('var5', X['var5'])),
                      "var5:Range([1,13])")

    discretizer = skbn.BNDiscretizer(discretizationThreshold=13)
    self.assertEquals(str(discretizer.createVariable('var1', X['var1'])),
                      "var1:Range([1,11])")
    self.assertEquals(str(discretizer.createVariable('var2', X['var2'])),
                      "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})")
    self.assertEquals(str(discretizer.createVariable('var3', X['var3'])),
                      "var3:Integer({1|2|5})")
    self.assertEquals(str(discretizer.createVariable('var4', X['var4'])),
                      "var4:Discretized(<(1.11;2.5481[,[2.5481;3.87[,[3.87;5.301[,[5.301;6.78[,[6.78;8.2[,[8.2;9.132[,[9.132;10.211[,[10.211;11.556[,[11.556;12.973[,[12.973;14.5)>)")
    self.assertEquals(str(discretizer.createVariable('var5', X['var5'])),
                      "var5:Range([1,13])")

    discretizer = skbn.BNDiscretizer(discretizationThreshold=11)
    self.assertEquals(str(discretizer.createVariable('var1', X['var1'])),
                      "var1:Range([1,11])")
    self.assertEquals(str(discretizer.createVariable('var2', X['var2'])),
                      "var2:Labelized({a|b|c|d|e|f|g|h|i|j|k|l|m|n})")
    self.assertEquals(str(discretizer.createVariable('var3', X['var3'])),
                      "var3:Integer({1|2|5})")
    self.assertEquals(str(discretizer.createVariable('var4', X['var4'])),
                      "var4:Discretized(<(1.11;2.5481[,[2.5481;3.87[,[3.87;5.301[,[5.301;6.78[,[6.78;8.2[,[8.2;9.132[,[9.132;10.211[,[10.211;11.556[,[11.556;12.973[,[12.973;14.5)>)")
    self.assertEquals(str(discretizer.createVariable('var5', X['var5'])),
                      "var5:Discretized(<(1;1.3[,[1.3;2.6[,[2.6;3.9[,[3.9;5.2[,[5.2;6.5[,[6.5;7.8[,[7.8;9.1[,[9.1;10.4[,[10.4;11.7[,[11.7;13)>)")


ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
