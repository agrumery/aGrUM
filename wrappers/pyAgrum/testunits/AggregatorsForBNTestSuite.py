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
        c1,c2=[bn.addAND(gum.LabelizedVariable(item,item,2)) for item in ['C1','C2']]
        a=bn.add(gum.LabelizedVariable('a','a',2))
        bn.addArc(c1,a)
        bn.addArc(c2,a)
        print(bn.cpt(a))
        inst=gum.Instantiation(bn.cpt(a))
        while not inst.end():
            print(bn.cpt(a).get(inst))
            inst.inc()

    def testOr(self):
        bn = gum.BayesNet()
        c1,c2=[bn.addOR(gum.LabelizedVariable(item,item,2)) for item in ['C1','C2']]
        a=bn.add(gum.LabelizedVariable('a','a',2))
        bn.addArc(c1,a)
        bn.addArc(c2,a)
        print(bn.cpt(a))

ts = unittest.TestSuite()
ts.addTest(AggregatorsForBNTestCase('testAnd'))
ts.addTest(AggregatorsForBNTestCase('testOr'))
