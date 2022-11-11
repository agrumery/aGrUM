"""
The purpose of this module is to provide tools for mapping Markov network (and inference) in dot language in order to
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

import matplotlib.pyplot as plt
import pydot as dot

import pyAgrum as gum
import pyAgrum.lib._colors as gumcols

from .proba_histogram import saveFigProba


def MN2UGdot(mn, size="4", nodeColor=None, edgeWidth=None, edgeLabel=None, edgeColor=None, cmapNode=None, cmapEdge=None, showMsg=None):
  """
  Create a pydot representation of the Markov Network as an undirected graph

  Parameters
  ----------
  mn : pyAgrum.MarkovNet
    The Markov network
  size : int |str
    Size of the rendered graph
  nodeColor : Dict[int,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  edgeWidth : Dict[Tuple(int,int),float]
    a edgeMap of values to be shown as width of edges
  edgeLabel: Dict[Tuple(int,int),str]
    an edgeMap of labels to be shown next to edges
  edgeColor: Dict[Tuple(int,int),float]
    a edgeMap of values to be shown as color of edges
  cmapNode : matplotlib.color.colormap
    color map to show the vals of Nodes
  cmapEdge : matplotlib.color.colormap
    color map to show the vals of Edges.
  showMsg : Dict[int,str]
     a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the MN as a dot graph
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapEdge is None:
    cmapEdge = plt.get_cmap(gum.config["notebook", "default_edge_cmap"])

  # default
  maxedges = 100
  minedges = 0

  if edgeWidth is not None:
    minedges = min(edgeWidth.values())
    maxedges = max(edgeWidth.values())

  graph = dot.Dot(graph_type='graph', bgcolor="transparent")

  for n in mn.names():
    if nodeColor is None or n not in nodeColor:
      bgcol = gum.config["notebook", "default_node_bgcolor"]
      fgcol = gum.config["notebook", "default_node_fgcolor"]
      res = ""
    else:
      bgcol = gumcols.proba2bgcolor(nodeColor[n], cmapNode)
      fgcol = gumcols.proba2fgcolor(nodeColor[n], cmapNode)
      res = f" : {nodeColor[n] if showMsg is None else showMsg[n]:2.5f}"

    node = dot.Node('"' + n + '"', style="filled",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    tooltip=f'"({mn.idFromName(n)}) {n}{res}"'
                    )
    graph.add_node(node)

  for a in mn.edges():
    (n, j) = a
    pw = 1
    av = f"{n}&nbsp;&mdash;&nbsp;{j}"
    col = gumcols.getBlackInTheme()
    lb = ""

    if edgeWidth is not None:
      if a in edgeWidth:
        if maxedges != minedges:
          pw = 0.1 + 5 * (edgeWidth[a] - minedges) / (maxedges - minedges)
        av = f"{n}&nbsp;&mdash;&nbsp;{j} : {edgeWidth[a]}"

    if edgeColor is not None:
      if a in edgeColor:
        col = gumcols.proba2color(edgeColor[a], cmapEdge)

    if edgeLabel is not None :
        if a in edgeLabel:
            lb=edgeLabel[a]

    edge = dot.Edge('"' + mn.variable(a[0]).name() + '"',
                    '"' + mn.variable(a[1]).name() + '"',
                    label=lb, fontsize="10",
                    penwidth=pw, color=col,
                    tooltip=av
                    )
    graph.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  graph.set_size(size)
  return graph


def MN2FactorGraphdot(mn, size=None, nodeColor=None, factorColor=None, cmapNode=None, showMsg=None):
  """
  Create a pydot representation of the Markov Network as a factor graph

  Parameters
  ----------
  mn: pyAgrum.MarkovNet
    the model
  size: float|str
    the size of the rendered graph
  nodeColor: Dict[str,float]
    a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  factorColor:
    a function returning a value (between 0 and 1) to be shown as a color of factor.
  cmapNode: matplotlib.colors.Colormap
    colormap for nodes
  showMsg: Dict[str,str]
    a nodeMap of values to be shown as tooltip

  Returns
  -------
  pydot.Dot
    the desired representation of the MN as a dot graph
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  graph = dot.Dot(graph_type='graph', bgcolor="transparent", layout="neato")

  for n in mn.names():
    if nodeColor is None or n not in nodeColor:
      bgcol = gum.config["factorgraph", "default_node_bgcolor"]
      fgcol = gum.config["factorgraph", "default_node_fgcolor"]
      res = ""
    else:
      bgcol = gumcols.proba2bgcolor(nodeColor[n], cmapNode)
      fgcol = gumcols.proba2fgcolor(nodeColor[n], cmapNode)
      res = f" : {nodeColor[n] if showMsg is None else showMsg[n]:2.5f}"

    node = dot.Node('"' + n + '"',
                    style="filled",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    shape="rectangle",
                    margin=0.04, width=0, height=0,
                    tooltip=f'"({mn.idFromName(n)}) {n}{res}"'
                    )
    graph.add_node(node)

  def factorname(factor):
    return '"f' + "#".join(map(str, sorted(list(factor)))) + '"'

  for f in mn.factors():
    if factorColor is None:
      bgcol = gum.config["factorgraph", "default_factor_bgcolor"]
    else:
      bgcol = gumcols.proba2bgcolor(factorColor(f), cmapNode)
    node = dot.Node(factorname(f),
                    style="filled",
                    fillcolor=bgcol,
                    shape="point",
                    width=0.1,
                    height=0.1
                    )
    graph.add_node(node)

  for f in mn.factors():
    for n in f:
      edge = dot.Edge(factorname(f),
                      '"' + mn.variable(n).name() + '"',
                      color=gumcols.getBlackInTheme(),
                      len=gum.config["factorgraph", "edge_length"]
                      )
      graph.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  graph.set_size(size)
  return graph


def MNinference2UGdot(mn, size=None, engine=None, evs=None, targets=None, nodeColor=None, factorColor=None,
                      arcWidth=None, arcColor=None,
                      cmapNode=None, cmapArc=None, view=None
                      ):
  """
  create a pydot representation of an inference in a MN as an UG

  :param pyAgrum.MarkovNet mn:
  :param string size: size of the rendered graph
  :param pyAgrum Inference engine: inference algorithm used. If None, ShaferShenoyMNInference will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets. If targets={} then each node is a target
  :param nodeColor: a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  :param arcWidth: a arcMap of values to be shown as bold arcs
  :param arcColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmapNode: color map to show the vals of Nodes
  :param cmapArc: color map to show the vals of Arcs

  :return: the desired representation of the inference
  """
  if evs is None:
    evs={}
  if targets is None:
    targets={}

  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(gum.config["notebook", "default_arc_cmap"])

  # default
  minarcs=0
  maxarcs=100

  if arcWidth is not None:
    minarcs = min(arcWidth.values())
    maxarcs = max(arcWidth.values())

  startTime = time.time()
  if engine is None:
    ie = gum.ShaferShenoyMNInference(mn)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  from tempfile import mkdtemp
  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "graph structs {\n  fontcolor=\"" + \
           gumcols.getBlackInTheme() + "\";bgcolor=\"transparent\";"

  if gum.config.asBool["notebook", "show_inference_time"]:
    dotstr += f'  label="Inference in {1000 * (stopTime - startTime):6.2f}ms";\n'

  dotstr += '  node [fillcolor="' + gum.config["notebook", "default_node_bgcolor"] + \
            '", style=filled,color="' + \
            gum.config["notebook", "default_node_fgcolor"] + '"];' + "\n"
  dotstr += '  edge [color="' + gumcols.getBlackInTheme() + '"];' + "\n"

  for nid in mn.nodes():
    name = mn.variable(nid).name()

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
      saveFigProba(ie.posterior(name), filename, bgcolor=bgcol)
      dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
    else:
      dotstr += f' "{name}" [{colorattribute}]'

  for a in mn.edges():
    (n, j) = a
    pw = 1
    av =f"{n}&nbsp;&mdash;&nbsp;{j}"
    col = gumcols.getBlackInTheme()
    lb=""

    if arcWidth is not None:
      if (n, j) in arcWidth:
        if maxarcs != minarcs:
          pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
        av = f"{n}&nbsp;&mdash;&nbsp;{j} : {arcWidth[a]}"

    if arcColor is not None:
      if a in arcColor:
        col = gumcols.proba2color(arcColor[a], cmapArc)

    dotstr += f' "{mn.variable(n).name()}"--"{mn.variable(j).name()}" [penwidth="{pw}",tooltip="{av}",color="{col}"];'
  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g


def MNinference2FactorGraphdot(mn, size=None, engine=None, evs=None, targets=None,
                               nodeColor=None, factorColor=None,
                               cmapNode=None
                               ):
  """
  create a pydot representation of an inference in a MN as an factor graph

  :param pyAgrum.MarkovNet mn:
  :param string size: size of the rendered graph
  :param pyAgrum Inference engine: inference algorithm used. If None, ShaferShenoyMNInference will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets. If targets={} then each node is a target
  :param nodeColor: a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  :param cmapNode: color map to show the vals of Nodes

  :return: the desired representation of the inference
  """
  if evs is None:
    evs = {}
  if targets is None:
    targets = {}
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  startTime = time.time()
  if engine is None:
    ie = gum.ShaferShenoyMNInference(mn)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  from tempfile import mkdtemp
  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "graph {\n  layout=\"neato\";\n  fontcolor=\"" + \
           gumcols.getBlackInTheme() + "\";bgcolor=\"transparent\";"

  if gum.config.asBool["notebook", "show_inference_time"]:
    dotstr += f'  label="Inference in {1000 * (stopTime - startTime):6.2f}ms";\n'

  dotstr += '  node [fillcolor="' + gum.config["notebook", 'default_node_bgcolor'] + \
            '", style=filled,color="' + \
            gum.config["notebook", "default_node_fgcolor"] + '"];' + "\n"
  dotstr += '  edge [color="' + gumcols.getBlackInTheme() + '"];' + "\n"

  for nid in mn.nodes():
    name = mn.variable(nid).name()

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
      saveFigProba(ie.posterior(name), filename, bgcolor=bgcol)
      dotstr += f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
    else:
      dotstr += f' "{name}" [shape=rectangle,margin=0.04,width=0,height=0,{colorattribute}];\n'

  def factorname(f):
    return '"f' + "#".join(map(str, sorted(list(f)))) + '"'

  for f in mn.factors():
    if factorColor is None:
      bgcol = gum.config["factorgraph", "default_factor_bgcolor"]
    else:
      bgcol = gumcols.proba2bgcolor(factorColor(f), cmapNode)
    dotstr += f'  {factorname(f)} [style=filled,fillcolor={bgcol},shape=point,width=0.1,height=0.1];\n'

  for f in mn.factors():
    col = gumcols.getBlackInTheme()
    for n in f:
      dotstr += f' {factorname(f)}->"{mn.variable(n).name()}" [tooltip="{f}:{n}",color="{col}",fillcolor="{bgcol}",len="{gum.config["factorgraph", "edge_length_inference"]}"];\n'
  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g
