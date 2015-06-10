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

    def testDBNTonda(self):
      dbn=gum.BayesNet()
      l=[dbn.add(gum.LabelizedVariable(name,name,nbr)) for (name,nbr) in [
        ("bf_0",4),
        ("bf_t",4),
        ("c_0" ,5),
        ("c_t" ,5),
        ("h_0" ,5),
        ("h_t" ,5),
        ("tf_0",5),
        ("tf_t",5),
        ("wl_0",4),
        ("wl_t",4)
        ]]
      for node in ["c_t","h_t","wl_t"]:
        dbn.addArc(dbn.idFromName("tf_0"),dbn.idFromName(node))
        dbn.addArc(dbn.idFromName("bf_0"),dbn.idFromName(node))
      dbn.addArc(dbn.idFromName("c_0"),dbn.idFromName("c_t"))
      dbn.addArc(dbn.idFromName("h_0"),dbn.idFromName("h_t"))
      dbn.addArc(dbn.idFromName("wl_0"),dbn.idFromName("wl_t"))

      csvfile=self.agrumSrcDir('src/testunits/ressources/DBN_Tonda.csv')
      l1=gum.BNLearner(csvfile)
      l1.useScoreLog2Likelihood()
      l1.useAprioriSmoothing()
      bn1=l1.learnParameters(dbn)

      l2=gum.BNLearner(csvfile,dbn)
      l2.useScoreLog2Likelihood()
      l2.useAprioriSmoothing()
      bn2=l2.learnParameters(dbn)

      p1=bn1.cpt(bn1.idFromName("c_0"))
      I1=gum.Instantiation(p1)
      p2=bn2.cpt(bn2.idFromName("c_0"))
      I2=gum.Instantiation(p2)
      I1.setFirst()
      I2.setFirst()
      while not I1.end():
        self.assertEquals(p1.get(I1),p2.get(I2))
        I1.inc()
        I2.inc()

ts = unittest.TestSuite()
ts.addTest(BNLearnerCSVTestCase('testHillClimbing'))
ts.addTest(BNLearnerCSVTestCase('testHillClimbingAccurate'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabu'))
ts.addTest(BNLearnerCSVTestCase('testLocalSearchWithTabuAccurate'))
ts.addTest(BNLearnerCSVTestCase('testParameterLearning'))
ts.addTest(BNLearnerCSVTestCase('testDBNTonda'))
