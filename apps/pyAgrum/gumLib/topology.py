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
import sys
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

def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print os.path.basename(sys.argv[0]),"bn.{"+gum.availableBNExts()+"}"
    sys.exit(exit_value)
    
if __name__=="__main__":
    pyAgrum_header(2011)
    
    if len(sys.argv)!=2:
        module_help()

    bn_name=sys.argv[1]

    bn=gum.loadBN(bn_name)
    
    if len(bn)>0:
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
