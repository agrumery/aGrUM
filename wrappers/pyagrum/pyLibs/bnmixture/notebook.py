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

import IPython
import hashlib
import time
import csv
import math

import pyagrum as gum
import pyagrum.lib.notebook as gnb
import pyagrum.lib._colors as gumcols
import pydot as dot
import matplotlib.pyplot as plt
import matplotlib as mpl
import pyagrum.bnmixture as BNM
import numpy as np

from pyagrum.lib import proba_histogram
from tempfile import mkdtemp
from matplotlib.colors import LinearSegmentedColormap
from matplotlib.backends.backend_agg import FigureCanvasAgg as fc
from base64 import encodebytes
from math import ceil

_colors = ["lightgreen", "lightseagreen", "green"]
_cmap1 = LinearSegmentedColormap.from_list("mycmap", _colors)


def _compareBN(dotref: dot.Dot, bncmp: dot.Dot) -> dot.Dot:
  """
  Allow to create a dot representation of a BN with the nodes at the same position as in ``dotref``.

  Notes
  -----
  Considering that all the BNs of a BNMixture have the same variables, it is not necessary to have a reference BN in parameters.

  Parameters
  ----------
  dotref : pydot.Dot
      pydot graph to get positions from.
  bncmp : pyagrum.BayesNet
      BN to get pydot representation for.

  Returns
  -------
  pydot.Dot
      pydot representation of Bayesian Network ``bncmp`` with nodes at same position as ``dotref`` nodes.
  """
  g = dotref

  # loading positions
  positions = {l[1]: f"{l[2]},{l[3]}!"
               for l in csv.reader(g.create(format="plain")
                                   .decode("utf8")
                                   .split("\n")
                                   , delimiter=' ', quotechar='"')
               if len(l) > 3 and l[0] == "node"}

  res = dot.Dot(graph_type='digraph', bgcolor="transparent", layout="fdp", splines=True)

  # adding nodes
  for i1 in bncmp.nodes():
    res.add_node(dot.Node(f'"{bncmp.variable(i1).name()}"',
                          style="filled",
                          fillcolor=gum.config["bnmixture", "default_node_bgcolor"],
                          fontcolor=gum.config["bnmixture", "default_node_fgcolor"],
                          pos=positions[bncmp.variable(i1).name()]
                          ))

  # adding arcs
  for (i1, i2) in bncmp.arcs():
    n1 = bncmp.variable(i1).name()
    n2 = bncmp.variable(i2).name()
    res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                          style=gum.config["bnmixture", "default_arc_style"],
                          color=gum.config["bnmixture", "default_arc_color"],
                          constraint="false"))

  return res


def _compareBNinf(bnref: gum.BayesNet, refdot: dot.Dot, cmpDot: dot.Dot, scale=1.0):
  """
  Allow to modify a pydot graph of inference to have its nodes at the same positions as the nodes in the reference graph.

  Parameters
  ----------
  bnref : pyagrum.BayesNet
      BN used as a reference to get name of variables.
  refdot : pydot.Dot
      dot used as a reference to get the position of the nodes.
  cmpDot : pydot.Dot
      dot of the graph to modify to have the nodes at same position as the nodes in ``refdot``.
  """
  # nop2 layout allows to give positions to nodes
  cmpDot.set("layout", "nop2")
  cmpDot.set("splines", "true")

  # loading positions of reference
  # positions are scaled
  x_scale = 80 * scale * 1.5  # 50 * scale * 2
  y_scale = 60 * scale * 1.3  # 50 * scale * 1.5
  positions = {l[1]: f"{str(float(l[2]) * x_scale)},{str(float(l[3]) * y_scale)}!"
               for l in csv.reader(refdot.create(format="plain")
                                   .decode("utf8")
                                   .split("\n")
                                   , delimiter=' ', quotechar='"')
               if len(l) > 3 and l[0] == "node"}

  # modifying positions
  for node in cmpDot.get_nodes():
    name = node.get_name()
    # converts a dot name to a normal name
    # necessary because dot name format is ""<name>"".
    namecut = name[1:-1]
    if namecut not in bnref.names():
      continue
    node.set("pos", positions[namecut])


