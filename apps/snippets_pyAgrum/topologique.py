# -*- coding: utf-8 -*-
import pyAgrum as gum

bn=gum.BayesNet()
bn.loadBIF("bn.bif")

seq=bn.getTopologicalOrder()

for i in range(0,len(bn)):
	print "%d : %d (%s)"%(i,seq[i],bn.variable(seq[i]).name())
