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

from .utils.progress_bar import ProgressBar
from .utils.pyAgrum_header import pyAgrum_header

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
    for field in bn.names():
        if not field in fields:
            print("** field '{0}' is missing in {1}".format(field,csv_name))
            isOK=False
        else:
            res[bn.idFromName(field)]=fields[field]

    if not isOK:
        res=None

    return res


def computeAUC(points):
    """
    Given a set of points drawing a ROC curve, compute the AUC value
    """
    #computes the integral from 0 to 1
    somme=0
    for i in range(1,len(points)):
      somme+=(points[i-1][0]-points[i][0])*points[i][1]

    return -somme

def computeROCpoints(bn,csv_name,target,label,visible=False):
    idTarget=bn.idFromName(target)
    idLabel=-1
    for i in range(bn.variable(idTarget).domainSize()):
      if bn.variable(idTarget).label(i)==label:
        idLabel=i
        break;
    assert(idLabel>=0)

    engine=gum.LazyPropagation(bn)

    nbr_lines=lines_count(csv_name)-1

    csvfile = open(csv_name, "r") # python2 = "rb"
    dialect = csv.Sniffer().sniff(csvfile.read(1024))
    csvfile.seek(0)

    batchReader = csv.reader(open(csv_name,'r'),dialect) # python2 = "rb"

    titre = batchReader.__next__() # python2 = .next()
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
        idTargetLine=int(data[positions[idTarget]])
        if idTargetLine==idLabel:
            totalP+=1
        else:
            totalN+=1

        engine.eraseAllEvidence()
        e={}
        for var in bn.names():
            if not var.__eq__(target):
                e[var]=bn.variableFromName(var).label(int(data[positions[bn.idFromName(var)]]))

        try:
            engine.setEvidence(e)
            engine.makeInference()
            px=engine.posterior(idTarget)[{target:label}]
            res.append((px,int(data[positions[idTarget]])))
        except gum.OutOfBounds as err:
            print(err)
            print("erreur : "+str(e))

        if visible:
            prog.increment_amount()
            prog.display()

    if visible:
        print

    return (res,totalP,totalN,idLabel)

def computeROC(bn,values,totalP,totalN,idLabel,modalite):
    res=sorted(values,key=lambda x:x[0])

    vp=0.0
    fp=0.0
    xopt=0.0
    yopt=0.0
    opt=100.0
    seuilopt=0
    points=[(0,0)] # first one
    old_seuil=res[0][0]
    for i in range(len(res)):
        # we add a point only if the seuil has changed
        if res[i][0]-old_seuil>1e-6: # the seuil allows to take computation variation into account
          x=vp/totalP
          y=fp/totalN
          d=x*x+(1-y)*(1-y)
          if d<opt:
            opt=d
            xopt=x
            yopt=y
            seuilopt=old_seuil
          points.append((x,y))
          old_seuil=res[i][0]

        res_id=res[i][1]
        if res_id==idLabel:
            vp+=1.0
        else:
            fp+=1.0

    points.append((1,1)) # last one

    return points,(xopt,yopt),seuilopt

def module_help(exit_value=1,message=""):
    """
    defines help viewed if args are not OK on command line, and exit with exit_value
    """
    print(os.path.basename(sys.argv[0]),"src.{"+gum.availableBNExts()+"} data[.csv] variable label")
    print()
    print(message)
    print()
    sys.exit(exit_value)

def drawROC(points,zeTitle,zeFilename,visible,show_fig,save_fig=True,
            special_point=None,special_value=None,special_label=None):
    AUC=computeAUC(points)
    import pylab

    pylab.clf()
    pylab.grid(color='#aaaaaa', linestyle='-', linewidth=1,alpha=0.5)

    pylab.plot([x[0] for x in points], [y[1] for y in points], '-', linewidth=3,color="#000088",zorder=3)
    pylab.fill_between([x[0] for x in points], [y[1] for y in points],0,color='0.9')
    pylab.plot([0.0,1.0], [0.0, 1.0], '-',color="#AAAAAA")

    pylab.ylim((-0.01,1.01))
    pylab.xlim((-0.01,1.01))
    pylab.xticks(pylab.arange(0,1.1,.1))
    pylab.yticks(pylab.arange(0,1.1,.1))
    pylab.grid(True)

    ax=pylab.gca()
    r = pylab.Rectangle((0,0), 1, 1, edgecolor='#444444', facecolor='none',zorder=1)
    ax.add_patch(r)
    [spine.set_visible(False) for spine in ax.spines.values()]

    if len(points)<10:
      for i in range(1,len(points)-1):
        pylab.plot(points[i][0],points[i][1],'o',color="#000066",zorder=6)

    pylab.xlabel('False positive rate')
    pylab.ylabel('True positive rate')

    if special_point is not None:
        pylab.plot(special_point[0],special_point[1],'o',color="#DD9999",zorder=6)
        if special_value is not None:
            pylab.text(special_point[0]+0.01,special_point[1]-0.01, special_value,
                       {'color' : '#DD5555', 'fontsize' : 10},
                       horizontalalignment = 'left',
                       verticalalignment = 'top',
                       rotation = 0,
                       clip_on = False)
    if special_label is not None:
        if special_label!="":
            labels=[special_label]
            colors=['#DD9999']
            circles=[pylab.Circle((0, 0), 1, fc=colors[0])]
            legend_location = 'lower right'
            pylab.legend(circles, labels, loc=legend_location)

    pylab.text(0.5, 0.3,'AUC=%f'%AUC,
     horizontalalignment='center',
     verticalalignment='center',
     fontsize=18)

    pylab.title(zeTitle)

    if save_fig:
        pylab.savefig(zeFilename,dpi=300)
        print("\n result in "+zeFilename)

    if show_fig:
        pylab.show()

def showROC(bn,csv_name,variable,label,visible=True,show_fig=False):
  (res,totalP,totalN,idTarget)=computeROCpoints(bn,csv_name,variable,label,visible)
  points,opt,seuil=computeROC(bn,res,totalP,totalN,idTarget,label)

  try:
    shortname=os.path.basename(bn.property("name"))
  except IndexError:
    shortname="unnamed"
    
  title=shortname+" vs "+csv_name+ " - "+variable+"="+str(label)

  figname='roc_'+shortname+"-"+csv_name+ "-"+variable+"-"+str(label)+'.png'

  drawROC(points,title,figname,not visible,show_fig,special_point=opt,special_value=seuil)

def checkROCargs():
  pyAgrum_header("2011-13")

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

  return (bn,csv_name,variable,label)

if __name__=="__main__":
  (bn,csv_name,variable,label)=checkROCargs()
  showROC(bn,csv_name,variable,label,True,False)