def getMixtureGraph(bnm: BNM.IMixture, size=None, ref=False):
  """
  HTML representation of a Mixture.

  Parameters
  ----------
  bnm : pyagrum.bnmixture.IMixture
      Mixture to get graph from.
  size : str | int
      Size of the graph.
  ref : bool
      if True, the representation will contain the reference BN's graph.
  """
  gnb.flow.clear()
  if size is None:
    size = gum.config['bnmixture', 'default_graph_size']

  dotref = gnb.BN2dot(bnm._refBN, size=size)
  if ref:
    gnb.flow.add(gnb.getGraph(dotref), caption=f"reference BN : {bnm._refName}")

  for bn in bnm.BNs():
    name = bn.property("name")
    gnb.flow.add(gnb.getGraph(_compareBN(dotref, bn), size=size), caption=f"{name}, w={bnm.weight(name)}")

  return gnb.flow.html()


def showMixtureGraph(bnm: BNM.IMixture, size=None, ref=False):
  """
  Display a HTML representation of a Mixture.

  Parameters
  ----------
  bnm : pyagrum.bnmixture.IMixture
      Mixture to get graph from.
  size : str | int
      Size of the graph.
  ref : bool
      if True, the representation will contain the reference BN's graph.
  """
  html = getMixtureGraph(bnm, size=size, ref=ref)
  IPython.display.display(html)


def BNMixtureInference2dot(bnm: BNM.BNMixture, engine=None, size=None, evs=None, targets=None, nodeColor=None,
                           arcWidth=None, arcColor=None,
                           cmapNode=None, cmapArc=None, dag=None):
  """

  Creates a HTML representation of the inference graph of a BNM.BNMixture (average of all posteriors in the BNMixture).

  Parameters
  ----------
  bnm : pyagrum.bnmixture.BNMixture
      The Bayesian Net Mixture used.
  size : str
      size of the rendered graph
  engine :  pyagrum.Inference
      inference algorithm used. If None, LazyPropagation will be used. Note : this is an unitialized class object.
  evs : dict
      map of evidence
  targets : set
      set of targets. If targets={} then each node is a target
  nodeColor : dict
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  arcWidth : dict
      a arcMap of values to be shown as bold arcs
  arcColor : dict
      a arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode : ColorMap
      color map to show the vals of Nodes
  cmapArc : ColorMap
      color map to show the vals of Arcs
  dag : pyagrum.DAG
      only shows nodes that have their id in the dag.
  """
  if evs is None:
    evs = {}
  if targets is None:
    targets = {}
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(gum.config["notebook", "default_arc_cmap"])

  # default
  maxarcs = 100
  minarcs = 0

  if arcWidth is not None:
    minarcs = min(arcWidth.values())
    maxarcs = max(arcWidth.values())

  startTime = time.time()
  if engine is None:
    ie = BNM.BNMixtureInference(bnm)
  else:
    ie = BNM.BNMixtureInference(bnm, engine=engine)

  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "digraph structs {\n  fontcolor=\"" + gumcols.getBlackInTheme() + "\";bgcolor=\"transparent\";"

  if gum.config.asBool["notebook", "show_inference_time"]:
    dotstr += f"  label=\"Inference in {1000 * (stopTime - startTime):6.2f}ms\";\n"

  fontname, fontsize = gumcols.fontFromMatplotlib()
  dotstr += f'  node [fillcolor="{gum.config["notebook", "default_node_bgcolor"]}", style=filled,color="{gum.config["notebook", "default_node_fgcolor"]}",fontname="{fontname}",fontsize="{fontsize}"];\n'
  dotstr += f'  edge [color="{gumcols.getBlackInTheme()}"];\n'

  showdag = bnm._refBN.dag() if dag is None else dag

  for nid in showdag.nodes():
    name = bnm._refBN.variable(nid).name()

    # defaults
    bgcol = gum.config["notebook", "default_node_bgcolor"]
    fgcol = gum.config["notebook", "default_node_fgcolor"]
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = gum.config["notebook", "figure_facecolor"]

    if nodeColor is not None and (name in nodeColor or nid in nodeColor):
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
      # proba_histogram.saveFigProba(ie.posterior(name), filename, bgcolor=bgcol)
      saveFigProba(ie, name, filename, bgcolor=bgcol, scale=float(gum.config['bnmixture', 'default_histo_scale']))
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

    dotstr += f' "{bnm._refBN.variable(n).name()}"->"{bnm._refBN.variable(j).name()}" [penwidth="{pw}",tooltip="{av}",color="{col}"];'

  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g


