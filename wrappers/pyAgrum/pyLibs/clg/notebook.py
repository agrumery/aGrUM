# (c) Copyright 2022-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)

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

"""
The purpose of this module is to provide tools for mapping Conditional Linear Gaussian in dot language in order to
be displayed/saved as image.
"""

import matplotlib.pyplot as plt
import pyAgrum as gum
import pydot as dot

import pyAgrum.lib._colors as gumcols
import pyAgrum.lib.image as gimg

import pyAgrum.clg.CLG as CLG
import pyAgrum.clg.inference as clginference
import pyAgrum.lib.notebook as gnb


def CLG2dot(clg, *, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmapNode=None, cmapArc=None, showMsg=None):
  """
  create a pydot representation of the CLG

  Parameters
  ----------
  clg : CLG
    Conditional Linear Gaussian.
  size: str
    Size of the rendered graph.
  nodeColor: dict[Tuple(int,int),float]
    A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth: dict[Tuple(int,int),float]
    An arcMap of values to be shown as bold arcs.
  arcColor: dict[Tuple(int,int),float]
    An arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode: ColorMap
    Color map to show the vals of Nodes.
  cmapArc: ColorMap
    Color map to show the vals of Arcs.
  showMsg: dict
    A nodeMap of values to be shown as tooltip.

  Returns
  -------
  pydot.Dot
    The desired representation of the Conditional Linear Gaussian.
  """
  arcLabel = {arc: clg.coefArc(*arc) for arc in clg.arcs()}

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

  dotobj = dot.Dot(graph_type='digraph', bgcolor="transparent")

  for n in clg.names():
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
                    tooltip=f'"({clg.idFromName(n)}) {n}{res}"',
                    label=f"{{{n}|{{&mu;={clg.variable(n).mu():.3f}|&sigma;={clg.variable(n).sigma():.3f}}}}}",
                    shape="Mrecord"
                    )
    dotobj.add_node(node)

  for a in clg.arcs():
    if arcWidth is None:
      pw = 1
      av = ""
    else:
      if a in arcWidth:
        if maxarcs == minarcs:
          pw = 1
        else:
          pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
        av = arcWidth[a]
      else:
        pw = 1
        av = 1

    if arcColor is None:
      col = gumcols.getBlackInTheme()
    else:
      if a in arcColor:
        col = gumcols.proba2color(arcColor[a], cmapArc)
      else:
        col = gumcols.getBlackInTheme()

    if arcLabel is None:
      lb = ""
    else:
      if a in arcLabel:
        lb = "%.2f" % (arcLabel[a])
      else:
        lb = ""

    edge = dot.Edge('"' + clg.variable(a[0]).name() + '"', '"' + clg.variable(a[1]).name() + '"',
                    label=lb, fontsize="15",
                    penwidth=pw, color=col,
                    tooltip=f"{a} : {av}"
                    )
    dotobj.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  # dynamic member makes pylink unhappy
  # pylint: disable=no-member
  dotobj.set_size(size)

  return dotobj


def CLGInference2dot(clg, *, evs=None, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmapNode=None,
                     cmapArc=None, showMsg=None):
  """
  create a pydot representation of the CLG

  Parameters
  ----------
  clg : CLG
    Conditional Linear Gaussian.
  evs : dict[str,float]
    A dictionary of evidence.
  size: str
    Size of the rendered graph.
  nodeColor: dict[Tuple(int,int),float]
    A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth: dict[Tuple(int,int),float]
    An arcMap of values to be shown as bold arcs.
  arcColor: dict[Tuple(int,int),float]
    An arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode: ColorMap
    Color map to show the vals of Nodes.
  cmapArc: ColorMap
    Color map to show the vals of Arcs.
  showMsg: dict
    A nodeMap of values to be shown as tooltip.

  Returns
  -------
  pydot.Dot
    The desired representation of the Conditional Linear Gaussian.
  """
  ie = clginference.CLGVariableElimination(clg)
  ie.updateEvidence(evs)

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

  dotobj = dot.Dot(graph_type='digraph', bgcolor="transparent")

  for n in clg.names():
    if n in evs:
      bgcol = gum.config["notebook", "evidence_bgcolor"]
      fgcol = gum.config["notebook", "evidence_fgcolor"]
      res = ""
    else:
      if nodeColor is None or n not in nodeColor:
        bgcol = gum.config["notebook", "histogram_color"]
        fgcol = gumcols.rgb2brightness(*gumcols.hex2rgb(bgcol))
        res = ""
      else:
        bgcol = gumcols.proba2bgcolor(nodeColor[n], cmapNode)
        fgcol = gumcols.proba2fgcolor(nodeColor[n], cmapNode)
        res = f" : {nodeColor[n] if showMsg is None else showMsg[n]:2.5f}"
    v = ie.posterior(n)
    node = dot.Node('"' + n + '"', style="filled",
                    fillcolor=bgcol,
                    fontcolor=fgcol,
                    tooltip=f'"({clg.idFromName(n)}) {n}{res}"',
                    label=f"{{{n}|{{&mu;={v.mu():.3f}|&sigma;={v.sigma():.3f}}}}}",
                    shape="Mrecord"
                    )
    dotobj.add_node(node)

  for a in clg.arcs():
    if arcWidth is None:
      pw = 1
      av = ""
    else:
      if a in arcWidth:
        if maxarcs == minarcs:
          pw = 1
        else:
          pw = 0.1 + 5 * (arcWidth[a] - minarcs) / (maxarcs - minarcs)
        av = arcWidth[a]
      else:
        pw = 1
        av = 1
    if arcColor is None:
      col = gumcols.getBlackInTheme()
    else:
      if a in arcColor:
        col = gumcols.proba2color(arcColor[a], cmapArc)
      else:
        col = gumcols.getBlackInTheme()

    edge = dot.Edge('"' + clg.variable(a[0]).name() + '"', '"' + clg.variable(a[1]).name() + '"',
                    penwidth=pw, color=col,
                    tooltip=f"{a} : {av}"
                    )
    dotobj.add_edge(edge)

  if size is None:
    size = gum.config["notebook", "default_graph_size"]

  # dynamic member makes pylink unhappy
  # pylint: disable=no-member
  dotobj.set_size(size)

  return dotobj


