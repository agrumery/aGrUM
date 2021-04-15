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

import time
import sys

import IPython.display
import re

# fix DeprecationWarning of base64.encodestring()
try:
  from base64 import encodebytes
except ImportError:  # 3+
  from base64 import encodestring as encodebytes

import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

import numpy as np
import pydotplus as dot

from IPython.core.display import Image, display_png
from IPython.core.pylabtools import print_figure
from IPython.display import display, HTML, SVG

import pyAgrum as gum
from pyAgrum.lib.bn2graph import BN2dot, BNinference2dot
from pyAgrum.lib.cn2graph import CN2dot, CNinference2dot
from pyAgrum.lib.id2graph import ID2dot, LIMIDinference2dot
from pyAgrum.lib.mn2graph import MN2UGdot, MNinference2UGdot
from pyAgrum.lib.mn2graph import MN2FactorGraphdot, MNinference2FactorGraphdot
from pyAgrum.lib.bn_vs_bn import GraphicalBNComparator
from pyAgrum.lib.proba_histogram import proba2histo, probaMinMaxH

_cdict = {
  'red': ((0.0, 0.1, 0.3),
          (1.0, 0.6, 1.0)),
  'green': ((0.0, 0.0, 0.0),
            (1.0, 0.6, 0.8)),
  'blue': ((0.0, 0.0, 0.0),
           (1.0, 1, 0.8))
}
_INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap', _cdict, 256)


def configuration():
  """
  Display the collection of dependance and versions
  """
  from collections import OrderedDict
  import sys
  import os

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


def __insertLinkedSVGs(mainSvg):
  re_buggwhitespace = re.compile(r"(<image [^>]*>)")
  re_images = re.compile(r"(<image [^>]*>)")
  re_xlink = re.compile(r"xlink:href=\"([^\"]*)")
  re_viewbox = re.compile(r"(viewBox=\"[^\"]*\")")

  # analyze mainSvg (find the secondary svgs)
  __fragments = {}
  for img in re.finditer(re_images, mainSvg):
    # print(img)
    secondarySvg = re.findall(re_xlink, img.group(1))[0]
    content = ""
    with open(secondarySvg, encoding='utf8') as f:
      inSvg = False
      for line in f:
        if line[0:4] == "<svg":
          inSvg = True
          viewBox = re.findall(re_viewbox, line)[0]
          # print("VIEWBOX {}".format(viewBox))
        elif inSvg:
          content += line
    __fragments[secondarySvg] = (viewBox, content)

  if len(__fragments) > 0:
    # replace image tags by svg tags
    img2svg = re.sub(r"<image ([^>]*)/>", "<svg \g<1>>", mainSvg)

    # insert secondaries into main
    def ___insertSecondarySvgs(matchObj):
      vb, code = __fragments[matchObj.group(1)]
      return vb + matchObj.group(2) + code

    mainSvg = re.sub(r'xlink:href="([^"]*)"(.*>)',
                     ___insertSecondarySvgs, img2svg)

  # remove buggy white-space (for notebooks)
  mainSvg = mainSvg.replace("white-space:pre;", "")
  return mainSvg


def _reprGraph(gr, size, asString):
  """
  repr a pydot graph in a notebook

  :param string size : size of the rendered graph
  :param boolean asString : display the graph or return a string containing the corresponding HTML fragment
  """
  gr.set_size(size)
  if gum.config["notebook", "graph_format"] == "svg":
    gsvg = SVG(__insertLinkedSVGs(gr.create_svg().decode('utf-8')))
    if asString:
      return gsvg.data
    else:
      display(gsvg)
  else:
    i = Image(format="png", data=gr.create_png())
    if asString:
      return '<img style="margin:0" src="data:image/png;base64,{}"/>'.format(encodebytes(i.data).decode())
    else:
      display_png(i)


def showGraph(gr, size=None):
  """
  show a pydot graph in a notebook

  :param gr: pydot graph
  :param size:  size of the rendered graph
  :return: the representation of the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  return _reprGraph(gr, size, asString=False)


def getGraph(gr, size=None):
  """
  get a HTML string representation of pydot graph

  :param gr: pydot graph
  :param size:  size of the rendered graph
  :param format: render as "png" or "svg"
  :return: the HTML representation of the graph as a string
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  return _reprGraph(gr, size, asString=True)


