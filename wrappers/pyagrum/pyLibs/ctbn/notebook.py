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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
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
This file defines some helpers for handling CTBNs in notebooks
"""

from tempfile import mkdtemp
import matplotlib.pyplot as plt
import IPython
import time
import hashlib

import pyagrum as gum
from pyagrum.lib import proba_histogram
import pyagrum.lib._colors as gumcols
import pyagrum.lib.notebook as gnb
import pydot as dot

from pyagrum.ctbn import CTBN
from pyagrum.ctbn import ForwardSamplingInference


def getCtbnGraph(ctbn: CTBN, size=None):
  """
  Returns a HTML representing the CTBN.

  Parameters
  ----------
  ctbn : CTBN
      The CTBN.
  size : int|str
      The size of the rendered graph.

  Returns
  -------
  pydot.Dot
      The dot representation.
  """
  if size is None:
    size = gum.config["ctbn", "default_graph_size"]
  return gnb.getDot(ctbn.toDot(), size)


def showCtbnGraph(ctbn: CTBN, size=None):
  """
  Show a pydot svg representation of the CTBN.

  Parameters
  ----------
  ctbn : CTBN
      The CTBN.
  size : int|str
      The size of the rendered graph.
  """

  if size is None:
    size = gum.config["ctbn", "default_graph_size"]
  gnb.showDot(ctbn.toDot(), size)


def getCIMs(ctbn: CTBN, size=None):
  """
  Returns a HTML representing the CIMS of the CTBN.

  Parameters
  ----------
  ctbn : CTBN
      The CTBN.
  size : int|str
  """
  gnb.flow.clear()
  for var in ctbn.names():
    gnb.flow.add(ctbn.CIM(var)._pot, caption=f"CIM({var})")
  return gnb.flow.html()


def showCIMs(ctbn: CTBN, size=None):
  """
  Show a pydot svg representation of the CIMs of the CTBN.

  Parameters
  ----------
  ctbn : CTBN
      The CTBN.
  size : int|str
  """
  html = getCIMs(ctbn, size=size)
  IPython.display.display(html)


def CTBNinference2dot(
  ctbn, engine, size=None, targets=None, nodeColor=None, arcWidth=None, arcColor=None, cmapNode=None, cmapArc=None
):
  """
  create a pydot representation of an inference in a CTBN

  Parameters
  ----------
    ctbn : CTBN
      the Bayesian network
    size: str
      size of the rendered graph
    engine:  pyagrum.Inference
      inference algorithm used. If None, LazyPropagation will be used
    targets: set
      set of targets. If targets={} then each node is a target
    nodeColor: dict
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
    arcWidth: dict
      a arcMap of values to be shown as bold arcs
    arcColor: dict
      a arcMap of values (between 0 and 1) to be shown as color of arcs
    cmapNode: ColorMap
      color map to show the vals of Nodes
    cmapArc: ColorMap
      color map to show the vals of Arcs

  Returns
  -------
    the desired representation of the inference
  """
  if targets is None:
    targets = {}
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(gum.config["notebook", "default_arc_cmap"])

  # defaukt
  maxarcs = 100
  minarcs = 0

  if arcWidth is not None:
    minarcs = min(arcWidth.values())
    maxarcs = max(arcWidth.values())

  startTime = time.time()
  if engine is None:
    ie = ForwardSamplingInference(ctbn)
  else:
    ie = engine

  ie.makeInference()
  stopTime = time.time()

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = 'digraph structs {\n  fontcolor="' + gumcols.getBlackInTheme() + '";bgcolor="transparent";'

  if gum.config.asBool["notebook", "show_inference_time"]:
    dotstr += f'  label="Inference in {1000 * (stopTime - startTime):6.2f}ms";\n'

  fontname, fontsize = gumcols.fontFromMatplotlib()
  dotstr += f'  node [fillcolor="{gum.config["notebook", "default_node_bgcolor"]}", style=filled,color="{gum.config["notebook", "default_node_fgcolor"]}",fontname="{fontname}",fontsize="{fontsize}"];\n'
  dotstr += f'  edge [color="{gumcols.getBlackInTheme()}"];\n'

  showdag = ctbn._graph

  for nid in showdag.nodes():
    name = ctbn.variable(nid).name()

    # defaults
    bgcol = gum.config["notebook", "default_node_bgcolor"]
    fgcol = gum.config["notebook", "default_node_fgcolor"]
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = gum.config["notebook", "figure_facecolor"]

    if nodeColor is not None and (name in nodeColor or nid in nodeColor):
      bgcol = gumcols.proba2bgcolor(nodeColor[name], cmapNode)
      fgcol = gumcols.proba2fgcolor(nodeColor[name], cmapNode)

    # 'hard' colour for evidence (?)
    # if name in evs or nid in evs:
    #   bgcol = gum.config["notebook", "evidence_bgcolor"]
    #   fgcol = gum.config["notebook", "evidence_fgcolor"]
    colorattribute = f'fillcolor="{bgcol}", fontcolor="{fgcol}", color="#000000"'
    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + hashlib.md5(name.encode()).hexdigest() + "." + gum.config["notebook", "graph_format"]
      proba_histogram.saveFigProba(ie.posterior(name), filename, bgcolor=bgcol)
      dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
    else:
      dotstr += f' "{name}" [{colorattribute}]'

  for a in showdag.arcs():
    (n, j) = a
    pw = 1
    av = f"{n}&nbsp;&rarr;&nbsp;{j}"
    col = gumcols.getBlackInTheme()

    if arcWidth is not None and a in arcWidth:
      if maxarcs != minarcs:
        pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
      av = f"{n}&nbsp;&rarr;&nbsp;{j} : {arcWidth[a]}"

    if arcColor is not None and a in arcColor:
      col = gumcols.proba2color(arcColor[a], cmapArc)

    dotstr += (
      f' "{ctbn.variable(n).name()}"->"{ctbn.variable(j).name()}" [penwidth="{pw}",tooltip="{av}",color="{col}"];'
    )

  dotstr += "}"

  g = dot.graph_from_dot_data(dotstr)[0]

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g


def getCtbnInference(ctbn: CTBN, engine, targets=None, size=None):
  if size is None:
    size = gum.config["ctbn", "default_graph_size"]

  return gnb.getGraph(CTBNinference2dot(ctbn, engine, targets=targets, size=size))


def showCtbnInference(ctbn: CTBN, engine, targets=None, size=None):
  gnb._showGraph(getCtbnInference(ctbn, engine, targets=targets, size=size))


CTBN._repr_html_ = lambda ctbn: getCtbnGraph(ctbn)
