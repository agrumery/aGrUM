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
tools for BN analysis in ipython (and spyder)
"""

import base64
import os
import sys

import IPython
import IPython.display
import matplotlib as mpl
import matplotlib.colors
import matplotlib.pyplot as plt
import numpy as np
import pydot as dot

from matplotlib_inline.backend_inline import set_matplotlib_formats
from IPython.display import Image, display

import pyagrum
from pyagrum.lib.bn2graph import BN2dot
from pyagrum.lib.cn2graph import CN2dot
from pyagrum.lib.id2graph import ID2dot
from pyagrum.lib.mrf2graph import MRF2UGdot, MRF2FactorGraphdot
from pyagrum.lib.bn_vs_bn import GraphicalBNComparator
from pyagrum.lib.utils import setDarkTheme
import pyagrum.lib._colors as gumcols
from pyagrum.lib.proba_histogram import proba2histo, probaMinMaxH
from pyagrum.lib.image import prepareShowInference
from pyagrum.lib.jt2graph import _junctionTreeMapDot, _junctionTreeDotStr

# check if an instance of ipython exists
try:
  get_ipython  # type: ignore[name-defined]
except NameError:
  raise ImportError(
    "[pyAgrum ERROR] pyagrum.lib.ipython has to be imported from an IPython's instance (mainly ipython's console)."
  ) from None

# SVG is not reliably supported outside Jupyter notebooks — force PNG for all rendering
pyagrum.config["notebook", "graph_format"] = "png"

# ipython console is typically dark-background
setDarkTheme()


def _inline_png_render(data, metadata: dict | None = None) -> None:
  """Display PNG inline using iTerm2 protocol (no ACK response, no console garbage)."""
  if isinstance(data, bytes):
    b64 = base64.b64encode(data).decode("ascii")
  else:
    b64 = data  # already base64-encoded string
  sys.stdout.write(f"\033]1337;File=inline=1;preserveAspectRatio=1:{b64}\007")
  sys.stdout.flush()


# Replace any image/png renderer IPython installed (Kitty/iTerm2 renderers produce ACK garbage)
_ip = get_ipython()  # type: ignore[name-defined]
if hasattr(_ip, "mime_renderers") and "image/png" in _ip.mime_renderers:
  _ip.mime_renderers["image/png"] = _inline_png_render
del _ip


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
  packages["MatPlotLib"] = mpl.__version__
  packages["Numpy"] = np.__version__
  packages["pyAgrum"] = pyagrum.__version__

  for name in packages:
    print(f"{name} : {packages[name]}")


def showGraph(gr: dot.Dot, size: float | str | None = None) -> None:
  """
  show a pydot graph in ipython

  Parameters
  ----------
  gr: pydot.Dot
    the graph to show
  size: int|str
    the size of the visualisation
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  gumcols.prepareDot(gr, size=size)
  display(Image(data=gr.create_png(), format="png"))


def _from_dotstring(dotstring: str) -> dot.Dot:
  _graphs = dot.graph_from_dot_data(dotstring)
  assert _graphs is not None
  return _graphs[0]


def showDot(dotstring: str, size: float | str | None = None) -> None:
  """
  show a dot string as a graph

  Parameters
  ----------
  dotstring: str
    the dot string
  size: float | str
    the size of the graph
  """
  showGraph(_from_dotstring(dotstring), size)


def showBNDiff(bn1: pyagrum.BayesNet, bn2: pyagrum.BayesNet, size: float | str | None = None) -> None:
  """
  Show a graphical diff between the arcs of bn1 (reference) with those of bn2.

  * full black line: the arc is common for both
  * full red line: the arc is common but inverted in bn2
  * dotted black line: the arc is added in bn2
  * dotted red line: the arc is removed in bn2

  Parameters
  ----------
  bn1 : pyagrum.BayesNet
      Referent model for the comparison.
  bn2 : pyagrum.BayesNet
      BN compared to the referent model.
  size : int or str, optional
      Size of the rendered graph.
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]
  cmp = GraphicalBNComparator(bn1, bn2)
  showGraph(cmp.dotDiff(), size)


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
  Show the map of the junction tree of a Bayesian network.

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  size: str, optional
    size (for graphviz) of the rendered graph
  scaleClique: float, optional
    scale for the size of the clique nodes
  scaleSep: float, optional
    scale for the size of the separator nodes
  lenEdge: float, optional
    desired length of edges
  colorClique: str, optional
    color for the clique nodes
  colorSep: str, optional
    color for the separator nodes
  """
  if size is None:
    size = pyagrum.config["notebook", "junctiontree_map_size"]
  showGraph(_junctionTreeMapDot(bn, scaleClique, scaleSep, lenEdge, colorClique, colorSep), size)


