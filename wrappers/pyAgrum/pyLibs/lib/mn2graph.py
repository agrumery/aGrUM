#!/usr/bin/python
# -*- coding: utf-8 -*-

# (c) Copyright by Pierre-Henri Wuillemin, UPMC, 2017
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

from __future__ import print_function

import time
import math
import hashlib

import matplotlib.pyplot as plt
import numpy as np
import pyAgrum as gum
import pydotplus as dot
import shutil

from .proba_histogram import proba2histo, saveFigProba


def MN2UGdot(mn, size="4", nodeColor=None, edgeWidth=None, edgeColor=None, cmapNode=None, cmapEdge=None, showMsg=None):
  """
  create a pydotplus representation of the Markov Network as an undirected graph

  :param pyAgrum.MarkovNet mn:
  :param string size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param edgeWidth: a arcMap of values to be shown as width of arcs
  :param edgeColor: a arcMap of values (between 0 and 1) to be shown as color of arcs
  :param cmapNode: color map to show the vals of Nodes
  :param cmapEdge: color map to show the vals of Edges.
  :param showMsg: a nodeMap of values to be shown as tooltip

  :return: the desired representation of the MN as a dot graph
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapEdge is None:
    cmapEdge = plt.get_cmap(gum.config["notebook", "default_edge_cmap"])

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
      bgcol = gum._proba2bgcolor(nodeColor[n], cmapNode)
      fgcol = gum._proba2fgcolor(nodeColor[n], cmapNode)
      res = " : {0:2.5f}".format(
        nodeColor[n] if showMsg is None else showMsg[n])

    node = dot.Node('"' + n + '"', style="filled",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    tooltip='"({0}) {1}{2}"'.format(mn.idFromName(n), n, res))
    graph.add_node(node)

  for a in mn.edges():
    if edgeWidth is None:
      pw = 1
      av = ""
    else:
      if a in edgeWidth:
        if maxedges == minedges:
          pw = 1
        else:
          pw = 0.1 + 5 * (edgeWidth[a] - minedges) / (maxedges - minedges)
        av = edgeWidth[a]
      else:
        pw = 1
        av = 1
    if edgeColor is None:
      col = gum.getBlackInTheme()
    else:
      if a in edgeColor:
        col = gum._proba2color(edgeColor[a], cmapEdge)
      else:
        col = gum.getBlackInTheme()

    edge = dot.Edge('"' + mn.variable(a[0]).name() + '"',
                    '"' + mn.variable(a[1]).name() + '"',
                    penwidth=pw, color=col,
                    tooltip="{} : {}".format(a, av))
    graph.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  graph.set_size(size)
  return graph


def MN2FactorGraphdot(mn, size="4", nodeColor=None, factorColor=None, cmapNode=None, showMsg=None):
  """
  create a pydotplus representation of the Markov Network as a factor graph

  :param pyAgrum.MarkovNet mn:
  :param string size: size of the rendered graph
  :param nodeColor: a nodeMap of values (between 0 and 1) to be shown as color of nodes (with special colors for 0 and 1)
  :param factorColor: a function returning a value (beeween 0 and 1) to be shown as a color of factor.
  :param cmapNode: color map to show the vals of Nodes
  :param showMsg: a nodeMap of values to be shown as tooltip

  :return: the desired representation of the MN as a dot graph
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
      bgcol = gum._proba2bgcolor(nodeColor[n], cmapNode)
      fgcol = gum._proba2fgcolor(nodeColor[n], cmapNode)
      res = " : {0:2.5f}".format(
        nodeColor[n] if showMsg is None else showMsg[n])

    node = dot.Node('"' + n + '"',
                    style="filled",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    shape="rectangle",
                    margin=0.04, width=0, height=0,
                    tooltip='"({0}) {1}{2}"'.format(mn.idFromName(n), n, res))
    graph.add_node(node)

  def factorname(f):
    return '"f' + "#".join(map(str, sorted(list(f)))) + '"'

  for f in mn.factors():
    if factorColor is None:
      bgcol = gum.config["factorgraph", "default_factor_bgcolor"]
    else:
      bgcol = gum._proba2bgcolor(factorColor(f), cmapNode)
    node = dot.Node(factorname(f),
                    style="filled",
                    fillcolor=bgcol,
                    shape="point",
                    width=0.1,
                    height=0.1)
    graph.add_node(node)

  for f in mn.factors():
    for n in f:
      edge = dot.Edge(factorname(f),
                      '"' + mn.variable(n).name() + '"',
                      color=gum.getBlackInTheme(),
                      len=gum.config["factorgraph", "edge_length"])
      graph.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]
  graph.set_size(size)
  return graph


