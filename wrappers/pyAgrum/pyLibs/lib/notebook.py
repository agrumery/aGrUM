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
tools for BN in jupyter notebook
"""

import time
import re
import sys

# fix DeprecationWarning of base64.encodestring()
try:
  from base64 import encodebytes
except ImportError:  # 3+
  from base64 import encodestring as encodebytes

import io
import base64

import matplotlib as mpl
import matplotlib.pyplot as plt

try:
  from matplotlib_inline.backend_inline import set_matplotlib_formats
except ImportError:  # because of python 2.7, matplotlib-inline cannot be part of requirements.txt
  def set_matplotlib_formats(*args, **kwargs):
    # dummy version when no matplotlib_inline package
    print("** pyAgrum** For better visualizations, please install matplotlib-inline.")

import numpy as np
import pydot as dot

import IPython.core.display
import IPython.core.pylabtools
import IPython.display

import pyAgrum as gum
from pyAgrum.lib.bn2graph import BN2dot, BNinference2dot
from pyAgrum.lib.cn2graph import CN2dot, CNinference2dot
from pyAgrum.lib.id2graph import ID2dot, LIMIDinference2dot
from pyAgrum.lib.mn2graph import MN2UGdot, MNinference2UGdot
from pyAgrum.lib.mn2graph import MN2FactorGraphdot, MNinference2FactorGraphdot
from pyAgrum.lib.bn_vs_bn import GraphicalBNComparator, graphDiff
from pyAgrum.lib.proba_histogram import proba2histo, probaMinMaxH
from pyAgrum.lib.image import prepareShowInference, prepareLinksForSVG

from pyAgrum.lib._colors import setDarkTheme, setLightTheme, getBlackInTheme

import pyAgrum.lib._colors as gumcols


class FlowLayout(object):
  """"
  A class / object to display plots in a horizontal / flow layout below a cell

  based on : https://stackoverflow.com/questions/21754976/ipython-notebook-arrange-plots-horizontally
  """

  def __init__(self):
    self.clear()

  def clear(self):
    """
    clear the flow
    """
    # string buffer for the HTML: initially some CSS; images to be appended
    self.sHtml = f"""
      <style>
      .floating-box {{
      display: inline-block;
      margin: 7px;
      padding : 3px;
      border: {gum.config.asInt["notebook","flow_border_width"]}px solid {gum.config["notebook","flow_border_color"]};  
      valign:middle;
      background-color: {gum.config["notebook","flow_background_color"]};
      }}
      </style>
      """
    return self

  def _getCaption(self, caption):
    if caption == "":
      return ""
    return f"<br><center><small><em>{caption}</em></small></center>"

  def add_html(self, html, caption=None, title=None):
    """
    add an html element in the row (title is an obsolete parameter)
    """
    if caption is None:
      if title is None:
        cap = ""
      else:
        print("`title` is obsolete since `0.22.8`. Please use `caption`.")
        cap = title
    else:
      cap = caption

    self.sHtml += f'<div class="floating-box">{html}{self._getCaption(cap)}</div>'
    return self

  def add_separator(self, size=3):
    """
    add a (poor) separation between elements in a row
    """
    self.add_html("&nbsp;" * size)
    return self

  def add_plot(self, oAxes, caption=None, title=None):
    """
    Add a PNG representation of a Matplotlib Axes object
    (title is an obsolete parameter)
    """
    if caption is None:
      if title is None:
        cap = ""
      else:
        print("`title` is obsolete since `0.22.8`. Please use `caption`.")
        cap = title
    else:
      cap = caption

    Bio = io.BytesIO()  # bytes buffer for the plot
    fig = oAxes.get_figure()
    fig.canvas.print_png(Bio)  # make a png of the plot in the buffer

    # encode the bytes as string using base 64
    sB64Img = base64.b64encode(Bio.getvalue()).decode()
    self.sHtml += f'<div class="floating-box"><img src="data:image/png;base64,{sB64Img}\n">{self._getCaption(cap)}</div>'
    plt.close()
    return self

  def new_line(self):
    """
    add a breakline (a new row)
    """
    self.sHtml += '<br/>'
    return self

  def html(self):
    """
    Returns its content as HTML object
    """
    return IPython.display.HTML(self.sHtml)

  def display(self):
    """
    Display the accumulated HTML
    """
    IPython.display.display(self.html())
    self.clear()

  def add(self, obj, caption=None, title=None):
    """
    add an element in the row by trying to treat it as plot or html if possible.
    (title is an obsolete parameter)
    """
    if caption is None:
      if title is None:
        cap = ""
      else:
        print("`title` is obsolete since `0.22.8`. Please use `caption`.")
        cap = title
    else:
      cap = caption

    if hasattr(obj, "get_figure"):
      self.add_plot(obj, cap)
    elif hasattr(obj, "_repr_html_"):
      self.add_html(obj._repr_html_(), cap)
    else:
      self.add_html(obj, cap)

    return self

  def row(self, *args, captions=None):
    """
    Create a row with flow with the same syntax as `pyAgrum.lib.notebook.sideBySide`.
    """
    self.clear()
    for i, arg in enumerate(args):
      if captions is None:
        self.add(arg)
      else:
        self.add(arg, captions[i])

    self.display()


flow = FlowLayout()


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
  packages["Matplotlib"] = mpl.__version__
  packages["Numpy"] = np.__version__
  packages["pyDot"] = dot.__version__
  packages["pyAgrum"] = gum.__version__

  res = "<table><tr><th>Library</th><th>Version</th></tr>"

  for name in packages:
    res += "<tr><td>%s</td><td>%s</td></tr>" % (name, packages[name])

  res += "</table><div align='right'><small>%s</small></div>" % time.strftime(
    '%a %b %d %H:%M:%S %Y %Z'
  )

  IPython.display.display(IPython.display.HTML(res))


def _reprGraph(gr, size, asString, format=None):
  """
  repr a pydot graph in a notebook

  Parameters
  ----------
  gr : dot.Dot
    the dot representation of the graph
  size: int | str
    the size argument for the representation
  asString : bool
    display the graph or return a string containing the corresponding HTML fragment
  format! str
    "svg" or "png" ?

  Returns
  -------
  str | None
    return the HTML representation as a str or display the graph
  """
  if size is not None:
    gr.set_size(size)

  if format is None:
    format = gum.config["notebook", "graph_format"]

  if format == "svg":
    gsvg = IPython.display.SVG(prepareLinksForSVG(
        gr.create_svg(encoding="utf-8").decode('utf-8')))
    if asString:
      return gsvg.data
    else:
      IPython.display.display(gsvg)
  else:
    i = IPython.core.display.Image(format="png", data=gr.create_png())
    if asString:
      return f'<img style="margin:0" src="data:image/png;base64,{encodebytes(i.data).decode()}"/>'
    else:
      IPython.core.display.display_png(i)


def showGraph(gr: dot.Dot, size=None):
  """
  show a pydot graph in a notebook

  Parameters
  ----------
  gr: pydot.Dot
    the graph
  size: float|str
    the size of the rendered graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  return _reprGraph(gr, size, asString=False)


