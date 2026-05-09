############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2026 by                                             #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#                                                                          #
#   The aGrUM/pyAgrum library is free software; you can redistribute it    #
#   and/or modify it under the terms of either :                           #
#                                                                          #
#    - the GNU Lesser General Public License as published by               #
#      the Free Software Foundation, either version 3 of the License,      #
#      or (at your option) any later version,                              #
#    - the MIT license (MIT),                                              #
#    - or both in dual license, as here.                                   #
#                                                                          #
#   (see https://agrum.gitlab.io/articles/dual-licenses-lgplv3mit.html)    #
#                                                                          #
#   This aGrUM/pyAgrum library is distributed in the hope that it will be  #
#   useful, but WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED,          #
#   INCLUDING BUT NOT LIMITED TO THE WARRANTIES MERCHANTABILITY or FITNESS #
#   FOR A PARTICULAR PURPOSE  AND NONINFRINGEMENT. IN NO EVENT SHALL THE   #
#   AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER #
#   LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,        #
#   ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR  #
#   OTHER DEALINGS IN THE SOFTWARE.                                        #
#                                                                          #
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2026                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
tools for BN in jupyter notebook
"""

import time
import warnings

# fix DeprecationWarning of base64.encodestring()
try:
  from base64 import encodebytes
except ImportError:  # 3+
  from base64 import encodestring as encodebytes

import io
import base64

import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib_inline.backend_inline import set_matplotlib_formats

import numpy as np
import pydot as dot

try:
  dot.call_graphviz("dot", ["--help"], ".")
except FileNotFoundError:
  print("""Graphviz is not installed. 
  Please install this program in order to visualize graphical models in pyagrum. 
  See https://graphviz.org/download/""")

import IPython.core.display
import IPython.core.pylabtools
import IPython.display

import pyagrum
from pyagrum.lib.bn2graph import BN2dot
from pyagrum.lib.cn2graph import CN2dot
from pyagrum.lib.id2graph import ID2dot
from pyagrum.lib.mrf2graph import MRF2UGdot, MRF2FactorGraphdot

from pyagrum.lib.bn_vs_bn import graphDiff
from pyagrum.lib.proba_histogram import proba2histo, probaMinMaxH
from pyagrum.lib.image import prepareShowInference, prepareLinksForSVG

import pyagrum.lib._colors as gumcols


class FlowLayout:
  """ "
  A class / object to display plots in a horizontal / flow layout below a cell

  based on : https://stackoverflow.com/questions/21754976/ipython-notebook-arrange-plots-horizontally
  """

  def __init__(self) -> None:
    self.clear()

  def clear(self) -> "FlowLayout":
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
      border: {pyagrum.config.asInt["notebook", "flow_border_width"]}px solid {pyagrum.config["notebook", "flow_border_color"]};  
      valign:middle;
      background-color: {pyagrum.config["notebook", "flow_background_color"]};
      }}
      </style>
      """
    return self

  def _getCaption(self, caption: str) -> str:
    if caption == "":
      return ""
    return f"<br><center><small><em>{caption}</em></small></center>"

  def add_html(self, html: str, caption: str | None = None, title: str | None = None) -> "FlowLayout":
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

  def add_separator(self, size: int = 3) -> "FlowLayout":
    """
    add a (poor) separation between elements in a row
    """
    self.add_html("&nbsp;" * size)
    return self

  def add_plot(self, oAxes: "mpl.axes.Axes", caption: str | None = None, title: str | None = None) -> "FlowLayout":
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
    self.sHtml += (
      f'<div class="floating-box"><img src="data:image/png;base64,{sB64Img}\n">{self._getCaption(cap)}</div>'
    )
    plt.close()
    return self

  def new_line(self) -> "FlowLayout":
    """
    add a breakline (a new row)
    """
    self.sHtml += "<br/>"
    return self

  def html(self) -> "IPython.display.HTML":
    """
    Returns its content as HTML object
    """
    return IPython.display.HTML(self.sHtml)

  def display(self) -> None:
    """
    Display the accumulated HTML
    """
    IPython.display.display(self.html())
    self.clear()

  def add(self, obj, caption: str | None = None, title: str | None = None) -> "FlowLayout":
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

  def row(self, *args, captions: list[str] | None = None) -> "FlowLayout":
    """
    Create a row with flow with the same syntax as `pyagrum.lib.notebook.sideBySide`.
    """
    self.clear()
    for i, arg in enumerate(args):
      if captions is None:
        self.add(arg)
      else:
        self.add(arg, captions[i])
    return self

  def _repr_html_(self) -> str:
    return self.html().data


flow = FlowLayout()


def configuration() -> None:
  """
  Display the collection of dependance and versions
  """
  from collections import OrderedDict
  import sys
  import os

  packages = OrderedDict()
  packages["OS"] = f"{os.name} [{sys.platform}]"
  packages["Python"] = sys.version
  packages["IPython"] = IPython.__version__
  packages["Matplotlib"] = mpl.__version__
  packages["Numpy"] = np.__version__
  packages["pyDot"] = dot.__version__
  packages["pyAgrum"] = pyagrum.__version__

  res = "<table><tr><th>Library</th><th>Version</th></tr>"

  for name in packages:
    res += f"<tr><td>{name}</td><td>{packages[name]}</td></tr>"

  res += "</table><div align='right'><small>{}</small></div>".format(time.strftime("%a %b %d %H:%M:%S %Y %Z"))

  IPython.display.display(IPython.display.HTML(res))


def _reprGraph(gr: dot.Dot, size: float | str | None, asString: bool, graph_format: str | None = None) -> str | None:
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
  graph_format: str
    "svg" or "png" ?

  Returns
  -------
  str | None
    return the HTML representation as a str or display the graph
  """
  gumcols.prepareDot(gr, size=size)

  if graph_format is None:
    graph_format = pyagrum.config["notebook", "graph_format"]

  if graph_format == "svg":
    gsvg = IPython.display.SVG(prepareLinksForSVG(gr.create_svg(encoding="utf-8").decode("utf-8")))
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
  return None


