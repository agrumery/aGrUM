############################################################################
#   This file is part of the aGrUM/pyAgrum library.                        #
#                                                                          #
#   Copyright (c) 2005-2025 by                                             #
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
#   See the GNU Lesser General Public License (LICENSE.LGPL) and the MIT   #
#   licence (LICENSE.MIT) for more details.                                #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""
tools for BN qualitative analysis and explainability
"""

import math
from typing import Dict
import itertools
import warnings
from typing import Union

import pylab
import pydot as dot

import matplotlib as mpl
import matplotlib.pyplot as plt

import pyagrum as gum
from pyagrum.lib.bn2graph import BN2dot
import pyagrum.lib._colors as gumcols

# importing ShapValues in the name space of explain
from pyagrum.lib.shapley import ShapValues

_cdict = {
  'red': ((0.0, 0.1, 0.3),
          (1.0, 0.6, 1.0)),
  'green': ((0.0, 0.0, 0.0),
            (1.0, 0.6, 0.8)),
  'blue': ((0.0, 0.0, 0.0),
           (1.0, 1, 0.8))
}
_INFOcmap = mpl.colors.LinearSegmentedColormap('my_colormap', _cdict, 256)


def _independenceListForPairs(bn, target=None):
  """
    returns a list of triples `(i,j,k)` for each non arc `(i,j)` such that `i` is independent of `j` given `k`.

    Parameters
    ----------
    bn: gum.BayesNet
      the Bayesian Network

    target: (optional) str or int
      the name or id of the target variable. If a target is given, only the independence given a subset of the markov blanket of the target are tested.

    Returns
    -------
    List[(str,str,List[str])]
      A list of independence found in the structure of BN.
    """

  def powerset(iterable):
    xs = list(iterable)
    # note we return an iterator rather than a list
    return itertools.chain.from_iterable(itertools.combinations(xs, n) for n in range(len(xs) + 1))

  # testing every d-separation
  l = []
  nams = sorted(bn.names())
  if target is None:
    firstnams = nams.copy()
    indepnodes = bn.names()
  else:
    indepnodes = {bn.variable(i).name() for i in gum.MarkovBlanket(bn, target).nodes()}
    if isinstance(target, str):
      firstnams = [target]
    else:
      firstnams = [bn.variable(target).name()]

  for i in firstnams:
    nams.remove(i)
    for j in nams:
      if not (bn.existsArc(i, j) or bn.existsArc(j, i)):
        for k in powerset(sorted(indepnodes - {i, j})):
          if bn.isIndependent(i, j, k):
            l.append((i, j, tuple(k)))
            break
  return l


def independenceListForPairs(bn, filename, target=None, plot=True, alphabetic=False):
  """
    get the p-values of the chi2 test of a (as simple as possible) independence proposition for every non arc.

    Parameters
    ----------
    bn : gum.BayesNet
      the Bayesian network

    filename : str
      the name of the csv database

    alphabetic : bool
      if True, the list is alphabetically sorted else it is sorted by the p-value

    target: (optional) str or int
      the name or id of the target variable

    plot : bool
      if True, plot the result

    Returns
    -------
      the list
    """

  learner = gum.BNLearner(filename, bn)
  vals = {}
  for indep in _independenceListForPairs(bn, target):
    vals[indep] = learner.chi2(*indep)[1]

  if plot:
    plotvals = dict()
    for indep in vals:
      key = "$" + indep[0] + " \\perp " + indep[1]
      if len(indep[2]) > 0:
        key += " \\mid " + ",".join(indep[2])
      key += "$"
      plotvals[key] = vals[indep]

    if not alphabetic:
      sortedkeys = sorted(plotvals, key=plotvals.__getitem__, reverse=False)
    else:
      sortedkeys = list(plotvals.keys())

    fig = pylab.figure(figsize=(10, 1 + 0.25 * len(plotvals)))
    ax = fig.add_subplot(1, 1, 1)
    ax.plot([plotvals[k] for k in sortedkeys], sortedkeys, "o")
    ax.grid(True)
    ax.vlines(x=0.05, ymin=-0.5, ymax=len(vals) - 0.5, colors='purple')
    ax.add_patch(mpl.patches.Rectangle((0, -0.5), 0.05, len(vals), color="yellow"))

  return vals


def _normalizeVals(vals, hilightExtrema=False):
  """
    normalisation if vals is not a proba (max>1)
    """
  ma = float(max(vals.values()))
  mi = float(min(vals.values()))
  if ma == mi:
    return None

  if not hilightExtrema:
    vmi = 0.01
    vma = 0.99
  else:
    vmi = 0
    vma = 1

  res = {name: vmi + (val - mi) * (vma - vmi) / (ma - mi) for name, val in vals.items()}
  return res


def getInformationGraph(bn, evs=None, size=None, cmap=_INFOcmap, withMinMax=False):
  """
  Create a dot representation of the information graph for this BN

  Parameters
  ----------
  bn: gum.BayesNet
    the BN
  evs : Dict[str,str|int|List[float]]
    map of evidence
  size: str|int
    size of the graph
  cmap: matplotlib.colors.Colormap
    color map
  withMinMax: bool
    min and max in the return values ?

  Returns
  -------
  dot.Dot | Tuple[dot.Dot,float,float,float,float]
    graph as a dot representation and if asked, min_information_value, max_information_value, min_mutual_information_value, max_mutual_information_value
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  ie = gum.LazyPropagation(bn)
  ie.setEvidence(evs)
  ie.makeInference()

  idEvs = ie.hardEvidenceNodes() | ie.softEvidenceNodes()

  nodevals = dict()
  for n in bn.nodes():
    if n not in idEvs:
      v = ie.H(n)
      if v != v:  # is NaN
        warnings.warn(f"For {bn.variable(n).name()}, entropy is NaN.")
        v = 0
      nodevals[bn.variable(n).name()] = v

  arcvals = dict()
  for x, y in bn.arcs():
    v = ie.jointMutualInformation({x, y})
    if v != v:  # is NaN
      warnings.warn(f"For {bn.variable(x).name()}->{bn.variable(y).name()}, mutual information is Nan.")
      v = 0
    arcvals[(x, y)] = v

  gr = BN2dot(bn, size, nodeColor=_normalizeVals(nodevals, hilightExtrema=False), arcWidth=arcvals, cmapNode=cmap,
              cmapArc=cmap, showMsg=nodevals)

  if withMinMax:
    mi_node = min(nodevals.values())
    ma_node = max(nodevals.values())
    mi_arc = min(arcvals.values())
    ma_arc = max(arcvals.values())
    return gr, mi_node, ma_node, mi_arc, ma_arc
  else:
    return gr


