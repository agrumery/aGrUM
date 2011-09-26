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

import sys,os,csv,math

from pyAgrum_header import pyAgrum_header
from topology import parents

import pyAgrum as gum

def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print os.path.basename(sys.argv[0]),"src.{"+gum.availableBNExts()+"}"
    sys.exit(exit_value)


if __name__=="__main__":
    pyAgrum_header()

    if len(sys.argv)<2:
        module_help()

    bn=gum.loadBN(sys.argv[1])
    inf=gum.LazyPropagation(bn)
    inf.makeInference()

    print "Entropy of nodes (a posteriori)"
    res=[]
    for i in bn.getTopologicalOrder():
      res.append((bn.variable(i).name(),inf.H(i)))
    res=sorted(res,key=lambda x:-x[1])
    for i in range(len(res)):
        print res[i]

    print
    print "Mutual information and Variation of information for arcs (a posteriori)"
    res=[]
    for i in bn.getTopologicalOrder():
      for j in parents(bn,i):
        res.append((bn.variable(j).name()+"->"+bn.variable(i).name(),inf.I(i,j),inf.VI(i,j)))
    res=sorted(res,key=lambda x:-x[1])
    for i in range(len(res)):
      print res[i]