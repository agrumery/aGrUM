#!/usr/bin/python
# -*- coding: utf-8 -*-

# ***************************************************************************
# *   Copyright (c) 2015 by Pierre-Henri WUILLEMIN                          *
# *   {prenom.nom}_at_lip6.fr                                               *
# *                                                                         *
# *   This program is free software; you can redistribute it and/or modify  *
# *   it under the terms of the GNU General Public License as published by  *
# *   the Free Software Foundation; either version 2 of the License, or     *
# *   (at your option) any later version.                                   *
# *                                                                         *
# *   This program is distributed in the hope that it will be useful,       *
# *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
# *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
# *   GNU General Public License for more details.                          *
# *                                                                         *
# *   You should have received a copy of the GNU General Public License     *
# *   along with this program; if not, write to the                         *
# *   Free Software Foundation, Inc.,                                       *
# *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
# ***************************************************************************
import pydot as pdp
from pathlib import Path
import re
from typing import Dict, List, Set


def _header_filter(splitted_filename:str)->bool:
  filename=splitted_filename[-1]
  if len(filename) > 5:
    if filename[-6:] == "_tpl.h":
      return False
    if filename[-6:] == "_inl.h":
      return False

  if filename == "agrum.h":
    return False

  if filename == "config.h":
    return False  

  return True


def _gumScan(file: Path)->List[str]:
  patt = re.compile(r"^#[\s]*include <agrum/([^>]*)>")
  s = []

  with file.open() as f:
    for line in f.readlines():
      m = patt.match(line)
      if m:
        filename = m.group(1)
        parts = filename.split("/")
        if _header_filter(parts):
          s.append(filename)

  return s

def _getDependencies()->Dict[str,List[str]]:
  deps = {}
  p = Path('../src/agrum')
  for file in p.glob('**/*.h'):
    if _header_filter(file.parts):
      key = "/".join(file.parts[3:])
      deps[key] = _gumScan(file)

  return deps

def _buildAncestral(ancestrals:Dict[str,Set[str]],deps:Dict[str,List[str]],k:str):
  if k not in ancestrals:
    ancestrals[k]=None
    anc_k={}
    for f in deps[k]:
      anc_k[f]=1 if f not in anc_k else anc_k[f]+1
      b = _buildAncestral(ancestrals,deps,f)
      if b is None:
        raise ValueError(f"Cycle detected from {f}->{k}")
      for pf in _buildAncestral(ancestrals,deps,f):
        anc_k[pf]=1 if pf not in anc_k else anc_k[pf]+1
    ancestrals[k]=anc_k
  return ancestrals[k]

def _simplyfyDependencies(ancestral,deps):
  nbr=0
  for k in deps.keys():
    first=0
    l=list(deps[k])
    for p in l:
      if ancestral[k][p]>1:
        if first==0:
          warn("")
          warn("-"*len(k))
          warn(k)
          warn("-"*len(k))
          first=1
        nbr+=1
        warn(f"{nbr:03d} {p}->{k} can be removed")
        deps[k].remove(p)

def analyzeDeps():
  deps=_getDependencies()
  ancestrals={}
  for k in deps.keys():
    _buildAncestral(ancestrals,deps,k)
  _simplyfyDependencies(ancestrals,deps)
def drawGumDeps(complete=False,optimized=False):
  colors = {
    "tools/core":            ("blues9","#6677AA"),
    "tools/database":        ("blues9","#8899AA"),
    "tools/variables":       ("blues9","#8899BB"),
    "tools/graphicalModels": ("blues9","#99AACC"),
    "tools/graphs":          ("blues9","#99AADD"),
    "tools/multidim":        ("blues9","#AAAAEE"),
    "tools/stattests":       ("blues9","#AABBFF"),

    "BN": ("set38",4),
    "PRM": ("set38",2),
    "MN": ("set38",3),
    "CN": ("set38",1),
    "learning": ("set38",6),
    "FMDP": ("set38",7),
    "ID": ("set38",8),

    "tools/external": ("greys9",3),
    "legend": ("greys9",1),
    "legend_tools": ("greys9",2)
  }

  def _getNode(name, label=None, theme=None):
    if label is None:
      label = name
    if theme is None:
      theme = name

    nod = pdp.Node(name)

    nod.set("fontname", "Arial")
    nod.set("fontsize", 6)
    nod.set("shape", "box")
    nod.set("margin", 0.03)
    nod.set("width", 0)
    nod.set("height", 0)
    nod.set("style", "filled")

    nod.set("label", '"' + label + '"')
    if not theme in colors:
      print(f"Missing (or irrelevant) type : {theme}")
      nod.set("fillcolor", "green")
    else:
      cs,c=colors[theme]
      nod.set("fillcolor", c)
      nod.set("colorscheme", cs)

    return nod

  agru = pdp.Dot()

  oldcol = "legend"
  for col in colors:
    if col[:6]!="legend":
      if col[:5]=="tools":
        agru.add_node(_getNode(col, col[6:]))
      else:
        agru.add_node(_getNode(col))
    else:
      agru.add_node(_getNode("legend", "pyAgrum"))
      agru.add_node(_getNode("legend_tools", label="tools"))

  for col in colors:
    if col != "legend":
      if col[:5] == "tools":
        agru.add_edge(pdp.Edge("legend_tools", col))
      else:
        agru.add_edge(pdp.Edge("legend", col))

  arcsiz = 0
  nodsiz = 0

  agru.set_name("gum")
  agru.set_type("digraph")
  agru.set_suppress_disconnected(True)
  agru.set("splines", "compound")
  agru.set("background", "transparent")

  if complete:
    deps=_getDependencies()

    if optimized:
      ancestrals={}
      for k in deps.keys():
        _buildAncestral(ancestrals,deps,k)
      _simplyfyDeps(ancestrals,deps)

    for k in deps.keys():
      parts = k.split("/")
      if len(parts)>1 and parts[1]=="learning":
        theme="learning"
      else:
        theme = parts[0]
        if theme == "tools":
          theme+="/"+parts[1]

      nod = _getNode(k, parts[-1][:-2], theme)
      agru.add_node(nod)
      nodsiz += 1

    for k in deps.keys():
      for l in deps[k]:
        agru.add_edge(pdp.Edge(l, k))
        arcsiz += 1

  print("# aGrUM headers map")
  print(f"#  + Nbr of nodes : {nodsiz}")
  print(f"#  + Nbr of arcs : {arcsiz}")

  agru.write_pdf("agrum-map.pdf", prog="fdp")

if __name__ == "__main__":
  # execute only if run as a script
  drawGumDeps(complete=True,optimized=False)
  #analyzeDeps()
