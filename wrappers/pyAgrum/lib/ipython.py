# -*- coding: utf-8 -*-

"""
tools for BN analysis in ipython (and spyder)
"""
from __future__ import print_function

import IPython.display
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pyAgrum as gum
import pydotplus as dot

from IPython.display import Image,display

import pyAgrum as gum
import pyAgrum.lib.bn2graph as bng
from pyAgrum.lib.pretty_print import cpt2txt

def configuration():
  """
  Display the collection of dependance and versions
  """
  from collections import OrderedDict
  import sys, os

  packages = OrderedDict()
  packages["OS"] = "%s [%s]" % (os.name, sys.platform)
  packages["Python"] = sys.version
  packages["IPython"] = IPython.__version__
  packages["MatPlotLib"] = mpl.__version__
  packages["Numpy"] = np.__version__
  packages["pyAgrum"] = gum.__version__

  for name in packages:
    print("%s : %s" % (name, packages[name]))


def showGraph(gr, size="4", format="png"):
  """
  show a pydot graph in a notebook

  :param gr: pydot graph
  :param size:  size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the graph
  """
  gr.set_size(size)
  if format == "svg":
      print("pyAgrum warning : svg is not possible without HTML rendering. Please use notebooks")
  display(Image(gr.create_png()))

def showDot(dotstring, size="4", format="png"):
  """
  show a dot string as a graph

  :param dotstring: dot string
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the graph
  """
  return showGraph(dot.graph_from_dot_data(dotstring), size, format)

def showJunctionTree(bn, withNames=True, size="4", format="png"):
  """
  Show a junction tree

  :param bn: the bayesian network
  :param boolean withNames: display the variable names or the node id in the clique
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  """
  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  if withNames:
    return showDot(jt.toDotWithNames(bn), size, format)
  else:
    return showDot(jt.toDot(), size, format)


def showBN(bn, size="4", format="svg", arcvals=None, vals=None, cmap=None):
  """
  show a Bayesian network

  :param bn: the bayesian network
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :param vals: a nodeMap of values to be shown as color nodes
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  """
  gr=bng.BN2dot(bn, size, arcvals, vals, cmap)
  display(Image(gr.create_png()))
    
def showInference(bn, engine=None, evs={}, targets={}, size="7", format='png', vals=None, arcvals=None, cmap=None):
  """
  show pydot graph for an inference in a notebook

  :param gum.BayesNet bn:
  :param gum.Inference engine: inference algorithm used. If None, LazyPropagation will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets
  :param string size: size of the rendered graph
  :param string format: render as "png" or "svg"
  :param vals: a nodeMap of values to be shown as color nodes
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  """
  gr=bng.BNinference2dot(bn, 
                          size, 
                          engine, 
                          evs, 
                          targets, 
                          format, 
                          vals, 
                          arcvals, 
                          cmap)
  display(Image(gr.create_png()))
  
def showInfluenceDiagram(diag, size="4", format="png"):
  """
  show an influence diagram as a graph

  :param diag: the influence diagram
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the influence diagram
  """
  return showDot(diag.toDot(), size, format)

def showPotential(p,digits=4):
    print(cpt2txt(p,digits=digits))
    