def getGraph(gr: dot.Dot, size=None) -> str:
  """
  get an HTML representation of a pydot graph

  Parameters
  ----------
  gr: pydot.Dot
    the graph
  size: float|str
    the size of the rendered graph

  Returns
  -------
    the HTML representation of the graph (as a string)
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  gr.del_node('"\\n"')
  gr.del_node('"\\n\\n"')

  return _reprGraph(gr, size, asString=True)


def _from_dotstring(dotstring):
  g = dot.graph_from_dot_data(dotstring)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')
  g.del_node('"\\n\\n"')

  g.set_bgcolor("transparent")
  for e in g.get_edges():
    if e.get_color() is None:
      e.set_color(getBlackInTheme())
  for n in g.get_nodes():
    if n.get_color() is None:
      n.set_color(getBlackInTheme())
    if n.get_fontcolor() is None:
      n.set_fontcolor(getBlackInTheme())
  return g


def showDot(dotstring: str, size=None):
  """
  show a dot string as a graph

  Parameters
  ----------
  dotstring:str
    the dot string
  size: float|str
    size of the rendered graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  showGraph(_from_dotstring(dotstring), size)


def getDot(dotstring: str, size=None) -> str:
  """
  get an HTML representation of a dot string

  Parameters
  ----------
  dotstring:str
    the dot string
  size: float|str
    size of the rendered graph

  Returns
  -------
    the HTML representation of the dot string
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  return getGraph(_from_dotstring(dotstring), size)


def getBNDiff(bn1, bn2, size=None, noStyle=False):
  """
  get a HTML string representation of a graphical diff between the arcs of _bn1 (reference) with those of _bn2.

  if `noStyle` is False use 4 styles (fixed in pyAgrum.config) :
    - the arc is common for both
    - the arc is common but inverted in `bn2`
    - the arc is added in `bn2`
    - the arc is removed in `bn2`

  Parameters
  ----------
  bn1: pyAgrum.BayesNet
    the reference
  bn2: pyAgrum.BayesNet
    the compared one
  size: float|str
    size of the rendered graph
  noStyle: bool
    with style or not.

  Returns
  -------
    the HTML representation of the comparison
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  return getGraph(graphDiff(bn1, bn2, noStyle), size)


