#!/usr/bin/python
# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
# (pierre-henri.wuillemin@lip6.fr)
# Permission to use, copy, modify, and distribute this
# software and its documentation for any purpose and
# without fee or royalty is hereby granted, provided
# that the above copyright notice appear in all copies
# and that both that copyright notice and this permission
# notice appear in supporting documentation or portions
# thereof, including modifications, that you make.
# THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
# SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
# OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
# RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
# ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
# OR PERFORMANCE OF THIS SOFTWARE!

"""
tools for BN analysis in jupyter notebook
"""
from __future__ import print_function

import base64
import time

import IPython.display
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pyAgrum as gum
import pydotplus as dot
from IPython.core.display import Image, display_png
from IPython.core.pylabtools import print_figure
from IPython.display import display, HTML, SVG
from matplotlib.backends.backend_agg import FigureCanvasAgg as fc
from pyAgrum.lib.bn2graph import BN2dot, proba2histo, BNinference2dot, _proba2bgcolor

_cdict = {
  'red'  : ((0.0, 0.1, 0.3),
            (1.0, 0.6, 1.0)),
  'green': ((0.0, 0.0, 0.0),
            (1.0, 0.6, 0.8)),
  'blue' : ((0.0, 0.0, 0.0),
            (1.0, 1, 0.8))
}
_INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap', _cdict, 256)


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

  res = "<table width='100%'><tr><th>Library</th><th>Version</th></tr>"

  for name in packages:
    res += "<tr><td>%s</td><td>%s</td></tr>" % (name, packages[name])

  res += "</table><div align='right'><small>%s</small></div>" % time.strftime(
      '%a %b %d %H:%M:%S %Y %Z')

  display(HTML(res))