def getInference(clg, *, evs=None, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmapNode=None, cmapArc=None,
                 showMsg=None):
  """
  return a pydot representation of the CLG

  Parameters
  ----------
  clg : CLG
    Conditional Linear Gaussian.
  evs : dict[str,float]
    A dictionary of evidence.
  size: str
    Size of the rendered graph.
  nodeColor: dict[Tuple(int,int),float]
    A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth: dict[Tuple(int,int),float]
    An arcMap of values to be shown as bold arcs.
  arcColor: dict[Tuple(int,int),float]
    An arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode: ColorMap
    Color map to show the vals of Nodes.
  cmapArc: ColorMap
    Color map to show the vals of Arcs.
  showMsg: dict
    A nodeMap of values to be shown as tooltip.

  Returns
  -------
  pydot.Dot
    The desired representation of the Conditional Linear Gaussian.
  """
  return CLGInference2dot(clg, evs=evs, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcColor=arcColor,
                          cmapNode=cmapNode, cmapArc=cmapArc, showMsg=showMsg)


def showInference(clg, *, evs=None, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmapNode=None,
                  cmapArc=None, showMsg=None):
  """
  display a pydot representation of the CLG

  Parameters
  ----------
  clg : CLG
    Conditional Linear Gaussian.
  evs : dict[str,float]
    A dictionary of evidence.
  size: str
    Size of the rendered graph.
  nodeColor: dict[Tuple(int,int),float]
    A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth: dict[Tuple(int,int),float]
    An arcMap of values to be shown as bold arcs.
  arcColor: dict[Tuple(int,int),float]
    An arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode: ColorMap
    Color map to show the vals of Nodes.
  cmapArc: ColorMap
    Color map to show the vals of Arcs.
  showMsg: dict
    A nodeMap of values to be shown as tooltip.
  """
  gnb.showDot(CLGInference2dot(clg, evs=evs, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcColor=arcColor,
                               cmapNode=cmapNode, cmapArc=cmapArc, showMsg=showMsg))


def exportCLG(clg, filename, *, size=None, nodeColor=None, arcWidth=None, arcColor=None, cmapNode=None, cmapArc=None,
              showMsg=None):
  """
  Export the clg as a picture.

  Parameters
  ----------
  clgdot : pydot.Dot
    pydot representation of the CLG.
  filename : str
    The filename of the exported file.
  size: str
    Size of the rendered graph.
  nodeColor: dict[Tuple(int,int),float]
    A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth: dict[Tuple(int,int),float]
    An arcMap of values to be shown as bold arcs.
  arcColor: dict[Tuple(int,int),float]
    An arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode: ColorMap
    Color map to show the vals of Nodes.
  cmapArc: ColorMap
    Color map to show the vals of Arcs.
  showMsg: dict
    A nodeMap of values to be shown as tooltip.
  """
  clgdot = CLG2dot(clg, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcColor=arcColor, cmapNode=cmapNode,
                   cmapArc=cmapArc, showMsg=showMsg)
  gimg.export(clgdot, filename)


def exportInference(clg, filename: str, *, evs=None, size=None, nodeColor=None, arcWidth=None, arcColor=None,
                    cmapNode=None, cmapArc=None,
                    showMsg=None):
  """
  export a representation of a CLG inference as a picture

  Parameters
  ----------
  clg : CLG
    Conditional Linear Gaussian.
  filename : str
    The filename of the exported file.
  evs : dict[str,float]
    A dictionary of evidence.
  size: str
    Size of the rendered graph.
  nodeColor: dict[Tuple(int,int),float]
    A nodeMap of values to be shown as color nodes (with special color for 0 and 1).
  arcWidth: dict[Tuple(int,int),float]
    An arcMap of values to be shown as bold arcs.
  arcColor: dict[Tuple(int,int),float]
    An arcMap of values (between 0 and 1) to be shown as color of arcs.
  cmapNode: ColorMap
    Color map to show the vals of Nodes.
  cmapArc: ColorMap
    Color map to show the vals of Arcs.
  showMsg: dict
    A nodeMap of values to be shown as tooltip.

  Returns
  -------
  pydot.Dot
    The desired representation of the Conditional Linear Gaussian.
  """
  clgdot = CLGInference2dot(clg, evs=evs, size=size, nodeColor=nodeColor, arcWidth=arcWidth, arcColor=arcColor,
                            cmapNode=cmapNode, cmapArc=cmapArc, showMsg=showMsg)
  gimg.export(clgdot, filename)


CLG.toDot = lambda self: str(CLG2dot(self))
CLG._repr_html_ = lambda self: gnb.getDot(self.toDot())