def BootstrapInference2dot(bnm: BNM.BootstrapMixture, size=None, engine=None, evs=None, targets=None, nodeColor=None,
                           arcWidth=None, arcColor=None,
                           cmapNode=None, cmapArc=None, dag=None, quantiles=False, show_mu_sigma=False):
  """
  create a pydot representation of an inference in a BootstrapMixture (reference BN's posterior is used, while other BNs are used to compute stats).

  Parameters
  ----------
  bnm : pyagrum.bnmixture.BootstrapMixture
      the Mixture.
  size: str
      size of the rendered graph
  engine:  pyagrum.Inference
      inference algorithm used. If None, LazyPropagation will be used. This is the class, not the initialized object.
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
  dag : pyagrum.DAG
      only shows nodes that have their id in the dag (and not in the whole BN)
  quantiles : bool
      if True, shows quantiles on tensors. Quantiles default values can be set using pyagrum.config.

  Returns
  -------
      the desired representation of the inference
  """
  if evs is None:
    evs = {}
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
    ie = BNM.BootstrapMixtureInference(bnm)
  else:
    ie = BNM.BootstrapMixtureInference(bnm, engine=engine)
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()

  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "digraph structs {\n  fontcolor=\"" + gumcols.getBlackInTheme() + "\";bgcolor=\"transparent\";"

  lab = ""
  if gum.config.asBool["notebook", "show_inference_time"]:
    lab += f"Inference in {1000 * (stopTime - startTime):6.2f}ms"

  if quantiles:
    q1 = float(gum.config['bnmixture', 'left_quantile']) * 100
    q2 = float(gum.config['bnmixture', 'right_quantile']) * 100
    title = f"\nquantiles=[{q1:.1f}%, {q2:.1f}%]"
    lab += f"\n{title}"
  dotstr += f"  label=\"{lab}\";\n"

  fontname, fontsize = gumcols.fontFromMatplotlib()
  dotstr += f'  node [fillcolor="{gum.config["notebook", "default_node_bgcolor"]}", style=filled,color="{gum.config["notebook", "default_node_fgcolor"]}",fontname="{fontname}",fontsize="{fontsize}"];\n'
  dotstr += f'  edge [color="{gumcols.getBlackInTheme()}"];\n'

  showdag = bnm._refBN.dag() if dag is None else dag

  for nid in showdag.nodes():
    name = bnm.variable(nid).name()

    # defaults
    bgcol = gum.config["notebook", "default_node_bgcolor"]
    fgcol = gum.config["notebook", "default_node_fgcolor"]
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = gum.config["notebook", "figure_facecolor"]

    if nodeColor is not None and (name in nodeColor or nid in nodeColor):
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
      saveFigProba(ie, name, filename, bgcolor=bgcol, quantiles=quantiles,
                   scale=float(gum.config['bnmixture', 'default_boot_histo_scale']), show_mu_sigma=show_mu_sigma)
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

    dotstr += f' "{bnm.variable(n).name()}"->"{bnm.variable(j).name()}" [penwidth="{pw}",tooltip="{av}",color="{col}"];'

  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)[0]

  # workaround for some badly parsed graph (pyparsing>=3.03)
  g.del_node('"\\n"')

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  g.temp_dir = temp_dir

  return g