def showBNDiff(bn1, bn2, size=None, noStyle=False):
  """
  show a graphical diff between the arcs of _bn1 (reference) with those of _bn2.

  if `noStyle` is False use 4 styles (fixed in pyAgrum.config) :
    - the arc is common for both
    - the arc is common but inverted in `bn2`
    - the arc is added in `bn2`
    - the arc is removed in `bn2`

  Parameters
  ----------
  bn1: pyAgrum.BayesNet
    the reference
  bn2: pyAgrum.BayesNet
    the compared one
  size: float|str
    size of the rendered graph
  noStyle: bool
    with style or not.
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  showGraph(graphDiff(bn1, bn2, noStyle), size)


def showInformation(*args, **kwargs):
  print(
    "[pyAgrum] pyAgrum.lib.notebook.showInformation is deprecated since 0.20.2. Please use pyAgrum.lib.explain.showInfomation instead."
  )
  import pyAgrum.lib.explain as explain
  explain.showInformation(*args, **kwargs)


def getJunctionTreeMap(bn, size:str=None,scaleClique: float = None, scaleSep: float = None, lenEdge: float = None, colorClique: str = None,
          colorSep: str = None):
  """
  Return a representation of the map of the junction tree of a Bayesian network

  Parameters
  ----------
  bn: pyAgrum.BayesNet
    the model
  scaleClique: float
    the scale for the size of the clique nodes (depending on the number of nodes in the clique)
  scaleSep: float
    the scale for the size of the separator nodes (depending on the number of nodes in the clique)
  lenEdge: float
    the desired length of edges
  colorClique: str
    color for the clique nodes
  colorSep: str
    color for the separator nodes
  """
  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  return getGraph(jt.map(scaleClique,scaleSep,lenEdge,colorClique,colorSep),size)


def showJunctionTreeMap(bn, size:str=None,scaleClique: float = None, scaleSep: float = None, lenEdge: float = None, colorClique: str = None,
          colorSep: str = None):
  """
  Show the map of the junction tree of a Bayesian network

  Parameters
  ----------
  bn: pyAgrum.BayesNet
    the model
  scaleClique: float
    the scale for the size of the clique nodes (depending on the number of nodes in the clique)
  scaleSep: float
    the scale for the size of the separator nodes (depending on the number of nodes in the clique)
  lenEdge: float
    the desired length of edges
  colorClique: str
    color for the clique nodes
  colorSep: str
    color for the separator nodes
  """
  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  showGraph(jt.map(scaleClique,scaleSep,lenEdge,colorClique,colorSep),size)


def showJunctionTree(bn, withNames=True, size=None):
  """
  Show a junction tree of a Bayesian network

  Parameters
  ----------
  bn: pyAgrum.BayesNet
    the model
  withNames: bool
    names or id in the graph (names can created very large nodes)
  size: float|str
    size of the rendered graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  jt._engine = jtg
  jtg._model = bn

  if withNames:
    showDot(jt.toDotWithNames(bn), size)
  else:
    showDot(jt.toDot(), size)


def getJunctionTree(bn, withNames=True, size=None):
  """
  get a HTML string for a junction tree (more specifically a join tree)

  :param bn: the Bayesian network
  :param boolean withNames: display the variable names or the node id in the clique
  :param size: size of the rendered graph
  :return: the HTML representation of the graph
  """
  if size is None:
    size = gum.config["notebook", "junctiontree_graph_size"]

  jtg = gum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  jt._engine = jtg
  jtg._model = bn

  if withNames:
    return getDot(jt.toDotWithNames(bn), size)
  else:
    return getDot(jt.toDot(), size)


def showProba(p, scale=1.0):
  """
  Show a mono-dim Potential (a marginal)

  Parameters
  ----------
  p: pyAgrum.Potential
    the marginal to show
  scale: float
    the zoom factor
  """
  fig = proba2histo(p, scale)
  set_matplotlib_formats(gum.config["notebook", "graph_format"])
  plt.show()


def _getMatplotFig(fig):
  bio = io.BytesIO()  # bytes buffer for the plot
  # .canvas.print_png(bio)  # make a png of the plot in the buffer
  fig.savefig(bio, format='png', bbox_inches='tight')

  # encode the bytes as string using base 64
  sB64Img = base64.b64encode(bio.getvalue()).decode()
  res = f'<img src="data:image/png;base64,{sB64Img}\n">'
  plt.close()
  return res


