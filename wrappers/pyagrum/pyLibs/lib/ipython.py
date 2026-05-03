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

import IPython.display
import matplotlib as mpl
import matplotlib.colors
import matplotlib.pyplot as plt
import numpy as np
import pydot as dot

from IPython.display import Image, display

import pyagrum
from pyagrum.lib.bn2graph import BN2dot
from pyagrum.lib.id2graph import ID2dot
from pyagrum.lib.mrf2graph import MRF2UGdot
from pyagrum.lib.mrf2graph import MRF2FactorGraphdot
from pyagrum.lib.bn_vs_bn import GraphicalBNComparator
from pyagrum.lib.proba_histogram import proba2histo

# check if an instance of ipython exists
try:
  get_ipython
except NameError:
  raise ImportError(
    "[pyAgrum ERROR] pyagrum.lib.ipython has to be imported from an IPython's instance (mainly ipython's console)."
  ) from None


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
  show a pydot graph in a notebook

  Parameters
  ----------
  gr: pydot.Dot
    the graph to show
  size: int|str
    the size of the visualisation
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  gr.set_size(size)
  display(Image(gr.create_png()))


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
    the size of the graphe
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
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  if withNames:
    return showDot(jt.toDotWithNames(bn), size)
  else:
    return showDot(jt.toDot(), size)


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
  cmap : matplotlib.colors.Colormap, optional
      Color map to show the node values.
  """
  if size is None:
    size = pyagrum.config["notebook", "default_graph_size"]

  if cmapArc is None:
    cmapArc = cmap

  return showGraph(BN2dot(bn, size, nodeColor, arcWidth, arcColor, cmap, cmapArc), size)


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
  #  fig.patch.set_facecolor(pyagrum.config["notebook", "figure_facecolor"])
  IPython.display.set_matplotlib_formats(pyagrum.config["notebook", "graph_format"])
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
  target : str
      Name of target variable.
  """
  showProba(pyagrum.getPosterior(bn, evs=evs, target=target))


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
      ‘graph’ | ‘factorgraph’ | None (default).
  size : int or str, optional
      Size of the rendered graph.
  nodeColor : dict, optional
      A nodeMap of values (between 0 and 1) to be shown as color of nodes.
  factorColor : callable, optional
      A function returning a value (between 0 and 1) to be shown as a color of factor.
      Used when view=’factorgraph’.
  edgeWidth : dict, optional
      A edgeMap of values to be shown as width of edges. Used when view=’graph’.
  edgeColor : dict, optional
      A edgeMap of values (between 0 and 1) to be shown as color of edges. Used when view=’graph’.
  cmap : matplotlib.colors.Colormap, optional
      Color map to show the node colors.
  cmapEdge : matplotlib.colors.Colormap, optional
      Color map to show the edge color if distinction is needed.

  Returns
  -------
  pydot.Dot
      The graph.
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

  return showGraph(dottxt, size)


def showInfluenceDiagram(diag: pyagrum.InfluenceDiagram, size: float | str | None = None) -> None:
  """
  Show an influence diagram as a graph.

  Parameters
  ----------
  diag : pyagrum.InfluenceDiagram
      The influence diagram.
  size : int or str, optional
      Size of the rendered graph.

  Returns
  -------
  pydot.Dot
      The representation of the influence diagram.
  """
  if size is None:
    size = pyagrum.config["influenceDiagram", "default_id_size"]

  return showGraph(ID2dot(diag), size)


def showInference(
  model,
  engine=None,
  evs=None,
  targets=None,
  size=None,
  nodeColor=None,
  factorColor=None,
  arcWidth=None,
  arcColor=None,
  cmap=None,
  cmapArc=None,
  graph=None,
  view=None,
) -> None:
  import warnings

  warnings.warn(
    "pyagrum.lib.ipython does not provide `showInference` due to the use of svg format (not compatible with spyder)."
  )


def showTensor(p: pyagrum.Tensor) -> None:
  print(p)


def show(
  model: pyagrum.GraphicalModel | pyagrum.Tensor,
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
  elif isinstance(model, pyagrum.MarkovRandomField):
    showMRF(model, size)
  elif isinstance(model, pyagrum.InfluenceDiagram):
    showInfluenceDiagram(model, size)
  elif isinstance(model, pyagrum.Tensor):
    showTensor(model)
  else:
    raise pyagrum.InvalidArgument("Argument model should be a PGM (BayesNet, MarkovRandomField or Influence Diagram")


# check if an instance of ipython exists
try:
  get_ipython
except NameError:
  import warnings

  warnings.warn("""
  ** pyagrum.lib.notebook has to be import from an IPython's instance.
  """)
