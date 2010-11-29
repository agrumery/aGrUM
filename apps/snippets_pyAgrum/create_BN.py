# -*- coding: utf-8 -*-
"""
How to create a BN with pyAgrum
"""

import pyAgrum as gum

bn=gum.BayesNet()

x,y,z=([bn.add(gum.LabelizedVar(nom,"",2)) for nom in "x y z".split()])
bn.insertArc(x,y)
bn.insertArc(y,z)

#this should create an impossible arc (since a bn is a DAG)
#bn.insertArc(z,x)