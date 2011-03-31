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

import sys,os,csv,math

from progress_bar import ProgressBar
from pyAgrum_header import pyAgrum_header

import pyAgrum as gum

def lines_count(filename):
    """ count lines in a file
    """
    numlines = 0
    for line in open(filename): numlines += 1

    return numlines

def checkCompatibility(bn,fields,csv_name):
    """check if variables of the bn are in the fields

    if not : return None
    if compatibilty : return a list of position for variables in fields
    """
    res={}
    isOK=True
    names=bn.names()
    for field in bn.names():
        if not field in fields:
            print "** field '{0}' is missing in {1}".format(field,csv_name)
            isOK=False
        else:
            res[names[field]]=fields[field]

    if not isOK:
        res=None

    return res

def computeROC(bn,csv_name,target,label,visible=False,transforme_label=None):
    idTarget=bn.idFromName(target)
    engine=gum.LazyPropagation(bn)
    
    nbr_lines=lines_count(csv_name)-1

    csvfile = open(csv_name, "rb")
    dialect = csv.Sniffer().sniff(csvfile.read(1024))
    csvfile.seek(0)

    batchReader = csv.reader(open(csv_name,'rb'),dialect)

    titre = batchReader.next()
    fields = {}
    for i,nom in enumerate(titre):
        fields[nom]=i

    positions=checkCompatibility(bn,fields,csv_name)
    if positions is None:
         sys.exit(1)

    if visible:
        prog = ProgressBar(csv_name+' : ',0, nbr_lines, 77,  mode='static', char='#')
        prog.display()

    totalP=0
    totalN=0
    res=[]
    for data in batchReader:
        if data[positions[idTarget]].__eq__(label):
            totalP+=1
        else:
            totalN+=1
            
        engine.eraseAllEvidence()
        e={}
        for var in bn.names():
            if not var.__eq__(target):
                e[var]=data[positions[bn.idFromName(var)]]
                if not transforme_label is None:
                    e[var]=transforme_label(e[var])
        engine.setEvidence(e)
        engine.makeInference()
        px=engine.marginal(idTarget)[{target:label}]
        res.append((px,data[positions[idTarget]]))

        if visible:
            prog.increment_amount()
            prog.display()

    if visible:
        print
    
    res=sorted(res,key=lambda x:-x[0])
    
    vp=0.0
    fp=0.0
    points=[(vp/totalP,fp/totalN)]
    for i in range(len(res)):
        if res[i][1].__eq__(label):
            vp+=1.0
        else:
            fp+=1.0
        points.append((vp/totalP,fp/totalN))
    
    return points

def getNumLabel(inst,i,label,transforme_label):
    if transforme_label is not None:
        label=transforme_label(label)
    
    if label.isdigit(): # an indice
        return int(label)
    else:
        return inst.variable(i)[label]

def add_state(label):
    if label.isdigit():
        return "State_"+str(int(label))
    else:
        return label

def module_help(exit_value=1,message=""):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print os.path.basename(sys.argv[0]),"src.{bif|dsl} data[.csv] variable label"
    print
    print message
    print
    sys.exit(exit_value)

if __name__=="__main__":
    pyAgrum_header()

    bn_name=sys.argv[1] if len(sys.argv)>1 else ""
    csv_name=sys.argv[2] if len(sys.argv)>2 else ""
    variable=sys.argv[3] if len(sys.argv)>3 else ""
    label=sys.argv[4] if len(sys.argv)>4 else ""
    
    if bn_name.__eq__(""): 
        module_help()    
    
    bn=gum.loadBN(bn_name)
    
    if csv_name.__eq__(""): 
        module_help()
    
    if variable.__eq__(""): 
        module_help(message=" Variables : "+str(bn.names()))
    else:
        if variable not in bn.names():
            module_help(message=" Variable '"+variable+"'not found.\n Variables : "+str(bn.names()))
        
    if label.__eq__(""): 
        module_help(message=" Labels : "+str(bn.variableFromName(variable)))
    else:
        try:
            bn.variableFromName(variable)[label]
        except gum.OutOfBounds:
            module_help(message=" Label '"+label+"' not found.\n Labels : "+str(bn.variableFromName(variable)))

    points=computeROC(bn,csv_name,variable,label,visible=True,transforme_label=add_state)
    print points[0]
    print points[1]
    print points[2]
    print points[3]
    print "..."
    print points[len(points)-4]
    print points[len(points)-3]
    print points[len(points)-2]
    print points[len(points)-1]
    
    import pylab

    pylab.clf()
    pylab.ylim((0,1))
    pylab.xlim((0,1))
    pylab.xticks(pylab.arange(0,1.1,.1))
    pylab.yticks(pylab.arange(0,1.1,.1))
    pylab.grid(True)
        
    pylab.plot([x[0] for x in points], [y[1] for y in points], '-', linewidth=1)
    pylab.plot([0.0,1.0], [0.0, 1.0], 'k-', label= bn_name)
    
    pylab.legend(loc='lower right')
    pylab.show()