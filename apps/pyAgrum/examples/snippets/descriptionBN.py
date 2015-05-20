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
description of a BN
"""
import pyAgrum as gum

def showBN(bn):
    print('---------------------------------')
    print(bn.property("name"))
    print('---------------------------------')
    print(bn)
    print('---------------------------------')
    
    l=[len(bn.variable(i)) for i in bn.ids()]
    print('variables domainSize : min={0} max={1}'.format(min(l),max(l)))
    
    print('parents : max={0}'.format(max([len(bn.parents(i)) for i in bn.ids()])))
    print('---------------------------------')

    for i in bn.ids():
        print('{0} : {1}'.format(i,str(bn.variable(i))))
    print('---------------------------------')
    
    for (i,j) in bn.arcs():
        print('{0}->{1}'.format(bn.variable(i).name(),bn.variable(j).name()))
    print('---------------------------------')
        

#load the file alarm.dsl
bn=gum.loadBN("../resources/alarm.dsl")

showBN(bn)