def getProba(p, scale=1.0) -> str:
  """
  get a mono-dim Potential as html (png/svg) image

  Parameters
  ----------
  p: pyAgrum.Potential
    the marginal to show
  scale: float
    the zoom factor

  Returns
  -------
  str
    the HTML representation of the marginal
  """
  set_matplotlib_formats(gum.config["notebook", "graph_format"])
  return _getMatplotFig(proba2histo(p, scale))


def showProbaMinMax(pmin, pmax, scale=1.0):
  """
  Show a bi-Potential (min,max)

  Parameters
  ----------
  pmin: pyAgrum.Potential
    the min pmarginal to show
  pmax: pyAgrum.Potential
    the max pmarginal to show
  scale: float
    the zoom factor
  """
  fig = probaMinMaxH(pmin, pmax, scale)
  set_matplotlib_formats(gum.config["notebook", "graph_format"])
  plt.show()


def getProbaMinMax(pmin, pmax, scale=1.0) -> str:
  """
  get a bi-Potential (min,max) as html (png/svg) img

  Parameters
  ----------
  pmin: pyAgrum.Potential
    the min pmarginal to show
  pmax: pyAgrum.Potential
    the max pmarginal to show
  scale: float
    the zoom factor

  Returns
  -------
  str
    the HTML representation of the marginal min,max
  """
  set_matplotlib_formats(gum.config["notebook", "graph_format"])
  return _getMatplotFig(probaMinMaxH(pmin, pmax, scale))


def getPosterior(bn, evs, target):
  """
  shortcut for proba2histo(gum.getPosterior(bn,evs,target))

  :param bn: the BayesNet
  :type bn: gum.BayesNet
  :param evs: map of evidence
  :type evs: dict(str->int)
  :param target: name of target variable
  :type target: str
  :return: the matplotlib graph
  """
  fig = proba2histo(gum.getPosterior(bn, evs=evs, target=target))
  plt.close()
  return _getMatplotFig(fig)


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
  f = plt.gcf()

  h = gum.PythonApproximationListener(
    apsc._asIApproximationSchemeConfiguration()
  )
  apsc.setVerbosity(True)
  apsc.listener = h

  def stopper(x):
    IPython.display.clear_output(True)
    plt.title(
      f"{x} \n Time : {apsc.currentTime()}s | Iterations : {apsc.nbrIterations()} | Epsilon : {apsc.epsilon()}"
    )

  def progresser(x, y, z):
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.plot(scale(apsc.history()), 'g')
    IPython.display.clear_output(True)
    IPython.display.display(f)

  h.setWhenStop(stopper)
  h.setWhenProgress(progresser)


def showApproximationScheme(apsc, scale=np.log10):
  if apsc.verbosity():
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.title(
      f"Time : {apsc.currentTime()}s | Iterations : {apsc.nbrIterations()} | Epsilon : {apsc.epsilon()}"
    )
    plt.plot(scale(apsc.history()), 'g')


def showMN(mn, view=None, size=None, nodeColor=None, factorColor=None, edgeWidth=None, edgeColor=None, cmap=None,
           cmapEdge=None
           ):
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
                      edgeColor, cmap, cmapEdge
                      )
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


def showBN(bn, size=None, nodeColor=None, arcWidth=None, arcLabel=None, arcColor=None, cmap=None, cmapArc=None):
  """
  show a Bayesian network

  Parameters
  ----------
    bn : pyAgrum.BayesNet
      the Bayesian network
    size: str
      size of the rendered graph
    nodeColor: dict[Tuple(int,int),float]
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict[Tuple(int,int),float]
      an arcMap of values to be shown as bold arcs
    arcLabel: dict[Tuple(int,int),str]
        an arcMap of labels to be shown next to arcs
    arcColor: dict[Tuple(int,int),float]
      an arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: ColorMap
      color map to show the vals of Nodes
    cmapArc: ColorMap
      color map to show the vals of Arcs
    showMsg: dict
      a nodeMap of values to be shown as tooltip
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return showGraph(BN2dot(bn, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcLabel=arcLabel, arcColor=arcColor, cmapNode=cmap, cmapArc=cmapArc), size)


def showCN(cn, size=None, nodeColor=None, arcWidth=None, arcLabel=None, arcColor=None, cmap=None, cmapArc=None):
  """
  show a credal network

  Parameters
  ----------
    cn : pyAgrum.CredalNet
      the Credal network
    size: str
      size of the rendered graph
    nodeColor: dict[int,float]
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict[Tuple(int,int),float]
      an arcMap of values to be shown as bold arcs
    arcLabel: dict[Tuple(int,int),float]
        an arcMap of labels to be shown next to arcs
    arcColor: dict[Tuple(int,int),float]
      an arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: matplotlib.color.colormap
      color map to show the vals of Nodes
    cmapArc: matplotlib.color.colormap
      color map to show the vals of Arcs
    showMsg : dict[int,str]
      a nodeMap of values to be shown as tooltip

  Returns
  -------
    the graph
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return showGraph(CN2dot(cn, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcLabel=arcLabel, arcColor=arcColor, cmapNode=cmap, cmapArc=cmapArc), size)


