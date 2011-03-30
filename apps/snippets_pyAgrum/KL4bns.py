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

    print 'P:"{0}" vs Q:"{1}"'.format(bn1_name,bn2_name)
    bnP=gum.loadBN(bn1_name)
    bnQ=gum.loadBN(bn2_name)

    kl=gum.BruteForceKL(bnP,bnQ)
    r=kl.compute()

    print
    print 'KL(P||Q)={0}[{2}]   KL(Q||P)={1}[{3}]'.format(r['klPQ'],r['klQP'],r['errorPQ'],r['errorQP'])
    print 'hellinger(P,Q)={0}'.format(r['hellinger'])
    print
