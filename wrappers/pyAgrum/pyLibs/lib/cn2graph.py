"""
The purpose of this module is to provide tools for mapping credal Network (and inference) in dot language in order to
be displayed/saved as image.
"""

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

import time
import hashlib

from tempfile import mkdtemp

import matplotlib.pyplot as plt
import pydot as dot

import pyAgrum as gum
import pyAgrum.lib._colors as gumcols
from pyAgrum.lib.proba_histogram import saveFigProbaMinMax


def CN2dot(cn, size=None, nodeColor=None, arcWidth=None, arcLabel=None, arcColor=None, cmapNode=None, cmapArc=None, showMsg=None):
  """
  create a pydot representation of the Credal Network

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
  bn = cn.current_bn()
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(gum.config["notebook", "default_arc_cmap"])

  # default
  minarcs = 0
  maxarcs = 100

  if arcWidth is not None:
    minarcs = min(arcWidth.values())
    maxarcs = max(arcWidth.values())

  graph = dot.Dot(graph_type='digraph', bgcolor="transparent")

  for n in bn.names():
    if nodeColor is None or n not in nodeColor:
      bgcol = gum.config["credalnet", "default_node_bgcolor"]
      fgcol = gum.config["credalnet", "default_node_fgcolor"]
      res = ""
    else:
      bgcol = gumcols.proba2bgcolor(nodeColor[n], cmapNode)
      fgcol = gumcols.proba2fgcolor(nodeColor[n], cmapNode)
      res = f" : {nodeColor[n] if showMsg is None else showMsg[n]:2.5f}"

    node = dot.Node('"' + n + '"', style="filled",
                    shape="polygon",
                    sides="7",
                    peripheries="1",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    tooltip=f'"({bn.idFromName(n)}) {n}{res}"'
                    )
    graph.add_node(node)

  for a in bn.arcs():
    (n, j) = a
    pw = 1
    av = f"{n}&nbsp;&rarr;&nbsp;{j}"
    col = gumcols.getBlackInTheme()
    lb = ""

    if arcWidth is not None:
      if a in arcWidth:
        if maxarcs != minarcs:
          pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
        av = f"{n}&nbsp;&rarr;&nbsp;{j} : {arcWidth[a]}"

    if arcColor is not None:
      if a in arcColor:
        col = gumcols.proba2color(arcColor[a], cmapArc)

    if arcLabel is not None:
        if a in arcLabel:
            lb=arcLabel[a]

    edge = dot.Edge('"' + bn.variable(a[0]).name() + '"', '"' + bn.variable(a[1]).name() + '"',
                    label=lb, fontsize="10",
                    penwidth=pw, color=col,
                    tooltip=av
                    )
    graph.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  # dynamic member makes pylink unhappy
  # pylint: disable=no-member
  graph.set_size(size)
  return graph


def CNinference2dot(cn, size=None, engine=None, evs=None, targets=None, nodeColor=None, arcWidth=None, arcColor=None,
                    cmapNode=None, cmapArc=None, dag=None
                    ):
  """
  create a pydot representation of an inference in a BN

  Parameters
  ----------
    cn : pyAgrum.CredalNet
      the credal network
    size: str
      size of the rendered graph
    engine:  pyAgrum.Inference
      inference algorithm used. If None, LazyPropagation will be used
    evs: dict
      map of evidence
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
    dag : pyAgrum.DAG
      only shows nodes that have their id in the dag (and not in the whole BN)

  Returns
  -------
    the desired representation of the inference
  """
  if evs is None:
    evs = {}
  if targets is None:
    targets = {}
  bn = cn.current_bn()
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(gum.config["notebook", "default_arc_cmap"])

  # by default
  minarcs = 0
  maxarcs = 100

  if arcWidth is not None:
    minarcs = min(arcWidth.values())
    maxarcs = max(arcWidth.values())

  startTime = time.time()
  if engine is None:
    ie = gum.CNMonteCarloSampling(cn)
  else:
    ie = engine
  # ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "digraph structs {\n  fontcolor=\"" + \
           gumcols.getBlackInTheme() + "\";bgcolor=\"transparent\";"

  if gum.config.asBool["notebook", "show_inference_time"]:
    dotstr += f"  label=\"Inference in {1000 * (stopTime - startTime):6.2f}ms\";\n"

  fontname, fontsize = gumcols.fontFromMatplotlib()
  dotstr += f'  node [fillcolor="{gum.config["notebook", "default_node_bgcolor"]}", style=filled,color="{gum.config["notebook", "default_node_fgcolor"]}",fontname="{fontname}",fontsize="{fontsize}"];\n'
  dotstr += f'  edge [color="{gumcols.getBlackInTheme()}"];\n'

  showdag = bn.dag() if dag is None else dag
  for nid in showdag.nodes():
    name = bn.variable(nid).name()

    # defaults
    bgcol = gum.config["notebook", "default_node_bgcolor"]
    fgcol = gum.config["notebook", "default_node_fgcolor"]
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = gum.config["notebook", "figure_facecolor"]

    if nodeColor is not None:
      if name in nodeColor or nid in nodeColor:
        bgcol = gumcols.proba2bgcolor(nodeColor[name], cmapNode)
        fgcol = gumcols.proba2fgcolor(nodeColor[name], cmapNode)

    # 'hard' colour for evidence (?)
    if name in evs or nid in evs:
      bgcol = gum.config["notebook", "evidence_bgcolor"]
      fgcol = gum.config["notebook", "evidence_fgcolor"]

    colorattribute = f'fillcolor="{bgcol}", fontcolor="{fgcol}", color="#000000"'
    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + \
                 hashlib.md5(name.encode()).hexdigest() + "." + \
                 gum.config["notebook", "graph_format"]
      saveFigProbaMinMax(ie.marginalMin(name), ie.marginalMax(name), filename, bgcolor=bgcol)
      dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
    else:
      dotstr += f' "{name}" [shape=polygon,sides=7,peripheries=1,{colorattribute}]'

  for a in showdag.arcs():
    (n, j) = a
    pw = 1
    av = f"{n}&nbsp;&rarr;&nbsp;{j}"
    col = gumcols.getBlackInTheme()

    if arcWidth is not None:
      if a in arcWidth:
        if maxarcs != minarcs:
          pw = 0.1 + 5 * (arcWidth[n, j] - minarcs) / (maxarcs - minarcs)
        av = f"{n}&nbsp;&rarr;&nbsp;{j} : {arcWidth[a]}"

    if arcColor is not None:
      if a in arcColor:
        col = gumcols.proba2color(arcColor[n, j], cmapArc)

    dotstr += f' "{bn.variable(n).name()}"->"{bn.variable(j).name()}" [penwidth="{pw}",tooltip="{av}",color="{col}"];'

  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g
