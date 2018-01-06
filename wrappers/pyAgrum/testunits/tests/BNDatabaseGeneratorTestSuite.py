# -*- encoding: UTF-8 -*-
import unittest

import pyAgrum as gum
from pyAgrumTestSuite import pyAgrumTestCase, addTests


class GenerateCSVTestCase(pyAgrumTestCase):
  def testSimpleUse(self):
    bn = gum.fastBN("A->B->C;A->D->C;D->E;")
    gum.generateCSV(bn, self.agrumSrcDir('src/testunits/ressources/genere.csv'),30)


class BNDatabaseGeneratorTestCase(pyAgrumTestCase):
  def testConstructor(self):
    bn = gum.fastBN("A->B->C;A->D->C;D->E;")
    dbgen=gum.BNDatabaseGenerator(bn)

  def testSetVarOrder(self):
    bn = gum.loadBN(self.agrumSrcDir('src/testunits/ressources/survey.bif'))
    dbgen=gum.BNDatabaseGenerator(bn)

    goodOrder=["A", "E", "O", "R", "S", "T"]
    dbgen.setVarOrder(goodOrder)


ts = unittest.TestSuite()
addTests(ts, GenerateCSVTestCase)
addTests(ts, BNDatabaseGeneratorTestCase)