def _from_dotstring(dotstring):
  g = dot.graph_from_dot_data(dotstring)
  g.set_bgcolor("transparent")
  for e in g.get_edges():
    if e.get_color() is None:
      e.set_color(gum.getBlackInTheme())
  for n in g.get_nodes():
    if n.get_color() is None:
      n.set_color(gum.getBlackInTheme())
    if n.get_fontcolor() is None:
      n.set_fontcolor(gum.getBlackInTheme())
  return g


def showDot(dotstring, size=None):
  """
  show a dot string as a graph

  :param dotstring: dot string
  :param size: size of the rendered graph
  :return: the representation of the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  return showGraph(_from_dotstring(dotstring), size)


def getDot(dotstring, size=None):
  """
  get a dot string as a HTML string

  :param dotstring: dot string
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :param bg: color for background
  :return: the HTML representation of the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  return getGraph(_from_dotstring(dotstring), size)


def getBNDiff(bn1, bn2, size=None):
  """ get a HTML string representation of a graphical diff between the arcs of _bn1 (reference) with those of _bn2.

  * full black line: the arc is common for both
  * full red line: the arc is common but inverted in _bn2
  * dotted black line: the arc is added in _bn2
  * dotted red line: the arc is removed in _bn2

  :param BayesNet bn1: referent model for the comparison
  :param BayesNet bn2: bn compared to the referent model
  :param size: size of the rendered graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  cmp = GraphicalBNComparator(bn1, bn2)
  return getGraph(cmp.dotDiff(), size)


def showBNDiff(bn1, bn2, size=None):
  """ show a graphical diff between the arcs of _bn1 (reference) with those of _bn2.

  * full black line: the arc is common for both
  * full red line: the arc is common but inverted in _bn2
  * dotted black line: the arc is added in _bn2
  * dotted red line: the arc is removed in _bn2

  :param BayesNet bn1: referent model for the comparison
  :param BayesNet bn2: bn compared to the referent model
  :param size: size of the rendered graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  cmp = GraphicalBNComparator(bn1, bn2)
  showGraph(cmp.dotDiff(), size)


