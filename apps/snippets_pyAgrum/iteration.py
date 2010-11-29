# -*- coding: utf-8 -*-
import pyAgrum as gum

bn=gum.BayesNet()
bn.loadBIF("bn.bif")

p_a=bn.cpt(0) #cpt de A
i=gum.Instantiation(p_a)
i.setFirst()
s=0.0;
while (not i.end()):
	print i
	s+=p_a.get(i)
	i.inc()
print s
