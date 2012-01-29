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
from gumLib.pyAgrum_header import pyAgrum_header

pyAgrum_header(2011)

bn=gum.loadBN("bn.bif")

print bn.variable(0).name()
# a

print bn.cpt(0)[{'e':0,'f':1}]
# [ 0.0250064  0.974994 ]

bn.cpt(0)[{'e':0,'f':1}]=[1,0]
print bn.cpt(0)[{'e':0,'f':1}]
# [ 1. 0. ]

bn.cpt(0)[{'a':0, 'e':0,'f':1}]=12
print bn.cpt(0)[{'e':0,'f':1}]
# [ 12. 0. ]