def showBNMixtureInference(bnm: BNM.BNMixture, engine=None, size=None, evs=None, targets=None, nodeColor=None,
                           arcWidth=None, arcColor=None,
                           cmapNode=None, cmapArc=None, dag=None):
  """
  Displays a HTML representation of the inference graph of a BNM.BNMixture (average of all posteriors in the BNMixture).

  Parameters
  ----------
  bnm : pyagrum.bnmixture.BNMixture
      The Bayesian Net Mixture used.
  size : str
      size of the rendered graph
  engine :  pyagrum.Inference
      inference algorithm used. If None, LazyPropagation will be used. Note : this is an unitialized class object.
  evs : dict
      map of evidence
  targets : set
      set of targets. If targets={} then each node is a target
  nodeColor : dict
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  arcWidth : dict
      a arcMap of values to be shown as bold arcs
  arcColor : dict
      a arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode : ColorMap
      color map to show the vals of Nodes
  cmapArc : ColorMap
      color map to show the vals of Arcs
  dag : pyagrum.DAG
      only shows nodes that have their id in the dag.
  """
  html = BNMixtureInference2dot(bnm, engine=engine, size=size, evs=evs, targets=targets, nodeColor=nodeColor,
                                arcWidth=arcWidth,
                                arcColor=arcColor, cmapNode=cmapNode, cmapArc=cmapArc)
  refdot = gnb.BN2dot(bnm._refBN)
  _compareBNinf(bnm._refBN, refdot, html, scale=float(gum.config['bnmixture', 'default_histo_scale']))
  IPython.display.display(html)


def showBootstrapMixtureInference(bnm: BNM.BootstrapMixture, engine=None, size=None, evs=None, targets=None,
                                  nodeColor=None, arcWidth=None, arcColor=None,
                                  cmapNode=None, cmapArc=None, dag=None, quantiles=False, show_mu_sigma=False):
  """
  Displays a HTML representation of the inference graph of a BNM.BNMixture (average of all posteriors in the BNMixture).

  Parameters
  ----------
  bnm : pyagrum.bnmixture.BootstrapMixture
      The Bayesian Net Mixture used.
  size : str
      size of the rendered graph
  engine :  pyagrum.Inference
      inference algorithm used. If None, LazyPropagation will be used. Note : this is an unitialized class object.
  evs : dict
      map of evidence
  targets : set
      set of targets. If targets={} then each node is a target
  nodeColor : dict
      a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  arcWidth : dict
      a arcMap of values to be shown as bold arcs
  arcColor : dict
      a arcMap of values (between 0 and 1) to be shown as color of arcs
  cmapNode : ColorMap
      color map to show the vals of Nodes
  cmapArc : ColorMap
      color map to show the vals of Arcs
  dag : pyagrum.DAG
      only shows nodes that have their id in the dag.
  quantiles : bool
      if True, shows quantiles on tensors. Quantiles default values can be set using pyagrum.config.
  """
  html = BootstrapInference2dot(bnm, engine=engine, size=size, evs=evs, targets=targets, nodeColor=nodeColor,
                                arcWidth=arcWidth, arcColor=arcColor, cmapNode=cmapNode, cmapArc=cmapArc,
                                quantiles=quantiles, show_mu_sigma=show_mu_sigma)
  refdot = gnb.BN2dot(bnm._refBN)
  _compareBNinf(bnm._refBN, refdot, html, scale=float(gum.config['bnmixture', 'default_boot_histo_scale']))
  IPython.display.display(html)


def _normalizedArcsWeight(bnm: BNM.IMixture):
  """
  Counts arcs in the BNs of the mixture. The value of an arc is the weight of the BN containing it.
  Result is normalized.
  """
  countArcs = {nod1: {nod2: 0. for nod2 in bnm._refBN.names() if nod2 != nod1} for nod1 in bnm._refBN.names()}
  sum_weight = sum(bnm._weights.values())

  mi = 1
  ma = 0
  for bn_name in bnm.names():
    bn = bnm.BN(bn_name)
    w = bnm.weight(bn_name)
    for a, b in bn.arcs():
      tail = bn.variable(a).name()
      head = bn.variable(b).name()
      countArcs[tail][head] += w

  for n1 in bnm._refBN.names():
    for n2 in bnm._refBN.names():
      if n1 == n2:
        continue
      countArcs[n1][n2] = countArcs[n1][n2] / sum_weight
      if 0 != countArcs[n1][n2] < mi:
        mi = countArcs[n1][n2]
      if countArcs[n1][n2] > ma:
        ma = countArcs[n1][n2]

  return countArcs, mi, ma


