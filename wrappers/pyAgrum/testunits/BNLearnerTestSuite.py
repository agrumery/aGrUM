# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import unittest
from pyAgrumTestSuite import pyAgrumTestCase
import pyAgrum as gum


class BNLearnerCSVTestCase(pyAgrumTestCase):

    def setUp(self):
        pass


    def testHillClimbing(self):
        learner=gum.BNLearnerFromCSV()
        bn=learner.GreedyHillClimbing("../../../src/testunits/ressources/asia.csv")
        self.assertEqual(len(bn),8)

        self.assertRaises(gum.IOError,learner.GreedyHillClimbing("shouldNotExist.csv"))


ts = unittest.TestSuite()
ts.addTest(BNLearnerCSVTestCase('testHillClimbing'))