# -*- coding: utf-8 -*-
from pyAgrum import *

bn=BayesNet('exo1')

# sexe = 0(H)/1(F)
# daltonisme = 0(D) / 1 (nonD)

sexe,daltonisme=[bn.add(LabelizedVar(nom,'',2)) for nom in 'sexe daltonisme'.split()]
bn.insertArc(sexe,daltonisme)

bn.cpt(sexe)[:]=[0.5, 0.5]

bn.cpt(daltonisme)[0,:]=[0.08, 0.92]
bn.cpt(daltonisme)[1,:]=[0.005,0.995]

bn.saveBIF('exo1.bif')