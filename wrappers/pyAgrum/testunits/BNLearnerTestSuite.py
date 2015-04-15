# -*- encoding: UTF-8 -*-
import pyAgrum as gum 

import unittest
from pyAgrumTestSuite import pyAgrumTestCase


class BNLearnerCSVTestCase(pyAgrumTestCase):

    def setUp(self):
        pass


    def testHillClimbing(self):
        learner=gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia.csv'))
        learner.useGreedyHillClimbing()
        bn=learner.learnBN()
        self.assertEqual(bn.size(),8)

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
        learner=gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia.csv'))
        
        witness=['smoking?',
                 'lung_cancer?',
                 'bronchitis?',
                 'visit_to_Asia?',
                 'tuberculosis?',
                 'tuberculos_or_cancer?',
                 'dyspnoea?',
                 'positive_XraY?']
        for n in witness:
          self.assertTrue(n in learner.names())
        for n in learner.names():
          self.assertTrue(n in witness)
          
        learner.useGreedyHillClimbing()
        bn=learner.learnBN()

        ref=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/asia2.bif'))

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
        learner=gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia.csv'))
        learner.useLocalSearchWithTabuList()
        
        bn=learner.learnBN()

        ref=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/asia2.bif'))

        f=gum.BruteForceKL(bn,ref)
        res=f.compute()
        self.assertDelta(res['klPQ'],0,1)
        
     
    def testParameterLearning(self):
        bn=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/asia_bool.bif'))
        
        learner=gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3.csv'),bn)  
        learner.useScoreLog2Likelihood()
        learner.useAprioriSmoothing(1.0)
                
        bn2=learner.learnParameters(bn)
        for i in range(bn.size()):	
          self.assertEquals(str(bn2.variable(i)),str(bn.variable(bn.idFromName(bn2.variable(i).name()))))	
        
        bn=gum.loadBN(self.agrumSrcDir('src/testunits/ressources/asia_bool.bif'))        
        # there is a beurk modality in asia3-faulty.csv
        with self.assertRaises(gum.UnknownLabelInDatabase):
          learner=gum.BNLearner(self.agrumSrcDir('src/testunits/ressources/asia3-faulty.csv'),bn)  

        
ts = unittest.TestSuite()
ts.addTest(BNLearnerCSVTestCase('testHillClimbing'))
ts.addTest(BNLearnerCSVTestCase('testHillClimbingAccurate'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabu'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabuAccurate'))
ts.addTest(BNLearnerCSVTestCase('testParameterLearning'))