def showGraph(gr: dot.Dot, size: float | str | None = None) -> None:
  """
  show a pydot graph in a notebook

  Parameters
  ----------
  gr: pydot.Dot
    the graph
  size: float|str
    the size (for graphviz) of the rendered graph
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  return _reprGraph(gr, size, asString=False)


def getGraph(gr: dot.Dot, size: float | str | None = None) -> str:
  """
  get an HTML representation of a pydot graph

  Parameters
  ----------
  gr: pydot.Dot
    the graph
  size: float|str
    the size (for graphviz) of the rendered graph

  Returns
  -------
  str
    the HTML representation of the graph (as a string)
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  return _reprGraph(gr, size, asString=True)


def _from_dotstring(dotstring: str) -> dot.Dot:
  _graphs = dot.graph_from_dot_data(dotstring)
  assert _graphs is not None
  return _graphs[0]


def showDot(dotstring: str, size: float | str | None = None) -> None:
  """
  show a dot string as a graph

  Parameters
  ----------
  dotstring:str
    the dot string
  size: float|str
    size (for graphviz) of the rendered graph
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]
  showGraph(_from_dotstring(dotstring), size)


def getDot(dotstring: str, size: float | str | None = None) -> str:
  """
  get an HTML representation of a dot string

  Parameters
  ----------
  dotstring:str
    the dot string
  size: float|str
    size (for graphviz) of the rendered graph

  Returns
  -------
    the HTML representation of the dot string
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  return getGraph(_from_dotstring(dotstring), size)


def getBNDiff(
  bn1: pyagrum.BayesNet, bn2: pyagrum.BayesNet, size: float | str | None = None, noStyle: bool = False
) -> str:
  """
  get a HTML string representation of a graphical diff between the arcs of _bn1 (reference) with those of _bn2.

  if `noStyle` is False use 4 styles (fixed in pyagrum.config) :
    - the arc is common for both
    - the arc is common but inverted in `bn2`
    - the arc is added in `bn2`
    - the arc is removed in `bn2`

  Parameters
  ----------
  bn1: pyagrum.BayesNet
    the reference
  bn2: pyagrum.BayesNet
    the compared one
  size: float|str
    size (for graphviz) of the rendered graph
  noStyle: bool
    with style or not.

  Returns
  -------
  str
    the HTML representation of the comparison
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  return getGraph(graphDiff(bn1, bn2, noStyle), size)


def showBNDiff(
  bn1: pyagrum.BayesNet, bn2: pyagrum.BayesNet, size: float | str | None = None, noStyle: bool = False
) -> None:
  """
  show a graphical diff between the arcs of _bn1 (reference) with those of _bn2.

  if `noStyle` is False use 4 styles (fixed in pyagrum.config) :
    - the arc is common for both
    - the arc is common but inverted in `bn2`
    - the arc is added in `bn2`
    - the arc is removed in `bn2`

  Parameters
  ----------
  bn1: pyagrum.BayesNet
    the reference
  bn2: pyagrum.BayesNet
    the compared one
  size: float|str
    size (for graphviz) of the rendered graph
  noStyle: bool
    with style or not.
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  showGraph(graphDiff(bn1, bn2, noStyle), size)


def getJunctionTreeMap(
  bn: pyagrum.BayesNet,
  size: str | None = None,
  scaleClique: float | None = None,
  scaleSep: float | None = None,
  lenEdge: float | None = None,
  colorClique: str | None = None,
  colorSep: str | None = None,
) -> str:
  """
  Return a representation of the map of the junction tree of a Bayesian network

  Parameters
  ----------
  bn: pyagrum.BayesNet
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
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  if size is None:
    size = pyagrum.config["notebook", "junctiontree_map_size"]
  return getGraph(jt.map(scaleClique, scaleSep, lenEdge, colorClique, colorSep), size)


def showJunctionTreeMap(
  bn: pyagrum.BayesNet,
  size: str | None = None,
  scaleClique: float | None = None,
  scaleSep: float | None = None,
  lenEdge: float | None = None,
  colorClique: str | None = None,
  colorSep: str | None = None,
) -> None:
  """
  Show the map of the junction tree of a Bayesian network

  Parameters
  ----------
  bn: pyagrum.BayesNet
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
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  if size is None:
    size = pyagrum.config["notebook", "junctiontree_map_size"]
  showGraph(jt.map(scaleClique, scaleSep, lenEdge, colorClique, colorSep), size)


