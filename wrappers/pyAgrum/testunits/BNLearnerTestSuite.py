# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import unittest
from pyAgrumTestSuite import pyAgrumTestCase

# tests use the compiled version of pyAgrum and not the packaged one. So we directly call the __init__.py file
import __init__ as gum # read "import pyAgrum as gum"


class BNLearnerCSVTestCase(pyAgrumTestCase):

    def setUp(self):
        pass


    def testHillClimbing(self):
        learner=gum.BNLearnerFromCSV()
        bn=learner.GreedyHillClimbing("../../../src/testunits/ressources/asia.csv")
        self.assertEqual(len(bn),8)

        with self.assertRaises(gum.IOError):
          bn=learner.GreedyHillClimbing("shouldNotExist.csv")
          
        try:
          bn=learner.GreedyHillClimbing("shouldNotExist.csv")
        except gum.IOError:
          self.assertTrue(True)
          pass
        else:
          self.assertTrue(False)
          
    def testHillClimbingAccurate(self):
        learner=gum.BNLearnerFromCSV()
        bn=learner.GreedyHillClimbing("../../../src/testunits/ressources/asia.csv")
        
        ref=gum.loadBN("../../../src/testunits/ressources/asia2.bif")
        
        f=gum.BruteForceKL(bn,ref)
        res=f.compute()
        self.assertDelta(res['klPQ'],0,1)

    def testLocalSearchWithTabu(self):
        learner=gum.BNLearnerFromCSV()
        bn=learner.LocalSearchWithTabu("../../../src/testunits/ressources/asia.csv")
        self.assertEqual(len(bn),8)

        with self.assertRaises(gum.IOError):
          bn=learner.LocalSearchWithTabu("shouldNotExist.csv")
          
        try:
          bn=learner.LocalSearchWithTabu("shouldNotExist.csv")
        except gum.IOError:
          self.assertTrue(True)
          pass
        else:
          self.assertTrue(False)
          
    def testLocalSearchWithTabuAccurate(self):
        learner=gum.BNLearnerFromCSV()
        bn=learner.LocalSearchWithTabu("../../../src/testunits/ressources/asia.csv")
        
        ref=gum.loadBN("../../../src/testunits/ressources/asia2.bif")
        
        f=gum.BruteForceKL(bn,ref)
        res=f.compute()
        self.assertDelta(res['klPQ'],0,1)
        

ts = unittest.TestSuite()
ts.addTest(BNLearnerCSVTestCase('testHillClimbing'))
ts.addTest(BNLearnerCSVTestCase('testHillClimbingAccurate'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabu'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabuAccurate'))