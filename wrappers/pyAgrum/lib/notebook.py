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
from base64 import encodestring
import numpy as np
import time

import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

import pydotplus as dot

import IPython.display
from IPython.core.pylabtools import print_figure

import pyAgrum as gum

def configuration():
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

def getPosterior(bn,ev,target):
    """
    Compute the posterior of a single target (variable) in a BN given evidence using Lazy Propagation (for now).
    """
    inf=gum.LazyPropagation(bn)
    inf.setEvidence(ev)
    inf.makeInference()
    return gum.Potential(inf.posterior(bn.idFromName(target)))
    # creating a new Potential from posterior (will disappear with ie)

def showProba(p):
    """
    Show a matplotlib barh (horizontal histogram) for a Potential p.

    """
    var=p.variable(0)
    ra=np.arange(var.domainSize())
    vx=["{0}:{1:1.4f}".format(var.label(int(i)),p[i]) for i in ra]

    fig=plt.figure()
    fig.set_figheight(var.domainSize()/4.0)

    ax=fig.add_subplot(111)

    ax.barh(ra,p.tolist(),align='center')
    ax.set_xlim(0,1)
    ax.set_yticks(ra)
    ax.set_yticklabels(vx)
    ax.set_xlabel('Probability')
    ax.set_title(var.name())
    ax.get_xaxis().grid(True)
    plt.show()

def showPosterior(bn,ev,target):
    """
    shortcut for showProba(getPosterior(bn,ev,target))
    """
    showProba(getPosterior(bn,ev,target))

def animApproximationScheme(apsc,scale=np.log10):
  """
  show an animated version of an approximation scheme
  """
  from IPython.display import clear_output,display
  f=plt.gcf()

  h=gum.PythonApproximationListener(apsc.castAsASC())
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
      r="%02x"%(r*256)
      g="%02x"%(g*256)
      b="%02x"%(b*256)

    return r,g,b

def _proba2bgcolor(p,cmap=INFOcmap):
    r,g,b=_proba2rgb(p,cmap)
    return "#"+r+g+b

def _proba2fgcolor(p,cmap=INFOcmap):
    if max([eval("0x"+s[0]) for s in _proba2rgb(p,cmap)])<=12:
        return "#FFFFFF"
    else:
        return "#000000"

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

        node=dot.Node(n,style="filled",
                          fillcolor=bgcol,
                          fontcolor=fgcol,
                          tooltip='"({0}) {1}{2}"'.format(bn.idFromName(n),n,res))
        graph.add_node(node)
    for a in bn.arcs():
        edge=dot.Edge(bn.variable(a[0]).name(),bn.variable(a[1]).name())
        graph.add_edge(edge)
    graph.set_size(size)
    return IPython.display.SVG(graph.create_svg())

def showBN(bn,size="4",vals=None,cmap=INFOcmap):
  gr=getBN(bn,size,vals,cmap)
  IPython.display.display(IPython.display.HTML("<div align='center'>"+gr.data+"</div>"))


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

def showInference(bn,ie,size="4",cmap=INFOcmap):
  """
  Shows a bn annoted with results from inference : entropy and mutual informations
  """
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
  png_legend="<img style='vertical-align:middle' src='data:image/png;base64,%s'>"%encodestring(png).decode('ascii')

  IPython.display.display(IPython.display.HTML("<div align='center'>"+
                              gr.data+
                              "</div><div align='center'>"+
                              "<font color='"+_proba2bgcolor(0.01,cmap)+"'>"+str(mi)+"</font>"
                              +png_legend+
                              "<font color='"+_proba2bgcolor(0.99,cmap)+"'>"+str(ma)+"</font>"
                              "</div>"))

def getJunctionTree(bn,size="4"):
  ie=gum.LazyPropagation(bn)
  graph=dot.graph_from_dot_data(ie.junctionTreeToDot())
  graph.set_size(size)
  return IPython.display.SVG(graph.create_svg())

def showJunctionTree(bn,size="4"):
  gr=getJunctionTree(bn,size)
  IPython.display.display(IPython.display.HTML("<div align='center'>"+gr.data+"</div>"))

def getInfluenceDiagram(diag,size="4"):
  graph=dot.graph_from_dot_data(diag.toDot())
  graph.set_size(size)
  return IPython.display.SVG(graph.create_svg())

def showInfluenceDiagram(diag,size="4"):
  gr=getInfluenceDiagram(diag,size)
  IPython.display.display(IPython.display.HTML("<div align='center'>"+gr.data+"</div>"))

