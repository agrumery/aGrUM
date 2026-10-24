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

import re
from collections.abc import Sequence
from pathlib import Path

from .utils import notif, notif_oneline, warn


def _header_filter(split_filename: Sequence[str]) -> bool:
  filename = split_filename[-1]
  # exceptions
  exceptions = {
    "agrum.h",
    "base.h",
    "bn.h",
    "cm.h",
    "cn.h",
    "id.h",
    "ktbn.h",
    "mrf.h",
    "structuralConstraintPatternHeader.h",
  }
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
  for k in deps:
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
        warn(f"{nbr:03d} {k} : include {p} can be removed")
        deps[k].remove(p)


# colors of the themes: the submodules of base (blues) and the other modules (vivid colors, neither blue nor grey)
_COLORS = {
  "base/core": ("blues9", "#6677AA"),
  "base/database": ("blues9", "#8899AA"),
  "base/variables": ("blues9", "#8899BB"),
  "base/graphicalModels": ("blues9", "#99AACC"),
  "base/graphs": ("blues9", "#99AADD"),
  "base/multidim": ("blues9", "#AAAAEE"),
  "base/stattests": ("blues9", "#AABBFF"),
  "base/io": ("blues9", "#BBCCFF"),
  "CN": ("X11", "#F58231"),
  "PRM": ("X11", "#911EB4"),
  "MRF": ("X11", "#3CB44B"),
  "BN": ("X11", "#E6194B"),
  "FMDP": ("X11", "#9A6324"),
  "ID": ("X11", "#F032E6"),
  "CM": ("X11", "#FFE119"),
  "KTBN": ("X11", "#BFEF45"),
  "base/external": ("greys9", 3),
  "legend": ("greys9", 1),
  "legend_base": ("greys9", 2),
}

_ARROW_SIZE = 0.5


def _font_color(fillcolor: str | int) -> str:
  """Readable text color on a fill color: same rule as gum::dot::contrastColor (only for '#RRGGBB' colors)."""
  if not (isinstance(fillcolor, str) and fillcolor.startswith("#")):
    return "black"
  red, green, blue = (int(fillcolor[i : i + 2], 16) for i in (1, 3, 5))
  return "white" if 299 * red + 587 * green + 114 * blue <= 153000 else "black"


def _theme(header: str) -> str:
  """The theme of a header: its module, or its submodule for base (e.g. 'BN', 'base/graphs')."""
  parts = header.split("/")
  return f"base/{parts[1]}" if parts[0] == "base" else parts[0]


def _get_node(pdp, name: str, label: str | None = None, th: str | None = None, tooltip: str | None = None):
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
  if tooltip is not None:
    node.set("tooltip", '"' + tooltip + '"')
  if th not in _COLORS:
    notif(f"Missing (or irrelevant) type : {th}")
    node.set("fillcolor", "green")
  else:
    cs, c = _COLORS[th]
    node.set("fillcolor", c)
    node.set("fontcolor", _font_color(c))
    node.set("colorscheme", cs)

  return node


def _write_graph(graph, basename: str, prog: str, svg: bool):
  graph.write_pdf(f"{basename}.pdf", prog=prog)
  notif(f"  + [[{basename}.pdf]] generated")
  if svg:
    graph.write_svg(f"{basename}.svg", prog=prog)
    notif(f"  + [[{basename}.svg]] generated")


