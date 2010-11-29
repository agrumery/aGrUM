# -*- coding: utf-8 -*-
import pyAgrum as gum

bn=gum.BayesNet()
bn.loadBIF("bn.bif")

print bn.variable(0).name()
# a

print bn.cpt(0)
#[[[ 0.48319     0.51681   ]
#  [ 0.108427    0.89157301]
#  [ 0.43582699  0.56417298]]
#
# [[ 0.0250064   0.974994  ]
#  [ 0.677037    0.322963  ]
#  [ 0.78677201  0.213228  ]]]

print bn.cpt(0)[{'e':0,'f':1}]
# [ 0.0250064  0.974994 ]