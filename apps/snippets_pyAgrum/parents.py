# -*- coding: utf-8 -*-
import pyAgrum as gum

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
    bn=gum.BayesNet()
    bn.loadBIF("bn.bif")
    
    #liste des noeuds
    print map(lambda x:str(x)+':'+bn.variable(x).name(),range(0,len(bn)))
    
    #calcul des parents du noeud 0
    print "noeud : ",bn.variable(0).name()
    print "parents : ",parents(bn,0)
    
    #calcul des enfant du noeud 4
    print "noeud : ",bn.variable(4).name()
    print "enfants : ",enfants(bn,4)