def draw_gum_dependencies(deps: dict[str, list[str]], svg: bool = False):
  import pydot as pdp

  notif("    - building the graph")
  deps_graph = pdp.Dot()

  def _same_size(node):
    # all the descendants of the "aGrUM" legend node share the same size (wide enough for "graphicalModels")
    node.set("width", 0.75)
    node.set("height", 0.15)
    node.set("fixedsize", "true")
    return node

  for col in _COLORS:
    if not col.startswith("legend"):
      if col.startswith("base"):
        deps_graph.add_node(_same_size(_get_node(pdp, col, col[5:])))
      else:
        deps_graph.add_node(_same_size(_get_node(pdp, name=col, label=col)))

  deps_graph.add_node(_get_node(pdp, "legend", "aGrUM"))
  deps_graph.add_node(_same_size(_get_node(pdp, "legend_base", label="base")))

  for col in _COLORS:
    if col != "legend":
      if col.startswith("base"):
        deps_graph.add_edge(pdp.Edge("legend_base", col, arrowsize=_ARROW_SIZE))
      else:
        deps_graph.add_edge(pdp.Edge("legend", col, arrowsize=_ARROW_SIZE))

  deps_graph.set_name("gum")
  deps_graph.set_type("digraph")
  deps_graph.set_suppress_disconnected(True)
  # 'compound' splines take ~10 minutes with fdp for an equivalent drawing
  deps_graph.set("splines", "true")
  deps_graph.set("background", "transparent")

  for k in deps:
    nod = _get_node(pdp, k, k.split("/")[-1][:-2], _theme(k), tooltip=k)
    deps_graph.add_node(nod)

  for k in deps:
    for nei in deps[k]:
      deps_graph.add_edge(pdp.Edge(nei, k, arrowsize=_ARROW_SIZE))

  notif("    - drawing (please be patient...)")
  _write_graph(deps_graph, "agrum-map", "fdp", svg)


def draw_gum_summary(deps: dict[str, list[str]], svg: bool = False):
  """Map of the modules (and of the submodules of base): an arc is labelled by its number of includes."""
  import math

  import pydot as pdp

  notif("    - building the summary graph")
  nb_headers = {}
  nb_includes = {}
  for k in deps:
    th = _theme(k)
    nb_headers[th] = nb_headers.get(th, 0) + 1
    for nei in deps[k]:
      arc = (_theme(nei), th)
      if arc[0] != arc[1]:
        nb_includes[arc] = nb_includes.get(arc, 0) + 1

  summary = pdp.Dot()
  summary.set_name("gum_summary")
  summary.set_type("digraph")
  summary.set("background", "transparent")

  for th in _COLORS:
    if th in nb_headers:
      nod = _get_node(
        pdp, th, f"{th.removeprefix('base/')}\\n({nb_headers[th]} header{'s' if nb_headers[th] > 1 else ''})", th
      )
      nod.set("fontsize", 10)
      nod.set("margin", 0.1)
      summary.add_node(nod)

  for (src, dst), nb in nb_includes.items():
    summary.add_edge(
      pdp.Edge(
        src,
        dst,
        label=nb,
        fontname="Arial",
        fontsize=8,
        penwidth=round(0.5 + 1.5 * math.log10(nb), 2),
        arrowsize=_ARROW_SIZE,
        tooltip=f'"{src} -> {dst} : {nb} includes"',
      )
    )

  _write_graph(summary, "agrum-summary", "dot", svg)


def remove_redundant_dependencies(target, includes):
  patt = re.compile(r"^#\s*include <agrum/([^>]*)>")

  to_keep = set(includes)

  res = ""
  with open(f"./src/agrum/{target}") as f:
    for line in f:
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


def build_gum_dependencies_graph(headers_map: bool = True, summary: bool = False, svg: bool = False):
  deps = _get_dependencies()
  if headers_map:
    notif("  + drawing headers map in [[agrum-map]]")
    draw_gum_dependencies(deps, svg)
  if summary:
    notif("  + drawing modules map in [[agrum-summary]]")
    draw_gum_summary(deps, svg)


def check_gum_dependencies(details: bool = True, correction: bool = False):
  deps = _get_dependencies()
  nb_non_opt = {k: len(v) for k, v in deps.items()}

  nb_arcs = sum([len(c) for c in deps.values()])

  if correction or details:
    notif("aGrUM headers dependencies")
    notif(f"  + Nbr of headers : {len(deps)}")
    notif(f"  + Nbr of dependencies : {nb_arcs}")

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
        notif_oneline(f"[[{k}]]")
        remove_redundant_dependencies(k, deps[k])

  return nb_arcs - nb_opt_arcs