def getMN(mn, view=None, size=None, nodeColor=None, factorColor=None, edgeWidth=None, edgeColor=None, cmap=None,
          cmapEdge=None
          ):
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
                      edgeColor, cmap, cmapEdge
                      )
  else:
    dottxt = MN2FactorGraphdot(mn, size, nodeColor, factorColor, cmapNode=cmap)

  return getGraph(dottxt, size)


def getBN(bn, size=None, nodeColor=None, arcWidth=None, arcLabel=None, arcColor=None, cmap=None, cmapArc=None):
  """
  get a HTML string for a Bayesian network

  Parameters
  ----------
    bn : pyAgrum.BayesNet
      the Bayesian network
    size: str
      size of the rendered graph
    nodeColor: dict[Tuple(int,int),float]
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict[Tuple(int,int),float]
      an arcMap of values to be shown as bold arcs
    arcLabel: dict[Tuple(int,int),str]
        an arcMap of labels to be shown next to arcs
    arcColor: dict[Tuple(int,int),float]
      an arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: ColorMap
      color map to show the vals of Nodes
    cmapArc: ColorMap
      color map to show the vals of Arcs
    showMsg: dict
      a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the Bayesian network
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return getGraph(BN2dot(bn, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcLabel=arcLabel,arcColor=arcColor, cmapNode=cmap, cmapArc=cmapArc), size)


def getCN(cn, size=None, nodeColor=None, arcWidth=None, arcLabel=None, arcColor=None, cmap=None, cmapArc=None):
  """
  get a HTML string for a credal network

  Parameters
  ----------
    cn : pyAgrum.CredalNet
      the Credal network
    size: str
      size of the rendered graph
    nodeColor: dict[int,float]
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict[Tuple(int,int),float]
      an arcMap of values to be shown as bold arcs
    arcLabel: dict[Tuple(int,int),float]
        an arcMap of labels to be shown next to arcs
    arcColor: dict[Tuple(int,int),float]
      an arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: matplotlib.color.colormap
      color map to show the vals of Nodes
    cmapArc: matplotlib.color.colormap
      color map to show the vals of Arcs
    showMsg : dict[int,str]
      a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the Credal Network
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return getGraph(CN2dot(cn, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcLabel=arcLabel,arcColor=arcColor, cmapNode=cmap, cmapArc=cmapArc), size)