def _compareArcs2dot(bnm: BNM.IMixture, size=None, refStruct=False):
  """
  Pydot representation of a graph that shows confidence value for every arc in the mixture.
  """
  countArcs, mi, ma = _normalizedArcsWeight(bnm)
  g = gnb.BN2dot(bnm._refBN, size=size)
  positions = {l[1]: f"{str(float(l[2]) * 2)},{str(float(l[3]) * 2)}!"
               for l in csv.reader(g.create(format="plain")
                                   .decode("utf8")
                                   .split("\n")
                                   , delimiter=' ', quotechar='"')
               if len(l) > 3 and l[0] == "node"}

  if refStruct:
    res = dot.Dot(graph_type='digraph', bgcolor="transparent", layout=gum.config["bnmixture", "default_layout"],
                  splines=True)
  else:
    res = dot.Dot(graph_type='digraph', bgcolor="transparent", layout=gum.config["bnmixture", "default_layout"],
                  splines=True, overlap_scaling=gum.config["bnmixture", "default_overlap"], sep=3)

  for vname in bnm._refBN.names():
    if refStruct:
      pos = positions[vname]
    else:
      pos = ''
    res.add_node(dot.Node(f'"{vname}"',
                          style="filled",
                          fillcolor=gum.config["bnmixture", "default_node_bgcolor"],
                          fontcolor=gum.config["bnmixture", "default_node_fgcolor"],
                          pos=pos
                          ))

  for n1 in bnm._refBN.names():
    for n2 in bnm._refBN.names():
      if n1 == n2 or countArcs[n1][n2] == 0:
        continue
      if bnm._refBN.existsArc(n1, n2):
        style = gum.config["bnmixture", "correct_arc_style"]
      else:
        style = gum.config["bnmixture", "incorrect_arc_style"]

      # print(f"({n1}, {n2}) {countArcs[n1][n2]}")
      col = gumcols.proba2color(min(countArcs[n1][n2], 0.99), _cmap1)
      res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                            style=style,
                            color=col,
                            penwidth=ceil(countArcs[n1][n2] * 6),
                            arrowhead=gum.config["bnmixture", "default_arrow_type"],
                            arrowsize=gum.config["bnmixture", "default_head_size"] * ceil(countArcs[n1][n2] * 6),
                            constraint="false"))

  return res, mi, ma

def getComparison(bnm, size=None, refStruct=False):
  """
  dot representation of a graph that shows confidence value for every arc in the mixture.

  Parameters
  ----------
  bnm : the mixture
  size : the size of the dot figure
  refStruct : do we use a reference structure

  Returns
  -------
  the dot representation
  """
  gr, _, _ = _compareArcs2dot(bnm, size=size, refStruct=refStruct)
  return gr

def showComparison(bnm, size=None, refStruct=False):
  """
  draw the graph that shows confidence representation for every arc in the mixture.

  Parameters
  ----------
  bnm : the mixture
  size : the size of the dot figure
  refStruct : do we use a reference structure
  """
  gnb.show(getComparison(bnm, size, refStruct=refStruct))


def getArcsComparison(bnm, size=None, refStruct=False):
  """
  html representation of a graph that shows confidence value for every arc in the mixture.
  Shows the confidence value of minimum and maximum on a confidence axis.
  """
  gr, mi, ma = _compareArcs2dot(bnm, size=size, refStruct=refStruct)
  gsvg = IPython.display.SVG(gr.create_svg(encoding="utf-8"))
  width = int(gsvg.data.split("width=")[1].split('"')[1].split("pt")[0]) / mpl.pyplot.rcParams[
    'figure.dpi']  # pixel in inches
  if width < 5:
    width = 5

  fig = mpl.figure.Figure(figsize=(width, 1))
  fig.patch.set_alpha(0)
  canvas = fc(fig)
  ax1 = fig.add_axes([0.05, 0.80, 0.9, 0.15])
  norm = mpl.colors.Normalize()
  cb1 = mpl.colorbar.ColorbarBase(ax1, cmap=_cmap1,
                                  norm=norm,
                                  orientation='horizontal'
                                  )
  cb1.set_label('Confidence')
  cb1.ax.text(0.1, -3, f"min {mi:.4f}", ha='left', va='top', color=gumcols.proba2bgcolor(max(mi, 0.01), _cmap1))
  cb1.ax.text(0.9, -3, f"max {ma:.4f}", ha='right', va='top', color=gumcols.proba2bgcolor(min(ma, .99), _cmap1))
  cb1.ax.text(mi, 1, "|", ha='center', va='top', color="red")
  cb1.ax.text(ma, 1, "|", ha='center', va='top', color="red")
  png = IPython.core.pylabtools.print_figure(canvas.figure, "png")  # from IPython.core.pylabtools
  png_legend = f"<img style='vertical-align:middle' src='data:image/png;base64,{encodebytes(png).decode('ascii')}'>"

  sss = f"<div align='center'>{gsvg.data}<br/>{png_legend}</div>"
  return sss


