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

import sys
import os
import subprocess

from utils.pyAgrum_header import pyAgrum_header

import pyAgrum as gum

def dotize(aBN,name,style='pdf'):
  """
  From a bn 'bn' and a name 'bn', ize creates 'bn.dot' and 'bn.style', representation of the bn in
	dot format and in style. style in [pdf,png,fig,jpg,svg]
  """
  if style not in ['pdf','png','fig','jpg','svg']:
      raise Exception,"<%s> in not a correct style ([pdf,png,fig,jpg,svg])"%style

  if isinstance(aBN,str):
    bn=gum.loadBN(aBN)
  else:
    bn=aBN

  dotfile=name+'.dot'
  pngfile=name+'.'+style

  f=open(dotfile,'w')
  f.write(bn.toDot())
  f.close()

  cmd=['dot','-T'+style,dotfile,('-o'+pngfile)]
  return subprocess.call(cmd)


def pngize(aBN,name):
  """
  From a bn 'bn' and a name 'bn', pngize creates 'bn.dot' and 'bn.png', representation of the bn in
	dot format and in png.
  """
  dotize(aBN,name,'png')

def pdfize(aBN,name):
  """
  From a bn 'bn' and a name 'bn', pdfize creates 'bn.dot' and 'bn.pdf', representation of the bn in
	dot format and in pdf.
  """
  dotize(aBN,name,'pdf')

if __name__=="__main__":
    pyAgrum_header("2011-13")

    if len(sys.argv)<2:
        print(os.path.basename(sys.argv[0]),"file.{"+gum.availableBNExts()+"}")
    else:
        base,ext=os.path.splitext(sys.argv[1])
        pngize(sys.argv[1],base)
