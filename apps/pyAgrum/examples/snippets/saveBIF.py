# -*- coding: utf-8 -*-

#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!# -*- coding: utf-8 -*-

import sys

import pyAgrum as gum
from gumLib.pyAgrum_header import pyAgrum_header

pyAgrum_header(2011)

bn=gum.BayesNet('exo1')

# sexe = 0(H)/1(F)
# daltonisme = 0(D) / 1 (nonD)

sexe,daltonisme=[bn.add(gum.LabelizedVar(nom,'',2)) for nom in 'sexe daltonisme'.split()]
bn.insertArc(sexe,daltonisme)

bn.cpt(sexe)[:]=[0.5, 0.5]

bn.cpt(daltonisme)[0,:]=[0.08, 0.92]
bn.cpt(daltonisme)[1,:]=[0.005,0.995]

bn.saveBIF("exo1.bif")
for line in open("exo1.bif"):
    print line,

print("for gum.loadBN or gum.saveBN, possible files ext are ="+gum.availableBNExts())
gum.saveBN(bn,"exo1.dsl")
for line in open("exo1.dsl"):
    print line,
