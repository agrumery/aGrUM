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

import pyAgrum as gum
from pyAgrum_header import pyAgrum_header

def nodeId(bn,n):
    if type(n)==str:
        return bn.idFromName(n)
    else:
        return n

def nodeName(bn,n):
    if type(n)==str:
        return n
    else:
        return bn.variable(n).name()

def parents_name(bn,n):
    """
    gives a list of name of parents of node n
    """
    return map(lambda n:bn.variable(n).name(),bn.parents(nodeId(bn,n)))
    return l

def children_name(bn,n):
    """
    gives a list of name of children of node n
    """
    return map(lambda n:bn.variable(n).name(),bn.children(nodeId(bn,n)))

if __name__=="__main__":
    pyAgrum_header()

    bn=gum.loadBN("bn.bif")

    #liste des noeuds
    print map(lambda x:str(x)+':'+bn.variable(x).name(),bn.ids())

    #calcul des parents du noeud 0
    print "noeud : ",bn.variable(0).name()
    print "parents : ",bn.parents(0)
    print "parents : ",parents_name(bn,0)

    #calcul des enfant du noeud 4
    print "noeud : ",bn.variable(4).name()
    print "enfants : ",bn.children(4)
    print "enfants : ",children_name(bn,4)
