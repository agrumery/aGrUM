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
import matplotlib as mpl
import matplotlib.pyplot as plt
import pyAgrum as gum
import numpy as np

def getPosterior(bn,ev,target):
    """
    Compute the posterior of a single target (variable) in a BN given evidence using Lazy Propagation (for now).
    """
    inf=gum.LazyPropagation(bn)
    inf.setEvidence(ev)
    inf.makeInference()
    return inf.marginal(bn.idFromName(target))

def showProba(p):
    """
    Show a matplotlib barh (horizontal histogram) for a Potential p.

    """
    var=p.variable(0)
    ra=np.arange(len(var))
    vx=[var.label(int(i)) for i in ra]

    fig=plt.figure()
    ax=fig.add_subplot(111)

    ax.barh(ra,p.tolist(),align='center')
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

def showBN(bn,size="4"):
    """
    Shows a graphviz svg representation of the BN using size ("1" ,"2" , ..., "6")
    """
    import pydot
    import IPython.core.display

    p=pydot.graph_from_dot_data(bn.toDot())
    p.set_size(size)
    i=IPython.core.display.SVG(p.create_svg())
    IPython.core.display.display(i)
    
def animApproximationScheme(apsc):
  """
  show an animated version of an approximation scheme
  """
  from IPython.display import clear_output,display
  f=plt.gcf()
  
  h=gum.PythonApproximationListener(apsc)
  apsc.setVerbosity(True)
  apsc.listener=h
  
  def stopper(x):
    clear_output()
    print("Solutions : {0}".format(x))
    print("Time : {0} s".format(apsc.currentTime()))
    print("Iterations : {0}".format(apsc.nbrIterations()))
    
  def progresser(x,y,z):
    clear_output()
    if len(apsc.history())<10:
      plt.xlim(1,10)
    else:
      plt.xlim(1,len(apsc.history()))
    plt.plot(np.log10(apsc.history()), 'g')
    display(f)
          
  h.setWhenStop(stopper)
  h.setWhenProgress(progresser)
  
  
