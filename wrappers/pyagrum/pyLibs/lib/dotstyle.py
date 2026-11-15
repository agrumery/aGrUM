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
completion of the dot graphs built in Python by pyAgrum (prepareDot) and filling of `pyagrum.DotOptions`.
"""

import hashlib
import os
import warnings
import weakref
from tempfile import TemporaryDirectory

import matplotlib as mpl
import matplotlib.colors
import pydot as dot

import pyagrum
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
  SyntaxError
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


def contrastFgColor(bgcolor: str) -> str:
  """
  The font color which is readable on a background color.

  Parameters
  ----------
  bgcolor: str
    the background color (a name or "#RRGGBB")

  Returns
  -------
  str
    the font color
  """
  return pyagrum.pyagrumcpp._resolveFgColor("adaptive", bgcolor, getBlackInTheme())


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


def _parseDot(dotstring: str) -> dot.Dot:
  """The first pydot graph of a dot string, or a ValueError if the string is not valid dot."""
  graphs = dot.graph_from_dot_data(dotstring)
  if not graphs:
    raise ValueError("the string is not a valid dot graph")
  return graphs[0]


def dotFromAgrum(dotstring: str) -> dot.Dot:
  """
  The pydot graph of a dot string written by a ``toDot()`` of aGrUM. Such a graph is already complete (see prepareDot).

  Parameters
  ----------
  dotstring: str
    the dot string

  Returns
  -------
  pydot.Dot
    the graph
  """
  return _parseDot(dotstring)


def setNodesOptions(
  options: pyagrum.DotOptions,
  model,
  nodeColor: dict | None,
  cmapNode: matplotlib.colors.Colormap,
  showMsg: dict | None,
) -> None:
  """
  Gives to a `pyagrum.DotOptions` the colors of the nodes of a model computed from values with a color map.

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


def _widthBounds(linkWidth: dict | None) -> tuple[float, float]:
  """The smallest and the largest values shown by the widths of the links ((0, 0) without any)."""
  return (min(linkWidth.values()), max(linkWidth.values())) if linkWidth else (0.0, 0.0)


def _scaledWidth(value: float, bounds: tuple[float, float]) -> float:
  """The width of a link: its value scaled between 0.1 and 5.1 (1.0 if all the values are equal)."""
  minw, maxw = bounds
  return 0.1 + 5 * (value - minw) / (maxw - minw) if maxw != minw else 1.0


def _linkTooltip(n: int, j: int, separator: str, value=None) -> str:
  """The tooltip of the link n-j, with its value if given."""
  ends = f"{n}&nbsp;{separator}&nbsp;{j}"
  return ends if value is None else f"{ends} : {value}"


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
  Gives to a `pyagrum.DotOptions` the width, color and label of the arcs (or edges) of a model.

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
  bounds = _widthBounds(linkWidth)
  for a in links:
    (n, j) = a
    if linkWidth is not None and a in linkWidth:
      options.setArcWidth(n, j, _scaledWidth(linkWidth[a], bounds))
      options.setArcTooltip(n, j, _linkTooltip(n, j, separator, linkWidth[a]))
    if linkColor is not None and a in linkColor:
      options.setArcColor(n, j, proba2color(linkColor[a], cmapLink))
    if linkLabel is not None and a in linkLabel:
      options.setArcLabel(n, j, linkLabel[a])


def _dotDefaults(dotgraph: dot.Dot) -> dict[str, dict[str, str]]:
  """
  The attributes given by the statements ``graph [...]``, ``node [...]`` and ``edge [...]`` of a graph (pydot reads them
  as nodes with these names).
  """
  defaults: dict[str, dict[str, str]] = {"graph": {}, "node": {}, "edge": {}}
  for n in dotgraph.get_nodes():
    name = n.get_name().strip('"')
    if name in defaults:
      defaults[name].update({k: str(v).strip('"') for k, v in n.get_attributes().items()})
  return defaults


