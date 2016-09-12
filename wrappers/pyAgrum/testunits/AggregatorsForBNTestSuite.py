# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-
import pyAgrum as gum

import unittest
from pyAgrumTestSuite import pyAgrumTestCase


class AggregatorsForBNTestCase(pyAgrumTestCase):

    def setUp(self):
      pass

    def testAnd(self):
        bn = gum.BayesNet()
        c1,c2=[bn.add(gum.LabelizedVariable(item,item,2)) for item in ['C1','C2']]
        a=bn.addAND(gum.LabelizedVariable('a','a',2))
        bn.addArc(c1,a)
        bn.addArc(c2,a)
        self.assertEqual(str(bn.cpt(a)),"a<0,1>=and(C1<0,1>,C2<0,1>)")

        for i in range(2):
            bn.cpt(c1)[:]=[i,1-i]
            for j in range(2):
                bn.cpt(c2)[:]=[j,1-j]
                ie=gum.LazyPropagation(bn)
                ie.makeInference()
                if i+j==0:
                    self.assertEqual(ie.posterior(a)[:][0],0.0)
                    self.assertEqual(ie.posterior(a)[:][1],1.0)
                else:
                    self.assertEqual(ie.posterior(a)[:][0],1.0)
                    self.assertEqual(ie.posterior(a)[:][1],0.0)

    def testOr(self):
        bn = gum.BayesNet()
        c1,c2=[bn.add(gum.LabelizedVariable(item,item,2)) for item in ['C1','C2']]
        a=bn.addOR(gum.LabelizedVariable('a','a',2))
        bn.addArc(c1,a)
        bn.addArc(c2,a)

        self.assertEqual(str(bn.cpt(a)),"a<0,1>=or(C1<0,1>,C2<0,1>)")

        for i in range(2):
            bn.cpt(c1)[:]=[i,1-i]
            for j in range(2):
                bn.cpt(c2)[:]=[j,1-j]
                ie=gum.LazyPropagation(bn)
                ie.makeInference()
                if i*j==0:
                    self.assertEqual(ie.posterior(a)[:][0],0.0)
                    self.assertEqual(ie.posterior(a)[:][1],1.0)
                else:
                    self.assertEqual(ie.posterior(a)[:][0],1.0)
                    self.assertEqual(ie.posterior(a)[:][1],0.0)
        
    def testZeroParent(self):
      bn=gum.BayesNet()
      
      a=bn.addOR(gum.LabelizedVariable('a','a',2))
      self.assertEqual(bn.cpt(a).tolist(),[1.0,0.0])
      
      b=bn.addAND(gum.LabelizedVariable('b','b',2))
      self.assertEqual(bn.cpt(b).tolist(),[0.0,1.0])
      

ts = unittest.TestSuite()
ts.addTest(AggregatorsForBNTestCase('testAnd'))
ts.addTest(AggregatorsForBNTestCase('testOr'))
ts.addTest(AggregatorsForBNTestCase('testZeroParent'))
