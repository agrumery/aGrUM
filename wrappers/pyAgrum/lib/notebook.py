#!/usr/bin/python
# -*- coding: utf-8 -*-

#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2012  (pierre-henri.wuillemin@lip6.fr)

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

"""
tools for BN analysis in ipython qtconsole and notebook

(but can be used every where)
"""
from __future__ import print_function
import base64 
import numpy as np
import time

import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

import pydotplus as dot

import IPython.display
from IPython.core.pylabtools import print_figure
from IPython.core.display import Image,display_png
from IPython.display import display,HTML,SVG

import shutil

import pyAgrum as gum

def configuration():
  """
  Display the collection of dependance and versions 
  """
  from collections import OrderedDict
  import sys,os

  packages=OrderedDict()
  packages["OS"]=   "%s [%s]" % (os.name, sys.platform)
  packages["Python"]= sys.version
  packages["IPython"]= IPython.__version__
  packages["MatPlotLib"]= mpl.__version__
  packages["Numpy"]= np.__version__
  packages["pyAgrum"]= gum.__version__

  res = "<table width='100%'><tr><th>Library</th><th>Version</th></tr>"

  for name in packages:
      res += "<tr><td>%s</td><td>%s</td></tr>" % (name, packages[name])

  res += "</table><div align='right'><small>%s</small></div>" % time.strftime(
      '%a %b %d %H:%M:%S %Y %Z')

  return IPython.display.HTML(res)


def showGraph(gr,size="4",format="png",asString=False):
  """
  show a pydot graph in a notebook
  
  @param string format : render as png or create_svg
  @param string size : size of the rendered graph
  @param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  gr.set_size(size)
  if format=="svg":
    gsvg=SVG(gr.create_svg())
    if asString:
        return gsvg.data
    else:
        display(HTML(gsvg.data))
  else:
    i=Image(format="png",data=gr.create_png())
    if asString:
        return '<img style="margin:0" src="data:image/png;base64,{}"/>'.format(base64.encodebytes(i.data).decode())
    else:
        display_png(Image(format="png",data=gr.create_png()))
        

def showDot(dotstring,size="4",format="png",asString=False):
  """
  show a dot string as a graph
  
  @param string format : render as "png" or "svg"
  @param string size : size of the rendered graph
  @param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  return showGraph(dot.graph_from_dot_data(dotstring),size,format,asString)

def showJunctionTree(bn,size="4",format="png",asString=False):
  """
  show a junctionTree
  
  @param string format : render as png or create_svg
  @param string size : size of the rendered graph
  @param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  ie=gum.LazyPropagation(bn)
  return showDot(ie.junctionTreeToDot(),size,format,asString)

def showInfluenceDiagram(diag,size="4",format="png",asString=False):
  """
  show an influence diagram as a graph
  
  @param string format : render as png or create_svg
  @param string size : size of the rendered graph
  @param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  return showDot(diag.toDot(),size,format,asString)

def getPosterior(bn,evs,target):
    """
    Compute the posterior of a single target (variable) in a BN given evidence using Lazy Propagation (for now).
    """
    inf=gum.LazyPropagation(bn)
    inf.setEvidence(evs)
    inf.makeInference()
    return gum.Potential(inf.posterior(bn.idFromName(target)))
    # creating a new Potential from posterior (will disappear with ie)

def getFigProbaV(p):
    """
    Show a matplotlib bar (vertical histogram) for a Potential p.

    """
    var=p.variable(0)
    ra=np.arange(var.domainSize())

    fig=plt.figure()
    fig.set_figwidth(var.domainSize()/4.0)
    fig.set_figheight(2)
    
    ax=fig.add_subplot(111)

    ax.bar(ra,p.tolist(),align='center')
    #ax.set_ylim(0,1)
    ax.set_xticks(ra)
    ax.set_xticklabels([])
    #ax.set_xlabel('Probability')
    ax.set_title(var.name())
    ax.get_yaxis().grid(True)
    return fig

