# -*- encoding: UTF-8 -*-
import unittest

import numpy
import pandas as pd
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum.skbn as bnc


class BNCLassifierTestCase(pyAgrumTestCase):
  def testFitFromCsv(self):
    csvfile = self.agrumSrcDir('src/testunits/ressources/miniasia.csv')

    asia_targetColumn = 'lung_cancer'

    classif1 = bnc.BNClassifier()
    classif1.fit(filename=csvfile, targetName=asia_targetColumn)

    self.assertEqual(classif1.bn.size(), 8)
    self.assertEqual(classif1.target, asia_targetColumn)
    self.assertTrue(classif1.threshold <= 1)

    self.assertEqual(classif1.MarkovBlanket.size(), 6)

  def testClassifierFromDf(self):
    csvfile = self.agrumSrcDir('src/testunits/ressources/miniasia.csv')

    df_asia = pd.read_csv(csvfile)
    asia_targetColumn = 'lung_cancer'

    x_train_asia = df_asia[:9000].drop(asia_targetColumn, axis=1)
    y_train_asia = df_asia[:9000][asia_targetColumn]

    x_test_asia = df_asia[-1000:].drop(asia_targetColumn, axis=1)
    y_test_asia = df_asia[-1000:][asia_targetColumn]

    classif2 = bnc.BNClassifier()
    classif2.fit(x_train_asia, y_train_asia)

    self.assertEqual(classif2.bn.size(), 8)

    self.assertEqual(classif2.target, asia_targetColumn)
    self.assertTrue(classif2.threshold <= 1)

    yproba = classif2.predict_proba(x_test_asia)
    self.assertEqual(yproba.shape, (299, 2))
    self.assertEqual(yproba[0].sum(), 1)

    ypred = classif2.predict(x_test_asia)
    self.assertEqual(ypred.shape, (299,))
    self.assertIn(ypred[0], [0, 1])

    self.assertEqual(classif2.MarkovBlanket.size(), 6)

ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
