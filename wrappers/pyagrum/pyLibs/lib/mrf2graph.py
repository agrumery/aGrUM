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
The purpose of this module is to provide tools for mapping Markov random field (and inference) in dot language in order to
be displayed/saved as image.
"""

import time
import hashlib

import matplotlib.colors
import matplotlib.pyplot as plt
import pydot as dot

import pyagrum
import pyagrum.lib.dotstyle as gds

from .proba_histogram import saveFigProba


def MRF2UGdot(
  mrf: pyagrum.MarkovRandomField,
  size: float | str | None = None,
  nodeColor: dict[int, float] | None = None,
  edgeWidth: dict[tuple[int, int], float] | None = None,
  edgeLabel: dict[tuple[int, int], str] | None = None,
  edgeColor: dict[tuple[int, int], float] | None = None,
  cmapNode: matplotlib.colors.Colormap | None = None,
  cmapEdge: matplotlib.colors.Colormap | None = None,
  showMsg: dict | None = None,
) -> dot.Dot:
  """
  Create a pydot representation of the Markov random field as an undirected graph

  Parameters
  ----------
  mrf: pyagrum.MarkovRandomField
    The Markov random field
  size : int |str
    Size of the rendered graph (by default, the one of the configuration)
  nodeColor : dict[int,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  edgeWidth : dict[Tuple(int,int),float]
    a edgeMap of values to be shown as width of edges
  edgeLabel: dict[Tuple(int,int),str]
    an edgeMap of labels to be shown next to edges
  edgeColor: dict[Tuple(int,int),float]
    a edgeMap of values to be shown as color of edges
  cmapNode : matplotlib.color.colormap
    color map to show the vals of Nodes
  cmapEdge : matplotlib.color.colormap
    color map to show the vals of Edges.
  showMsg : dict[int,str]
     a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the MRF as a dot graph
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap(pyagrum.config["notebook", "node_cmap"])

  if cmapEdge is None:
    cmapEdge = plt.get_cmap(pyagrum.config["notebook", "edge_cmap"])

  # the look comes from the configuration (written by the toDot of the MRF), the values by the options
  options = pyagrum.DotOptions()
  options.setSize(str(pyagrum.config["notebook", "graphSize"] if size is None else size))
  gds.setNodesOptions(options, mrf, nodeColor, cmapNode, showMsg)
  gds.setLinksOptions(options, mrf.edges(), edgeWidth, edgeLabel, edgeColor, cmapEdge, "&mdash;")

  return gds.dotFromAgrum(mrf.toDot(options))


def MRF2FactorGraphdot(
  mrf: pyagrum.MarkovRandomField,
  size: float | str | None = None,
  nodeColor: dict[str, float] | None = None,
  factorColor=None,
  cmapNode: matplotlib.colors.Colormap | None = None,
  showMsg: dict[str, str] | None = None,
) -> dot.Dot:
  """
  Create a pydot representation of the Markov random field as a factor graph

  Parameters
  ----------
  mrf: pyagrum.MarkovRandomField
    the model
  size: float|str
    the size of the rendered graph
  nodeColor: dict[str,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor:
    a function returning a value (between 0 and 1) to be shown as a color of factor.
  cmapNode: matplotlib.colors.Colormap
    colormap for nodes
  showMsg: dict[str,str]
    a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the MRF as a dot graph
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap(pyagrum.config["notebook", "node_cmap"])

  # the look comes from the configuration (written by the toDot of the MRF), the values by the options
  options = pyagrum.DotOptions()
  options.setSize(str(pyagrum.config["notebook", "graphSize"] if size is None else size))
  gds.setNodesOptions(options, mrf, nodeColor, cmapNode, showMsg)
  if factorColor is not None:
    for f in mrf.factors():
      options.setFactorBgColor(f, gds.proba2bgcolor(factorColor(f), cmapNode))

  return gds.dotFromAgrum(mrf.toDotAsFactorGraph(options))


def MRFinference2UGdot(
  mrf: pyagrum.MarkovRandomField,
  size: float | str | None = None,
  engine: pyagrum.MRFInference | None = None,
  evs: dict | None = None,
  targets: set | None = None,
  nodeColor: dict | None = None,
  factorColor=None,
  arcWidth: dict | None = None,
  arcColor: dict | None = None,
  cmapNode: matplotlib.colors.Colormap | None = None,
  cmapArc: matplotlib.colors.Colormap | None = None,
  view=None,
) -> dot.Dot:
  """
  Create a pydot representation of an inference in a MRF as an undirected graph.

  Parameters
  ----------
  mrf : pyagrum.MarkovRandomField
      The Markov random field.
  size : str, optional
      Size of the rendered graph.
  engine : pyagrum inference engine, optional
      Inference algorithm used. If None, ShaferShenoyMRFInference will be used.
  evs : dict, optional
      Map of evidence.
  targets : set, optional
      Set of targets. If targets={} then each node is a target.
  nodeColor : dict, optional
      A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth : dict, optional
      A arcMap of values to be shown as bold arcs.
  arcColor : dict, optional
      A arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode : matplotlib.colors.Colormap, optional
      Color map to show the node values.
  cmapArc : matplotlib.colors.Colormap, optional
      Color map to show the arc values.

  Returns
  -------
  pydot.Dot
      The desired representation of the inference.
  """
  if evs is None:
    evs = {}
  if targets is None:
    targets = set()

  if cmapNode is None:
    cmapNode = plt.get_cmap(pyagrum.config["notebook", "node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(pyagrum.config["notebook", "arc_cmap"])

  # default
  minarcs = 0
  maxarcs = 100

  if arcWidth is not None:
    minarcs = min(arcWidth.values())
    maxarcs = max(arcWidth.values())

  startTime = time.time()
  if engine is None:
    ie = pyagrum.ShaferShenoyMRFInference(mrf)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  from tempfile import mkdtemp

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = 'graph structs {\n  fontcolor="' + gds.getBlackInTheme() + '";bgcolor="transparent";'

  if pyagrum.config.typed["notebook", "show_inference_time"]:
    dotstr += f'  label="Inference in {1000 * (stopTime - startTime):6.2f}ms";\n'

  fontname, fontsize = gds.fontFromMatplotlib()
  dotstr += f'  node [fillcolor="{pyagrum.config["notebook", "nodeBgColor"]}", style=filled,color="{gds.configFgColor("notebook", "nodeFgColor")}",fontname="{fontname}",fontsize="{fontsize}"];\n'
  dotstr += f'  edge [color="{gds.getBlackInTheme()}"];\n'

  for nid in mrf.nodes():
    name = mrf.variable(nid).name()

    # defaults
    bgcol = pyagrum.config["notebook", "nodeBgColor"]
    fgcol = gds.configFgColor("notebook", "nodeFgColor")
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = pyagrum.config["notebook", "figure_facecolor"]

    if nodeColor is not None and (name in nodeColor or nid in nodeColor):
      bgcol = gds.proba2bgcolor(nodeColor[name], cmapNode)
      fgcol = gds.proba2fgcolor(nodeColor[name], cmapNode)

    # 'hard' colour for evidence (?)
    if nid in ie.hardEvidenceNodes() | ie.softEvidenceNodes():
      bgcol = pyagrum.config["notebook", "evidence_bgcolor"]
      fgcol = pyagrum.config["notebook", "evidence_fgcolor"]

    colorattribute = f'fillcolor="{bgcol}", fontcolor="{fgcol}", color="#000000"'

    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + hashlib.md5(name.encode()).hexdigest() + "." + pyagrum.config["notebook", "graph_format"]
      saveFigProba(ie.posterior(name), filename, bgcolor=bgcol)
      dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
    else:
      dotstr += f' "{name}" [{colorattribute}]'

  for a in mrf.edges():
    (n, j) = a
    pw = 1.0
    av = f"{n}&nbsp;&mdash;&nbsp;{j}"
    col = gds.getBlackInTheme()

    if arcWidth is not None and (n, j) in arcWidth:
      if maxarcs != minarcs:
        pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
      av = f"{n}&nbsp;&mdash;&nbsp;{j} : {arcWidth[a]}"

    if arcColor is not None and a in arcColor:
      col = gds.proba2color(arcColor[a], cmapArc)

    dotstr += f' "{mrf.variable(n).name()}"--"{mrf.variable(j).name()}" [penwidth="{pw}",tooltip="{av}",color="{col}"];'
  dotstr += "}"

  _graphs = dot.graph_from_dot_data(dotstr)
  assert _graphs is not None
  g = _graphs[0]

  if size is None:
    size = pyagrum.config["notebook", "graph_inference_size"]
  g.set_size(str(size))
  g.temp_dir = temp_dir

  return gds.prepareDot(g)


def MRFinference2FactorGraphdot(
  mrf: pyagrum.MarkovRandomField,
  size: float | str | None = None,
  engine: pyagrum.MRFInference | None = None,
  evs: dict | None = None,
  targets: set | None = None,
  nodeColor: dict | None = None,
  factorColor=None,
  cmapNode: matplotlib.colors.Colormap | None = None,
) -> dot.Dot:
  """
  Create a pydot representation of an inference in a MRF as a factor graph.

  Parameters
  ----------
  mrf : pyagrum.MarkovRandomField
      The Markov random field.
  size : str, optional
      Size of the rendered graph.
  engine : pyagrum inference engine, optional
      Inference algorithm used. If None, ShaferShenoyMRFInference will be used.
  evs : dict, optional
      Map of evidence.
  targets : set, optional
      Set of targets. If targets={} then each node is a target.
  nodeColor : dict, optional
      A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  cmapNode : matplotlib.colors.Colormap, optional
      Color map to show the node values.

  Returns
  -------
  pydot.Dot
      The desired representation of the inference.
  """
  if evs is None:
    evs = {}
  if targets is None:
    targets = set()
  if cmapNode is None:
    cmapNode = plt.get_cmap(pyagrum.config["notebook", "node_cmap"])

  startTime = time.time()
  if engine is None:
    ie = pyagrum.ShaferShenoyMRFInference(mrf)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  from tempfile import mkdtemp

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:
  dotstr = f'''graph{{ 
    layout="{pyagrum.config["factorgraph", "graphLayout"]}";
    fontcolor="{gds.getBlackInTheme()}";
    bgcolor="transparent";
  '''

  if pyagrum.config.typed["notebook", "show_inference_time"]:
    dotstr += f'  label="Inference in {1000 * (stopTime - startTime):6.2f}ms";\n'

  dotstr += (
    '  node [fillcolor="'
    + pyagrum.config["notebook", "nodeBgColor"]
    + '", style=filled,color="'
    + gds.configFgColor("notebook", "nodeFgColor")
    + '"];'
    + "\n"
  )
  dotstr += '  edge [color="' + gds.getBlackInTheme() + '"];' + "\n"

  for nid in mrf.nodes():
    name = mrf.variable(nid).name()

    # defaults
    bgcol = pyagrum.config["notebook", "nodeBgColor"]
    fgcol = gds.configFgColor("notebook", "nodeFgColor")
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = pyagrum.config["notebook", "figure_facecolor"]

    if nodeColor is not None and (name in nodeColor or nid in nodeColor):
      bgcol = gds.proba2bgcolor(nodeColor[name], cmapNode)
      fgcol = gds.proba2fgcolor(nodeColor[name], cmapNode)

    # 'hard' colour for evidence (?)
    if nid in ie.hardEvidenceNodes() | ie.softEvidenceNodes():
      bgcol = pyagrum.config["notebook", "evidence_bgcolor"]
      fgcol = pyagrum.config["notebook", "evidence_fgcolor"]

    colorattribute = f'fillcolor="{bgcol}", fontcolor="{fgcol}", color="#000000"'
    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + hashlib.md5(name.encode()).hexdigest() + "." + pyagrum.config["notebook", "graph_format"]
      saveFigProba(ie.posterior(name), filename, bgcolor=bgcol)
      dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
    else:
      dotstr += f' "{name}" [shape=rectangle,margin=0.04,width=0,height=0,{colorattribute}];\n'

  def factorname(f):
    return '"f' + "#".join(map(str, sorted(list(f)))) + '"'

  for f in mrf.factors():
    if factorColor is None:
      bgcol = pyagrum.config["factorgraph", "factorBgColor"]
    else:
      bgcol = gds.proba2bgcolor(factorColor(f), cmapNode)
    dotstr += f"  {factorname(f)} [style=filled,fillcolor={bgcol},shape=point,width=0.1,height=0.1];\n"

  for f in mrf.factors():
    col = gds.getBlackInTheme()
    for n in f:
      dotstr += f' {factorname(f)}->"{mrf.variable(n).name()}" [tooltip="{f}:{n}",color="{col}",fillcolor="{bgcol}",len="{pyagrum.config["factorgraph", "edge_length_inference"]}"];\n'
  dotstr += "}"

  _graphs = dot.graph_from_dot_data(dotstr)
  assert _graphs is not None
  g = _graphs[0]

  if size is None:
    size = pyagrum.config["notebook", "graph_inference_size"]
  g.set_size(str(size))
  g.temp_dir = temp_dir

  return gds.prepareDot(g)
