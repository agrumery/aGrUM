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
Look of the graphs drawn with graphviz: colors and their conversions, theme, reading of the dot written by aGrUM,
completion of the dot graphs built in Python by pyAgrum (prepareDot) and filling of pyagrum.DotOptions.
"""

import warnings

import matplotlib as mpl
import matplotlib.colors
import pyagrum
import pydot as dot
from pyagrum.lib.utils import getBlackInTheme


# _Rgb/_parseColor/_contrastColor/_configFgColor/_resolveFgColor are private on purpose: internal bindings, not part of the public API
# noinspection PyProtectedMember
def color2rgb(color: str) -> list[int]:
  """
  from "#FFFFFF" to [255,255,255]
  from 'DarkBlue' to [0,0,139]

  Parameters
  ----------
  color: str
    the color: '#RGB', '#RGBA', '#RRGGBB', '#RRGGBBAA' (the transparency is ignored) or the name of an HTML color

  Returns
  -------
  list[int]
    the list [r,g,b]

  Raises
  ------
  ValueError
    if the color can not be interpreted
  """
  rgb = pyagrum.pyagrumcpp._Rgb()
  if not pyagrum.pyagrumcpp._parseColor(color, rgb):
    raise ValueError(f"Unknown color {color}")
  return [rgb.red, rgb.green, rgb.blue]


def hex2rgb(vstr: str) -> list[int]:
  """
  from "#FFFFFF" to [255,255,255]
  from 'DarkBlue' to [0,0,139]

  Warnings
  --------
  Deprecated after 3.2.1: use :func:`color2rgb` instead.

  Parameters
  ----------
  vstr: str
    the rbg string  or an html color

  Returns
  -------
  list[int]
    the list [r,g,b]
  """
  warnings.warn("hex2rgb is deprecated after 3.2.1, use color2rgb instead", DeprecationWarning, stacklevel=2)
  return color2rgb(vstr)


def hextuple2rgb(vtuple: list[str]) -> list[int]:
  """
  from ("FF","FF","FF") to [255,255,255]

  Parameters
  ----------
  vtuple : tuple[str,str,str]
    the Tuple of hexa values

  Returns
  -------
  list[int,int,int]
    the list [r,g,b]
  """
  return [int(v, 16) for v in vtuple]


# noinspection PyProtectedMember
def rgb2brightness(r: int, g: int, b: int) -> str:
  """
  Give the fgcol for a background (r,g,b).

  Parameters
  ----------
  g: int[0,255]
  r: int[0,255]
  b: int[0,255]

  Returns
  -------
  str
    "white" or "black"
  """
  rgb = pyagrum.pyagrumcpp._Rgb()
  rgb.red, rgb.green, rgb.blue = r, g, b
  return pyagrum.pyagrumcpp._contrastColor(rgb)


# noinspection PyProtectedMember
def configFgColor(section: str, key: str) -> str:
  """
  Give a font color of the configuration, resolved against the background color which goes with it.

  Parameters
  ----------
  section: str
    the section of the configuration (for instance 'notebook')
  key: str
    the key of the font color, which ends with FgColor (for instance 'nodeFgColor'); the background color is the key
    which ends with BgColor instead (here 'nodeBgColor').

  Returns
  -------
  str
    the font color, never 'adaptive'

  Raises
  ------
  pyagrum.InvalidArgument
    if key does not end with FgColor
  pyagrum.SyntaxError
    if the font color or the background color is not a key of the configuration
  """
  return pyagrum.pyagrumcpp._configFgColor(section, key)


def proba2hex(p: float, cmap: matplotlib.colors.Colormap, withSpecialColor: bool) -> tuple[str, str, str]:
  """
  From a proba p and cmap gives the HTML rgb color

  Parameters
  ----------
  p: float
    the proba
  cmap: matplotlib.colors.Colormap
    the cmap
  withSpecialColor: bool
    do we have special colors for p=0 or 1 ?

  Returns
  -------
  Tuple(str,str,str)
    the hex values for r,g,b.
  """
  if withSpecialColor:  # add special color for p=0 or p=1
    if p <= 0.0:
      return "FF", "33", "33"
    elif p >= 1.0:
      return "AA", "FF", "FF"

  a, b, c, _ = cmap(p)
  return f"{round(a * 255):02x}", f"{round(b * 255):02x}", f"{round(c * 255):02x}"


def proba2color(p: float, cmap: matplotlib.colors.Colormap) -> str:
  """
  From a proba p and cmap gives the HTML rgb color

  Parameters
  ----------
  p: float
    a value in [0,1]
  cmap: matplotlib.colors.Colormap

  Returns
  -------
  str
    the html representation of the color
  """
  r, g, b = proba2hex(p, cmap, withSpecialColor=False)
  return "#" + r + g + b


def proba2bgcolor(p: float, cmap: matplotlib.colors.Colormap) -> str:
  """
  From a proba p and cmap gives the HTML rgb color (with special colors for p=0 and p=1)

  Parameters
  ----------
  p: float
    a value in [0,1]
  cmap: matplotlib.colors.Colormap

  Returns
  -------
  str
    the HTML representation of the background color
  """
  r, g, b = proba2hex(p, cmap, withSpecialColor=True)
  return "#" + r + g + b


def proba2fgcolor(p: float, cmap: matplotlib.colors.Colormap) -> str:
  """
  From a proba p and cmap, returns the best choice for text color for the bgcolor(p,cmap).

  Parameters
  ----------
  p: float
    a value in [0,1]
  cmap: matplotlib.colors.Colormap

  Returns
  -------
  str
    the html representation of the foreground color
  """
  return rgb2brightness(*hextuple2rgb(proba2hex(p, cmap, withSpecialColor=True)))


def fontFromMatplotlib() -> tuple[str, float]:
  """
  Find the font name and the font size ysed by matplotlib

  Returns
  -------
    fontname,size : font name and size from matplotlib
  """
  family = mpl.rcParams["font.family"][0]
  if family == "sans-serif":
    family = mpl.rcParams["font.sans-serif"][0]
  return family, mpl.rcParams["font.size"]


def dotFromAgrum(dotstring: str) -> dot.Dot:
  """
  The pydot graph of a dot string written by a `toDot()` of aGrUM. Such a graph is already complete (see prepareDot).

  Parameters
  ----------
  dotstring: str
    the dot string

  Returns
  -------
  pydot.Dot
    the graph
  """
  graphs = dot.graph_from_dot_data(dotstring)
  assert graphs is not None
  return graphs[0]


def setNodesOptions(
  options: pyagrum.DotOptions,
  model,
  nodeColor: dict | None,
  cmapNode: matplotlib.colors.Colormap,
  showMsg: dict | None,
) -> None:
  """
  Gives to a pyagrum.DotOptions the colors of the nodes of a model computed from values with a color map.

  Parameters
  ----------
  options: pyagrum.DotOptions
    the options to fill
  model:
    a graphical model (with names() and idFromName())
  nodeColor: dict[str,float]
    a map name -> value (between 0 and 1), shown as a color of the node (with special colors for 0 and 1)
  cmapNode: matplotlib.colors.Colormap
    the color map
  showMsg: dict[str,float]
    a map name -> value shown in the tooltip instead of the value of nodeColor
  """
  if nodeColor is None:
    return
  for n in model.names():
    if n in nodeColor:
      i = model.idFromName(n)
      options.setNodeBgColor(i, proba2bgcolor(nodeColor[n], cmapNode))
      options.setNodeFgColor(i, proba2fgcolor(nodeColor[n], cmapNode))
      res = f" : {nodeColor[n] if showMsg is None else showMsg[n]:2.5f}"
      options.setNodeTooltip(i, f"({i}) {n}{res}")


def setLinksOptions(
  options: pyagrum.DotOptions,
  links,
  linkWidth: dict | None,
  linkLabel: dict | None,
  linkColor: dict | None,
  cmapLink: matplotlib.colors.Colormap,
  separator: str,
) -> None:
  """
  Gives to a pyagrum.DotOptions the width, color and label of the arcs (or edges) of a model.

  Parameters
  ----------
  options: pyagrum.DotOptions
    the options to fill
  links:
    the arcs (or edges) of the model, as pairs of ids
  linkWidth: dict[tuple[int,int],float]
    a map arc -> value shown as the width of the line (the widths are scaled between 0.1 and 5.1)
  linkLabel: dict[tuple[int,int],str]
    a map arc -> label shown next to the arc
  linkColor: dict[tuple[int,int],float]
    a map arc -> value (between 0 and 1) shown as the color of the arc
  cmapLink: matplotlib.colors.Colormap
    the color map for the arcs
  separator: str
    the html entity between the ends of the arc in its tooltip
  """
  minw, maxw = (0.0, 100.0) if linkWidth is None else (min(linkWidth.values()), max(linkWidth.values()))
  for a in links:
    (n, j) = a
    if linkWidth is not None and a in linkWidth:
      pw = 0.1 + 5 * (linkWidth[a] - minw) / (maxw - minw) if maxw != minw else 1.0
      options.setArcWidth(n, j, pw)
      options.setArcTooltip(n, j, f"{n}&nbsp;{separator}&nbsp;{j} : {linkWidth[a]}")
    if linkColor is not None and a in linkColor:
      options.setArcColor(n, j, proba2color(linkColor[a], cmapLink))
    if linkLabel is not None and a in linkLabel:
      options.setArcLabel(n, j, linkLabel[a])


def _dotDefaults(dotgraph: dot.Dot) -> dict[str, dict[str, str]]:
  """
  The attributes given by the statements `graph [...]`, `node [...]` and `edge [...]` of a graph (pydot reads them
  as nodes with these names).
  """
  defaults: dict[str, dict[str, str]] = {"graph": {}, "node": {}, "edge": {}}
  for n in dotgraph.get_nodes():
    name = n.get_name().strip('"')
    if name in defaults:
      defaults[name].update({k: str(v).strip('"') for k, v in n.get_attributes().items()})
  return defaults


# noinspection PyProtectedMember
def prepareDot(dotgraph: dot.Dot) -> dot.Dot:
  """
  Complete a pydot graph with the look of the configuration: transparent background, direction and layout,
  color of the arcs and of the border of the nodes, color of the text of the nodes (black or white according to their
  background color).

  It is meant for the graphs built in Python by pyAgrum: each of these builders returns a graph already completed by
  this function. The graphs written by the `toDot()` of aGrUM are already complete, and the functions which show or
  export a graph (showGraph, export...) only apply its size: a graph given by the user is shown as it is.

  What the graph already gives (for an element or by default for all of them: `node [...]`) is respected, so calling it
  on a complete graph changes nothing.

  Parameters
  ----------
  dotgraph: pydot.Dot
    the graph to complete (modified in place)

  Returns
  -------
  pydot.Dot
    the graph itself
  """
  defaults = _dotDefaults(dotgraph)
  black = getBlackInTheme()

  if dotgraph.get_rankdir() is None and "rankdir" not in defaults["graph"]:
    dotgraph.set_rankdir(pyagrum.config["notebook", "graphRankDir"])
  if dotgraph.get_layout() is None and "layout" not in defaults["graph"]:
    dotgraph.set_layout(pyagrum.config["notebook", "graphLayout"])

  dotgraph.set_bgcolor("transparent")
  for e in dotgraph.get_edges():
    if e.get_color() is None and "color" not in defaults["edge"]:
      e.set_color(black)
  for n in dotgraph.get_nodes():
    if n.get_name().strip('"') in defaults:  # the statements `graph [...]`, `node [...]`, `edge [...]`
      continue
    if n.get_color() is None and "color" not in defaults["node"]:
      n.set_color(black)
    if n.get_fontcolor() is not None or "fontcolor" in defaults["node"]:  # already given
      continue
    fillcolor = n.get_fillcolor()
    if fillcolor is None:
      fillcolor = defaults["node"].get("fillcolor")
    if fillcolor is not None:
      n.set_fontcolor(pyagrum.pyagrumcpp._resolveFgColor("adaptive", fillcolor, black))
    else:
      n.set_fontcolor(black)

  return dotgraph