def showJunctionTree(bn: pyagrum.BayesNet, withNames: bool = True, size: float | str | None = None) -> None:
  """
  Show a junction tree of a Bayesian network

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  withNames: bool
    names or id in the graph (names can created very large nodes)
  size: float|str
    size (for graphviz) of the rendered graph
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  jt._engine = jtg
  jtg._model = bn

  if withNames:
    showDot(jt.toDotWithNames(bn), size)
  else:
    showDot(jt.toDot(), size)


def getJunctionTree(bn: pyagrum.BayesNet, withNames: bool = True, size: float | str | None = None) -> str:
  """
  get a HTML string for a junction tree (more specifically a join tree)

  Parameters
  ----------
    bn: "pyagrum.BayesNet"
     the Bayesian network
    withNames: Boolean
     display the variable names or the node id in the clique
    size: str
     size (for graphviz) of the rendered graph
  Returns
  -------
    str
      the HTML representation of the graph
  """
  if size is None:
    size = pyagrum.config["notebook", "junctiontree_graph_size"]

  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)

  jt._engine = jtg
  jtg._model = bn

  if withNames:
    return getDot(jt.toDotWithNames(bn), size)
  else:
    return getDot(jt.toDot(), size)


def showProba(p: pyagrum.Tensor, scale: float | None = None) -> None:
  """
  Show a mono-dim Tensor (a marginal)

  Parameters
  ----------
  p: pyagrum.Tensor
    the marginal to show
  scale: float
    the zoom factor
  """
  _ = proba2histo(p, scale)
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
  plt.show()


def _getMatplotFig(fig: "mpl.figure.Figure") -> str:
  bio = io.BytesIO()  # bytes buffer for the plot
  # .canvas.print_png(bio)  # make a png of the plot in the buffer
  fig.savefig(bio, format="png", bbox_inches="tight")

  # encode the bytes as string using base 64
  sB64Img = base64.b64encode(bio.getvalue()).decode()
  res = f'<img src="data:image/png;base64,{sB64Img}\n">'
  plt.close()
  return res


def getProba(p: pyagrum.Tensor, scale: float | None = None) -> str:
  """
  get a mono-dim Tensor as html (png/svg) image

  Parameters
  ----------
  p: pyagrum.Tensor
    the marginal to show
  scale: float
    the zoom factor

  Returns
  -------
  str
    the HTML representation of the marginal
  """
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
  # return _getMatplotFig(proba2histo(p, scale))
  fig = proba2histo(p, scale)
  plt.close()
  return _getMatplotFig(fig)


def showProbaMinMax(pmin: pyagrum.Tensor, pmax: pyagrum.Tensor, scale: float = 1.0) -> None:
  """
  Show a bi-Tensor (min,max)

  Parameters
  ----------
  pmin: pyagrum.Tensor
    the min pmarginal to show
  pmax: pyagrum.Tensor
    the max pmarginal to show
  scale: float
    the zoom factor
  """
  _ = probaMinMaxH(pmin, pmax, scale)
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
  plt.show()


def getProbaMinMax(pmin: pyagrum.Tensor, pmax: pyagrum.Tensor, scale: float = 1.0) -> str:
  """
  get a bi-Tensor (min,max) as html (png/svg) img

  Parameters
  ----------
  pmin: pyagrum.Tensor
    the min pmarginal to show
  pmax: pyagrum.Tensor
    the max pmarginal to show
  scale: float
    the zoom factor

  Returns
  -------
  str
    the HTML representation of the marginal min,max
  """
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
  return _getMatplotFig(probaMinMaxH(pmin, pmax, scale))


def getPosterior(bn: pyagrum.BayesNet, evs: dict, target: str | int) -> str:
  """
  shortcut for proba2histo(pyagrum.getPosterior(bn,evs,target))

  Parameters
  ----------
  bn: "pyagrum.BayesNet"
    the BayesNet
  evs: dict[str|int:int|str|list[float]]
    map of evidence
  target: str|int
    name of target variable

  Returns
  ------
    the matplotlib graph
  """
  fig = proba2histo(pyagrum.getPosterior(bn, evs=evs, target=target))
  plt.close()
  return _getMatplotFig(fig)


def showPosterior(bn: pyagrum.BayesNet, evs: dict, target: str | int) -> None:
  """
  shortcut for showProba(pyagrum.getPosterior(bn,evs,target))

  Parameters
  ----------
  bn: "pyagrum.BayesNet"
    the BayesNet
  evs: dict[str|int:int|str|list[float]]
    map of evidence
  target: str|int
    name of target variable
  """
  showProba(pyagrum.getPosterior(bn, evs=evs, target=target))


def animApproximationScheme(apsc: pyagrum.ApproximationScheme, scale=np.log10) -> None:
  """
  show an animated version of an approximation algorithm

  Parameters
  ----------
  apsc
    the approximation algorithm
  scale
    a function to apply to the figure
  """
  f = plt.gcf()

  h = pyagrum.PythonApproximationListener(apsc._asIApproximationSchemeConfiguration())
  apsc.setVerbosity(True)
  apsc.listener = h

  def stopper(x):
    IPython.display.clear_output(True)
    plt.title(f"{x} \n Time : {apsc.currentTime()}s | Iterations : {apsc.nbrIterations()} | Epsilon : {apsc.epsilon()}")

  def progresser(x, y, z):
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.plot(scale(apsc.history()), "g")
    IPython.display.clear_output(True)
    IPython.display.display(f)

  h.setWhenStop(stopper)
  h.setWhenProgress(progresser)


def showApproximationScheme(apsc: pyagrum.ApproximationScheme, scale=np.log10) -> None:
  """
  show the state of an approximation algorithm

  Parameters
  ----------
  apsc
    the approximation algorithm
  scale
    a function to apply to the figure
  """
  if apsc.verbosity():
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.title(f"Time : {apsc.currentTime()}s | Iterations : {apsc.nbrIterations()} | Epsilon : {apsc.epsilon()}")
    plt.plot(scale(apsc.history()), "g")


def showMRF(
  mrf: pyagrum.MarkovRandomField,
  view: str | None = None,
  size: float | str | None = None,
  nodeColor=None,
  factorColor=None,
  edgeWidth=None,
  edgeColor=None,
  cmapNode=None,
  cmapEdge=None,
) -> None:
  """
  show a Markov random field

  Parameters
  ----------
  mrf : "pyagrum.MarkovRandomField"
   the Markov random field
  view : str
   'graph' | 'factorgraph’ | None (default)
  size : str
   size (for graphviz) of the rendered graph
  nodeColor: dict[int,float]
   a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor: dict[int,float]
   a function returning a value (between 0 and 1) to be shown as a color of factor. (used when view='factorgraph')
  edgeWidth: dict[tuple[int,int],float]
   a edgeMap of values to be shown as width of edges  (used when view='graph')
  edgeColor: dict[int,float]
   a edgeMap of values (between 0 and 1) to be shown as color of edges (used when view='graph')
  cmapNode: dict[tuple[int,int],float]
   color map to show the colors (if cmapEdge is None, this color map is used also for edges)
  cmapEdge: "matplotlib.ColorMap"
   color map to show the edge color if distinction is needed

  Returns
  ------
   the graph
  """
  if view is None:
    view = pyagrum.config["notebook", "default_markovrandomfield_view"]

  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapEdge is None:
    cmapEdge = cmapNode

  if view == "graph":
    dottxt = MRF2UGdot(
      mrf, size, nodeColor=nodeColor, edgeWidth=edgeWidth, edgeColor=edgeColor, cmapNode=cmapNode, cmapEdge=cmapEdge
    )
  else:
    dottxt = MRF2FactorGraphdot(mrf, size, nodeColor=nodeColor, factorColor=factorColor, cmapNode=cmapNode)

  return showGraph(dottxt, size)


def showInfluenceDiagram(diag: pyagrum.InfluenceDiagram, size: float | str | None = None) -> None:
  """
  show an influence diagram as a graph

  Parameters
  ----------
  diag : "pyagrum.InfluenceDiagram"
    the influence diagram
  size : str
    size (for graphviz) of the rendered graph

  Returns
  -------
    the representation of the influence diagram
  """
  if size is None:
    size = pyagrum.config["influenceDiagram", "default_id_size"]

  return showGraph(ID2dot(diag), size)


def getInfluenceDiagram(diag: pyagrum.InfluenceDiagram, size: float | str | None = None) -> str:
  """
  get a HTML string for an influence diagram as a graph

  Parameters
  ----------
  diag : "pyagrum.InfluenceDiagram"
    the influence diagram
  size : str
    size (for graphviz) of the rendered graph

  Returns
  -------
  str
    the HTML representation of the influence diagram
  """
  if size is None:
    size = pyagrum.config["influenceDiagram", "default_id_size"]

  return getGraph(ID2dot(diag), size)


def showBN(
  bn: pyagrum.BayesNet,
  size: float | str | None = None,
  nodeColor=None,
  arcWidth=None,
  arcLabel=None,
  arcColor=None,
  cmapNode=None,
  cmapArc=None,
) -> None:
  """
  show a Bayesian network

  Parameters
  ----------
  bn : pyagrum.BayesNet
    the Bayesian network
  size: str
    size (for graphviz) of the rendered graph
  nodeColor: dict[Tuple(int,int),float]
    a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  arcWidth: dict[Tuple(int,int),float]
    an arcMap of values to be shown as bold arcs
  arcLabel: dict[Tuple(int,int),str]
      an arcMap of labels to be shown next to arcs
  arcColor: dict[Tuple(int,int),float]
    an arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode: ColorMap
    color map to show the vals of Nodes ( (if cmapEdge is None, this color map is used also for edges)
  cmapArc: ColorMap
    color map to show the vals of Arcs
  showMsg: dict
    a nodeMap of values to be shown as tooltip
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmapNode

  return showGraph(
    BN2dot(
      bn,
      size=size,
      nodeColor=nodeColor,
      arcWidth=arcWidth,
      arcLabel=arcLabel,
      arcColor=arcColor,
      cmapNode=cmapNode,
      cmapArc=cmapArc,
    ),
    size,
  )


def showCN(
  cn: pyagrum.CredalNet,
  size: float | str | None = None,
  nodeColor=None,
  arcWidth=None,
  arcLabel=None,
  arcColor=None,
  cmapNode=None,
  cmapArc=None,
) -> None:
  """
  show a credal network

  Parameters
  ----------
  cn : pyagrum.CredalNet
    the Credal network
  size: str
    size (for graphviz) of the rendered graph
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
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmapNode

  return showGraph(
    CN2dot(
      cn,
      size=size,
      nodeColor=nodeColor,
      arcWidth=arcWidth,
      arcLabel=arcLabel,
      arcColor=arcColor,
      cmapNode=cmapNode,
      cmapArc=cmapArc,
    ),
    size,
  )


def getMRF(
  mrf: pyagrum.MarkovRandomField,
  view: str | None = None,
  size: float | str | None = None,
  nodeColor=None,
  factorColor=None,
  edgeWidth=None,
  edgeColor=None,
  cmapNode=None,
  cmapEdge=None,
) -> str:
  """
  get an HTML string for a Markov random field

  Parameters
  ----------
  mrf : "pyagrum.MarkovRandomField"
    the Markov random field
  view: str
    'graph' | 'factorgraph’ | None (default)
  size: str
    size (for graphviz) of the rendered graph
  nodeColor: dict[str,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor: dict[str,float]
    a function returning a value (beeween 0 and 1) to be shown as a color of factor. (used when view='factorgraph')
  edgeWidth: dict[tuple[str,str],float]
    a edgeMap of values to be shown as width of edges  (used when view='graph')
  edgeColor: dict[tuple[str,str],float]
    a edgeMap of values (between 0 and 1) to be shown as color of edges (used when view='graph')
  cmapNode: matplotlib.ColorMap
    color map to show the colors (if cmapEdge is None, cmapNode is used for edges)
  cmapEdge: matplotlib.ColorMap
    color map to show the edge color if distinction is needed

  Returns
  -------
    the graph
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapEdge is None:
    cmapEdge = cmapNode

  if view is None:
    view = pyagrum.config["notebook", "default_markovrandomfield_view"]

  if view == "graph":
    dottxt = MRF2UGdot(mrf, size, nodeColor, edgeWidth, edgeColor, cmapNode, cmapEdge)
  else:
    dottxt = MRF2FactorGraphdot(mrf, size, nodeColor, factorColor, cmapNode=cmapNode)

  return getGraph(dottxt, size)


def getBN(
  bn: pyagrum.BayesNet,
  size: float | str | None = None,
  nodeColor=None,
  arcWidth=None,
  arcLabel=None,
  arcColor=None,
  cmapNode=None,
  cmapArc=None,
) -> str:
  """
  get a HTML string for a Bayesian network

  Parameters
  ----------
  bn : pyagrum.BayesNet
    the Bayesian network
  size: str
    size (for graphviz) of the rendered graph
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
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmapNode

  return getGraph(
    BN2dot(
      bn,
      size=size,
      nodeColor=nodeColor,
      arcWidth=arcWidth,
      arcLabel=arcLabel,
      arcColor=arcColor,
      cmapNode=cmapNode,
      cmapArc=cmapArc,
    ),
    size,
  )


def getCN(
  cn: pyagrum.CredalNet,
  size: float | str | None = None,
  nodeColor=None,
  arcWidth=None,
  arcLabel=None,
  arcColor=None,
  cmapNode=None,
  cmapArc=None,
) -> str:
  """
  get a HTML string for a credal network

  Parameters
  ----------
  cn : pyagrum.CredalNet
    the Credal network
  size: str
    size (for graphviz) of the rendered graph
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
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmapNode

  return getGraph(
    CN2dot(
      cn,
      size=size,
      nodeColor=nodeColor,
      arcWidth=arcWidth,
      arcLabel=arcLabel,
      arcColor=arcColor,
      cmapNode=cmapNode,
      cmapArc=cmapArc,
    ),
    size,
  )


def showInference(model: pyagrum.PGM, **kwargs) -> None:
  """
  show pydot graph for an inference in a notebook

  Parameters
  ----------
  model: pyagrum.GraphicalModel
    the model in which to infer (pyagrum.BayesNet, pyagrum.MarkovRandomField or pyagrum.InfluenceDiagram)
  engine: pyagrum.BNInference | pyagrum.MRFInference | pyagrum.CNInference | pyagrum.IDInference
    inference algorithm used. If None, pyagrum.LazyPropagation will be used for BayesNet, pyagrum.ShaferShenoy for pyagrum.MarkovRandomField and pyagrum.ShaferShenoyLIMIDInference for pyagrum.InfluenceDiagram.
  evs: dict[int|str,int|str|list[float]]
    map of evidence
  targets: Set[str]
    set of targets
  size: string
    size (for graphviz) of the rendered graph
  nodeColor: dict[str,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor: dict[int,float]
    a nodeMap of values (between 0 and 1) to be shown as color of factors (in MarkovRandomField representation)
  arcWidth: : dict[(str,str),float]
    an arcMap of values to be shown as width of arcs
  arcColor: : dict[(str,str),float]
    a arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode: matplotlib.ColorMap
     map to show the color of nodes and arcs
  cmapArc: matplotlib.ColorMap
    color map to show the vals of Arcs.
  graph: pyagrum.Graph
    only shows nodes that have their id in the graph (and not in the whole BN)
  view: str
    graph | factorgraph | None (default) for Markov random field

  Returns
  -------
    the desired representation of the inference
  """
  if "size" in kwargs:
    size = kwargs["size"]
  else:
    size = pyagrum.config["notebook", "default_graph_inference_size"]

  showGraph(prepareShowInference(model, **kwargs), size)


def getInference(model: pyagrum.PGM, **kwargs) -> str:
  """
  get a HTML string for an inference in a notebook

  Parameters
  ----------
  model: pyagrum.GraphicalModel
    the model in which to infer (pyagrum.BayesNet, pyagrum.MarkovRandomField or pyagrum.InfluenceDiagram)
  engine: pyagrum.BNInference | pyagrum.MRFInference | pyagrum.CNInference | pyagrum.IDInference
    inference algorithm used. If None, pyagrum.LazyPropagation will be used for BayesNet, pyagrum.ShaferShenoy for pyagrum.MarkovRandomField and pyagrum.ShaferShenoyLIMIDInference for pyagrum.InfluenceDiagram.
  evs: dict[int|str,int|str|list[float]]
    map of evidence
  targets: Set[str]
    set of targets
  size: string
    size (for graphviz) of the rendered graph
  nodeColor: dict[str,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor: dict[int,float]
    a nodeMap of values (between 0 and 1) to be shown as color of factors (in MarkovRandomField representation)
  arcWidth: : dict[(str,str),float]
    an arcMap of values to be shown as width of arcs
  arcColor: : dict[(str,str),float]
    a arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode: matplotlib.ColorMap
     map to show the color of nodes and arcs
  cmapArc: matplotlib.ColorMap
    color map to show the vals of Arcs.
  graph: pyagrum.Graph
    only shows nodes that have their id in the graph (and not in the whole BN)
  view: str
    graph | factorgraph | None (default) for Markov random field

  Returns
  -------
    the desired representation of the inference
  """
  if "size" in kwargs:
    size = kwargs["size"]
  else:
    size = pyagrum.config["notebook", "default_graph_inference_size"]

  grinf = prepareShowInference(model, **kwargs)
  return getGraph(grinf, size)


def _reprTensor(
  pot: pyagrum.Tensor,
  digits: int | None = None,
  withColors: bool | None = None,
  varnames: list[str] | None = None,
  asString: bool = False,
) -> str:
  """
  return a representation of a pyagrum.Tensor as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  Parameters
  ---------
  pot: pyagrum.Tensor
   the tensor to get
  digits: int
   number of digits to show
  withColors: Boolean
   background color for proba cells or not
  varnames: dict[str,str]
   a mapping that gives the aliases for variables name in the table
  asString: Boolean
   display the table or a HTML string

  Returns
  -------
  str
    the HTML table that represents the tensor
  """
  from fractions import Fraction

  r0, g0, b0 = gumcols.hex2rgb(pyagrum.config["notebook", "tensor_color_0"])
  r1, g1, b1 = gumcols.hex2rgb(pyagrum.config["notebook", "tensor_color_1"])

  if digits is None:
    digits = pyagrum.config.asInt["notebook", "tensor_visible_digits"]

  if withColors is None:
    withColors = pyagrum.config.asBool["notebook", "tensor_with_colors"]

  with_fraction = pyagrum.config["notebook", "tensor_with_fraction"] == "True"
  if with_fraction:
    fraction_limit = int(pyagrum.config["notebook", "tensor_fraction_limit"])
    fraction_round_error = float(pyagrum.config["notebook", "tensor_fraction_round_error"])
    fraction_with_latex = pyagrum.config["notebook", "tensor_fraction_with_latex"] == "True"

  def _rgb(r, g, b):
    return f"#{r:02x}{g:02x}{b:02x}"

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
          if frac_val.denominator > 1:
            str_val += f"\\frac{{{frac_val.numerator}}}{{{frac_val.denominator}}}"
          else:
            str_val += f"{frac_val.numerator}"
          str_val += "$$"
        else:
          str_val += f"{frac_val}"
        str_val += "</td>"
    if str_val == "":
      str_val = f"text-align:right;padding: 3px;'>{val:.{digits}f}</td>"

    return s + str_val

  html = list()
  html.append('<table style="border:1px solid black;border-collapse: collapse;">')
  if pot.empty():
    html.append("<tr><th>&nbsp;</th></tr>")
    html.append("<tr>" + _mkCell(pot.get(pyagrum.Instantiation())) + "</tr>")
  else:
    if varnames is not None and len(varnames) != pot.nbrDim():
      raise ValueError(f"varnames contains {len(varnames)} value(s) instead of the needed {pot.nbrDim()} value(s).")

    nparents = pot.nbrDim() - 1
    var = pot.variable(0)
    varname = var.name() if varnames is None else varnames[0]

    # first line
    if nparents > 0:
      html.append(f"""<tr><th colspan='{nparents}'></th>
      <th colspan='{var.domainSize()}' style='border:1px solid black;color:black;background-color:#808080;'><center>{varname}</center>
      </th></tr>""")
    else:
      html.append(f"""<tr style='border:1px solid black;color:black;background-color:#808080'>
      <th colspan='{var.domainSize()}'><center>{varname}</center></th></tr>""")

    # second line
    s = "<tr>"
    if nparents > 0:
      # parents order
      if pyagrum.config["notebook", "tensor_parent_values"] == "revmerge":
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
    s += "</tr>"

    html.append(s)

    inst = pyagrum.Instantiation(pot)
    off = 1
    offset = dict()
    for i in range(1, nparents + 1):
      offset[i] = off
      off *= inst.variable(i).domainSize()

    inst.setFirst()
    while not inst.end():
      s = "<tr>"
      # parents order
      if pyagrum.config["notebook", "tensor_parent_values"] == "revmerge":
        pmin, pmax, pinc = 1, nparents + 1, 1
      else:
        pmin, pmax, pinc = nparents, 0, -1
      for par in range(pmin, pmax, pinc):
        label = inst.variable(par).label(inst.val(par))
        if par == 1 or pyagrum.config["notebook", "tensor_parent_values"] == "nomerge":
          s += f"<th style='border:1px solid black;color:black;background-color:#BBBBBB'><center>{label}</center></th>"
        else:
          if sum([inst.val(i) for i in range(1, par)]) == 0:
            s += f"""<th style='border:1px solid black;color:black;background-color:#BBBBBB;' rowspan = '{offset[par]}'>
            <center>{label}</center></th>"""
      for _ in range(pot.variable(0).domainSize()):
        s += _mkCell(pot.get(inst))
        inst.inc()
      s += "</tr>"
      html.append(s)

  html.append("</table>")

  if asString:
    return "\n".join(html)
  else:
    return IPython.display.HTML("".join(html))


def __isKindOfProba(pot: pyagrum.Tensor) -> bool:
  """
  check if pot is a joint proba or a CPT

  Parameters
  ----------
  pot: pyagrum.Tensor
    the tensor

  Returns
  -------
  True or False
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
  q = pot.sumOut([pot.variable(0).name()])
  if abs(q.max() - 1) > epsilon:
    return False
  if abs(q.min() - 1) > epsilon:
    return False

  return True


def showPotential(
  pot: pyagrum.Tensor, digits: int | None = None, withColors: bool | None = None, varnames: list[str] | None = None
) -> None:
  warnings.warn("showPotential is deprecated since pyAgrum 2.0.0. Use showTensor instead", DeprecationWarning)
  showTensor(pot, digits, withColors, varnames)


def showTensor(
  pot: pyagrum.Tensor, digits: int | None = None, withColors: bool | None = None, varnames: list[str] | None = None
) -> None:
  """
  show a pyagrum.Tensor as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  Parameters
  ----------
  pot : pyagrum.Tensor
    the tensor to show
  digits : int
    number of digits to show
  withColors : bool
    background color for proba cells or not
  varnames : list[str]
    the aliases for variables name in the table
  """
  if withColors is None:
    withColors = pyagrum.config.asBool["notebook", "tensor_with_colors"]

  if withColors:
    withColors = __isKindOfProba(pot)

  s = _reprTensor(pot, digits, withColors, varnames, asString=False)
  IPython.display.display(s)


def getPotential(
  pot: pyagrum.Tensor, digits: int | None = None, withColors: bool | None = None, varnames: list[str] | None = None
) -> str:
  warnings.warn("getPotential is deprecated since pyAgrum 2.0.0. Use getTensor instead", DeprecationWarning)
  return getTensor(pot, digits, withColors, varnames)


def getTensor(
  pot: pyagrum.Tensor, digits: int | None = None, withColors: bool | None = None, varnames: list[str] | None = None
) -> str:
  """
  return a HTML string of a pyagrum.Tensor as a HTML table.
  The first dimension is special (horizontal) due to the representation of conditional probability table

  Parameters
  ----------
  pot : pyagrum.Tensor
    the tensor to show
  digits : int
    number of digits to show
  withColors : bool
    background for proba cells or not
  varnames : list[str]
    the aliases for variables name in the table

  Returns
  -------
  str
    the html representation of the Tensor (as a string)
  """
  if withColors is None:
    withColors = pyagrum.config.asBool["notebook", "tensor_with_colors"]

  if withColors:
    withColors = __isKindOfProba(pot)

  return _reprTensor(pot, digits, withColors, varnames, asString=True)


def showCPTs(bn: pyagrum.BayesNet) -> None:
  flow.clear()
  for i in bn.names():
    flow.add_html(getTensor(bn.cpt(i)))
  flow.display()


def getSideBySide(*args, **kwargs) -> str:
  """
  create an HTML table for args as string (using string, _repr_html_() or str())

  Parameters
  ----------
  args: str
    HMTL fragments as string arg, arg._repr_html_() or str(arg)
  captions: list[str], optional
    list of captions
  valign: str
    vertical position in the row (top|middle|bottom, middle by default)
  ncols: int
    number of columns (infinite by default)

  Returns
  -------
  str
    a string representing the table
  """
  vals = {"captions", "valign", "ncols"}
  if not set(kwargs.keys()).issubset(vals):
    raise TypeError(f"sideBySide() got unexpected keyword argument(s) : '{set(kwargs.keys()).difference(vals)}'")

  if "captions" in kwargs:
    captions = kwargs["captions"]
  else:
    captions = None

  if "valign" in kwargs and kwargs["valign"] in ["top", "middle", "bottom"]:
    v_align = f"vertical-align:{kwargs['valign']};"
  else:
    v_align = "vertical-align:middle;"

  ncols = None
  if "ncols" in kwargs:
    ncols = int(kwargs["ncols"])
    if ncols < 1:
      ncols = 1

  def reprHTML(s):
    if isinstance(s, str):
      return s
    elif hasattr(s, "_repr_html_"):
      return s._repr_html_()
    else:
      return str(s)

  s = '<table style="border-style: hidden; border-collapse: collapse;" width="100%"><tr>'
  for i in range(len(args)):
    s += f'<td style="border-top:hidden;border-bottom:hidden;{v_align}"><div align="center" style="{v_align}">'
    s += reprHTML(args[i])
    if captions is not None:
      s += f"<br><small><i>{captions[i]}</i></small>"
    s += "</div></td>"
    if ncols is not None and (i + 1) % ncols == 0:
      s += "</tr><tr>"
  s += "</tr></table>"
  return s


def sideBySide(*args, **kwargs) -> None:
  """
  display side by side args as HMTL fragment (using string, _repr_html_() or str())

  Parameters
  ----------
  args: str
    HMTL fragments as string arg, arg._repr_html_() or str(arg)
  captions: list[str], optional
    list of captions
  valign: str
    vertical position in the row (top|middle|bottom, middle by default)
  ncols: int
    number of columns (infinite by default)
  """
  IPython.display.display(IPython.display.HTML(getSideBySide(*args, **kwargs)))


def getInferenceEngine(ie, inferenceCaption: str) -> str:
  """
  display an inference as a BN+ lists of hard/soft evidence and list of targets

  Parameters
  ---------
  ie : "pyagrum.InferenceEngine"
    Inference engine
  caption: str
    inferenceCaption: caption for the inference

  Returns
  -------
  str
    the HTML representation
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

  if hasattr(ie, "nbrJointTargets") and ie.nbrJointTargets() > 0:
    t += "<li><b>Joint target(s)</b><br/>"
    t += ", ".join(["[" + (", ".join([ie.BN().variable(n).name() for n in ns])) + "]" for ns in ie.jointTargets()])
    t += "</li>"
  t += "</ul></div>"
  return getSideBySide(getBN(ie.BN()), t, captions=[inferenceCaption, "Evidence and targets"])


def getJT(jt: pyagrum.CliqueGraph, size: float | str | None = None) -> str:
  """
  returns the representation of a junction tree as a HTML string

  Parameters
  ----------
  jt: pyagrum.CliqueGraph
    the junction tree
  size: str
    the size (for graphviz) of the graph

  Returns
  -------
  str
    the representation of a junction tree as a HTML string

  """
  if pyagrum.config.asBool["notebook", "junctiontree_with_names"]:

    def cliqlabels(c):
      labels = ",".join(sorted([model.variable(n).name() for n in jt.clique(c)]))
      return f"({c}):{labels}"

    def cliqnames(c):
      return "-".join(sorted([model.variable(n).name() for n in jt.clique(c)]))

    def seplabels(c1, c2):
      return ",".join(sorted([model.variable(n).name() for n in jt.separator(c1, c2)]))

    def sepnames(c1, c2):
      return cliqnames(c1) + "+" + cliqnames(c2)
  else:

    def cliqlabels(c):
      ids = ",".join([str(n) for n in sorted(jt.clique(c))])
      return f"({c}):{ids}"

    def cliqnames(c):
      return "-".join([str(n) for n in sorted(jt.clique(c))])

    def seplabels(c1, c2):
      return ",".join([str(n) for n in sorted(jt.separator(c1, c2))])

    def sepnames(c1, c2):
      return cliqnames(c1) + "^" + cliqnames(c2)

  model = jt._engine._model
  name = model.propertyWithDefault("name", str(type(model)).split(".")[-1][:-2])
  graph = dot.Dot(graph_type="graph", graph_name=name, bgcolor="transparent")
  for c in jt.nodes():
    graph.add_node(
      dot.Node(
        '"' + cliqnames(c) + '"',
        label='"' + cliqlabels(c) + '"',
        style="filled",
        fillcolor=pyagrum.config["notebook", "junctiontree_clique_bgcolor"],
        fontcolor=pyagrum.config["notebook", "junctiontree_clique_fgcolor"],
        fontsize=pyagrum.config["notebook", "junctiontree_clique_fontsize"],
      )
    )
  for c1, c2 in jt.edges():
    graph.add_node(
      dot.Node(
        '"' + sepnames(c1, c2) + '"',
        label='"' + seplabels(c1, c2) + '"',
        style="filled",
        shape="box",
        width="0",
        height="0",
        margin="0.02",
        fillcolor=pyagrum.config["notebook", "junctiontree_separator_bgcolor"],
        fontcolor=pyagrum.config["notebook", "junctiontree_separator_fgcolor"],
        fontsize=pyagrum.config["notebook", "junctiontree_separator_fontsize"],
      )
    )
  for c1, c2 in jt.edges():
    graph.add_edge(dot.Edge('"' + cliqnames(c1) + '"', '"' + sepnames(c1, c2) + '"'))
    graph.add_edge(dot.Edge('"' + sepnames(c1, c2) + '"', '"' + cliqnames(c2) + '"'))

  graph.set_size(pyagrum.config["notebook", "junctiontree_graph_size"])

  return graph.to_string()


def getCliqueGraph(cg: pyagrum.CliqueGraph, size: float | str | None = None) -> str:
  """get a representation for clique graph. Special case for junction tree
  (clique graph with an attribute `_engine`)

  Parameters
      cg (pyagrum.CliqueGraph): the clique graph (maybe junction tree for a _model) to
                            represent

  Returns
  -------
  pydot.Dot
    the dot representation of the graph
  """
  if hasattr(cg, "_engine"):
    return getDot(getJT(cg), size)
  else:
    return getDot(cg.toDot())


def show(model, **kwargs) -> None:
  """
  propose a (visual) representation of a graphical model or a graph or a Tensor in a notebook

  Parameters
  ----------
  model
    the model to show (pyagrum.BayesNet, pyagrum.MarkovRandomField, pyagrum.InfluenceDiagram or pyagrum.Tensor) or a dot string, or a `pydot.Dot` or even just an object with a method `toDot()`.

  size: str
    size (for graphviz) to represent the graphical model (no effect for Tensor)
  nodeColor: dict[str,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor: dict[int,float]
    a nodeMap of values (between 0 and 1) to be shown as color of factors (in MarkovRandomField representation)
  arcWidth: : dict[(str,str),float]
    an arcMap of values to be shown as width of arcs
  arcColor: : dict[(str,str),float]
    a arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode: matplotlib.ColorMap
     map to show the color of nodes and arcs
  cmapArc: matplotlib.ColorMap
    color map to show the vals of Arcs.
  graph: pyagrum.Graph
    only shows nodes that have their id in the graph (and not in the whole BN)
  view: str
    graph | factorgraph | None (default) for Markov random field
  """
  if isinstance(model, pyagrum.BayesNet):
    showBN(model, **kwargs)
  elif isinstance(model, pyagrum.MarkovRandomField):
    showMRF(model, **kwargs)
  elif isinstance(model, pyagrum.InfluenceDiagram):
    showInfluenceDiagram(model, **kwargs)
  elif isinstance(model, pyagrum.CredalNet):
    showCN(model, **kwargs)
  elif isinstance(model, pyagrum.Tensor):
    showTensor(model)
  elif hasattr(model, "toDot"):
    showDot(model.toDot(), **kwargs)
  elif isinstance(model, dot.Dot):
    showGraph(model, **kwargs)
  else:
    raise pyagrum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, MarkovRandomField, Influence Diagram or Tensor or ..."
    )


def inspectBN(bn: pyagrum.BayesNet) -> "FlowLayout":
  """
  inspect a BN (graph and CPTs) in a notebook (using flow)
  Parameters
  ----------
  bn
  """
  flow.row(bn, *[bn.cpt(c) for c in sorted(bn.names())])
  return flow


def getCausalModel(cm: pyagrum.CausalModel, size: float | str | None = None) -> str:
  """
  return a HTML representing the causal model

  Parameters
  ----------
  cm: CausalModel
    the causal model
  size: int|str
    the size of the rendered graph

  Returns
  -------
  pydot.Dot
    the dot representation
  """
  if size is None:
    size = pyagrum.config["causal", "default_graph_size"]
  return getDot(cm.toDot(), size)


def showCausalModel(cm: pyagrum.CausalModel, size: float | str | None = None) -> None:
  """
  Shows a pydot svg representation of the causal DAG

  Parameters
  ----------
  cm: CausalModel
    the causal model
  size: int|str
    the size of the rendered graph
  """
  if size is None:
    size = pyagrum.config["causal", "default_graph_size"]
  showDot(cm.toDot(), size=size)


def getCausalImpact(
  model: pyagrum.CausalModel,
  on: str | set[str],
  doing: str | set[str],
  knowing: set[str] | None = None,
  values: dict[str, int] | None = None,
) -> "IPython.display.HTML":
  """
  return a HTML representing of the three values defining a causal impact : formula, value, explanation

  Parameters
  ----------
  model: CausalModel
    the causal model
  on: str | Set[str]
    the impacted variable(s)
  doing: str | Set[str]
    the interventions
  knowing: str | Set[str]
    the observations
  values: dict[str,int] default=None
    value for certain variables

  Returns
  -------
  HTML
  """
  formula, impact, explanation = pyagrum.causalImpact(model, on=on, doing=doing, knowing=knowing, values=values)

  if explanation.startswith("Effect not identifiable"):
    explanation = "Effect not identifiable"

  flow.clear()
  flow.add(model, caption="Causal Model")

  if not formula.isIdentified():
    flow.add(explanation, caption="Explanation")
    res = "No result"
  else:
    flow.add(
      "\n\n$$\n\\begin{equation*}" + formula.toLatex() + "\\end{equation*}\n$$\n\n",
      caption="Explanation : " + explanation,
    )
    if impact.variable(0).domainSize() < 5:
      res = impact
    else:
      res = getProba(impact)
  flow.add(res, caption="Impact")

  return flow.html()


def showCausalImpact(
  model: pyagrum.CausalModel,
  on: str | set[str],
  doing: str | set[str],
  knowing: set[str] | None = None,
  values: dict[str, int] | None = None,
) -> None:
  """
  display a HTML representing of the three values defining a causal impact :  formula, value, explanation

  Parameters
  ----------
  model: CausalModel
    the causal model
  on: str | Set[str]
    the impacted variable(s)
  doing: str | Set[str]
    the interventions
  knowing: str | Set[str]
    the observations
  values: dict[str,int] default=None
    value for certain variables
  """
  html = getCausalImpact(model, on, doing, knowing, values)
  IPython.display.display(html)


def _update_config_notebooks() -> None:
  # hook to control some parameters for notebook when config changes
  mpl.rcParams["figure.facecolor"] = pyagrum.config["notebook", "figure_facecolor"]
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])


