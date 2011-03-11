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

import sys,os,csv

from progress_bar import ProgressBar
from pyAgrum_header import pyAgrum_header

import pyAgrum as gum

def checkCompatibility(bn,fields,csv_name):
    """check if variables of the bn are in the fields
    """
    isOk=True
    for field in bn.names():
        if not field in fields:
            print "{0} is missing in {1}".format(field,csv_name)
            isOk=False
    return isOk

def computeLL(bn_name,csv_name):
    bn=gum.loadBN(bn_name)

    batchReader = csv.reader(open(csv_name,'rb'))

    titre = batchReader.next()
    fields = {}
    for i,nom in enumerate(titre):
        fields[nom]=i

    if not checkCompatibility(bn,fields,csv_name):
         module_help(1)

    return (0,0)


def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print os.path.basename(sys.argv[0]),"src.{bif|dsl} [data[.csv]]"
    sys.exit(exit_value)

if __name__=="__main__":
    pyAgrum_header()

    if len(sys.argv)<2:
        module_help()

    bn_name=sys.argv[1]

    if len(sys.argv)<=2:
        base,ext=os.path.splitext(bn_name)
        csv_name=base+'.csv'
    else:
        csv_name=sys.argv[2]
        base,ext=os.path.splitext(bn_name)
        if ext!=csv:
            csv_name+='.csv'

    (nbr,LL)=computeLL(bn_name,csv_name)

