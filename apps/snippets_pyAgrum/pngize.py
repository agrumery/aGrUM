# -*- coding: utf-8 -*-
import sys
import os
import subprocess
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
    ret=subprocess.call(cmd)
    
if __name__=="__main__":
    if len(sys.argv)<2:
        print sys.argv[0]," file.bif"
    else:
        f_bn=sys.argv[1]
        bn=gum.BayesNet()
        
        base,ext=os.path.splitext(f_bn)
        if ext=='.dsl':
          bn.loadDSL(f_bn)
          pngize(bn,base)
        elif ext=='.bif':
          bn.loadBIF(f_bn)
          pngize(bn,base)
        else:
          print "erreur de format : ",ext," non reconnu"

        