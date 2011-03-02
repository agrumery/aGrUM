#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys,os,csv,random

from progress_bar import ProgressBar
from pyAgrum_header import pyAgrum_header 

import pyAgrum as gum

"""
Classe for samples generation w.r.t to a probability distribution
represented by a Bayesian network.
"""
    
def draw(tab):
    """
    Draw a value using tab as probability table.
    """
    t = random.random()
    for i in range(len(tab)):
        if t<tab[i]:
            return i
        t-=tab[i]
    return len(tab)-1
        
def nameAndParents(bn,n):
    """
    Compute a list of parents for node n in BN bn
    return name of n,list of parents names

    (clearly not optimal)
    """
    l=bn.cpt(n).var_names
    node=l.pop()
    #l is the liste of var names
    return node,l #map(bn.idFromName,l)
        
def newSample(bn,seq):
    """
    Generate a sample w.r.t to the bn using the variable sequence seq 
    (topological order)
    """
    sample = {}
    for node_id in seq:
        nod,par=nameAndParents(bn,node_id)
        sample[nod] = draw(bn.cpt(node_id)[dict([(p,sample[p]) for p in par])])

    return sample

def generateCSV(name_in, name_out, n,visible=False):
    """
    From the file name_in (BN file), generate n samples and save them in name_out
    """
    bn=gum.loadBN(name_in)
    seq=bn.getTopologicalOrder()
    writer = csv.writer(open(name_out,'wb'))    

    titles=[bn.variable(item).name() for item in seq]
    writer.writerow(titles)
    
    if visible:
        prog = ProgressBar(0, n, 77, mode='static', char='#')
        
    for i in range(n):        
        cas = newSample(bn,seq)
        row=[cas[bn.variable(item).name()] for item in seq]
        writer.writerow(row)
        
        if visible:
            prog.increment_amount()
            prog.display()
        
    if visible:
        print
        print name_out,"fully generated.\n"

def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print os.path.basename(sys.argv[0]),"src.{bif|dsl} nbr [dst.csv]"
    sys.exit(exit_value)
    
if __name__=="__main__":
    pyAgrum_header()
    
    if len(sys.argv)<3:
        module_help()
    
    bn_name=sys.argv[1]
    try:
        n=int(sys.argv[2])   
    except ValueError:
        module_help()
        
    if len(sys.argv)<4:
        base,ext=os.path.splitext(bn_name)
        csv_name=base+'.csv'
    else:
        csv_name=sys.argv[3]
        base,ext=os.path.splitext(bn_name)
        if ext!=csv:
            csv_name+='.csv'
    generateCSV(bn_name,csv_name,n,True)