def _reprInformation(bn, evs=None, size=None, cmap=_INFOcmap, asString=False):
  """
  repr a bn annotated with results from inference : Information and mutual information

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  evs: Dict[str|int,str|int|List[float]]
    the observations
  size: int|str
    size of the rendered graph
  cmap: matplotlib.colours.Colormap
    the cmap
  asString: bool
    returns the string or display the HTML

  Returns
  -------
  str|None
    return the HTML string or directly display it.
  """
  import IPython.display
  import IPython.core.pylabtools
  from base64 import encodebytes
  from matplotlib.backends.backend_agg import FigureCanvasAgg as fc

  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  gr, mi, ma, _, _ = getInformationGraph(bn, evs, size, cmap, withMinMax=True)
  gumcols.prepareDot(gr, size=size)

  # dynamic member makes pylink unhappy
  # pylint: disable=no-member
  gsvg = IPython.display.SVG(gr.create_svg(encoding="utf-8"))
  width = int(gsvg.data.split("width=")[1].split('"')[1].split("pt")[0]) / mpl.pyplot.rcParams[
    'figure.dpi']  # pixel in inches
  if width < 5:
    width = 5

  fig = mpl.figure.Figure(figsize=(width, 1))
  fig.patch.set_alpha(0)
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize(vmin=mi, vmax=ma)
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=cmap,
                                  norm=norm,
                                  orientation='horizontal'
                                  )
  cb1.set_label('Entropy')
  cb1.ax.text(mi, -2, f"{mi:.4f}", ha='left', va='top', color=gumcols.proba2bgcolor(0.01, cmap))
  cb1.ax.text(ma, -2, f"{ma:.4f}", ha='right', va='top', color=gumcols.proba2bgcolor(0.99, cmap))
  png = IPython.core.pylabtools.print_figure(canvas.figure, "png")  # from IPython.core.pylabtools
  png_legend = f"<img style='vertical-align:middle' src='data:image/png;base64,{encodebytes(png).decode('ascii')}'>"

  sss = f"<div align='center'>{gsvg.data}<br/>{png_legend}</div>"

  if asString:
    return sss

  return IPython.display.display(IPython.display.HTML(sss))


def getInformation(bn, evs=None, size=None, cmap=_INFOcmap) -> str:
  """
  get a HTML string for a bn annotated with results from inference : entropy and mutual information

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  evs: Dict[str|int,str|int|List[float]]
    the observations
  size: int|str
    size of the rendered graph
  cmap: matplotlib.colours.Colormap
    the cmap

  Returns
  -------
  str
    return the HTML string
  """
  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  if evs is None:
    evs = {}

  return _reprInformation(bn, evs, size, cmap, asString=True)


def showInformation(bn, evs=None, size=None, cmap=_INFOcmap):
  """
  diplay a bn annotated with results from inference : entropy and mutual information

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  evs: Dict[str|int,str|int|List[float]]
    the observations
  size: int|str
    size of the rendered graph
  cmap: matplotlib.colours.Colormap
    the cmap
  """
  if evs is None:
    evs = {}

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  return _reprInformation(bn, evs, size, cmap, asString=False)


########################## MB(k) #####################################

