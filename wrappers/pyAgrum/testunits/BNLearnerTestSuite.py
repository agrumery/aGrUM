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
        learner=gum.BNLearner("../../../src/testunits/ressources/asia.csv")
        learner.useGreedyHillClimbing()
        bn=learner.learnBN()
        self.assertEqual(len(bn),8)

        with self.assertRaises(gum.IOError):
          learner=gum.BNLearner("shouldNotExist.csv")

        try:
          learner=gum.BNLearner("shouldNotExist.csv")
        except gum.IOError:
          self.assertTrue(True)
          pass
        else:
          self.assertTrue(False)

    def testHillClimbingAccurate(self):
        learner=gum.BNLearner("../../../src/testunits/ressources/asia.csv")
        learner.useGreedyHillClimbing()
        bn=learner.learnBN()

        ref=gum.loadBN("../../../src/testunits/ressources/asia2.bif")

        f=gum.BruteForceKL(bn,ref)
        res=f.compute()
        self.assertDelta(res['klPQ'],0,1)

    def testLocalSearchWithTabu(self):
        with self.assertRaises(gum.IOError):
          learner=gum.BNLearner("shouldNotExist.csv")

        try:
          learner=gum.BNLearner("shouldNotExist.csv")
        except gum.IOError:
          self.assertTrue(True)
          pass
        else:
          self.assertTrue(False)

    def testLocalSearchWithTabuAccurate(self):
        learner=gum.BNLearner("../../../src/testunits/ressources/asia.csv")
        learner.useLocalSearchWithTabuList()
        bn=learner.learnBN()

        ref=gum.loadBN("../../../src/testunits/ressources/asia2.bif")

        f=gum.BruteForceKL(bn,ref)
        res=f.compute()
        self.assertDelta(res['klPQ'],0,1)


ts = unittest.TestSuite()
ts.addTest(BNLearnerCSVTestCase('testHillClimbing'))
ts.addTest(BNLearnerCSVTestCase('testHillClimbingAccurate'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabu'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabuAccurate'))