def getFigProbaH(p):
    """
    Show a matplotlib barh (horizontal histogram) for a Potential p.

    """
    var=p.variable(0)
    ra=np.arange(var.domainSize())
    vx=["{0}:{1:1.4f}".format(var.label(int(i)),p[i]) for i in ra]

    fig=plt.figure()
    fig.set_figheight(var.domainSize()/4.0)
    fig.set_figwidth(2)

    ax=fig.add_subplot(111)

    ax.barh(ra,p.tolist(),align='center')
    ax.set_xlim(0,1)
    ax.set_yticks(ra)
    ax.set_yticklabels(vx)
    ax.set_xticklabels([])
    #ax.set_xlabel('Probability')
    ax.set_title(var.name())
    ax.get_xaxis().grid(True)
    return fig

def getFigProba(p):
    """
    Show a matplotlib histogram for a Potential p.

    """
    if p.variable(0).domainSize()>8:
        return getFigProbaV(p)
    else:
        return getFigProbaH(p)

def showProba(p):
    fig=getFigProba(p)
    plt.show()

def saveFigProba(p,filename,format="svg"):
    fig=getFigProba(p)
    fig.savefig(filename, bbox_inches='tight',transparent=True,pad_inches=0, dpi=fig.dpi, format=format)
    plt.close(fig)


def showPosterior(bn,evs,target):
    """
    shortcut for showProba(getPosterior(bn,evs,target))
    """
    showProba(getPosterior(bn,evs,target))

def animApproximationScheme(apsc,scale=np.log10):
  """
  show an animated version of an approximation scheme
  """
  from IPython.display import clear_output,display
  f=plt.gcf()

  h=gum.PythonApproximationListener(apsc)
  apsc.setVerbosity(True)
  apsc.listener=h

  def stopper(x):
    clear_output(True)
    plt.title("{0} \n Time : {1} s | Iterations : {2} | Espilon : {3}".format(x,apsc.currentTime(),apsc.nbrIterations(),apsc.epsilon()))


  def progresser(x,y,z):
    if len(apsc.history())<10:
      plt.xlim(1,10)
    else:
      plt.xlim(1,len(apsc.history()))
    plt.plot(scale(apsc.history()), 'g')
    clear_output(True)
    display(f)

  h.setWhenStop(stopper)
  h.setWhenProgress(progresser)


cdict = {'red': ((0.0, 0.1, 0.3),
                 (1.0, 0.6, 1.0)),
         'green': ((0.0, 0.0, 0.0),
                   (1.0, 0.6, 0.8)),
         'blue': ((0.0, 0.0, 0.0),
                  (1.0, 1, 0.8))}
INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap',cdict,256)

def _proba2rgb(p=0.99,cmap=INFOcmap):
    if p==0.0:
        r,g,b="FF","33","33"
    elif p==1.0:
        r,g,b="AA","FF","FF"
    else:
      (r,g,b,_)=cmap(p)
      r="%02x"%int(r*256)
      g="%02x"%int(g*256)
      b="%02x"%int(b*256)

    return r,g,b

def _proba2bgcolor(p,cmap=INFOcmap):
    r,g,b=_proba2rgb(p,cmap)
    return "#"+r+g+b

def _proba2fgcolor(p,cmap=INFOcmap):
    if max([eval("0x"+s[0]) for s in _proba2rgb(p,cmap)])<=12:
        return "#FFFFFF"
    else:
        return "#000000"

def getBNasSVG(bn,size="4"):
  return SVG(getBN(bn,size).create_svg())

def getBN(bn,size="4",vals=None,cmap=INFOcmap):
    """
    Shows a graphviz svg representation of the BN using size ("1" ,"2" , ...)
    vals is a dictionnary name:value of value in [0,1] for each node
    (with special color for 0 and 1)
    """
    graph=dot.Dot(graph_type='digraph')
    for n in bn.names():
        if vals is None or n not in vals:
          bgcol="#444444"
          fgcol="#FFFFFF"
          res=""
        else:
          bgcol=_proba2bgcolor(vals[n],cmap)
          fgcol=_proba2fgcolor(vals[n],cmap)
          res=" : {0:2.5f}".format(vals[n])

        node=dot.Node('"'+n+'"',style="filled",
                          fillcolor=bgcol,
                          fontcolor=fgcol,
                          tooltip='"({0}) {1}{2}"'.format(bn.idFromName(n),n,res))
        graph.add_node(node)
    for a in bn.arcs():
        edge=dot.Edge('"'+bn.variable(a[0]).name()+'"','"'+bn.variable(a[1]).name()+'"')
        graph.add_edge(edge)
    graph.set_size(size)
    return graph

