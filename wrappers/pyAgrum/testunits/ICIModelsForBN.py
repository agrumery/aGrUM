# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet,RangeVar
from pyAgrumTestSuite import pyAgrumTestCase


class ICIModelsForBNTestCase(pyAgrumTestCase):

    def setUp(self):
	  pass

    def testLogit(self):
        bn = BayesNet()
        bn.add(RangeVar("age","",35,67))

ts = unittest.TestSuite()
ts.addTest(ICIModelsForBNTestCase('testLogit'))
