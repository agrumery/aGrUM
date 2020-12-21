# -*- coding: utf-8 -*-

import os,sys
import pyAgrum as gum

#(c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

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

def pyAgrum_header(year,author="Pierre-Henri Wuillemin/LIP6/UPMC"):
    print(os.path.basename(sys.argv[0])+" (c) "+str(year)+" -- "+author+" --")
    print("powered by pyAgrum/aGrUM "+gum.__version__)
    print()

if __name__=="__main__":
    pyAgrum_header()

