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

from pathlib import Path
import re
from typing import Sequence, Optional

from .utils import warn, notif, notif_oneline


def _header_filter(split_filename: Sequence[str]) -> bool:
  filename = split_filename[-1]
  # exceptions
  exceptions = {"agrum.h", "base.h", "bn.h", "cn.h", "id.h", "mrf.h", "structuralConstraintPatternHeader.h"}
  if filename in exceptions:
    return False

  if filename == "config.h":
    return False

  if len(filename) > 5:
    if filename.endswith("_tpl.h"):
      return False
    if filename.endswith("_inl.h"):
      return False

  return True


def _gum_scan(file: Path) -> list[str]:
  patt = re.compile(r"^#\s*include <agrum/([^>]*)>")
  s = []

  with file.open() as f:
    for line in f.readlines():
      m = patt.match(line)
      if m:
        filename = m.group(1)
        if _header_filter(filename.split("/")):
          s.append(filename)

  return s


def _get_dependencies() -> dict[str, list[str]]:
  deps = {}
  p = Path("src/agrum")
  for file in p.glob("**/*.h"):
    if _header_filter(file.parts):
      key = "/".join(file.parts[2:])
      deps[key] = _gum_scan(file)
  return deps


def _build_ancestral(ancestors: dict[str, dict[str, int]], deps: dict[str, list[str]], k: str) -> dict[str, int]:
  if k not in ancestors:
    anc_k = {}
    for f in deps[k]:
      anc_k[f] = 1 if f not in anc_k else anc_k[f] + 1
      b = _build_ancestral(ancestors, deps, f)
      if b is None:
        raise ValueError(f"Cycle detected from {f}->{k}")
      else:
        for pf in b.keys():
          anc_k[pf] = 1 if pf not in anc_k else anc_k[pf] + 1
    ancestors[k] = anc_k
  return ancestors[k]


def _simplify_dependencies(ancestors: dict[str, dict[str, int]], deps: dict[str, list[str]]):
  nbr = 0
  for k in deps.keys():
    first = 0
    for p in deps[k]:
      if ancestors[k][p] > 1:
        if first == 0:
          warn("")
          warn("-" * len(k))
          warn(k)
          warn("-" * len(k))
          first = 1
        nbr += 1
        warn(f"{nbr:03d} {p}->{k} can be removed")
        deps[k].remove(p)


def draw_gum_dependencies(deps: dict[str, list[str]]):
  import pydot as pdp

  colors = {
    "tools/core": ("blues9", "#6677AA"),
    "tools/database": ("blues9", "#8899AA"),
    "tools/variables": ("blues9", "#8899BB"),
    "tools/graphicalModels": ("blues9", "#99AACC"),
    "tools/graphs": ("blues9", "#99AADD"),
    "tools/multidim": ("blues9", "#AAAAEE"),
    "tools/stattests": ("blues9", "#AABBFF"),
    "BN": ("set38", 4),
    "PRM": ("set38", 2),
    "MRF": ("set38", 3),
    "CN": ("set38", 1),
    "learning": ("set38", 6),
    "FMDP": ("set38", 7),
    "ID": ("set38", 8),
    "tools/external": ("greys9", 3),
    "legend": ("greys9", 1),
    "legend_tools": ("greys9", 2),
  }

  def _get_node(name, label: Optional[str] = None, th: Optional[str] = None):
    if label is None:
      label = name
    if th is None:
      th = name

    node = pdp.Node(name)

    node.set("fontname", "Arial")
    node.set("fontsize", 6)
    node.set("shape", "box")
    node.set("margin", 0.03)
    node.set("width", 0)
    node.set("height", 0)
    node.set("style", "filled")

    node.set("label", '"' + label + '"')
    if th not in colors:
      notif(f"Missing (or irrelevant) type : {th}")
      node.set("fillcolor", "green")
    else:
      cs, c = colors[th]
      node.set("fillcolor", c)
      node.set("colorscheme", cs)

    return node

  notif("    - building the graph")
  deps_graph = pdp.Dot()

  for col in colors:
    if not col.startswith("legend"):
      if not col.startswith("tools"):
        deps_graph.add_node(_get_node(col, col[6:]))
      else:
        deps_graph.add_node(_get_node(col))
    else:
      deps_graph.add_node(_get_node("legend", "pyAgrum"))
      deps_graph.add_node(_get_node("legend_tools", label="tools"))

  for col in colors:
    if col != "legend":
      if col.startswith("tools"):
        deps_graph.add_edge(pdp.Edge("legend_tools", col))
      else:
        deps_graph.add_edge(pdp.Edge("legend", col))

  deps_graph.set_name("gum")
  deps_graph.set_type("digraph")
  deps_graph.set_suppress_disconnected(True)
  deps_graph.set("splines", "compound")
  deps_graph.set("background", "transparent")

  for k in deps.keys():
    parts = k.split("/")
    if len(parts) > 1 and parts[1] == "learning":
      theme = "learning"
    else:
      theme = parts[0]
      if theme == "tools":
        theme += "/" + parts[1]

    nod = _get_node(k, parts[-1][:-2], theme)
    deps_graph.add_node(nod)

  for k in deps.keys():
    for nei in deps[k]:
      deps_graph.add_edge(pdp.Edge(nei, k))

  notif("    - drawing in pdf (please be patient...)")
  deps_graph.write_pdf("agrum-map.pdf", prog="fdp")


def remove_redundant_dependencies(target, includes):
  patt = re.compile(r"^#\s*include <agrum/([^>]*)>")

  to_keep = set(includes)

  res = ""
  with open(f"./src/agrum/{target}", "r") as f:
    for line in f.readlines():
      keep_line = True
      m = patt.match(line)
      if m:
        filename = m.group(1)
        if _header_filter(filename.split("/")) and filename not in to_keep:
          keep_line = False
      if keep_line:
        res += line

  with open(f"./src/agrum/{target}", "w") as f:
    print(res, file=f)


def check_gum_dependencies(graph: bool = True, details: bool = True, correction: bool = False):
  deps = _get_dependencies()
  nb_non_opt = {k: len(v) for k, v in deps.items()}

  nb_arcs = sum([len(c) for c in deps.values()])

  if correction or details:
    notif("AGrUM headers dependencies")
    notif(f"  + Nbr of headers : {len(deps)}")
    notif(f"  + Nbr of dependencies : {nb_arcs}")

  if graph:
    notif("  + drawing headers map in [agrum-map.pdf]")
    draw_gum_dependencies(deps)

  ancestors = {}
  for k in deps.keys():
    _build_ancestral(ancestors, deps, k)
  _simplify_dependencies(ancestors, deps)

  nb_opt_arcs = sum([len(c) for c in deps.values()])
  if correction or details:
    notif(f"  + Nbr of dependencies optimized : {nb_opt_arcs}")

  if correction:
    notif("Correction in progress")
    for k in deps.keys():
      # if some include have to be removed
      if nb_non_opt[k] != len(deps[k]):
        notif_oneline(f"[{k}]")
        remove_redundant_dependencies(k, deps[k])

  return nb_arcs - nb_opt_arcs
