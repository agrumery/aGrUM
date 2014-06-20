# -*- coding: utf-8 -*-
# -*- encoding: UTF-8 -*-

import unittest
from pyAgrum import BayesNet,RangeVar,LabelizedVar,LazyPropagation
from pyAgrumTestSuite import pyAgrumTestCase


class ICIModelsForBNTestCase(pyAgrumTestCase):

    def setUp(self):
	  pass

    def testLogit(self):
        bn = BayesNet()
        age=bn.add(RangeVar("age","",35,67))
        taux=bn.add(RangeVar("taux","",115,171))
        angine=bn.add(LabelizedVar("angine",""))
        coeur=bn.addLogit (LabelizedVar("coeur",""),14.4937)
        
        bn.addWeightedArc(age,coeur,-0.1256)
        bn.addWeightedArc(taux,coeur,-0.0636)
        bn.addWeightedArc(angine,coeur,1.779)
        
        witness_age    = ( "50", "49", "46", "49", "62", "35", "67", "65", "47")
        witness_taux   = ("126","126","144","139","154","156","160","140","143")
        witness_angine = (  "1",  "0",  "0",  "0",  "1",  "1",  "0",  "0",  "0")
        witness_coeur  = ("OUI","OUI","OUI","OUI","OUI","OUI","NON","NON","NON")
        
        witness_proba = (0.8786,0.5807,0.3912,0.3773,0.2127,0.8760,1-0.0163,1-0.0710,1-0.3765)
        
        ie=LazyPropagation(bn)
        for i in range(len(witness_age)):
		  ie.eraseAllEvidence()
		  ie.addHardEvidence(age,bn.variable(age).index(witness_age[i]))
		  print(witness_taux[i])

ts = unittest.TestSuite()
ts.addTest(ICIModelsForBNTestCase('testLogit'))
