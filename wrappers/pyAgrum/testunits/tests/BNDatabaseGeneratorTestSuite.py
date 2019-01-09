# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class GenerateCSVTestCase(pyAgrumTestCase):
  def testSimpleUse(self):
    bn = gum.fastBN("A->B->C;A->D->C;D->E;")
    gum.generateCSV(bn, self.agrumSrcDir('src/testunits/ressources/genere.csv'), 30)


class BNDatabaseGeneratorTestCase(pyAgrumTestCase):
  def testConstructor(self):
    bn = gum.fastBN("A->B->C;A->D->C;D->E;")
    dbgen = gum.BNDatabaseGenerator(bn)

  def testSetVarOrder(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/survey.bif'))
    dbgen = gum.BNDatabaseGenerator(bn)

    dbgen.setVarOrderFromCSV(self.agrumSrcDir('src/testunits/ressources/survey1.csv'))
    self.assertEqual(dbgen.varOrderNames(), ('E', 'A', 'O', 'T', 'R', 'S'))

    dbgen.setVarOrder(["A", "E", "O", "R", "S", "T"])
    self.assertEqual(dbgen.varOrderNames(), ('A', 'E', 'O', 'R', 'S', 'T'))

    with self.assertRaises(gum.FatalError):
      dbgen.setVarOrder(["A", "E", "O", "R", "A", "S", "T"])

    with self.assertRaises(gum.FatalError):
      dbgen.setVarOrder(["A", "O", "R", "S", "T"])

    with self.assertRaises(gum.NotFound):
      dbgen.setVarOrder(["A", "O", "R", "S", "T", "X"])

  def testDrawSamples(self):
    bn = gum.fastBN("A->B[4]->C;A->D->C;D->E[3];")
    dbgen = gum.BNDatabaseGenerator(bn)

    nbsample = 100
    nbsamples = [nbsample * i for i in [1, 100, 1000]]

    ns1, ns2, ns3 = nbsamples

    isOK=False
    for i in range(self.nbLoopForApproximatedTest):
      try:
        ll1, ll2, ll3 = [dbgen.drawSamples(n) for n in nbsamples]

        self.assertAlmostEqual(ns1 / ns2, ll1 / ll2, delta=0.1)
        self.assertAlmostEqual(ns3 / ns2, ll3 / ll2, delta=0.1)
        self.assertAlmostEqual(ns1 / ns3, ll1 / ll3, delta=0.1)
        isOK=True
        break
      except AssertionError:
        pass
    self.assertEqual(isOK,True," Error in loop for Approximated tests")



    jointe = gum.Potential().fillWith(1)
    for i in bn.nodes():
      jointe *= bn.cpt(i)
    entropy = jointe.entropy()

    self.assertAlmostEqual(entropy, -ll1 / ns1, delta=0.5)
    self.assertAlmostEqual(entropy, -ll2 / ns2, delta=0.2)
    self.assertAlmostEqual(entropy, -ll3 / ns3, delta=0.1)


ts = unittest.TestSuite()
addTests(ts, GenerateCSVTestCase)
addTests(ts, BNDatabaseGeneratorTestCase)