def showJunctionTree(bn: pyagrum.BayesNet, withNames: bool = True, size: float | str | None = None) -> None:
  """
  Show a junction tree.

  Parameters
  ----------
  bn : pyagrum.BayesNet
      The Bayesian network.
  withNames : bool
      Display the variable names or the node id in the clique.
  size : int or str, optional
      Size of the rendered graph.
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]
  showDot(_junctionTreeDotStr(bn, withNames), size)


def showProba(p: pyagrum.Tensor, scale: float = 1.0) -> None:
  """
  Show a mono-dim Tensor.

  Parameters
  ----------
  p : pyagrum.Tensor
      The mono-dim Tensor.
  scale : float, optional
      Scale factor.
  """
  _ = proba2histo(p, scale)
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
  plt.show()


def showProbaMinMax(pmin: pyagrum.Tensor, pmax: pyagrum.Tensor, scale: float = 1.0) -> None:
  """
  Show a bi-Tensor (min, max).

  Parameters
  ----------
  pmin: pyagrum.Tensor
    the min marginal to show
  pmax: pyagrum.Tensor
    the max marginal to show
  scale: float
    the zoom factor
  """
  _ = probaMinMaxH(pmin, pmax, scale)
  set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
  plt.show()


def showPosterior(bn: pyagrum.BayesNet, evs: dict, target: str | int) -> None:
  """
  Shortcut for showProba(pyagrum.getPosterior(bn, evs, target)).

  Parameters
  ----------
  bn : pyagrum.BayesNet
      The Bayesian network.
  evs : dict
      Map of evidence.
  target : str or int
      Name or id of target variable.
  """
  showProba(pyagrum.getPosterior(bn, evs=evs, target=target))


def animApproximationScheme(apsc: pyagrum.ApproximationScheme, scale=np.log10) -> None:
  """
  Show an animated version of an approximation algorithm.

  Parameters
  ----------
  apsc: pyagrum.ApproximationScheme
    the approximation algorithm
  scale: callable
    a function to apply to the history values
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
  Show the state of an approximation algorithm.

  Parameters
  ----------
  apsc: pyagrum.ApproximationScheme
    the approximation algorithm
  scale: callable
    a function to apply to the history values
  """
  if apsc.verbosity():
    set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
    if len(apsc.history()) < 10:
      plt.xlim(1, 10)
    else:
      plt.xlim(1, len(apsc.history()))
    plt.title(f"Time : {apsc.currentTime()}s | Iterations : {apsc.nbrIterations()} | Epsilon : {apsc.epsilon()}")
    plt.plot(scale(apsc.history()), "g")


def showBN(
  bn: pyagrum.BayesNet,
  size: float | str | None = None,
  nodeColor: dict | None = None,
  arcWidth: dict | None = None,
  arcColor: dict | None = None,
  cmap: matplotlib.colors.Colormap | None = None,
  cmapArc: matplotlib.colors.Colormap | None = None,
) -> None:
  """
  Show a Bayesian network.

  Parameters
  ----------
  bn : pyagrum.BayesNet
      The Bayesian network.
  size : int or str, optional
      Size of the rendered graph.
  nodeColor : dict, optional
      A nodeMap of values to be shown as color nodes.
  arcWidth : dict, optional
      A arcMap of values to be shown as bold arcs.
  arcColor : dict, optional
      A arcMap of values to be shown as color of arcs.
  cmap : matplotlib.colors.Colormap, optional
      Color map to show the node values.
  cmapArc : matplotlib.colors.Colormap, optional
      Color map to show the arc values.
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  showGraph(BN2dot(bn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


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
  Show a credal network.

  Parameters
  ----------
  cn : pyagrum.CredalNet
    the Credal network
  size: str, optional
    size (for graphviz) of the rendered graph
  nodeColor: dict[int,float], optional
    a nodeMap of values to be shown as color nodes
  arcWidth: dict[tuple,float], optional
    an arcMap of values to be shown as bold arcs
  arcLabel: dict[tuple,float], optional
    an arcMap of labels to be shown next to arcs
  arcColor: dict[tuple,float], optional
    an arcMap of values to be shown as color of arcs
  cmapNode: matplotlib.colors.Colormap, optional
    color map to show the vals of nodes
  cmapArc: matplotlib.colors.Colormap, optional
    color map to show the vals of arcs
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmapNode

  showGraph(
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


def showMRF(
  mrf: pyagrum.MarkovRandomField,
  view: str | None = None,
  size: float | str | None = None,
  nodeColor: dict | None = None,
  factorColor: dict | None = None,
  edgeWidth: dict | None = None,
  edgeColor: dict | None = None,
  cmap: matplotlib.colors.Colormap | None = None,
  cmapEdge: matplotlib.colors.Colormap | None = None,
) -> None:
  """
  Show a Markov random field.

  Parameters
  ----------
  mrf : pyagrum.MarkovRandomField
      The Markov random field.
  view : str, optional
      'graph' | 'factorgraph' | None (default).
  size : int or str, optional
      Size of the rendered graph.
  nodeColor : dict, optional
      A nodeMap of values (between 0 and 1) to be shown as color of nodes.
  factorColor : callable, optional
      A function returning a value (between 0 and 1) for factor color.
      Used when view='factorgraph'.
  edgeWidth : dict, optional
      A edgeMap of values to be shown as width of edges. Used when view='graph'.
  edgeColor : dict, optional
      A edgeMap of values (between 0 and 1) to be shown as color of edges. Used when view='graph'.
  cmap : matplotlib.colors.Colormap, optional
      Color map to show the node colors.
  cmapEdge : matplotlib.colors.Colormap, optional
      Color map to show the edge colors.
  """
  if view is None:
    view = pyagrum.config["notebook", "default_markovrandomfield_view"]

  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapEdge is None:
    cmapEdge = cmap

  if view == "graph":
    dottxt = MRF2UGdot(mrf, size, nodeColor, edgeWidth, edgeColor, cmap, cmapEdge)
  else:
    dottxt = MRF2FactorGraphdot(mrf, size, nodeColor, factorColor, cmapNode=cmap)

  showGraph(dottxt, size)


def showInfluenceDiagram(diag: pyagrum.InfluenceDiagram, size: float | str | None = None) -> None:
  """
  Show an influence diagram as a graph.

  Parameters
  ----------
  diag : pyagrum.InfluenceDiagram
      The influence diagram.
  size : int or str, optional
      Size of the rendered graph.
  """
  if size is None:
    size = pyagrum.config["influenceDiagram", "default_id_size"]

  showGraph(ID2dot(diag), size)


def showCausalModel(cm: pyagrum.CausalModel, size: float | str | None = None) -> None:
  """
  Show a causal model.

  Parameters
  ----------
  cm: pyagrum.CausalModel
    the causal model
  size: int|str, optional
    the size of the rendered graph
  """
  if size is None:
    size = pyagrum.config["causal", "default_graph_size"]
  showDot(cm.toDot(), size=size)


def showInference(model: pyagrum.PGM, **kwargs) -> None:
  """
  Show a graphical model with inference results (histograms in nodes).

  Parameters
  ----------
  model: pyagrum.GraphicalModel
    the model in which to infer (BayesNet, MarkovRandomField or InfluenceDiagram)
  engine: pyagrum.Inference, optional
    inference algorithm. If None, LazyPropagation used for BayesNet.
  evs: dict, optional
    map of evidence
  targets: set, optional
    set of targets
  size: str, optional
    size (for graphviz) of the rendered graph
  nodeColor: dict, optional
    nodeMap of values (0–1) shown as node colors
  arcWidth: dict, optional
    arcMap of values shown as arc widths
  arcColor: dict, optional
    arcMap of values (0–1) shown as arc colors
  cmap: matplotlib.colors.Colormap, optional
    color map for nodes
  cmapArc: matplotlib.colors.Colormap, optional
    color map for arcs
  graph: pyagrum.Graph, optional
    restrict display to nodes in this graph
  view: str, optional
    'graph' | 'factorgraph' | None for MarkovRandomField
  """
  size = kwargs.get("size", pyagrum.config["notebook", "default_graph_inference_size"])
  showGraph(prepareShowInference(model, **kwargs), size)


def showTensor(p: pyagrum.Tensor) -> None:
  print(p)


def showCPTs(bn: pyagrum.BayesNet) -> None:
  """
  Show all CPTs of a Bayesian network.

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the Bayesian network
  """
  for name in bn.names():
    print(f"P({name} | ...)")
    showTensor(bn.cpt(name))
    print()


def show(
  model,
  size: float | str | None = None,
) -> None:
  """
  Propose a (visual) representation of a model in ipython console.

  Parameters
  ----------
  model : pyagrum.GraphicalModel or pyagrum.Tensor
      The model to show.
  size : int, optional
      Size for the graphical model (no effect for Tensor).
  """
  if isinstance(model, pyagrum.BayesNet):
    showBN(model, size)
  elif isinstance(model, pyagrum.CredalNet):
    showCN(model, size)
  elif isinstance(model, pyagrum.MarkovRandomField):
    showMRF(model, size)
  elif isinstance(model, pyagrum.InfluenceDiagram):
    showInfluenceDiagram(model, size)
  elif isinstance(model, pyagrum.CausalModel):
    showCausalModel(model, size)
  elif isinstance(model, pyagrum.CliqueGraph):
    showDot(model.toDot(), size)
  elif isinstance(model, pyagrum.Tensor):
    showTensor(model)
  else:
    raise pyagrum.InvalidArgument(
      "Argument model should be a PGM (BayesNet, CredalNet, MarkovRandomField, InfluenceDiagram or CausalModel)"
    )