def _buildMB(model, x: int, k: int = 1):
  """
  Build the nodes and arcs of Markov Blanket (of order k) of node x

  Parameters
  ----------
  model: pyagrum.DirectedGraphicalModel
      i.e. a class with methods parents, children, variable(i), idFromName(name)
  x : int
      the nodeId of the node for the Markov blanket
  k: int
      the order of the Markov blanket. If k=2, build the MarkovBlanket(MarkovBlanket())

  Returns
  -------
      (nodes,arcs,depth) : the set of nodes, the set of arcs of the Markov Blanket and a dict[Str,int] that gives the MB-depth of each node in nodes.
  """
  nodes = {x}
  arcs = set()
  depth = dict()

  def _internal_build_markov_blanket(bn, x: int, k: int):
    nodes.add(x)
    depth[x] = k
    if k == 0:
      return
    for y in bn.parents(x):
      visit(y, k - 1)
      arcs.add((y, x))
    for y in bn.children(x):
      visit(y, k - 1)
      arcs.add((x, y))
      for z in bn.parents(y):
        visit(z, k - 1)
        arcs.add((z, y))

  def visit(x, k):
    if x in nodes and depth[x] >= k:
      return
    _internal_build_markov_blanket(model, x, k)

  _internal_build_markov_blanket(model, x, k)
  return nodes, arcs, depth


def generalizedMarkovBlanket(bn, var: Union[int, str], k: int = 1, cmapNode=None):
  """
  Build a pydot.Dot representation of the nested Markov Blankets (of order k) of node x

  Warnings
  --------
  It is assumed that k<=8. If not, every thing is fine except that the colorscale will change in order to accept more colors.

  Parameters
  ----------
  bn: pyagrum.DirectedGraphicalModel
      i.e. a class with methods parents, children, variable(i), idFromName(name)
  var : str|int
      the name or nodeId of the node for the Markov blanket
  k: int
      the order of the Markov blanket. If k=1, build the MarkovBlanket(MarkovBlanket())
  cmap: maplotlib.ColorMap
      the colormap used (if not, inferno is used)

  Returns
  -------
      pydotplus.Dot object
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap("inferno")  # gum.config["notebook", "default_arc_cmap"])

  maxcols = max(8,
                k)  # It is assumed that k<=8. If not, every thing is fine except that the colorscale will change in order to accept more colors.

  mb = dot.Dot(f'MB({var},{k}', graph_type='digraph', bgcolor='transparent')

  if isinstance(var, str):
    nx = bn.idFromName(var)
  else:
    nx = var
  nodes, arcs, visited = _buildMB(bn, nx, k)
  names = dict()

  for n in nodes:
    protected_name = f"\"{bn.variable(n).name()}\""
    pnode = dot.Node(protected_name, style="filled")
    if n == var:
      bgcol = "#99FF99"
      fgcol = "black"
    else:
      bgcol = gumcols.proba2bgcolor(1 - (k - visited[n]) / maxcols, cmapNode)
      fgcol = gumcols.proba2fgcolor(1 - (k - visited[n]) / maxcols, cmapNode)
    pnode.set_fillcolor(bgcol)
    pnode.set_fontcolor(fgcol)
    mb.add_node(pnode)
    names[n] = protected_name
  for n in nodes:
    for u in bn.parents(n).intersection(nodes):
      edge = dot.Edge(names[u], names[n])
      if (u, n) in arcs:
        edge.set_color('black')
      else:
        edge.set_color('#DDDDDD')
      mb.add_edge(edge)

  return mb

def generalizedMarkovBlanketNames(bn, var: Union[int, str], depth: int = 1):
  """
  List the name of all nodes in the nested Markov Blankets (of order k) in association with their depth

  Parameters
  ----------
  bn: pyagrum.DirectedGraphicalModel
      i.e. a class with methods parents, children, variable(i), idFromName(name)
  var : str|int
      the name or nodeId of the node for the Markov blanket
  depth: int
      the order of the Markov blanket. If k=1, build the MarkovBlanket(MarkovBlanket()

  Returns
  -------
  Dict[str,int]
    the list of names and their depth.
  """
  if isinstance(var, str):
    nx = bn.idFromName(var)
  else:
    nx = var
  nodes, _, visited = _buildMB(bn, nx, depth)
  return {bn.variable(node).name(): depth - visited[node] for node in nodes}


def showShapValues(bn,shaps,cmap='plasma')->None:
  """
  Show the Shap values in the DAG of the BN

  Parameters
  ----------
  bn : pyagrum.BayesNet
    The Bayesian network
  shaps: dict[str,float]
    The (Shap) values associates to each variable
  cmap: Matplotlib.ColorMap
    The colormap used for colouring the nodes
  """
  import pyagrum.lib.shapley as shapley
  import pyagrum.lib.notebook as gnb

  g = shapley.getShapValues(bn,shaps,cmap)
  gnb.showGraph(g)