def inferenceDotHeader(graphType: str = "digraph", inferenceTime: float | None = None, label: str | None = None) -> str:
  """
  The beginning of the dot text of a graph showing an inference: its look given by the theme and the configuration
  (font, default colors of the nodes and of the edges), and the time of the inference if
  ``pyagrum.config["notebook", "showInferenceTime"]``.

  Parameters
  ----------
  graphType: str
    "digraph" or "graph"
  inferenceTime: float, optional
    the time of the inference (in seconds)
  label: str, optional
    a text shown with the graph, above the time of the inference

  Returns
  -------
  str
    the beginning of the dot text (the graph is not closed)
  """
  res = f'{graphType} structs {{\n  fontcolor="{getBlackInTheme()}";bgcolor="transparent";'
  lines = [] if label is None else [label]
  if inferenceTime is not None and pyagrum.config.typed["notebook", "showInferenceTime"]:
    lines.append(f"Inference in {1000 * inferenceTime:6.2f}ms")
  if lines:
    res += '  label="' + "\n".join(lines) + '";\n'
  fontname, fontsize = fontFromMatplotlib()
  res += (
    f'  node [fillcolor="{pyagrum.config["dot", "nodeBgColor"]}", style=filled,'
    f'color="{configFgColor("dot", "nodeFgColor")}",fontname="{fontname}",fontsize="{fontsize}"];\n'
  )
  res += f'  edge [color="{getBlackInTheme()}"];\n'
  return res


def inferenceTemporaryDirectory() -> TemporaryDirectory:
  """
  The temporary directory of the images of the nodes of the graph of an inference.

  It is removed with the graph: :func:`inferenceDotGraph` keeps it in the graph and cleans it up when the graph is
  garbage collected, so that the images live as long as the graph.

  Returns
  -------
  tempfile.TemporaryDirectory
    the temporary directory
  """
  return TemporaryDirectory(prefix="tmp")


def inferenceImageFilename(temp_dir: TemporaryDirectory, name: str) -> str:
  """
  The name of the file of the image of a node in the temporary directory of the graph of an inference.

  Parameters
  ----------
  temp_dir: tempfile.TemporaryDirectory
    the temporary directory (see :func:`inferenceTemporaryDirectory`)
  name: str
    the name of the node

  Returns
  -------
  str
    the name of the file, in the format of the key ``notebook.graphFormat`` of `pyagrum.config`
  """
  return os.path.join(
    temp_dir.name,
    hashlib.md5(name.encode(), usedforsecurity=False).hexdigest() + "." + pyagrum.config["notebook", "graphFormat"],
  )


def valueOfNode(values: dict | None, name: str, nid: int):
  """
  The value given to a node in a dict keyed by names or by ids (the name wins).

  Parameters
  ----------
  values: dict[str | int, Any] or None
    the values of some nodes
  name: str
    the name of the node
  nid: int
    the id of the node

  Returns
  -------
  Any or None
    the value of the node, or None if it has none
  """
  if values is None:
    return None
  if name in values:
    return values[name]
  return values.get(nid)


def inferenceNodeDot(
  name: str,
  nid: int,
  targets,
  nodeColor: dict | None,
  cmapNode,
  isEvidence: bool,
  temp_dir: TemporaryDirectory,
  saveFig,
  otherAttributes: str = "",
) -> str:
  """
  The dot text of a node in the graph of an inference: the image of its posterior for a target, a colored node else.

  The colors come from the configuration, then from ``nodeColor``, then from the evidence.

  Parameters
  ----------
  name: str
    the name of the node
  nid: int
    the id of the node
  targets: set
    the names or ids of the targets (empty: all the nodes are targets)
  nodeColor: dict[str | int, float], optional
    the values of some nodes, keyed by names or by ids, colored with ``cmapNode``
  cmapNode: matplotlib.colors.Colormap
    the colormap of the nodes
  isEvidence: bool
    True if the node is observed
  temp_dir: tempfile.TemporaryDirectory
    the temporary directory of the images
  saveFig: Callable[[str, str], None]
    saves the image of the posterior of the node, given the name of the file and the background color
  otherAttributes: str
    other attributes of a node which is not a target, ending with a comma

  Returns
  -------
  str
    the dot text of the node
  """
  isTarget = len(targets) == 0 or name in targets or nid in targets

  bgcol = pyagrum.config["notebook", "figureFaceColor"] if isTarget else pyagrum.config["dot", "nodeBgColor"]
  fgcol = configFgColor("dot", "nodeFgColor")

  value = valueOfNode(nodeColor, name, nid)
  if value is not None:
    bgcol = proba2bgcolor(value, cmapNode)
    fgcol = proba2fgcolor(value, cmapNode)

  if isEvidence:
    bgcol = pyagrum.config["notebook", "evidenceBgColor"]
    fgcol = pyagrum.config["notebook", "evidenceFgColor"]

  colorattribute = f'fillcolor="{bgcol}", fontcolor="{fgcol}", color="#000000"'
  if isTarget:
    filename = inferenceImageFilename(temp_dir, name)
    saveFig(filename, bgcol)
    return f' "{name}" [shape=rectangle,image="{filename}",label="", {colorattribute}];\n'
  return f' "{name}" [{otherAttributes}{colorattribute}];\n'