def showBN(bn,size="4",vals=None,cmap=INFOcmap,format="svg",asString=False):
  """
  show a Bayesian network
  
  @param string format : render as png or create_svg
  @params dictionnary vals: a nodeMap of values to be shown as color nodes
  @params INFOcmap cmap: color map
  @param string size : size of the rendered graph
  @param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  gr=getBN(bn,size,vals,cmap)
  return showGraph(gr,size,format,asString)


def _normalizeVals(vals,hilightExtrema=False):
    """
    normalisation if vals is not a proba (max>1)
    """
    ma=float(max(vals.values()))
    mi=float(min(vals.values()))
    if ma==mi:
      return None
    else:
      if not hilightExtrema:
        vmi=0.01
        vma=0.99
      else:
        vmi=0
        vma=1

      try:
          items=vals.items()
      except AttributeError:
          items=vals.iteritems()

      return {name:vmi+(val-mi)*(vma-vmi)/(ma-mi) for name,val in items}

def showEntropy(bn,evs,size="4",cmap=INFOcmap):
  """
  Shows a bn annoted with results from inference : entropy and mutual informations
  """
  ie=gum.LazyPropagation(bn)
  ie.setEvidence(evs)
  ie.makeInference()

  vals={bn.variable(n).name():ie.H(n) for n in bn.ids()}
  gr=getBN(bn,size,_normalizeVals(vals,hilightExtrema=False),cmap)

  mi=min(vals.values())
  ma=max(vals.values())

  fig = mpl.figure.Figure(figsize=(8,3))
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap,
                                   norm=norm,
                                   orientation='horizontal')
  cb1.set_label('Entropy')
  png=print_figure(canvas.figure,"png") # from IPython.core.pylabtools
  png_legend="<img style='vertical-align:middle' src='data:image/png;base64,%s'>"%base64.encodestring(png).decode('ascii')

  gsvg=SVG(gr.create_svg())
  IPython.display.display(IPython.display.HTML("<div align='center'>"+
                              gsvg.data+
                              "</div><div align='center'>"+
                              "<font color='"+_proba2bgcolor(0.01,cmap)+"'>"+str(mi)+"</font>"
                              +png_legend+
                              "<font color='"+_proba2bgcolor(0.99,cmap)+"'>"+str(ma)+"</font>"
                              "</div>"))


def showInference(bn,engine=None,evs={},targets={},size="7",format='png',asString=False):
    """
    show an inference as a graph
    
    @param gum.Inference engine : inference algorithm used. If None, LazyPropagation will be used
    @params dictionnary evs : set of evidence
    @params set targets : set of targets
    @param string format : render as png or create_svg
    @param string size : size of the rendered graph
    @param boolean asString : display the graph or return a string containing the corresponding HTML fragment
    """
    startTime = time.time()
    # targets={} => each node is a target
    if engine is None:
      ie=gum.LazyPropagation(bn)
    else:
      ie=engine
    ie.setEvidence(evs)
    ie.makeInference()
    stopTime = time.time()

    from tempfile import mkdtemp
    temp_dir=mkdtemp("", "tmp", None) #with TemporaryDirectory() as temp_dir:

    dotstr ="digraph structs {\n"
    dotstr+="  label=\"Inference in {:6.2f}ms\";\n".format(1000*(stopTime-startTime))
    dotstr+="  node [fillcolor=floralwhite, style=filled,color=grey];\n"

    for i in bn.ids():
        name=bn.variable(i).name()
        if len(targets)==0 or name in targets:
          fill=", fillcolor=sandybrown" if name in evs else ""
          filename=temp_dir+name+"."+format
          saveFigProba(ie.posterior(i),filename,format=format)
          dotstr+=' "{0}" [shape=rectangle,image="{1}",label="" {2}];\n'.format(name,filename,fill)
        else:
          fill="[fillcolor=sandybrown]" if name in evs else ""
          dotstr+=' "{0}" {1}'.format(name,fill)
    for (i,j) in bn.arcs():
        dotstr+=' "{0}"->"{1}";'.format(bn.variable(i).name(),bn.variable(j).name())
    dotstr+='}'

    g=dot.graph_from_dot_data(dotstr)
    sss=showGraph(g,size=size,format=format,asString=asString)

    shutil.rmtree(temp_dir)
    return sss

def showPotential(pot,asString=False,digits=4,varnames=None):
    """
    Show a potential as a HTML table.
    The first dimension is special (horizontal) due to the representation of conditional probability table

    @param pot a aGrUM.Potential
    @param asString allows to postpone the HTML representation
    @param digits numbre of digits to show
    """
    from IPython.core.display import HTML    

    html=list()
    html.append("<table>")

    if varnames is not None and len(varnames)!=pot.nbrDim():
      raise ValueError("varnames contains {} values instead of the needed {} values.".format(len(varnames),pot.nbrDim()))
    
    nparents=pot.nbrDim()-1
    var=pot.variable(0)
    varname=var.name() if varnames==None else varnames[0]
    
    #first line
    if nparents>0:
        html.append("<tr><th colspan='{}'></th><th colspan='{}' style='background-color:#AAAAAA'><center>{}</center></th></tr>".format(nparents,var.domainSize(),varname))
    else:
        html.append("<tr style='background-color:#AAAAAA'><th colspan='{}'><center>{}</center></th></tr>".format(var.domainSize(),varname))
    #second line
    html.append("<tr>")
    if nparents>0:
        for parent in pot.var_names[:-1] if varnames==None else varnames[1:]:
            html.append("<th style='background-color:#AAAAAA'><center>{}</center></th>".format(parent))
    for label in var.labels():
        html.append("<th style='background-color:#BBBBBB'><center>{}</center></th>".format(label))

    inst=gum.Instantiation(pot)
    off=1
    offset=dict()
    for i in range(1,nparents+1):
        offset[i]=off
        off*=inst.variable(i).domainSize()

    html.append("<tr>")
    inst.setFirst()
    while not inst.end():
        if inst.val(0)==0:
            for par in range(nparents,0,-1):
                label=inst.variable(par).label(inst.val(par))
                if par==1:
                    html.append("<th style='background-color:#BBBBBB'>{}</th>".format(label))
                else:
                    if sum([inst.val(i) for i in range(1,par)])==0:
                        html.append("<th style='background-color:#BBBBBB;' rowspan = '{}'>{}</th>".format(offset[par],label))
        html.append(("<td style='text-align:right;'>{:."+str(digits)+"f}</td>").format(pot.get(inst)))
        inst.inc()
        if not inst.end() and inst.val(0)==0:
            html.append("</tr><tr>")
    html.append("</tr>")

    html.append("</table>")

    if asString:
        return "\n".join(html)
    else:
        return HTML("".join(html))


def sideBySide(htmls,titles=None):
  """
  display side by side args as HMTL fragment as string
  @param list htmls : list of HMTL fragments as string
  @param list titles : list of string (titles, optional)
  """
  s='<table style="border-style: hidden; border-collapse: collapse;" width="100%">'
  
  s+='<tr><td style="border-top:hidden;border-bottom:hidden;"><div align="center">'
  s+='</div></td><td style="border-top:hidden;border-bottom:hidden;"><div align="center">'.join(htmls)
  s+='</div></td></tr>'
  
  if titles is not None:
    s+='<tr><td style="border-top:hidden;border-bottom:hidden;"><div align="center">'
    s+='</div></td><td style="border-top:hidden;border-bottom:hidden;"><div align="center">'.join(titles)
    s+='</div></td></tr>'
  
  s+='</table>'
  
  display(HTML(s))
