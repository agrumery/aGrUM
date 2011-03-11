#!/usr/bin/python
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

import sys,os,math

from progress_bar import ProgressBar
from pyAgrum_header import pyAgrum_header

import pyAgrum as gum

def checkCompatibility(bn1,bn2):
    """ Compatibility between 2 BNs
    
    - all nodes in bn1 are in bn2
    - all nodes in bn2 are in bn1
    - all nodes with the same value have the same type.
    """
    l1=bn1.names()
    l2=bn2.names()
    
    for k in l1.keys():
        if not k in l2:
            return False
    for k in l2.keys():
        if not k in l1:
            return False
            
    for k in l1.keys():
        if len(bn1.variable(l1[k]))!=len(bn2.variable(l2[k])):
            return False
            
    return True

def computeKL(name1,name2,visible=False):
    """
    Compute KL(name1||name2) and KL(name2||name1)
    
    The function return a couple of couple : ((k1,e1),(k2,e2)). 
    - k1 and k2 are the computed values
    - e1 and e2 are the number of incompatibilities during the computations (when a proba. is null)
    """
    bn1=gum.loadBN(name1)
    bn2=gum.loadBN(name2)
    
    l2=bn2.names()
    
    if not checkCompatibility(bn1,bn2):
        print "Incompatibilité entre {0} et {1}".format(name1,name2)
        print
        sys.exit(-1)        
    
    in1=bn1.completeInstantiation()
    in2=gum.Instantiation()
    for i in range(in1.nbrDim()):
        in2.add(bn2.variable(l2[in1.variable(i).name()]))
        
    in1.setFirst()
    in2.setFirst()
    
    ex1=0
    ex2=0    
    
    kl1=0
    kl2=0
    
    while not in1.end():
        p=bn1.jointProbability(in1)
        q=bn2.jointProbability(in2)
        
        #p.ln(p/q)
        if q==0:
            if p!=0:
                ex1+=1
        else:
            if p!=0:
                kl1+=p*(math.log(p,2)-math.log(q,2))
                
        #q.ln(q/p)
        if p==0:
            if q!=0:
                ex2+=1
        else:
            if q!=0:
                kl2+=q*(math.log(q,2)-math.log(p,2))
                
        in1.inc()
        in2.inc()
        
    return ((-kl1,ex1),(-kl2,ex2))

def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print os.path.basename(sys.argv[0]),"bn1.{bif|dsl} bn2.{bif|dsl}"
    sys.exit(exit_value)

if __name__=="__main__":
    pyAgrum_header()

    if len(sys.argv)!=3:
        module_help()

    bn1_name=sys.argv[1]
    bn2_name=sys.argv[2]
    
    print '"{0}" vs "{1}"'.format(bn1_name,bn2_name)
    ((KL,eKL),(LK,eLK))=computeKL(bn1_name,bn2_name,visible=True)
    print 'KL({0}||{1})={2}[{4}]   KL({0}||{1})={3}[{5}]'.format(bn1_name,bn2_name,KL,LK,eKL,eLK)