def inferenceArcsDot(arcs, nameOf, arcWidth: dict | None, arcColor: dict | None, cmapArc, oriented: bool = True) -> str:
  """
  The dot text of the arcs (or edges) in the graph of an inference.

  Parameters
  ----------
  arcs: Iterable[tuple[int, int]]
    the arcs or the edges
  nameOf: Callable[[int], str]
    the name of a node from its id
  arcWidth: dict[tuple[int, int], float], optional
    the values of some arcs, shown by their width and in their tooltip
  arcColor: dict[tuple[int, int], float], optional
    the values of some arcs, colored with ``cmapArc``
  cmapArc: matplotlib.colors.Colormap
    the colormap of the arcs
  oriented: bool
    True for the arcs of a digraph, False for the edges of a graph

  Returns
  -------
  str
    the dot text of the arcs
  """
  link, symbol = ("->", "&rarr;") if oriented else ("--", "&mdash;")
  bounds = _widthBounds(arcWidth)

  res = ""
  for a in arcs:
    (n, j) = a
    pw = 1.0
    av = _linkTooltip(n, j, symbol)
    col = getBlackInTheme()

    if arcWidth is not None and a in arcWidth:
      pw = _scaledWidth(arcWidth[a], bounds)
      av = _linkTooltip(n, j, symbol, arcWidth[a])

    if arcColor is not None and a in arcColor:
      col = proba2color(arcColor[a], cmapArc)

    res += f' "{nameOf(n)}"{link}"{nameOf(j)}" [penwidth="{pw}",tooltip="{av}",color="{col}"];\n'
  return res


def inferenceDotGraph(
  dotstr: str, size, temp_dir: TemporaryDirectory, sizeKey: tuple[str, str] = ("notebook", "graphInferenceSize")
):
  """
  The pydot graph of the dot text of an inference, keeping the temporary directory of the images of its nodes.

  Parameters
  ----------
  dotstr: str
    the dot text
  size: str or int, optional
    the size of the graph (the value of the key ``sizeKey`` of `pyagrum.config` if None)
  temp_dir: tempfile.TemporaryDirectory
    the temporary directory of the images of the nodes (see :func:`inferenceTemporaryDirectory`): kept in the graph,
    it is removed with the graph
  sizeKey: tuple[str, str]
    the key of the default size in `pyagrum.config`

  Returns
  -------
  pydot.Dot
    the graph; the images of its nodes live as long as this graph object, so it must be kept while it is rendered

  Raises
  ------
  ValueError
    if ``dotstr`` is not a valid dot graph
  """
  g = _parseDot(dotstr)
  g.set_size(str(pyagrum.config[sizeKey] if size is None else size))
  g.temp_dir = temp_dir
  weakref.finalize(g, temp_dir.cleanup)  # an explicit cleanup: its implicit one emits a ResourceWarning
  return g


# noinspection PyProtectedMember
def prepareDot(dotgraph: dot.Dot) -> dot.Dot:
  """
  Complete a pydot graph with the look of the configuration: transparent background, direction and layout,
  color of the arcs and of the border of the nodes, color of the text of the nodes (black or white according to their
  background color).

  It is meant for the graphs built in Python by pyAgrum: each of these builders returns a graph already completed by
  this function. The graphs written by the ``toDot()`` of aGrUM are already complete, and the functions which show or
  export a graph (showGraph, export...) only apply its size: a graph given by the user is shown as it is.

  What the graph already gives (for an element or by default for all of them: ``node [...]``) is respected, so calling it
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
    dotgraph.set_rankdir(pyagrum.config["dot", "graphRankDir"])
  if dotgraph.get_layout() is None and "layout" not in defaults["graph"]:
    dotgraph.set_layout(pyagrum.config["dot", "graphLayout"])

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
      n.set_fontcolor(contrastFgColor(fillcolor))
    else:
      n.set_fontcolor(black)

  return dotgraph
