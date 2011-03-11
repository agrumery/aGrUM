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

def parents(bn,n):
    """
    calul une liste des nodeId des parents du noeud de nodeId n

    (clairement le calcul n'est pas très efficace...)
    """
    l=bn.cpt(n).var_names
    l.pop()
    #l is the liste of var names
    return map(bn.idFromName,l)

def enfants(bn,n):
    """
    calul une liste des nodeId des parents du noeud de nodeId n

    (clairement le calcul n'est pas efficace du tout...)
    """
    return filter(lambda x:n in parents(bn,x), range(0,len(bn)))


if __name__=="__main__":
    pyAgrum_header()

    bn=gum.loadBN("bn.bif")

    #liste des noeuds
    print map(lambda x:str(x)+':'+bn.variable(x).name(),range(0,len(bn)))

    #calcul des parents du noeud 0
    print "noeud : ",bn.variable(0).name()
    print "parents : ",parents(bn,0)

    #calcul des enfant du noeud 4
    print "noeud : ",bn.variable(4).name()
    print "enfants : ",enfants(bn,4)
