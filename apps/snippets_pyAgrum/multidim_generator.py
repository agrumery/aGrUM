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

from pyAgrum_header import pyAgrum_header

def multidim_generator(bornes,sauf={}):
    """
    Générateur pour une itération sur une série de variables dont on connait le nombre de modalités.

    bornes est un dictionnaire du type de {key1:3,key2:4,key3:5} (key1 est une variable à 3 modalités, etc.)
    sauf est un dictionnaire du type de {key2:1} : on ne veut pas itérer sur key2, qui vaudra 1 dans l'itérateur
    """
    valeurs={}
    for k in bornes:
        if (sauf.has_key(k)):
            valeurs[k]=sauf[k]
        else:
            valeurs[k]=0

    done=False
    while (not done):
        yield valeurs
        done=True
        for k in valeurs:
            if (sauf.has_key(k)):
                continue
            if (valeurs[k]<bornes[k]-1):
                valeurs[k]+=1
                done=False
                break
            else:
                valeurs[k]=0


if __name__=="__main__":
    pyAgrum_header()

    print "#### ITERATION SUR LES 3 VARIABLES #####"
    for i in multidim_generator(dict(x=4,y=3,z=2)):
        print i

    print "#### ITERATION SUR Y ET Z (X VAUT 3) #####"
    for i in multidim_generator(dict(x=4,y=3,z=2),dict(x=3)):
        print i

    print "#### ITERATION SUR X ET Y (Z VAUT 1) #####"
    for i in multidim_generator(dict(x=4,y=3,z=2),dict(z=1)):
        print i

    print "#### ITERATION SUR Y (X VAUT 1 et Z VAUT 0) #####"
    for i in multidim_generator(dict(x=4,y=3,z=2),dict(x=1,z=0)):
        print i