def showJunctionTree(bn, withNames=True, size=None):
  """
  Show a junction tree

  :param bn: the Bayesian network
  :param boolean withNames: display the variable names or the node id in the clique
  :param size: size of the rendered graph
  :return: the representation of the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  if withNames:
    return showDot(jt.toDotWithNames(bn), size)
  else:
    return showDot(jt.toDot(), size)


def getJunctionTree(bn, size=None):
  """
  get a HTML string for a junction tree (more specifically a join tree)

  :param bn: the Bayesian network
  :param size: size of the rendered graph
  :return: the HTML representation of the graph
  """
  if size is None:
    size = gum.config["notebook", "junctiontree_graph_size"]

  jtg = gum.JunctionTreeGenerator()
  jtg._model = bn
  jt = jtg.junctionTree(bn)
  jt._engine = jtg
  return getDot(getJT(jt, size), size)


def showProba(p, scale=1.0):
  """
  Show a mono-dim Potential

  :param p: the mono-dim Potential
  :param scale: the scale (zoom)
  """
  fig = proba2histo(p, scale)
  IPython.display.set_matplotlib_formats(
    gum.config["notebook", "graph_format"])
  plt.show()


def showProbaMinMax(pmin, pmax, scale=1.0):
  """
  Show a bi-Potential (min,max

  :param pmin: the mono-dim Potential for min values
  :param pmin: the mono-dim Potential for max values
  :param scale: the scale (zoom)
  """
  fig = probaMinMaxH(pmin, pmax, scale)
  IPython.display.set_matplotlib_formats(
    gum.config["notebook", "graph_format"])
  plt.show()


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

  h = gum.PythonApproximationListener(
    apsc._asIApproximationSchemeConfiguration())
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
    plt.title(
      "Time : {} s | Iterations : {} | Espilon : {}".format(apsc.currentTime(),
                                                            apsc.nbrIterations(),
                                                            apsc.epsilon()))
    plt.plot(scale(apsc.history()), 'g')


def showMN(mn, view=None, size=None, nodeColor=None, factorColor=None, edgeWidth=None, edgeColor=None, cmap=None,
           cmapEdge=None):
  """
  show a Markov network

  :param mn: the markov network
  :param view: 'graph' | 'factorgraph’ | None (default)
  :param size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param factorColor: a function returning a value (beeween 0 and 1) to be shown as a color of factor. (used when view='factorgraph')
  :param edgeWidth: a edgeMap of values to be shown as width of edges  (used when view='graph')
  :param edgeColor: a edgeMap of values (between 0 and 1) to be shown as color of edges (used when view='graph')
  :param cmap: color map to show the colors
  :param cmapEdge: color map to show the edge color if distinction is needed
  :return: the graph
  """
  if view is None:
    view = gum.config["notebook", "default_markovnetwork_view"]

  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapEdge is None:
    cmapEdge = cmap

  if view == "graph":
    dottxt = MN2UGdot(mn, size, nodeColor, edgeWidth,
                      edgeColor, cmap, cmapEdge)
  else:
    dottxt = MN2FactorGraphdot(mn, size, nodeColor, factorColor, cmapNode=cmap)

  return showGraph(dottxt, size)


def showInfluenceDiagram(diag, size=None):
  """
  show an influence diagram as a graph

  :param diag: the influence diagram
  :param size: size of the rendered graph
  :return: the representation of the influence diagram
  """
  if size is None:
    size = gum.config["influenceDiagram", "default_id_size"]

  return showGraph(ID2dot(diag), size)


def getInfluenceDiagram(diag, size=None):
  """
  get a HTML string for an influence diagram as a graph

  :param diag: the influence diagram
  :param size: size of the rendered graph
  :return: the HTML representation of the influence diagram
  """
  if size is None:
    size = gum.config["influenceDiagram", "default_id_size"]

  return getGraph(ID2dot(diag), size)


def showBN(bn, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmap=None, cmapArc=None):
  """
  show a Bayesian network

  :param bn: the Bayesian network
  :param size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the colors
  :param cmapArc: color map to show the arc color if distinction is needed
  :return: the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return showGraph(BN2dot(bn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


def showCN(cn, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmap=None, cmapArc=None):
  """
  show a credal network

  :param cn: the credal network
  :param size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the colors
  :param cmapArc: color map to show the arc color if distinction is needed
  :return: the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return showGraph(CN2dot(cn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


def getMN(mn, view=None, size=None, nodeColor=None, factorColor=None, edgeWidth=None, edgeColor=None, cmap=None,
          cmapEdge=None):
  """
  get an HTML string for a Markov network

  :param mn: the markov network
  :param view: 'graph' | 'factorgraph’ | None (default)
  :param size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param factorColor: a function returning a value (beeween 0 and 1) to be shown as a color of factor. (used when view='factorgraph')
  :param edgeWidth: a edgeMap of values to be shown as width of edges  (used when view='graph')
  :param edgeColor: a edgeMap of values (between 0 and 1) to be shown as color of edges (used when view='graph')
  :param cmap: color map to show the colors
  :param cmapEdge: color map to show the edge color if distinction is needed
  :return: the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapEdge is None:
    cmapEdge = cmap

  if view is None:
    view = gum.config["notebook", "default_markovnetwork_view"]

  if view == "graph":
    dottxt = MN2UGdot(mn, size, nodeColor, edgeWidth,
                      edgeColor, cmap, cmapEdge)
  else:
    dottxt = MN2FactorGraphdot(mn, size, nodeColor, factorColor, cmapNode=cmap)

  return getGraph(dottxt, size)


def getBN(bn, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmap=None, cmapArc=None):
  """
  get a HTML string for a Bayesian network

  :param bn: the Bayesian network
  :param size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the colors
  :param cmapArc: color map to show the arc color if distinction is needed

  :return: the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return getGraph(BN2dot(bn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


def getCN(cn, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmap=None, cmapArc=None):
  """
  get a HTML string for a credal network

  :param cn: the credal network
  :param size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the colors
  :param cmapArc: color map to show the arc color if distinction is needed

  :return: the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return getGraph(CN2dot(cn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


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
  nodevals = {bn.variable(n).name(): ie.H(n)
              for n in bn.nodes() if not n in idEvs}
  arcvals = {(x, y): ie.I(x, y) for x, y in bn.arcs()}
  gr = BN2dot(bn, size, nodeColor=_normalizeVals(nodevals, hilightExtrema=False), arcWidth=arcvals, cmapNode=cmap,
              cmapArc=cmap,
              showMsg=nodevals)

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
  png_legend = "<img style='vertical-align:middle' src='data:image/png;base64,%s'>" % encodebytes(png).decode(
    'ascii')

  gsvg = SVG(gr.create_svg())

  sss = "<div align='center'>" + gsvg.data + "</div>"
  sss += "<div align='center'>"
  sss += "<font color='" + \
         gum._proba2bgcolor(0.01, cmap) + "'>" + str(mi) + "</font>"
  sss += png_legend
  sss += "<font color='" + \
         gum._proba2bgcolor(0.99, cmap) + "'>" + str(ma) + "</font>"
  sss += "</div>"

  if asString:
    return sss
  else:
    return display(HTML(sss))


def getInformation(bn, evs=None, size=None, cmap=_INFOcmap):
  """
  get a HTML string for a bn annoted with results from inference : entropy and mutual informations

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the HTML string
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  return _reprInformation(bn, evs, size, cmap, asString=True)


def showInformation(bn, evs=None, size=None, cmap=_INFOcmap):
  """
  show a bn annoted with results from inference : entropy and mutual informations

  :param bn: the BN
  :param evs: map of evidence
  :param size:  size of the graph
  :param cmap: colour map used
  :return: the graph
  """
  if evs is None:
    evs = {}

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  return _reprInformation(bn, evs, size, cmap, asString=False)


def _get_showInference(model, engine=None, evs=None, targets=None, size=None,
                       nodeColor=None, factorColor=None, arcWidth=None,
                       arcColor=None, cmap=None, cmapArc=None, graph=None, view=None):
  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]

  if evs is None:
    evs = {}

  if targets is None:
    targets = {}

  if isinstance(model, gum.BayesNet):
    if engine is None:
      engine = gum.LazyPropagation(model)
    return BNinference2dot(model, size, engine, evs, targets, nodeColor=nodeColor, arcWidth=arcWidth, arcColor=arcColor,
                           cmapNode=cmap, cmapArc=cmapArc)
  elif isinstance(model, gum.MarkovNet):
    if view is None:
      view = gum.config["notebook", "default_markovnetwork_view"]
      if engine is None:
        engine = gum.ShaferShenoyMNInference(model)

      if view == "graph":
        return MNinference2UGdot(model, size, engine, evs, targets, nodeColor=nodeColor, factorColor=factorColor,
                                 arcWidth=arcWidth, arcColor=arcColor, cmapNode=cmap, cmapArc=cmapArc)
      else:
        return MNinference2FactorGraphdot(model, size, engine, evs, targets, nodeColor=nodeColor,
                                          factorColor=factorColor, cmapNode=cmap)
  elif isinstance(model, gum.InfluenceDiagram):
    if engine is None:
      engine = gum.ShaferShenoyLIMIDInference(model)
    return LIMIDinference2dot(model, size, engine, evs, targets)
  elif isinstance(model, gum.CredalNet):
    if engine is None:
      engine = gum.CNMonteCarloSampling(model)
    return CNinference2dot(model, size, engine, evs, targets, nodeColor, arcWidth, arcColor, cmap)
  else:
    raise gum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovNet or Influence Diagram")


def showInference(model, engine=None, evs=None, targets=None, size=None,
                  nodeColor=None, factorColor=None,
                  arcWidth=None, arcColor=None,
                  cmap=None, cmapArc=None, graph=None, view=None):
  """
  show pydot graph for an inference in a notebook

  :param GraphicalModel model: the model in which to infer (pyAgrum.BayesNet, pyAgrum.MarkovNet or
          pyAgrum.InfluenceDiagram)
  :param gum.Inference engine: inference algorithm used. If None, gum.LazyPropagation will be used for BayesNet,
          gum.ShaferShenoy for gum.MarkovNet and gum.ShaferShenoyLIMIDInference for gum.InfluenceDiagram.
  :param dictionnary evs: map of evidence
  :param set targets: set of targets
  :param string size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param factorColor: a nodeMap of values (between 0 and 1) to be shown as color of factors (in MarkovNet representation)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the color of nodes and arcs
  :param cmapArc: color map to show the vals of Arcs.
  :param graph: only shows nodes that have their id in the graph (and not in the whole BN)
  :param view: graph | factorgraph | None (default) for Markov network
  :return: the desired representation of the inference
  """
  grinf = _get_showInference(model, engine, evs, targets, size, nodeColor, arcWidth, arcColor, cmap, cmapArc, graph,
                             view)
  showGraph(grinf, size)


def getInference(model, engine=None, evs=None, targets=None, size=None,
                 nodeColor=None, factorColor=None,
                 arcWidth=None, arcColor=None,
                 cmap=None, cmapArc=None, graph=None, view=None):
  """
  get a HTML string for an inference in a notebook

  :param GraphicalModel model: the model in which to infer (pyAgrum.BayesNet, pyAgrum.MarkovNet or
          pyAgrum.InfluenceDiagram)
  :param gum.Inference engine: inference algorithm used. If None, gum.LazyPropagation will be used for BayesNet,
          gum.ShaferShenoy for gum.MarkovNet and gum.ShaferShenoyLIMIDInference for gum.InfluenceDiagram.
  :param dictionnary evs: map of evidence
  :param set targets: set of targets
  :param string size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param factorColor: a nodeMap of values (between 0 and 1) to be shown as color of factors (in MarkovNet representation)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the color of nodes and arcs
  :param cmapArc: color map to show the vals of Arcs.
  :param graph: only shows nodes that have their id in the graph (and not in the whole BN)
  :param view: graph | factorgraph | None (default) for Markov network

  :return: the desired representation of the inference
  """
  grinf = _get_showInference(model, engine, evs, targets, size, nodeColor=nodeColor, arcWidth=arcWidth,
                             arcColor=arcColor, cmap=cmap, cmapArc=cmapArc, graph=graph, view=view)
  return getGraph(grinf, size)


def _reprPotential(pot, digits=None, withColors=True, varnames=None, asString=False):
  """
  return a representation of a gum.Potential as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  :param pot: the potential to get
  :param digits: number of digits to show
  :param withColors: bgcolor for proba cells or not
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
      s += "color:black;background-color:" + _rgb(r, g, b) + ";"
    s += "text-align:right;'>{:." + str(digits) + "f}</td>"
    return s.format(val)

  if digits is None:
    digits = gum.config['notebook', 'potential_visible_digits']

  if gum.config["notebook", "potential_with_colors"] == "False":
    withColors = False

  html = list()
  html.append('<table style="border:1px solid black;">')
  if pot.empty():
    html.append(
      "<tr><th>&nbsp;</th></tr>")
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
        "<tr><th colspan='{}'></th><th colspan='{}' style='border:1px solid black;color:black;background-color:#808080;'><center>{"
        "}</center></th></tr>".format(
          nparents, var.domainSize(), varname))
    else:
      html.append(
        "<tr style='border:1px solid black;color:black;background-color:#808080'><th colspan='{}'><center>{}</center></th></tr>".format(
          var.domainSize(),
          varname))
    # second line
    s = "<tr>"
    if nparents > 0:
      # parents order
      if gum.config["notebook", "potential_parent_values"] == "revmerge":
        pmin, pmax, pinc = nparents - 1, 0 - 1, -1
      else:
        pmin, pmax, pinc = 0, nparents, 1
      for par in range(pmin, pmax, pinc):
        parent = pot.var_names[par] if varnames is None else varnames[par]
        s += "<th style='border:1px solid black;color:black;background-color:#808080'><center>{}</center></th>".format(
          parent)
    for label in var.labels():
      s += "<th style='border:1px solid black;border-bottom-style: double;color:black;background-color:#BBBBBB'><center>{}</center></th>".format(
        label)
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
      # parents order
      if gum.config["notebook", "potential_parent_values"] == "revmerge":
        pmin, pmax, pinc = 1, nparents + 1, 1
      else:
        pmin, pmax, pinc = nparents, 0, -1
      for par in range(pmin, pmax, pinc):
        label = inst.variable(par).label(inst.val(par))
        if par == 1 or gum.config["notebook", "potential_parent_values"] == "nomerge":
          s += "<th style='border:1px solid black;color:black;background-color:#BBBBBB'><center>{}</center></th>".format(
            label)
        else:
          if sum([inst.val(i) for i in range(1, par)]) == 0:
            s += "<th style='border:1px solid black;color:black;background-color:#BBBBBB;' rowspan = '{}'><center>{}</center></th>".format(
              offset[par], label)
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
  epsilon = 1e-5
  if pot.min() < -epsilon:
    return False
  if pot.max() > 1 + epsilon:
    return False

  # is it a joint proba ?
  if abs(pot.sum() - 1) < epsilon:
    return True

  # marginal and then not proba (because of the test just above)
  if pot.nbrDim() < 2:
    return False

  # is is a CPT ?
  q = pot.margSumOut([pot.variable(0).name()])
  if abs(q.max() - 1) > epsilon:
    return False
  if abs(q.min() - 1) > epsilon:
    return False
  return True


def showPotential(pot, digits=None, withColors=None, varnames=None):
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
    if gum.config["notebook", "potential_with_colors"] == "False":
      withColors = False
    else:
      withColors = __isKindOfProba(pot)

  display(_reprPotential(
    pot, digits, withColors, varnames, asString=False))


def getPotential(pot, digits=None, withColors=None, varnames=None):
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
    if gum.config["notebook", "potential_with_colors"] == "False":
      withColors = False
    else:
      withColors = __isKindOfProba(pot)

  return _reprPotential(pot, digits, withColors, varnames, asString=True)


def getSideBySide(*args, **kwargs):
  """
  create an HTML table for args as string (using string, _repr_html_() or str())

  :param args: HMTL fragments as string arg, arg._repr_html_() or str(arg)
  :param captions: list of strings (captions)
  :return: a string representing the table
  """

  if not set(kwargs.keys()).issubset(set(['captions', 'valign'])):
    raise TypeError("sideBySide() got unexpected keyword argument(s) : '{}'".format(
      set(kwargs.keys()).difference(set(['captions', 'valign']))))

  if 'captions' in kwargs:
    captions = kwargs['captions']
  else:
    captions = None

  if 'valign' in kwargs:
    v_align = 'vertical-align:' + kwargs['valign'] + ';'
  else:
    v_align = ""

  s = '<table style="border-style: hidden; border-collapse: collapse;" width="100%">'

  def reprHTML(s):
    if isinstance(s, str):
      return s
    elif hasattr(s, '_repr_html_'):
      return s._repr_html_()
    else:
      return str(s)

  s += '<tr><td style="border-top:hidden;border-bottom:hidden;' + v_align + '"><div align="center" style="' + v_align \
       + '">'
  s += (
      '</div></td><td style="border-top:hidden;border-bottom:hidden;' + v_align + '"><div align="center" style="' +
      v_align + '">').join(
    [reprHTML(arg)
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
  display(HTML(getSideBySide(*args, **kwargs)))


def getInferenceEngine(ie, inferenceCaption):
  """
  display an inference as a BN+ lists of hard/soft evidence and list of targets

  :param gum.InferenceEngine ie: inference engine
  :param string inferenceCaption: title for caption

  """
  t = '<div align="left"><ul>'
  if ie.nbrHardEvidence() > 0:
    t += "<li><b>hard evidence</b><br/>"
    t += ", ".join([ie.BN().variable(n).name()
                    for n in ie.hardEvidenceNodes()])
    t += "</li>"
  if ie.nbrSoftEvidence() > 0:
    t += "<li><b>soft evidence</b><br/>"
    t += ", ".join([ie.BN().variable(n).name()
                    for n in ie.softEvidenceNodes()])
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
                      + (", ".join([ie.BN().variable(n).name()
                                    for n in ns]))
                      + ']' for ns in ie.jointTargets()])
      t += "</li>"
  t += '</ul></div>'
  return getSideBySide(getBN(ie.BN()), t, captions=[inferenceCaption, "Evidence and targets"])


def getJT(jt, size=None):
  if gum.config["notebook", "junctiontree_with_names"] == "True":
    def cliqlabels(c):
      return " ".join(
        sorted([model.variable(n).name() for n in jt.clique(c)]))

    def cliqnames(
        c):
      return "-".join(sorted([model.variable(n).name() for n in jt.clique(c)]))

    def seplabels(c1, c2):
      return " ".join(
        sorted([model.variable(n).name() for n in jt.separator(c1, c2)]))

    def sepnames(c1, c2):
      return cliqnames(c1) + '+' + cliqnames(c2)
  else:
    def cliqlabels(c):
      return " ".join([str(n) for n in sorted(jt.clique(c))])

    def cliqnames(c):
      return "-".join([str(n) for n in sorted(jt.clique(c))])

    def seplabels(c1, c2):
      return " ".join(
        [str(n) for n in sorted(jt.separator(c1, c2))])

    def sepnames(c1, c2):
      return cliqnames(c1) + '^' + cliqnames(c2)

  model = jt._engine._model
  name = model.propertyWithDefault(
    "name", str(type(model)).split(".")[-1][:-2])
  graph = dot.Dot(graph_type='graph', graph_name=name, bgcolor="transparent")
  for c in jt.nodes():
    graph.add_node(dot.Node('"' + cliqnames(c) + '"',
                            label='"' + cliqlabels(c) + '"',
                            style="filled",
                            fillcolor=gum.config["notebook",
                                                 "junctiontree_clique_bgcolor"],
                            fontcolor=gum.config["notebook",
                                                 "junctiontree_clique_fgcolor"],
                            fontsize=gum.config["notebook", "junctiontree_clique_fontsize"]))
  for c1, c2 in jt.edges():
    graph.add_node(dot.Node('"' + sepnames(c1, c2) + '"',
                            label='"' + seplabels(c1, c2) + '"',
                            style="filled",
                            shape="box", width="0", height="0", margin="0.02",
                            fillcolor=gum.config["notebook",
                                                 "junctiontree_separator_bgcolor"],
                            fontcolor=gum.config["notebook",
                                                 "junctiontree_separator_fgcolor"],
                            fontsize=gum.config["notebook", "junctiontree_separator_fontsize"]))
  for c1, c2 in jt.edges():
    graph.add_edge(dot.Edge('"' + cliqnames(c1) +
                            '"', '"' + sepnames(c1, c2) + '"'))
    graph.add_edge(dot.Edge('"' + sepnames(c1, c2) +
                            '"', '"' + cliqnames(c2) + '"'))

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  graph.set_size(gum.config["notebook", "junctiontree_graph_size"])

  return graph.to_string()


def getCliqueGraph(cg, size=None):
  """get a representation for clique graph. Special case for junction tree
  (clique graph with an attribute `_engine`)

  Args:
      cg (gum.CliqueGraph): the clique graph (maybe junction tree for a _model) to
                            represent
  """
  if hasattr(cg, "_engine"):
    return getDot(getJT(cg), size)
  else:
    return getDot(cg.toDot())


def show(model, size=None):
  """
  propose a (visual) representation of a model in a notebook

  :param GraphicalModel model: the model to show (pyAgrum.BayesNet, pyAgrum.MarkovNet, pyAgrum.InfluenceDiagram or pyAgrum.Potential)

  :param int size: optional size for the graphical model (no effect for Potential)
  """
  if isinstance(model, gum.BayesNet):
    showBN(model, size)
  elif isinstance(model, gum.MarkovNet):
    showMN(model, size)
  elif isinstance(model, gum.InfluenceDiagram):
    showInfluenceDiagram(model, size)
  elif isinstance(model, gum.CredalNet):
    showCN(model, size)
  elif isinstance(model, gum.Potential):
    showPotential(model)
  else:
    raise gum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovNet or Influence Diagram")


def show(model, **kwargs):
  """
  propose a (visual) representation of a model in a notebook

  :param GraphicalModel model: the model to show (pyAgrum.BayesNet, pyAgrum.MarkovNet, pyAgrum.InfluenceDiagram or pyAgrum.Potential)

  :param int size: optional size for the graphical model (no effect for Potential)
  """
  if isinstance(model, gum.BayesNet):
    showBN(model, **kwargs)
  elif isinstance(model, gum.MarkovNet):
    showMN(model, **kwargs)
  elif isinstance(model, gum.InfluenceDiagram):
    showInfluenceDiagram(model, **kwargs)
  elif isinstance(model, gum.CredalNet):
    showCN(model, **kwargs)
  elif isinstance(model, gum.Potential):
    showPotential(model)
  elif hasattr(model, "toDot"):
    showDot(dot.graph_from_dot_data(model.toDot()), **kwargs)
  else:
    raise gum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovNet, Influence Diagram or Potential or ...")


def export(model, filename, **kwargs):
  """
  export the graphical representation of the model in filename (png, pdf,etc.)

  :param filename : the file to write

  :param GraphicalModel model: the model to show (pyAgrum.BayesNet, pyAgrum.MarkovNet, pyAgrum.InfluenceDiagram or pyAgrum.Potential)

  :param int size: optional size for the graphical model (no effect for Potential)
  """
  format = filename.split(".")[-1]
  if format not in ['pdf', 'png', 'fig', 'jpg', 'svg','ps']:
    raise Exception(
      f"{filename} in not a correct filename for export : extension '{format}' not in [pdf,png,fig,jpg,svg].")

  if isinstance(model, gum.BayesNet):
    fig = BN2dot(model, **kwargs)
  elif isinstance(model, gum.MarkovNet):
    if gum.config["notebook", "default_markovnetwork_view"] == "graph":
      fig = MN2UGdot(model, **kwargs)
    else:
      fig = MN2FactorGraphdot(model, **kwargs)
  elif isinstance(model, gum.InfluenceDiagram):
    fig = ID2dot(model, **kwargs)
  elif isinstance(model, gum.CredalNet):
    fig = CN2dot(model, **kwargs)
  elif hasattr(model, "toDot"):
    fig = dot.graph_from_dot_data(model.toDot(), **kwargs)
  else:
    raise gum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovNet or Influence Diagram")
  fig.write(filename, format=format)


def exportInference(model, filename, engine=None, evs=None, targets=None, size=None,
                    nodeColor=None, factorColor=None,
                    arcWidth=None, arcColor=None,
                    cmap=None, cmapArc=None, graph=None, view=None):
  """
  the graphical representation of an inference in a notebook

  :param GraphicalModel model: the model in which to infer (pyAgrum.BayesNet, pyAgrum.MarkovNet or
          pyAgrum.InfluenceDiagram)
  :param gum.Inference engine: inference algorithm used. If None, gum.LazyPropagation will be used for BayesNet,
          gum.ShaferShenoy for gum.MarkovNet and gum.ShaferShenoyLIMIDInference for gum.InfluenceDiagram.
  :param dictionnary evs: map of evidence
  :param set targets: set of targets
  :param string size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param factorColor: a nodeMap of values (between 0 and 1) to be shown as color of factors (in MarkovNet representation)
  :param arcWidth: a arcMap of values to be shown as width of arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmap: color map to show the color of nodes and arcs
  :param cmapArc: color map to show the vals of Arcs.
  :param graph: only shows nodes that have their id in the graph (and not in the whole BN)
  :param view: graph | factorgraph | None (default) for Markov network
  :return: the desired representation of the inference

  warning:: In order to export probability distribution in nodes, exportInference needs that Graphviz includes the Cairo renderer
  """
  format = filename.split(".")[-1]
  if format not in ['pdf', 'png', 'fig', 'jpg', 'svg']:
    raise Exception(
      f"{filename} in not a correct filename for export : extension '{format}' not in [pdf,png,fig,jpg,svg].")

  fig = _get_showInference(model, engine, evs, targets, size, nodeColor, arcWidth, arcColor, cmap, cmapArc, graph,
                           view)
  fig.write(filename, format=format)


def _update_config():
  # hook to control some parameters for notebook when config changes
  mpl.rcParams['figure.facecolor'] = gum.config["notebook", "figure_facecolor"]
  IPython.display.set_matplotlib_formats(
    gum.config["notebook", "graph_format"])


# check if an instance of ipython exists
try:
  get_ipython
except NameError as e:
  import warnings

  warnings.warn("""
  ** pyAgrum.lib.notebook has to be imported from an IPython's instance (mainly notebook).
  """)
else:
  gum.config.add_hook(_update_config)
  gum.config.run_hooks()

  # adding _repr_html_ to some pyAgrum classes !
  gum.BayesNet._repr_html_ = lambda self: getBN(self)
  gum.BayesNetFragment._repr_html_ = lambda self: getBN(self)
  gum.MarkovNet._repr_html_ = lambda self: getMN(self)
  gum.BayesNetFragment._repr_html_ = lambda self: getBN(self)
  gum.InfluenceDiagram._repr_html_ = lambda self: getInfluenceDiagram(self)
  gum.CredalNet._repr_html_ = lambda self: getCN(self)

  gum.CliqueGraph._repr_html_ = lambda self: getCliqueGraph(self)

  gum.Potential._repr_html_ = lambda self: getPotential(self)
  gum.LazyPropagation._repr_html_ = lambda self: getInferenceEngine(
    self, "Lazy Propagation on this BN")

  gum.UndiGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.DiGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.MixedGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.DAG._repr_html_ = lambda self: getDot(self.toDot())
  gum.EssentialGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.MarkovBlanket._repr_html_ = lambda self: getDot(self.toDot())