def showArcsComparison(bnm, size=None, refStruct=False):
  """
  Displays representation of a graph that shows confidence value for every arc in the mixture.
  Shows the confidence value of minimum and maximum on a confidence axis.
  """
  sss = getArcsComparison(bnm, size=size, refStruct=refStruct)
  return IPython.display.display(IPython.display.HTML(sss))


def arcsCompLegend():
  res = dot.Dot(graph_type='digraph', bgcolor="transparent", rankdir="LR")
  for i in "abcd":
    res.add_node(dot.Node(i, style="invis"))
  res.add_edge(dot.Edge("a", "b", label="Present in reference",
                        style=gum.config["bnmixture", "correct_arc_style"],
                        color=gum.config["bnmixture", "correct_arc_color"]))
  res.add_edge(dot.Edge("c", "d", label="Absent from reference",
                        style=gum.config["bnmixture", "incorrect_arc_style"],
                        color=gum.config["bnmixture", "correct_arc_color"]))

  return res


####################### tool box for quantiles #######################
def tensor2ref(ref, tens) -> "pyagrum.Tensor":
  """
  Returns a copy of ``tens`` but with a reference to ``ref`` 's variables instead. Allow to sum tensors that have the same variables
  but with different instantiations of them.

  Parameters
  ----------
  ref : pyagrum.Tensor
      Tensor containing variables of reference.
  tens : pyagrum.Tensor
      Tensor to convert.

  Returns
  -------
  pyagrum.Tensor
      The converted tensor with values of ``tens`` and variable references of ``ref``.
  """
  res = pyagrum.Tensor()
  for v in tens.names:
    res.add(ref.variable(v))
  return res.fillWith(tens)


def _stats(tens):
  """
  Returns mean and variance.
  """
  mu = 0.0
  mu2 = 0.0
  v = tens.variable(0)
  for i, p in enumerate(tens.tolist()):
    x = v.numerical(i)
    mu += p * x
    mu2 += p * x * x
  return mu, math.sqrt(mu2 - mu * mu)


def _getTitleHisto(p, show_mu_sigma=False):
  """
  Return title of a histogram.
  """
  var = p.variable(0)
  title = var.name()

  if show_mu_sigma:
    (mu, std) = _stats(p)
    if std > 0.0:
      title += f"\n$\\mu={mu:.2f}$; $\\sigma={std:.2f}$"

  return title


