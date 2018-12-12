#!/usr/bin/python
# -*- coding: utf-8 -*-

# ***************************************************************************
# *   Copyright (C) 2015 by Pierre-Henri WUILLEMIN                          *
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
from pathlib import Path
import re
import pydotplus as pdp


def _filter(tab):
  if tab[-1] == "agrum.h":
    return False

  if tab[-1] == "config.h":
    return False

  if len(tab) > 4:
    if tab[3] == "external":
      return False
  else:
    if tab[0] == "external":
      return False

  if len(tab[-1]) > 5:
    if tab[-1][-6:] == "_tpl.h":
      return False
    if tab[-1][-6:] == "_inl.h":
      return False

  return True


def _gumScan(file: Path):
  patt = re.compile(r"^#[\s]*include <agrum/([^>]*)>")
  s = []

  with file.open() as f:
    for line in f.readlines():
      m = patt.match(line)
      if m:
        filename = m.group(1)
        parts = filename.split("/")
        if _filter(parts):
          s.append(filename)

  return s


def drawGumDeps():
  colors = {
    "PRM"            : "aquamarine",
    "core"           : "tomato",
    "BN"             : "sandybrown",
    "CN"             : "thistle",
    "learning"       : "mediumorchid",
    "variables"      : "lightskyblue",
    "graphicalModels": "darkgoldenrod",
    "graphs"         : "lightpink",
    "multidim"       : "yellow",
    "FMDP"           : "darksalmon",
    "ID"             : "deepskyblue",
    "legend"         : "white"
  }
  colors = {
    "core"           : "tomato",
    "variables"      : "lightpink",
    "graphicalModels": "darkgoldenrod",
    "graphs"         : "darksalmon",
    "multidim"       : "sandybrown",

    "BN"             : "yellow",
    "PRM"            : "orchid",
    "CN"             : "yellowgreen",
    "learning"       : "lightskyblue",
    "FMDP"           : "darkseagreen",
    "ID"             : "aquamarine",

    "legend"         : "white"
  }

  def _getNode(name, label=None, theme=None):
    if label is None:
      label = name
    if theme is None:
      theme = name

    nod = pdp.Node(name)

    nod.set("fontname", "Arial")
    nod.set("fontsize", 9)
    nod.set("shape", "box")
    nod.set("margin", 0.05)
    nod.set("width", 0)
    nod.set("height", 0)
    nod.set("style", "filled")

    nod.set("label", '"' + label + '"')
    if not theme in colors:
      print(f"Missing (or irrelevant) type : {theme}")
      nod.set("fillcolor", "green")
    else:
      nod.set("fillcolor", colors[theme])

    return nod

  agru = pdp.Dot()

  oldcol = "legend"
  for col in colors:
    agru.add_node(_getNode(col))
    if col != "legend":
      agru.add_edge(pdp.Edge("legend", col))

  arcsiz = 0
  nodsiz = 0

  deps = {}
  p = Path('../src/agrum')
  for file in p.glob('**/*.h'):
    if _filter(file.parts):
      key = "/".join(file.parts[3:])
      deps[key] = _gumScan(file)

  agru.set_name("gum")
  agru.set_type("digraph")
  agru.set_suppress_disconnected(True)
  agru.set("splines", "compound")
  agru.set("background", "transparent")

  for k in deps.keys():
    parts = k.split("/")
    nod = _getNode(k, parts[-1][:-2], parts[0])
    agru.add_node(nod)
    nodsiz += 1

  for k in deps.keys():
    for l in deps[k]:
      agru.add_edge(pdp.Edge(l, k))
      arcsiz += 1

  print("aGrUM headers map")
  print(f" + Nbr of nodes : {nodsiz}")
  print(f" + Nbr of arcs : {arcsiz}")
  agru.write_pdf("agrum-map.pdf", prog="fdp")


if __name__ == "__main__":
  # execute only if run as a script
  drawGumDeps()
