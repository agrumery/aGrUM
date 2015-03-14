# -*- coding: utf-8 -*-

#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!

"""
see http://www.ncbi.nlm.nih.gov/pubmed/1762578
or
http://www.cs.cmu.edu/afs/cs/project/jair/pub/volume10/jaakkola99a-html/node2.html

The real QMR-DT model is copyright, but we can create a random QMR-like model as follows.
"""

import pyAgrum as gum
import random

def create_qmr(nD,nF,density):
    """
    create qmr BN using

    nD = number of disease
    nF = number of findings
    density = proba for disease i to be linked to finding j
    """
    name="qmr%d-%d-%f"%(nD,nF,density)
    bn=gum.BayesNet(name)

    lDis=[bn.add(gum.LabelizedVariable("D%4d"%i,"D%4d"%i,2)) for i in range(nD)]
    
    # add noisyOR nodes with random leaks
    lFind=[bn.addNoisyOR(gum.LabelizedVariable("F%d"%i,"F%d"%i,2),random.uniform(0.1,0.9)) for i in range(nF)]
        
    for node in lDis:
        prior=random.uniform(0.01,0.99)
        bn.cpt(node)[:]=[prior,1-prior]

    for i in lDis:
        for j in lFind:
            if random.random()<density:
                # add new cause i to noisyOR j with random weight
                bn.addWeightedArc(i,j,random.uniform(0.1,0.9))
    return bn

if __name__=="__main__":
    from gumLib.bn2graph import pdfize
    print("building qmr")
    bn=create_qmr(60,60,0.1)
    print("generating pdf")
    pdfize(bn,bn.property('name'))