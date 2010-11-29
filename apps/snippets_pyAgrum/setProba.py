# -*- coding: utf-8 -*-
import pyAgrum as gum

bn=gum.BayesNet()
bn.loadBIF("bn.bif")

print bn.variable(0).name()
# a

print bn.cpt(0)[{'e':0,'f':1}]
# [ 0.0250064  0.974994 ]

bn.cpt(0)[{'e':0,'f':1}]=[1,0]
print bn.cpt(0)[{'e':0,'f':1}]
# [ 1. 0. ]

bn.cpt(0)[{'a':0, 'e':0,'f':1}]=12
print bn.cpt(0)[{'e':0,'f':1}]
# [ 12. 0. ]