def showInference(model, **kwargs):
  """
  show pydot graph for an inference in a notebook

  :param GraphicalModel model: the model in which to infer (pyAgrum.BayesNet, pyAgrum.MarkovNet or pyAgrum.InfluenceDiagram)
  :param gum.Inference engine: inference algorithm used. If None, gum.LazyPropagation will be used for BayesNet, gum.ShaferShenoy for gum.MarkovNet and gum.ShaferShenoyLIMIDInference for gum.InfluenceDiagram.
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
  if "size" in kwargs:
    size = kwargs['size']
  else:
    size = gum.config["notebook", "default_graph_inference_size"]

  showGraph(prepareShowInference(model, **kwargs), size)


def getInference(model, **kwargs):
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
  if "size" in kwargs:
    size = kwargs['size']
  else:
    size = gum.config["notebook", "default_graph_inference_size"]

  grinf = prepareShowInference(model, **kwargs)
  return getGraph(grinf, size)


def _reprPotential(pot, digits=None, withColors=None, varnames=None, asString=False):
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
  from fractions import Fraction

  r0, g0, b0 = gumcols.hex2rgb(gum.config['notebook', 'potential_color_0'])
  r1, g1, b1 = gumcols.hex2rgb(gum.config['notebook', 'potential_color_1'])

  if digits is None:
    digits = gum.config.asInt['notebook', 'potential_visible_digits']

  if withColors is None:
    withColors = gum.config.asBool["notebook", "potential_with_colors"]

  with_fraction = gum.config['notebook', 'potential_with_fraction'] == "True"
  if with_fraction:
    fraction_limit = int(gum.config['notebook', 'potential_fraction_limit'])
    fraction_round_error = float(
        gum.config['notebook', 'potential_fraction_round_error'])
    fraction_with_latex = gum.config['notebook',
                                     'potential_fraction_with_latex'] == "True"

  def _rgb(r, g, b):
    return '#%02x%02x%02x' % (r, g, b)

  def _mkCell(val):
    s = "<td style='"
    if withColors and (0 <= val <= 1):
      r = int(r0 + val * (r1 - r0))
      g = int(g0 + val * (g1 - g0))
      b = int(b0 + val * (b1 - b0))

      tx = gumcols.rgb2brightness(r, g, b)

      s += "color:" + tx + ";background-color:" + _rgb(r, g, b) + ";"

    str_val = ""
    if with_fraction:
      frac_val = Fraction(val).limit_denominator(fraction_limit)
      val_app = frac_val.numerator / frac_val.denominator
      if abs(val_app - val) < fraction_round_error:
        str_val = "text-align:center;'>"
        if fraction_with_latex:
          str_val += "$$"
          if frac_val.denominator>1:
            str_val += f"\\frac{{{frac_val.numerator}}}{{{frac_val.denominator}}}"
          else:
            str_val += f"{frac_val.numerator}"
          str_val += "$$"
        else:
          str_val += f"{frac_val}"
        str_val += "</td>"
    if str_val == "":
      str_val = f"text-align:right;'>{val:.{digits}f}</td>"

    return s + str_val

  html = list()
  html.append('<table style="border:1px solid black;">')
  if pot.empty():
    html.append(
      "<tr><th>&nbsp;</th></tr>"
    )
    html.append("<tr>" + _mkCell(pot.get(gum.Instantiation())) + "</tr>")
  else:
    if varnames is not None and len(varnames) != pot.nbrDim():
      raise ValueError(
        f"varnames contains {len(varnames)} value(s) instead of the needed {pot.nbrDim()} value(s)."
      )

    nparents = pot.nbrDim() - 1
    var = pot.variable(0)
    varname = var.name() if varnames == None else varnames[0]

    # first line
    if nparents > 0:
      html.append(f"""<tr><th colspan='{nparents}'></th>
      <th colspan='{var.domainSize()}' style='border:1px solid black;color:black;background-color:#808080;'><center>{varname}</center>
      </th></tr>"""
                  )
    else:
      html.append(f"""<tr style='border:1px solid black;color:black;background-color:#808080'>
      <th colspan='{var.domainSize()}'><center>{varname}</center></th></tr>"""
                  )

    # second line
    s = "<tr>"
    if nparents > 0:
      # parents order
      if gum.config["notebook", "potential_parent_values"] == "revmerge":
        pmin, pmax, pinc = nparents - 1, 0 - 1, -1
      else:
        pmin, pmax, pinc = 0, nparents, 1

      if varnames is None:
        varnames = list(reversed(pot.names))
      for par in range(pmin, pmax, pinc):
        parent = varnames[par]
        s += f"<th style='border:1px solid black;color:black;background-color:#808080'><center>{parent}</center></th>"

    for label in var.labels():
      s += f"""<th style='border:1px solid black;border-bottom-style: double;color:black;background-color:#BBBBBB'>
      <center>{label}</center></th>"""
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
          s += f"<th style='border:1px solid black;color:black;background-color:#BBBBBB'><center>{label}</center></th>"
        else:
          if sum([inst.val(i) for i in range(1, par)]) == 0:
            s += f"""<th style='border:1px solid black;color:black;background-color:#BBBBBB;' rowspan = '{offset[par]}'>
            <center>{label}</center></th>"""
      for j in range(pot.variable(0).domainSize()):
        s += _mkCell(pot.get(inst))
        inst.inc()
      s += "</tr>"
      html.append(s)

  html.append("</table>")

  if asString:
    return "\n".join(html)
  else:
    return IPython.display.HTML("".join(html))


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

  Parameters
  ----------
  pot : gum.Potential
    the potential to show
  digits : int
    number of digits to show
  withColors : bool
    bgcolor for proba cells or not
  varnames : List[str]
    the aliases for variables name in the table
  """
  if withColors is None:
    withColors = gum.config.asBool["notebook", "potential_with_colors"]

  if withColors:
    withColors = __isKindOfProba(pot)

  s = _reprPotential(pot, digits, withColors, varnames, asString=False)
  IPython.display.display(s)


def getPotential(pot, digits=None, withColors=None, varnames=None):
  """
  return a HTML string of a gum.Potential as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  Parameters
  ----------
  pot : gum.Potential
    the potential to show
  digits : int
    number of digits to show
  withColors : bool
    bgcolor for proba cells or not
  varnames : List[str]
    the aliases for variables name in the table

  Returns
  -------
  str
    the html representation of the Potential (as a string)
  """
  if withColors is None:
    withColors=gum.config.asBool["notebook", "potential_with_colors"]

  if withColors:
    withColors = __isKindOfProba(pot)

  return _reprPotential(pot, digits, withColors, varnames, asString=True)