def _getProbaH(ie, var_name, scale=1.0, txtcolor="black", quantiles=False, show_mu_sigma=False):
  """
  Compute the representation of a horizontal histogram of a variable posterior

  Parameters
  ----------
  ie : IMixtureInference
      Inference of a mixture.
  var_name : str
      Name of the variable to get histogram for.
  scale : float
      scale for the size of histograms
  txtcolor : str
      color for text
  quantiles : bool
      shows quantiles.
  show_mu_sigma : bool
      shows mu and sigma.

  Returns
  -------
  matplotlib.Figure
      A matplotlib histogram of the posterior.
  """

  pots = ie._posteriors(var_name)
  for tens in pots.values():
    tens.normalize()
  avrg = ie.posterior(var_name)
  var = avrg.variable(0)
  ra = np.arange(var.domainSize())

  ra_reverse = np.arange(var.domainSize() - 1, -1, -1)  # reverse order

  vx = [var.label(int(i)) for i in ra_reverse]
  fig = plt.figure()
  fig.set_figheight(scale * var.domainSize() / 4.0)
  fig.set_figwidth(scale * 2)

  ax = fig.add_subplot(111)
  ax.set_facecolor('white')

  if gum.config.asBool['notebook', 'histogram_use_percent']:
    perc = 100
    suffix = "%"
  else:
    perc = 1
    suffix = ""

  if quantiles:
    pmin, pmax = ie.quantiles(var_name)
    vmin = pmin.tolist()
    vmin.reverse()
    vmax = pmax.tolist()
    vmax.reverse()
    vmean = avrg.tolist()
    vmean.reverse()
    error = [(abs(mean - mi), abs(ma - mean)) for mi, ma, mean in zip(vmin, vmax, vmean)]
    barmean = ax.barh(ra, vmean,
                      align='center',
                      height=float(gum.config['bnmixture', 'default_bar_height']),
                      color=gum.config['notebook', 'histogram_color'],
                      xerr=error,
                      capsize=float(gum.config['bnmixture', 'default_bar_capsize']) * scale)

    for b in barmean:
      txt = f"{b.get_width() * perc:.{gum.config.asInt['notebook', 'histogram_horizontal_visible_digits']}f}{suffix}"
      # ax.text(0.5, b.get_y(), txt, ha='center', va='bottom')
      if b.get_width() >= 0.2 * (2 / scale):
        ax.text(b.get_width(), b.get_y(), txt, ha='right', va='bottom', fontsize=10, color='white')
      else:
        ax.text(b.get_width(), b.get_y(), txt, ha='left', va='bottom', fontsize=10)

  else:
    vmean = avrg.tolist()
    vmean.reverse()
    barmean = ax.barh(ra, vmean,
                      align='center',
                      height=float(gum.config['bnmixture', 'default_bar_height']),
                      color=gum.config['notebook', 'histogram_color'])

    for b in barmean:
      txt = f"{b.get_width() * perc:.{gum.config.asInt['notebook', 'histogram_horizontal_visible_digits']}f}{suffix}"
      if b.get_width() >= 0.2 * (2 / scale):
        ax.text(b.get_width(), b.get_y(), txt, ha='right', va='bottom', fontsize=10, color='white')
      else:
        ax.text(b.get_width(), b.get_y(), txt, ha='left', va='bottom', fontsize=10)

  ax.set_xlim(0, 1)
  ax.set_yticks(np.arange(var.domainSize()))
  ax.set_yticklabels(vx, color=txtcolor)
  ax.set_xticklabels([])
  # ax.set_xlabel('Probability')
  # Even if utility, now we do show the mean/sigma of the distribution.
  ax.set_title(_getTitleHisto(avrg, show_mu_sigma=show_mu_sigma), color=txtcolor)
  ax.get_xaxis().grid(True)
  ax.margins(0)

  return fig


def proba2histo(ie, var_name, scale=1.0, txtcolor="Black", quantiles=False, show_mu_sigma=False):
  """
  Compute the representation of a horizontal histogram of a variable posterior

  Parameters
  ----------
  ie : IMixtureInference
      Inference of a mixture.
  var_name : str
      Name of the variable to get histogram for.
  scale : float
      scale for the size of histograms
  txtcolor : str
      color for text
  quantiles : bool
      shows quantiles.
  show_mu_sigma : bool
      shows mu and sigma.

  Returns
  -------
  matplotlib.Figure
      A matplotlib histogram of the posterior.
  """
  return _getProbaH(ie, var_name, scale=scale, txtcolor=txtcolor, quantiles=quantiles, show_mu_sigma=show_mu_sigma)


def saveFigProba(ie, var_name, filename, bgcolor=None, txtcolor="Black", quantiles=False, scale=1.0,
                 show_mu_sigma=False):
  """
  Saves a figure which is the representation of a histogram for a posterior.

  Parameters
  ----------
  ie : IMixtureInference
      Inference of a mixture.
  var_name : str
      Name of the variable to get histogram for.
  filename: str
      the name of the saved file
  bgcolor: str
      color for background (transparent if None)
  txtcolor : str
      color for text
  scale : float
      scale for the size of histograms
  quantiles : bool
      shows quantiles.
  show_mu_sigma : bool
      shows mu and sigma.
  """
  fig = proba2histo(ie, var_name, txtcolor=txtcolor, quantiles=quantiles, scale=scale, show_mu_sigma=show_mu_sigma)

  if bgcolor is None:
    fc = gum.config["notebook", "figure_facecolor"]
  else:
    fc = bgcolor

  fig.savefig(filename, bbox_inches='tight', transparent=False, facecolor=fc,
              pad_inches=0.05, dpi=fig.dpi, format=gum.config["notebook", "graph_format"])
  plt.close(fig)
