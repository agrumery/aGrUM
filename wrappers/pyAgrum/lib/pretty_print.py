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
import sys,os

from .utils.pyAgrum_header import pyAgrum_header
import pyAgrum as gum

DECIMAL_LENGTH=4
NUMBER_FORMAT='{0:.'+str(DECIMAL_LENGTH)+'f}'
BLANK=' '

def module_help(exit_value=1):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print(os.path.basename(sys.argv[0]),"src.{"+gum.availableBNExts()+"}")
    sys.exit(exit_value)

def max_length(v):
  m=len(v.name())
  for i in range(v.domainSize()):
    m=max(m,len(v.label(i)))
  return m

def pretty_cpt(cpt):
  size=(1+2+DECIMAL_LENGTH)*cpt.variable(0).domainSize()-1

  width={}
  total_width=0
  for j in range(1,cpt.nbrDim()):
    width[j]=2+max_length(cpt.variable(j))
    total_width+=width[j]+1
  total_width-=1

  print(' '*total_width+'|-'+'-'*size+'-|')

  line='-'*total_width+'|'
  line+="{0:^{1}}".format(cpt.variable(0).name()[0:size+2],size+2)
  line+='|'
  print(line)

  line=''
  for j in range(1,cpt.nbrDim()):
    line+="{0:^{1}}|".format(cpt.variable(j).name(),width[j])
  if line=='':
    line='|'
  line+=BLANK
  for j in range(cpt.variable(0).domainSize()):
    line+=("{0:^{1}}"+BLANK).format(cpt.variable(0).label(j)[0:DECIMAL_LENGTH+2],DECIMAL_LENGTH+2)
  line+='|'
  print(line)
  print(('-'*total_width)+'|'+'-'*(2+size)+'|')

  i=gum.Instantiation(cpt)
  i.setFirst()
  while not(i.end()):
      line=''
      for j in range(1,cpt.nbrDim()):
        line+="{0:^{1}}|".format(cpt.variable(j).label(i.val(j)),width[j])
      if line=='':
        line='|'
      line+=' '
      for j in range(cpt.variable(0).domainSize()):
        line+=NUMBER_FORMAT.format(cpt.get(i))+' '
        i.inc()
      line+='|'
      print(line)

  print(('-'*total_width)+'|'+'-'*(2+size)+'|')

def pretty_bn(aBN):
  if isinstance(aBN,str):
    bn=gum.loadBN(aBN)
  else:
    bn=aBN

  seq=bn.topologicalOrder()
  for nod in seq:
    pretty_cpt(bn.cpt(nod))
  print("")

if __name__=="__main__":
    pyAgrum_header(2011)

    if len(sys.argv)<2:
        module_help()

    pretty_bn(sys.argv[1])