def showCPTs(bn):
  flow.clear()
  for i in bn.names():
    flow.add_html(getPotential(bn.cpt(i)))
  flow.display()


def getSideBySide(*args, **kwargs):
  """
  create an HTML table for args as string (using string, _repr_html_() or str())

  :param args: HMTL fragments as string arg, arg._repr_html_() or str(arg)
  :param captions: list of strings (optional)
  :param valign: vertical position in the row (top|middle|bottom, middle by default)
  :param ncols: number of columns (infinite by default)
  :return: a string representing the table
  """
  vals = {'captions', 'valign', 'ncols'}
  if not set(kwargs.keys()).issubset(vals):
    raise TypeError(
        f"sideBySide() got unexpected keyword argument(s) : '{set(kwargs.keys()).difference(vals)}'")

  if 'captions' in kwargs:
    captions = kwargs['captions']
  else:
    captions = None

  if 'valign' in kwargs and kwargs['valign'] in ['top', 'middle', 'bottom']:
    v_align = f"vertical-align:{kwargs['valign']};"
  else:
    v_align = f"vertical-align:middle;"

  ncols = None
  if 'ncols' in kwargs:
    ncols = int(kwargs['ncols'])
    if ncols < 1:
      ncols = 1

  def reprHTML(s):
    if isinstance(s, str):
      return s
    elif hasattr(s, '_repr_html_'):
      return s._repr_html_()
    else:
      return str(s)

  s = '<table style="border-style: hidden; border-collapse: collapse;" width="100%"><tr>'
  for i in range(len(args)):
    s += f'<td style="border-top:hidden;border-bottom:hidden;{v_align}"><div align="center" style="{v_align}">'
    s += reprHTML(args[i])
    if captions is not None:
      s += f'<br><small><i>{captions[i]}</i></small>'
    s += '</div></td>'
    if ncols is not None and (i + 1) % ncols == 0:
      s += '</tr><tr>'
  s += '</tr></table>'
  return s


def sideBySide(*args, **kwargs):
  """
  display side by side args as HMTL fragment (using string, _repr_html_() or str())

  :param args: HMTL fragments as string arg, arg._repr_html_() or str(arg)
  :param captions: list of strings (captions)
  """
  IPython.display.display(IPython.display.HTML(getSideBySide(*args, **kwargs)))


def getInferenceEngine(ie, inferenceCaption):
  """
  display an inference as a BN+ lists of hard/soft evidence and list of targets

  :param gum.InferenceEngine ie: inference engine
  :param string inferenceCaption: caption for the inference

  """
  t = '<div align="left"><ul>'
  if ie.nbrHardEvidence() > 0:
    t += "<li><b>hard evidence</b><br/>"
    t += ", ".join([ie.BN().variable(n).name()
                    for n in ie.hardEvidenceNodes()]
                   )
    t += "</li>"
  if ie.nbrSoftEvidence() > 0:
    t += "<li><b>soft evidence</b><br/>"
    t += ", ".join([ie.BN().variable(n).name()
                    for n in ie.softEvidenceNodes()]
                   )
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
                                    for n in ns]
                                   ))
                      + ']' for ns in ie.jointTargets()]
                     )
      t += "</li>"
  t += '</ul></div>'
  return getSideBySide(getBN(ie.BN()), t, captions=[inferenceCaption, "Evidence and targets"])


