# (c) Copyright by Pierre-Henri Wuillemin, 2022
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
tools for BN analysis in ipython (and spyder)
"""

import IPython.display
import matplotlib as mpl
import matplotlib.pyplot as plt
import numpy as np
import pydot as dot

from IPython.display import Image, display

import pyAgrum as gum
from pyAgrum.lib.bn2graph import BN2dot, BNinference2dot
from pyAgrum.lib.id2graph import ID2dot, LIMIDinference2dot
from pyAgrum.lib.mn2graph import MN2UGdot, MNinference2UGdot
from pyAgrum.lib.mn2graph import MN2FactorGraphdot, MNinference2FactorGraphdot
from pyAgrum.lib.bn_vs_bn import GraphicalBNComparator
from pyAgrum.lib.proba_histogram import proba2histo

# check if an instance of ipython exists
try:
  get_ipython
except NameError as e:
  raise ImportError(
    "[pyAgrum ERROR] pyAgrum.lib.ipython has to be imported from an IPython's instance (mainly ipython's console).") from None


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

  for name in packages:
    print("%s : %s" % (name, packages[name]))


def showGraph(gr, size=None):
  """
  show a pydot graph in a notebook

  Parameters
  ----------
  gr: pydot.Dot
    the graph to show
  size: int|str
    the size of the visualisation
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  gr.set_size(size)
  display(Image(gr.create_png()))


def _from_dotstring(dotstring):
  g = dot.graph_from_dot_data(dotstring)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

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

  Parameters
  ----------
  dotstring: str
    the dot string
  size: float | str
    the size of the graphe
  """
  showGraph(_from_dotstring(dotstring), size)


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
  """
  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  if withNames:
    return showDot(jt.toDotWithNames(bn), size)
  else:
    return showDot(jt.toDot(), size)


def showBN(bn, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmap=None, cmapArc=None):
  """
  show a Bayesian network

  :param bn: the Bayesian network
  :param size: size of the rendered graph
  :param format: render as "png" or "svg"
  :param vals: a nodeMap of values to be shown as color nodes
  :param arcvals: a arcMap of values to be shown as bold arcs
  :param cmap: color map to show the vals
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return showGraph(BN2dot(bn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


def showProba(p, scale=1.0):
  """
  Show a mono-dim Potential

  :param p: the mono-dim Potential
  :return:
  """
  fig = proba2histo(p, scale)
  #  fig.patch.set_facecolor(gum.config["notebook", "figure_facecolor"])
  IPython.display.set_matplotlib_formats(
    gum.config["notebook", "graph_format"])
  plt.show()


def showPosterior(bn, evs, target):
  """
  shortcut for showProba(gum.getPosterior(bn,evs,target))

  :param bn: the BayesNet
  :param evs: map of evidence
  :param target: name of target variable
  """
  showProba(gum.getPosterior(bn, evs=evs, target=target))


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


def showInference(model, engine=None, evs=None, targets=None, size=None,
                  nodeColor=None, factorColor=None,
                  arcWidth=None, arcColor=None,
                  cmap=None, cmapArc=None, graph=None, view=None):
  import warnings
  warnings.warn(
    "gum.lib.ipython does not provide `showInference` due to the use of svg format (not compatible with spyder).")


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


def showPotential(p):
  print(p)


def show(model, size=None):
  """
  propose a (visual) representation of a model in ipython console

  :param GraphicalModel model: the model to show (pyAgrum.BayesNet, pyAgrum.MarkovNet, pyAgrum.InfluenceDiagram or pyAgrum.Potential)

  :param int size: optional size for the graphical model (no effect for Potential)
  """
  if isinstance(model, gum.BayesNet):
    showBN(model, size)
  elif isinstance(model, gum.MarkovNet):
    showMN(model, size)
  elif isinstance(model, gum.InfluenceDiagram):
    showInfluenceDiagram(model, size)
  elif isinstance(model, gum.Potential):
    showPotential(model)
  else:
    raise gum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovNet or Influence Diagram")


# check if an instance of ipython exists
try:
  get_ipython
except NameError as e:
  import warnings

  warnings.warn("""
  ** pyAgrum.lib.notebook has to be import from an IPython's instance.
  """)
