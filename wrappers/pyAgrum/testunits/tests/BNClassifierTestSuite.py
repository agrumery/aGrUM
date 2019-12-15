# -*- encoding: UTF-8 -*-
import unittest

import numpy
import pandas as pd
import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests

import pyAgrum.lib.classifier as bnc  # ok?


class BNCLassifierTestCase(pyAgrumTestCase):
  def testGetSeuil(self):
    # path de l'exemple de gaspard
    csvfile = self.agrumSrcDir('src/testunits/ressources/miniasia.csv')

    learner = gum.BNLearner(csvfile)
    learner.useGreedyHillClimbing()
    bn = learner.learnBN()
    seuil = bnc.get_threshold(bn, csvfile, 'lung_cancer?', 1)

    self.assertTrue(seuil <= 1)

  def testFitFromCsv(self):
    csvfile = self.agrumSrcDir('src/testunits/ressources/miniasia.csv')

    asia_targetColumn = 'lung_cancer?'

    bn1 = bnc.BNClassifier()
    bn1.fit_from_csv(csvfile, asia_targetColumn)

    self.assertEqual(bn1.bn().size(), 8)
    self.assertEqual(bn1.class_name, asia_targetColumn)
    self.assertTrue(bn1.threshold <= 1)

    self.assertEqual(bn1.MarkovBlanket().size(), 6)

  def testClassifierFromDf(self):
    csvfile = self.agrumSrcDir('src/testunits/ressources/miniasia.csv')

    df_asia = pd.read_csv(csvfile)
    asia_targetColumn = 'lung_cancer?'

    x_train_asia = df_asia[:9000].drop(asia_targetColumn, axis=1)
    y_train_asia = df_asia[:9000][asia_targetColumn]

    x_test_asia = df_asia[-1000:].drop(asia_targetColumn, axis=1)
    y_test_asia = df_asia[-1000:][asia_targetColumn]

    bn2 = bnc.BNClassifier()
    bn2.fit(x_train_asia, y_train_asia)

    self.assertEqual(bn2.bn().size(), 8)
    self.assertEqual(bn2.class_name, asia_targetColumn)
    self.assertTrue(bn2.threshold <= 1)

    yproba = bn2.predict_proba(x_test_asia)
    self.assertEqual(yproba.shape, (299, 2))
    self.assertEqual(yproba[0].sum(), 1)

    ypred = bn2.predict(x_test_asia)
    self.assertEqual(ypred.shape, (299,))
    self.assertIn(ypred[0], [0, 1])

    self.assertEqual(bn2.MarkovBlanket().size(), 6)


ts = unittest.TestSuite()
addTests(ts, BNCLassifierTestCase)