def getJT(jt, size=None):
  if gum.config.asBool["notebook", "junctiontree_with_names"]:
    def cliqlabels(c):
      labels = ",".join(
        sorted([model.variable(n).name() for n in jt.clique(c)])
      )
      return f"({c}):{labels}"

    def cliqnames(c):
      return "-".join(sorted([model.variable(n).name() for n in jt.clique(c)]))

    def seplabels(c1, c2):
      return ",".join(
        sorted([model.variable(n).name() for n in jt.separator(c1, c2)])
      )

    def sepnames(c1, c2):
      return cliqnames(c1) + '+' + cliqnames(c2)
  else:
    def cliqlabels(c):
      ids = ",".join([str(n) for n in sorted(jt.clique(c))])
      return f"({c}):{ids}"

    def cliqnames(c):
      return "-".join([str(n) for n in sorted(jt.clique(c))])

    def seplabels(c1, c2):
      return ",".join(
        [str(n) for n in sorted(jt.separator(c1, c2))]
      )

    def sepnames(c1, c2):
      return cliqnames(c1) + '^' + cliqnames(c2)

  model = jt._engine._model
  name = model.propertyWithDefault(
    "name", str(type(model)).split(".")[-1][:-2]
  )
  graph = dot.Dot(graph_type='graph', graph_name=name, bgcolor="transparent")
  for c in jt.nodes():
    graph.add_node(dot.Node('"' + cliqnames(c) + '"',
                            label='"' + cliqlabels(c) + '"',
                            style="filled",
                            fillcolor=gum.config["notebook",
                                                 "junctiontree_clique_bgcolor"],
                            fontcolor=gum.config["notebook",
                                                 "junctiontree_clique_fgcolor"],
                            fontsize=gum.config["notebook",
                                                "junctiontree_clique_fontsize"]
                            )
                   )
  for c1, c2 in jt.edges():
    graph.add_node(dot.Node('"' + sepnames(c1, c2) + '"',
                            label='"' + seplabels(c1, c2) + '"',
                            style="filled",
                            shape="box", width="0", height="0", margin="0.02",
                            fillcolor=gum.config["notebook",
                                                 "junctiontree_separator_bgcolor"],
                            fontcolor=gum.config["notebook",
                                                 "junctiontree_separator_fgcolor"],
                            fontsize=gum.config["notebook",
                                                "junctiontree_separator_fontsize"]
                            )
                   )
  for c1, c2 in jt.edges():
    graph.add_edge(dot.Edge('"' + cliqnames(c1) +
                            '"', '"' + sepnames(c1, c2) + '"'
                            )
                   )
    graph.add_edge(dot.Edge('"' + sepnames(c1, c2) +
                            '"', '"' + cliqnames(c2) + '"'
                            )
                   )

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


def show(model, **kwargs):
  """
  propose a (visual) representation of a model in a notebook

  :param model: the model to show (pyAgrum.BayesNet, pyAgrum.MarkovNet, pyAgrum.InfluenceDiagram or pyAgrum.Potential) or a dot string, or a `pydot.Dot` or even just an object with a method `toDot()`.

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
    showDot(model.toDot(), **kwargs)
  elif isinstance(model, dot.Dot):
    showGraph(model, **kwargs)
  else:
    raise gum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovNet, Influence Diagram or Potential or ..."
    )


def _update_config_notebooks():
  # hook to control some parameters for notebook when config changes
  mpl.rcParams['figure.facecolor'] = gum.config["notebook", "figure_facecolor"]
  set_matplotlib_formats(gum.config["notebook", "graph_format"])

# check if an instance of ipython exists
try:
  get_ipython
except NameError as e:
  import warnings

  warnings.warn("""
  ** pyAgrum.lib.notebook has to be imported from an IPython's instance (mainly notebook).
  """
                )
else:
  def map(self, scaleClique: float = None, scaleSep: float = None, lenEdge: float = None, colorClique: str = None,
          colorSep: str = None) -> dot.Dot:
    """
    show the map of the junction tree.

    Parameters
    ----------
    scaleClique: float
      the scale for the size of the clique nodes (depending on the number of nodes in the clique)
    scaleSep: float
      the scale for the size of the separator nodes (depending on the number of nodes in the clique)
    lenEdge: float
      the desired length of edges
    colorClique: str
      color for the clique nodes
    colorSep: str
      color for the separator nodes
    """
    if scaleClique is None:
      scaleClique = float(
          gum.config["notebook", "junctiontree_map_cliquescale"])
    if scaleSep is None:
      scaleSep = float(gum.config["notebook", "junctiontree_map_sepscale"])
    if lenEdge is None:
      lenEdge = float(gum.config["notebook", "junctiontree_map_edgelen"])
    if colorClique is None:
      colorClique = gum.config["notebook", "junctiontree_clique_bgcolor"]
    if colorSep is None:
      colorSep = gum.config["notebook", "junctiontree_separator_bgcolor"]
    return _from_dotstring(self.__map_str__(scaleClique, scaleSep, lenEdge,colorClique,colorSep))

  setattr(gum.CliqueGraph, "map", map)

  gum.config.add_hook(_update_config_notebooks)
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
    self, "Lazy Propagation on this BN"
  )

  gum.UndiGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.DiGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.MixedGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.DAG._repr_html_ = lambda self: getDot(self.toDot())
  gum.EssentialGraph._repr_html_ = lambda self: getDot(self.toDot())
  gum.MarkovBlanket._repr_html_ = lambda self: getDot(self.toDot())

  dot.Dot._repr_html_ = lambda self: getGraph(self)
