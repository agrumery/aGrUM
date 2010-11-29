# -*- coding: utf-8 -*-
import pyAgrum as gum

bn=gum.BayesNet()
bn.loadBIF("bn.bif")

print bn