def _reprGraph(gr, size, format, asString):
  """
  repr a pydot graph in a notebook

  :param string format : render as png or create_svg
  :param string size : size of the rendered graph
  :param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  gr.set_size(size)
  if format == "svg":
    gsvg = SVG(gr.create_svg())
    if asString:
      return gsvg.data
    else:
      display(HTML(gsvg.data))
  else:
    i = Image(format="png", data=gr.create_png())
    if asString:
      return '<img style="margin:0" src="data:image/png;base64,{}"/>'.format(base64.encodebytes(i.data).decode())
    else:
      display_png(Image(format="png", data=gr.create_png()))


def showGraph(gr, size="4", format="png"):
  """
  show a pydot graph in a notebook

  :param gr: pydot graph
  :param size:  size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the graph
  """
  return _reprGraph(gr, size, format, asString=False)


def getGraph(gr, size="4", format="png"):
  """
  get a HTML string representation of pydot graph

  :param gr: pydot graph
  :param size:  size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the HTML representation of the graph as a string
  """
  return _reprGraph(gr, size, format, asString=True)


def showDot(dotstring, size="4", format="png"):
  """
  show a dot string as a graph

  :param dotstring: dot string
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the graph
  """
  return showGraph(dot.graph_from_dot_data(dotstring), size, format)


def getDot(dotstring, size="4", format="png"):
  """
  get a dot string as a HTML string

  :param dotstring: dot string
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the HTML representation of the graph
  """
  g = dot.graph_from_dot_data(dotstring)
  g.set_bgcolor("transparent")
  return getGraph(g, size, format)


def showJunctionTree(bn, withNames=True, size="4", format="png"):
  """
  Show a junction tree

  :param bn: the bayesian network
  :param boolean withNames: display the variable names or the node id in the clique
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the graph
  """
  jtg = gum.JTGenerator()
  jt = jtg.junctionTree(bn)
  if withNames:
    return showDot(jt.toDotWithNames(bn), size, format)
  else:
    return showDot(jt.toDot(), size, format)


def getJunctionTree(bn, withNames=True, size="4", format="png"):
  """
  get a HTML string for a junction tree (more specifically a join tree)

  :param bn: the bayesian network
  :param boolean withNames: display the variable names or the node id in the clique
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the HTML representation of the graph
  """
  jtg = gum.JTGenerator()
  jt = jtg.junctionTree(bn)
  if withNames:
    return getDot(jt.toDotWithNames(bn), size, format)
  else:
    return getDot(jt.toDot(), size, format)


def showInfluenceDiagram(diag, size="4", format="png"):
  """
  show an influence diagram as a graph

  :param diag: the influence diagram
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the representation of the influence diagram
  """
  return showDot(diag.toDot(), size, format)


def getInfluenceDiagram(diag, size="4", format="png"):
  """
  get a HTML string for an influence diagram as a graph

  :param diag: the influence diagram
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the HTML representation of the influence diagram
  """
  return getDot(diag.toDot(), size, format)


def showProba(p):
  """
  Show a mono-dim Potential

  :param p: the mono-dim Potential
  :return:
  """
  fig = proba2histo(p)
  plt.show()


def _saveFigProba(p, filename, format="svg"):
  fig = proba2histo(p)
  fig.savefig(filename, bbox_inches='tight', transparent=True, pad_inches=0, dpi=fig.dpi, format=format)
  plt.close(fig)


def getPosterior(bn, evs, target):
  """
  shortcut for getProba(gum.getPosterior(bn,evs,target))

  :param bn: the BayesNet
  :type bn: gum.BayesNet
  :param evs: map of evidence
  :type evs: dict(str->int)
  :param target: name of target variable
  :type target: str
  :return: the matplotlib graph
  """
  return proba2histo(gum.getPosterior(bn, evs=evs, target=target))


def showPosterior(bn, evs, target):
  """
  shortcut for showProba(gum.getPosterior(bn,evs,target))

  :param bn: the BayesNet
  :param evs: map of evidence
  :param target: name of target variable
  """
  showProba(gum.getPosterior(bn, evs=evs, target=target))


def animApproximationScheme(apsc, scale=np.log10):
  """
  show an animated version of an approximation algorithm

  :param apsc: the approximation algorithm
  :param scale: a function to apply to the figure
  """
  from IPython.display import clear_output, display
  f = plt.gcf()

  h = gum.PythonApproximationListener(apsc.asIApproximationSchemeConfiguration())
  apsc.setVerbosity(True)
  apsc.listener = h

  def stopper(x):
    clear_output(True)
    plt.title(
        "{0} \n Time : {1} s | Iterations : {2} | Espilon : {3}".format(x, apsc.currentTime(), apsc.nbrIterations(),
                                                                        apsc.epsilon()))

  def progresser(x, y, z):
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.plot(scale(apsc.history()), 'g')
    clear_output(True)
    display(f)

  h.setWhenStop(stopper)
  h.setWhenProgress(progresser)


def showApproximationScheme(apsc, scale=np.log10):
  if apsc.verbosity():
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.title("{0} \n Time : {1} s | Iterations : {2} | Espilon : {3}".format(x, apsc.currentTime(), apsc.nbrIterations(),
                                                                              apsc.epsilon()))
    plt.plot(scale(apsc.history()), 'g')
  else:
    ie2.messageApproximationScheme()


def showBN(bn, size="4", format="svg", arcvals=None, vals=None, cmap=None):
  """
  show a Bayesian network

  :param bn: the bayesian network
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :param vals: a nodeMap of values to be shown as color nodes
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  :return: the graph
  """
  return showGraph(BN2dot(bn, size, arcvals, vals, cmap), size, format)


def getBN(bn, size="4", format="svg", arcvals=None, vals=None, cmap=None):
  """
  get a HTML string for a Bayesian network

  :param bn: the bayesian network
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :param vals: a nodeMap of values to be shown as color nodes
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  :return: the graph
  """
  return getGraph(BN2dot(bn, size, arcvals, vals, cmap), size, format)


def _normalizeVals(vals, hilightExtrema=False):
  """
  normalisation if vals is not a proba (max>1)
  """
  ma = float(max(vals.values()))
  mi = float(min(vals.values()))
  if ma == mi:
    return None
  else:
    if not hilightExtrema:
      vmi = 0.01
      vma = 0.99
    else:
      vmi = 0
      vma = 1

    try:
      items = vals.items()
    except AttributeError:
      items = vals.iteritems()

    return {name: vmi + (val - mi) * (vma - vmi) / (ma - mi) for name, val in items}


def _reprInformation(bn, evs, size, cmap, asString):
  """
  repr a bn annoted with results from inference : Information and mutual informations

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the HTML string
  """
  ie = gum.LazyPropagation(bn)
  ie.setEvidence(evs)
  ie.makeInference()

  idEvs = {bn.idFromName(name) for name in evs}
  nodevals = {bn.variable(n).name(): ie.H(n) for n in bn.nodes() if not n in idEvs}
  arcvals = {(x, y): ie.I(x, y) for x, y in bn.arcs()}
  gr = BN2dot(bn, size, arcvals, _normalizeVals(nodevals, hilightExtrema=False), cmap, showValues=nodevals)

  mi = min(nodevals.values())
  ma = max(nodevals.values())

  fig = mpl.figure.Figure(figsize=(8, 3))
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap,
                                  norm=norm,
                                  orientation='horizontal')
  cb1.set_label('Entropy')
  png = print_figure(canvas.figure, "png")  # from IPython.core.pylabtools
  png_legend = "<img style='vertical-align:middle' src='data:image/png;base64,%s'>" % base64.encodestring(png).decode(
      'ascii')

  gsvg = SVG(gr.create_svg())

  sss = "<div align='center'>" + gsvg.data + "</div>"
  sss += "<div align='center'>"
  sss += "<font color='" + _proba2bgcolor(0.01, cmap) + "'>" + str(mi) + "</font>"
  sss += png_legend
  sss += "<font color='" + _proba2bgcolor(0.99, cmap) + "'>" + str(ma) + "</font>"
  sss += "</div>"

  if asString:
    return sss
  else:
    return display(HTML(sss))


def getInformation(bn, evs={}, size="4", cmap=_INFOcmap):
  """
  get a HTML string for a bn annoted with results from inference : entropy and mutual informations

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the HTML string
  """
  return _reprInformation(bn, evs, size, cmap, asString=True)


def showInformation(bn, evs={}, size="4", cmap=_INFOcmap):
  """
  show a bn annoted with results from inference : entropy and mutual informations

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the graph
  """
  return _reprInformation(bn, evs, size, cmap, asString=False)


def showInference(bn, engine=None, evs={}, targets={}, size="7", format='png', vals=None, arcvals=None, cmap=None):
  """
  show pydot graph for an inference in a notebook

  :param gum.BayesNet bn:
  :param gum.Inference engine: inference algorithm used. If None, LazyPropagation will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets
  :param string size: size of the rendered graph
  :param string format: render as "png" or "svg"
  :param boolean asString: display the graph or return a string containing the corresponding HTML fragment
  :return: the desired representation of the inference
  """
  return showGraph(BNinference2dot(bn, size, engine, evs, targets, format, vals, arcvals, cmap), size, format)


def getInference(bn, engine=None, evs={}, targets={}, size="7", format='png', vals=None, arcvals=None, cmap=None):
  """
  get a HTML string for an inference in a notebook

  :param gum.BayesNet bn:
  :param gum.Inference engine: inference algorithm used. If None, LazyPropagation will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets
  :param string size: size of the rendered graph
  :param string format: render as "png" or "svg"
  :param boolean asString: display the graph or return a string containing the corresponding HTML fragment
  :return: the desired representation of the inference
  """
  return getGraph(BNinference2dot(bn, size, engine, evs, targets, format, vals, arcvals, cmap), size, format)


def _reprPotential(pot, digits=4, withColors=True, varnames=None, asString=False):
  """
  return a representation of a gum.Potential as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table


  :param pot: the potential to get
  :param digits: number of digits to show
  :param: withColors : bgcolor for proba cells or not
  :param varnames: the aliases for variables name in the table
  :param asString: display the table or a HTML string
  :return: the representation
  """
  from IPython.core.display import HTML

  def _rgb(r, g, b):
    return '#%02x%02x%02x' % (r, g, b)

  def _mkCell(val):
    s = "<td style='"
    if withColors and (0 <= val <= 1):
      r = int(255 - val * 128)
      g = int(127 + val * 128)
      b = 100
      s += "background-color:" + _rgb(r, g, b) + ";"
    s += "text-align:right;'>{:." + str(digits) + "f}</td>"
    return s.format(val)

  html = list()
  html.append("<table>")
  if pot.empty():
    html.append("<tr><th style='background-color:#AAAAAA'>&nbsp;</th></tr>")
    html.append("<tr>" + _mkCell(pot.get(gum.Instantiation())) + "</tr>")
  else:
    if varnames is not None and len(varnames) != pot.nbrDim():
      raise ValueError(
          "varnames contains {} values instead of the needed {} values.".format(len(varnames), pot.nbrDim()))

    nparents = pot.nbrDim() - 1
    var = pot.variable(0)
    varname = var.name() if varnames == None else varnames[0]

    # first line
    if nparents > 0:
      html.append(
          "<tr><th colspan='{}'></th><th colspan='{}' style='background-color:#AAAAAA'><center>{"
          "}</center></th></tr>".format(
              nparents, var.domainSize(), varname))
    else:
      html.append(
          "<tr style='background-color:#AAAAAA'><th colspan='{}'><center>{}</center></th></tr>".format(
              var.domainSize(),
              varname))
    # second line
    s = "<tr>"
    if nparents > 0:
      # for parent in pot.var_names[:-1] if varnames == None else varnames[1:]:
      for par in range(nparents - 1, 0 - 1, -1):
        parent = pot.var_names[par] if varnames is None else varnames[par]
        s += "<th style='background-color:#AAAAAA'><center>{}</center></th>".format(parent)
    for label in var.labels():
      s += "<th style='background-color:#BBBBBB'><center>{}</center></th>".format(label)
    s += '</tr>'

    html.append(s)

    inst = gum.Instantiation(pot)
    off = 1
    offset = dict()
    for i in range(1, nparents + 1):
      offset[i] = off
      off *= inst.variable(i).domainSize()

    inst.setFirst()
    while not inst.end():
      s = "<tr>"
      for par in range(1, nparents + 1):
        label = inst.variable(par).label(inst.val(par))
        if par == 1:
          s += "<th style='background-color:#BBBBBB'><center>{}</center></th>".format(label)
        else:
          if sum([inst.val(i) for i in range(1, par)]) == 0:
            s += "<th style='background-color:#BBBBBB;' rowspan = '{}'><center>{}</center></th>".format(offset[par],
                                                                                                        label)
      for j in range(pot.variable(0).domainSize()):
        s += _mkCell(pot.get(inst))
        inst.inc()
      s += "</tr>"
      html.append(s)

  html.append("</table>")

  if asString:
    return "\n".join(html)
  else:
    return HTML("".join(html))

def __isKindOfProba(pot):
  """
  check if pot is a joint proba or a CPT
  :param pot: the potential
  :return: True or False
  """
  if abs(pot.sum()-1)<1e-2:
    return True

  q=pot.margSumOut([pot.variable(0).name()])
  if abs(q.max()-1)>1e-2:
    return False
  if abs(q.min()-1)>1e-2:
    return False
  return True

def showPotential(pot, digits=4, withColors=None, varnames=None):
  """
  show a gum.Potential as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  :param gum.Potential pot: the potential to get
  :param int digits: number of digits to show
  :param: boolean withColors : bgcolor for proba cells or not
  :param list of strings varnames: the aliases for variables name in the table
  :return: the display of the potential
  """
  if withColors is None:
    withColors=__isKindOfProba

  display(_reprPotential(pot, digits, withColors, varnames, asString=False))


def getPotential(pot, digits=4, withColors=None, varnames=None):
  """
  return a HTML string of a gum.Potential as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  :param gum.Potential pot: the potential to get
  :param int digits: number of digits to show
  :param: boolean withColors : bgcolor for proba cells or not
  :param list of strings varnames: the aliases for variables name in the table
  :return: the HTML string
  """
  if withColors is None:
    withColors=__isKindOfProba(pot)

  return _reprPotential(pot, digits, withColors, varnames, asString=True)


def getSideBySide(*args, **kwargs):
  """
  create an HTML table for args as string (using string, _repr_html_() or str())

  :param args: HMTL fragments as string arg, arg._repr_html_() or str(arg)
  :param captions: list of strings (captions)
  :return: a string representing the table
  """

  for k in kwargs:
    if k != "captions":
      raise TypeError("sideBySide() got an unexpected keyword argument '{}'".format(k))

  if 'captions' in kwargs:
    captions = kwargs['captions']
  else:
    captions = None

  s = '<table style="border-style: hidden; border-collapse: collapse;" width="100%">'

  def reprHTML(s):
    if isinstance(s, str):
      return s
    elif hasattr(s, '_repr_html_'):
      return s._repr_html_()
    else:
      return str(s)

  s += '<tr><td style="border-top:hidden;border-bottom:hidden;"><div align="center">'
  s += '</div></td><td style="border-top:hidden;border-bottom:hidden;"><div align="center">'.join([reprHTML(arg)
                                                                                                   for arg in args])
  s += '</div></td></tr>'

  if captions is not None:
    s += '<tr><td style="border-top:hidden;border-bottom:hidden;"><div align="center"><small>'
    s += '</small></div></td><td style="border-top:hidden;border-bottom:hidden;"><div align="center"><small>'.join(
        captions)
    s += '</small></div></td></tr>'

  s += '</table>'
  return s


def sideBySide(*args, **kwargs):
  """
  display side by side args as HMTL fragment (using string, _repr_html_() or str())

  :param args: HMTL fragments as string arg, arg._repr_html_() or str(arg)
  :param captions: list of strings (captions)
  """
  for k in kwargs:
    if k != "captions":
      raise TypeError("sideBySide() got an unexpected keyword argument '{}'".format(k))

  display(HTML(getSideBySide(*args, **kwargs)))


def getInferenceEngine(ie, inferenceCaption):
  """
  display an inference as a BN+ lists of hard/soft evidence and list of targets

  :param gum.InferenceEngine ie: inference engine
  :param string inferenceCaption : title for caption
  """
  t = '<div align="left"><ul>'
  if ie.nbrHardEvidence() > 0:
    t += "<li><b>hard evidence</b><br/>"
    t += ", ".join([ie.BN().variable(n).name() for n in ie.hardEvidenceNodes()])
    t += "</li>"
  if ie.nbrSoftEvidence() > 0:
    t += "<li><b>soft evidence</b><br/>"
    t += ", ".join([ie.BN().variable(n).name() for n in ie.softEvidenceNodes()])
    t += "</li>"
  if ie.nbrTargets() > 0:
    t += "<li><b>target(s)</b><br/>"
    if ie.nbrTargets() == ie.BN().size():
      t += " all"
    else:
      t += ", ".join([ie.BN().variable(n).name() for n in ie.targets()])
    t += "</li>"

  if hasattr(ie, 'nbrJointTargets'):
    if ie.nbrJointTargets() > 0:
      t += "<li><b>Joint target(s)</b><br/>"
      t += ", ".join(['['
                      + (", ".join([ie.BN().variable(n).name() for n in ns]))
                      + ']' for ns in ie.jointTargets()])
      t += "</li>"
  t += '</ul></div>'
  return getSideBySide(getBN(ie.BN()), t, captions=[inferenceCaption, "Evidence and targets"])


# adding _repr_html_ to some pyAgrum classes !
gum.BayesNet._repr_html_ = lambda self: getBN(self)
gum.Potential._repr_html_ = lambda self: getPotential(self)
gum.LazyPropagation._repr_html_ = lambda self: getInferenceEngine(self, "Lazy Propagation on this BN")

gum.UndiGraph._repr_html_ = lambda self: getDot(self.toDot())
gum.DiGraph._repr_html_ = lambda self: getDot(self.toDot())
gum.MixedGraph._repr_html_ = lambda self: getDot(self.toDot())
gum.DAG._repr_html_ = lambda self: getDot(self.toDot())
gum.CliqueGraph._repr_html_ = lambda self: getDot(self.toDot())
gum.EssentialGraph._repr_html_ = lambda self: getDot(self.toDot())
gum.MarkovBlanket._repr_html_ = lambda self: getDot(self.toDot())
