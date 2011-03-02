#!/usr/bin/python
# -*- coding: utf-8 -*-

import sys
import os
import subprocess

from pyAgrum_header import pyAgrum_header 
import pyAgrum as gum

def pngize(bn,base):
  """
  From a bn 'bn' and a filename 'f_bn', pngize creates 'bn.dot' and 'bn.png', representation of the bn in dot format and in png.
  """
  dotfile=base+'.dot'
  pngfile=base+'.png'

  f=open(dotfile,'w')
  f.write(bn.toDot())
  f.close()

  cmd=['dot','-Tpng',dotfile,('-o'+pngfile)]
  return subprocess.call(cmd)

if __name__=="__main__":
    pyAgrum_header()
    
    if len(sys.argv)<2:
        print os.path.basename(sys.argv[0]),"file.{bif|dsl}"
    else:
        bn=gum.loadBN(sys.argv[1])
        base,ext=os.path.splitext(sys.argv[1])
        pngize(bn,base)
