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

from .proba_histogram import proba2histo,saveFigProba

def ID2dot(diag, size=None):
  res = "digraph  { \n graph [bgcolor=transparent];\n"

  # chance node
  res += '''
    node [fillcolor="{}",
          fontcolor="{}",
          style=filled,shape={}, height=0,margin=0.1];
  '''.format(gum.config["influenceDiagram", "default_chance_bgcolor"],
             gum.config["influenceDiagram", "default_chance_fgcolor"],
             gum.config["influenceDiagram", "chance_shape"])
  for node in diag.nodes():
    if diag.isChanceNode(node):
      res += '   "' + diag.variable(node).name() + '";' + "\n"

  # decision node
  res += '''
    node [fillcolor="{}",
          fontcolor="{}",
          style=filled,shape={}, height=0,margin=0.1];
  '''.format(gum.config["influenceDiagram", "default_decision_bgcolor"],
             gum.config["influenceDiagram", "default_decision_fgcolor"],
             gum.config["influenceDiagram", "decision_shape"])
  for node in diag.nodes():
    if diag.isDecisionNode(node):
      res += '   "' + diag.variable(node).name() + '";' + "\n"

  # utility node
  res += '''
    node [fillcolor="{}",
          fontcolor="{}",
          style=filled,shape={}, height=0,margin=0.1];
  '''.format(gum.config["influenceDiagram", "default_utility_bgcolor"],
             gum.config["influenceDiagram", "default_utility_fgcolor"],
             gum.config["influenceDiagram", "utility_shape"])
  for node in diag.nodes():
    if diag.isUtilityNode(node):
      res += '   "' + diag.variable(node).name() + '";' + "\n"

  # arcs
  res += "\n"
  for node in diag.nodes():
    for chi in diag.children(node):
      res += '  "' + diag.variable(node).name() + '"->"' + \
             diag.variable(chi).name() + '"'
      if diag.isDecisionNode(chi):
        res+=' [style="{}"]'.format(gum.config["influenceDiagram", "decision_arc_style"])
      elif diag.isUtilityNode(chi):
        res+=' [style="{}"]'.format(gum.config["influenceDiagram", "utility_arc_style"])
      res += ";\n"
  res += "}"

  g = dot.graph_from_dot_data(res)

  if size is None:
    size = gum.config["influenceDiagram", "default_id_size"]
  g.set_size(size)
  return g

def LIMIDinference2dot(diag, size, engine, evs, targets):
  """
  create a pydotplus representation of an inference in a influence diagram

  :param pyAgrum.InfluenceDiagram diag: influence diagram
  :param string size: size of the rendered graph
  :param pyAgrum Inference engine: inference algorithm used. If None, ShaferShenoyLIMIDInference will be used
  :param dictionnary evs: map of evidence
  :param set targets: set of targets. If targets={} then each node is a target

  :return: the desired representation of the inference
  """
  startTime = time.time()
  if engine is None:
    ie = gum.ShaferShenoyLIMIDInference(diag)
  else:
    ie = engine
  ie.setEvidence(evs)
  ie.makeInference()
  stopTime = time.time()
  meu=ie.MEU();

  from tempfile import mkdtemp
  temp_dir = mkdtemp("", "tmp", None)  # with TemporaryDirectory() as temp_dir:

  dotstr = "digraph structs {\n  fontcolor=\"" + \
      gum.getBlackInTheme()+"\";bgcolor=\"transparent\";"

  fmt=gum.config["influenceDiagram", "utility_format_number"]

  titut=("MEU {:."+fmt+"f}").format(meu['mean'])
  if gum.config["influenceDiagram", "utility_show_stdev"]=="True":
      titut+=(" (stdev={:."+fmt+"f})").format(math.sqrt(meu['variance']))

  dotstr += "label=\"{}\nInference in {:6.2f}ms\";\n".format(titut,1000 * (stopTime - startTime))

  for nid in diag.nodes():
    name = diag.variable(nid).name()

    # defaults
    bgcolor = gum.config["notebook", "default_node_bgcolor"]
    fgcolor = gum.config["notebook", "default_node_fgcolor"]
    shape="ellipse"

    if diag.isChanceNode(nid):
      bgcolor=gum.config["influenceDiagram", "default_chance_bgcolor"]
      fgcolor=gum.config["influenceDiagram", "default_chance_fgcolor"]
      shape=gum.config["influenceDiagram", "chance_shape"]
    elif diag.isDecisionNode(nid):
      bgcolor=gum.config["influenceDiagram", "default_decision_bgcolor"]
      fgcolor=gum.config["influenceDiagram", "default_decision_fgcolor"]
      shape=gum.config["influenceDiagram", "decision_shape"]
    else: # diag.isUtilityNode(nid):
      bgcolor=gum.config["influenceDiagram", "default_utility_bgcolor"]
      fgcolor=gum.config["influenceDiagram", "default_utility_fgcolor"]
      shape=gum.config["influenceDiagram", "utility_shape"]

    # 'hard' colour for evidence (?)
    if name in evs or nid in evs:
      bgcolor = gum.config["notebook", "evidence_bgcolor"]
      fgcolor = gum.config["notebook", "evidence_fgcolor"]

    styleattribute= 'style=filled, height=0,margin=0.1'
    colorattribute = 'fillcolor="{}", fontcolor="{}", color="#000000"'.format(
        bgcolor, fgcolor)
    if not diag.isUtilityNode(nid):
      if len(targets) == 0 or name in targets or nid in targets:
        #shape="rectangle"
        filename = temp_dir + \
            hashlib.md5(name.encode()).hexdigest() + "." + \
            gum.config["notebook", "graph_format"]
        saveFigProba(ie.posterior(name), filename,bgcol=bgcolor,util=ie.posteriorUtility(nid),txtcolor=fgcolor)
        dotstr += ' "{0}" [shape=rectangle,image="{1}",label="", {2}];\n'.format(name, filename, colorattribute)
      else:
        dotstr += ' "{0}" [{1},shape={2},{3}]'.format(name, colorattribute,shape,styleattribute)
    else: #utility node
      mv=ie.meanVar(name)
      fmt=gum.config["influenceDiagram", "utility_format_number"]
      labut=("{} : {:."+fmt+"f}").format(name,mv["mean"])
      if gum.config["influenceDiagram", "utility_show_stdev"]=="True":
        labut+=(" ({:."+fmt+"f})").format(math.sqrt(mv["variance"]))

      dotstr += ' "{0}" [label="{1}",{2},{3},shape={4}]'.format(name,labut, colorattribute,styleattribute,shape)

  # arcs
  dotstr += "\n"
  for node in diag.nodes():
    for chi in diag.children(node):
      dotstr += '  "' + diag.variable(node).name() + '"->"' + \
             diag.variable(chi).name() + '"'
      if diag.isDecisionNode(chi):
        dotstr+=' [style="{}"]'.format(gum.config["influenceDiagram", "decision_arc_style"])
      elif diag.isUtilityNode(chi):
        dotstr+=' [style="{}"]'.format(gum.config["influenceDiagram", "utility_arc_style"])
      dotstr += ";\n"
  dotstr += "}"

  g = dot.graph_from_dot_data(dotstr)

  if size is None:
    size = gum.config["influenceDiagram", "default_id_inference_size"]
  g.set_size(size)
  g.temp_dir=temp_dir

  return g