# check if an instance of ipython exists
try:
  get_ipython
except NameError:
  import warnings

  warnings.warn("""
  ** pyagrum.lib.notebook has to be imported from an IPython's instance (mainly notebook).
  """)
else:

  def map(
    self,
    scaleClique: float | None = None,
    scaleSep: float | None = None,
    lenEdge: float | None = None,
    colorClique: str | None = None,
    colorSep: str | None = None,
  ) -> dot.Dot:
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
      scaleClique = float(pyagrum.config["notebook", "junctiontree_map_cliquescale"])
    if scaleSep is None:
      scaleSep = float(pyagrum.config["notebook", "junctiontree_map_sepscale"])
    if lenEdge is None:
      lenEdge = float(pyagrum.config["notebook", "junctiontree_map_edgelen"])
    if colorClique is None:
      colorClique = pyagrum.config["notebook", "junctiontree_clique_bgcolor"]
    if colorSep is None:
      colorSep = pyagrum.config["notebook", "junctiontree_separator_bgcolor"]
    return _from_dotstring(self.__map_str__(scaleClique, scaleSep, lenEdge, colorClique, colorSep))

  setattr(pyagrum.CliqueGraph, "map", map)

  pyagrum.config.add_hook(_update_config_notebooks)
  pyagrum.config.run_hooks()

  # adding _repr_html_ to some pyAgrum classes !
  pyagrum.BayesNet._repr_html_ = lambda self: getBN(self)
  pyagrum.BayesNetFragment._repr_html_ = lambda self: getBN(self)
  pyagrum.MarkovRandomField._repr_html_ = lambda self: getMRF(self)
  pyagrum.BayesNetFragment._repr_html_ = lambda self: getBN(self)
  pyagrum.InfluenceDiagram._repr_html_ = lambda self: getInfluenceDiagram(self)
  pyagrum.CredalNet._repr_html_ = lambda self: getCN(self)

  pyagrum.CliqueGraph._repr_html_ = lambda self: getCliqueGraph(self)

  pyagrum.Tensor._repr_html_ = lambda self: getTensor(self)
  pyagrum.LazyPropagation._repr_html_ = lambda self: getInferenceEngine(self, "Lazy Propagation on this BN")

  pyagrum.UndiGraph._repr_html_ = lambda self: getDot(self.toDot())
  pyagrum.DiGraph._repr_html_ = lambda self: getDot(self.toDot())
  pyagrum.MixedGraph._repr_html_ = lambda self: getDot(self.toDot())
  pyagrum.DAG._repr_html_ = lambda self: getDot(self.toDot())
  pyagrum.EssentialGraph._repr_html_ = lambda self: getDot(self.toDot())
  pyagrum.MarkovBlanket._repr_html_ = lambda self: getDot(self.toDot())

  pyagrum.CausalImpact._repr_html_ = lambda self: f"$${self.toLatex()}$$"
  pyagrum.CausalModel._repr_html_ = lambda self: getCausalModel(self)

  pyagrum.Tensor._repr_html_ = lambda self: getTensor(self)
  dot.Dot._repr_html_ = lambda self: getGraph(self)