def MNinference2UGdot(mn, size=None, engine=None, evs={}, targets={}, nodeColor=None, factorColor=None,
                      arcWidth=None, arcColor=None,
                      cmapNode=None, cmapArc=None, view=None):
  """
  create a pydotplus representation of an inference in a MN as an UG

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
  if cmapNode is None:
    cmapNode = plt.get_cmap(gum.config["notebook", "default_node_cmap"])

  if cmapArc is None:
    cmapArc = plt.get_cmap(gum.config["notebook", "default_arc_cmap"])

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
           gum.getBlackInTheme() + "\";bgcolor=\"transparent\";"
  dotstr += "  label=\"Inference in {:6.2f}ms\";\n".format(
    1000 * (stopTime - startTime))
  dotstr += '  node [fillcolor="' + gum.config["notebook", "default_node_bgcolor"] + \
            '", style=filled,color="' + \
            gum.config["notebook", "default_node_fgcolor"] + '"];' + "\n"
  dotstr += '  edge [color="' + gum.getBlackInTheme() + '"];' + "\n"

  for nid in mn.nodes():
    name = mn.variable(nid).name()

    # defaults
    bgcol = gum.config["notebook", "default_node_bgcolor"]
    fgcol = gum.config["notebook", "default_node_fgcolor"]
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = "white"

    if nodeColor is not None:
      if name in nodeColor or nid in nodeColor:
        bgcol = gum._proba2bgcolor(nodeColor[name], cmapNode)
        fgcol = gum._proba2fgcolor(nodeColor[name], cmapNode)

    # 'hard' colour for evidence (?)
    if name in evs or nid in evs:
      bgcol = gum.config["notebook", "evidence_bgcolor"]
      fgcol = gum.config["notebook", "evidence_fgcolor"]

    colorattribute = 'fillcolor="{}", fontcolor="{}", color="#000000"'.format(
      bgcol, fgcol)
    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + \
                 hashlib.md5(name.encode()).hexdigest() + "." + \
                 gum.config["notebook", "graph_format"]
      saveFigProba(ie.posterior(name), filename)
      dotstr += ' "{0}" [shape=rectangle,image="{1}",label="", {2}];\n'.format(
        name, filename, colorattribute)
    else:
      dotstr += ' "{0}" [{1}]'.format(name, colorattribute)

  for a in mn.edges():
    (n, j) = a
    if arcWidth is None:
      pw = 1
      av = ""
    else:
      if (n, j) in arcWidth:
        if maxarcs == minarcs:
          pw = 1
        else:
          pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
        av = arcWidth[a]
      else:
        pw = 1
        av = ""

    if arcColor is None:
      col = gum.getBlackInTheme()
    else:
      if a in arcColor:
        col = gum._proba2color(arcColor[a], cmapArc)
      else:
        col = gum.getBlackInTheme()

    dotstr += ' "{0}"--"{1}" [penwidth="{2}",tooltip="{3}:{4}",color="{5}"];'.format(
      mn.variable(n).name(), mn.variable(j).name(), pw, a, av, col)
  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  return g


def MNinference2FactorGraphdot(mn, size=None, engine=None, evs={}, targets={},
                               nodeColor=None, factorColor=None,
                               cmapNode=None):
  """
  create a pydotplus representation of an inference in a MN as an factor graph

  :param pyAgrum.MarkovNet mn:
  :param string size: size of the rendered graph
  :param pyAgrum Inference engine: inference algorithm used. If None, ShaferShenoyMNInference will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets. If targets={} then each node is a target
  :param nodeColor: a nodeMap of values to be shown as color nodes (with special color for 0 and 1)
  :param cmapNode: color map to show the vals of Nodes

  :return: the desired representation of the inference
  """
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
           gum.getBlackInTheme() + "\";bgcolor=\"transparent\";"
  dotstr += '  label="Inference in {:6.2f}ms";\n'.format(1000 * (stopTime - startTime))
  dotstr += '  node [fillcolor="' + gum.config["notebook", 'default_node_bgcolor'] + \
            '", style=filled,color="' + \
            gum.config["notebook", "default_node_fgcolor"] + '"];' + "\n"
  dotstr += '  edge [color="' + gum.getBlackInTheme() + '"];' + "\n"

  for nid in mn.nodes():
    name = mn.variable(nid).name()

    # defaults
    bgcol = gum.config["notebook", "default_node_bgcolor"]
    fgcol = gum.config["notebook", "default_node_fgcolor"]
    if len(targets) == 0 or name in targets or nid in targets:
      bgcol = "white"

    if nodeColor is not None:
      if name in nodeColor or nid in nodeColor:
        bgcol = gum._proba2bgcolor(nodeColor[name], cmapNode)
        fgcol = gum._proba2fgcolor(nodeColor[name], cmapNode)

    # 'hard' colour for evidence (?)
    if name in evs or nid in evs:
      bgcol = gum.config["notebook", "evidence_bgcolor"]
      fgcol = gum.config["notebook", "evidence_fgcolor"]

    colorattribute = 'fillcolor="{}", fontcolor="{}", color="#000000"'.format(
      bgcol, fgcol)
    if len(targets) == 0 or name in targets or nid in targets:
      filename = temp_dir + \
                 hashlib.md5(name.encode()).hexdigest() + "." + \
                 gum.config["notebook", "graph_format"]
      saveFigProba(ie.posterior(name), filename)
      dotstr += ' "{0}" [shape=rectangle,image="{1}",label="", {2}];\n'.format(
        name, filename, colorattribute)
    else:
      dotstr += ' "{0}" [shape=rectangle,margin=0.04,width=0,height=0,{1}]' \
        .format(name, colorattribute)

  def factorname(f):
    return '"f' + "#".join(map(str, sorted(list(f)))) + '"'

  for f in mn.factors():
    if factorColor is None:
      bgcol = gum.config["factorgraph", "default_factor_bgcolor"]
    else:
      bgcol = gum._proba2bgcolor(factorColor(f), cmapNode)
    dotstr += '  {0} [style=filled,fillcolor={1},shape=point,width=0.1,height=0.1];\n' \
      .format(factorname(f), bgcol)

  for f in mn.factors():
    col = gum.getBlackInTheme()
    for n in f:
      dotstr += ' {0}->"{1}" [tooltip="{2}:{3}",color="{4}",fillcolor="{5}",len="{6}"];\n' \
        .format(factorname(f), mn.variable(n).name(), f, n, col, bgcol,
                gum.config["factorgraph", "edge_length_inference"])
  dotstr += '}'

  g = dot.graph_from_dot_data(dotstr)

  if size is None:
    size = gum.config["notebook", "default_graph_inference_size"]
  g.set_size(size)
  return g


def dotize(aBN, name, format='pdf'):
  """
  From a bn, creates an image of the BN

  :param pyAgrum.BayesNet bn: the bayes net to show
  :param string name: the filename (without extension) for the image
  :param string format: format in ['pdf','png','fig','jpg','svg']
  """
  if format not in ['pdf', 'png', 'fig', 'jpg', 'svg']:
    raise Exception(
      "<%s> in not a correct style ([pdf,png,fig,jpg,svg])" % style)

  if isinstance(aBN, str):
    bn = gum.loadBN(aBN)
  else:
    bn = aBN

  imgfile = name + '.' + format
  BN2dot(bn).write(imgfile, format=format)


def pngize(aBN, name):
  """
  From a bn, creates a png of the BN

  :param pyAgrum.BayesNet bn: the bayes net to show
  :param string name: the filename (without extension) for the image
  """
  dotize(aBN, name, 'png')


def pdfize(aBN, name):
  """
  From a bn, creates a pdf of the BN

  :param pyAgrum.BayesNet bn: the bayes net to show
  :param string name: the filename (without extension) for the image
  """
  dotize(aBN, name, 'pdf')


if __name__ == "__main__":
  pyAgrum_header("2011-19")
  if len(sys.argv) < 2:
    print(os.path.basename(sys.argv[0]),
          "file.{" + gum.availableBNExts() + "}")
  else:
    base, ext = os.path.splitext(sys.argv[1])
    pdfize(sys.argv[1], base)
