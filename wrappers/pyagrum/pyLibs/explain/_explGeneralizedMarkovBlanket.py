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

import pyagrum.lib._colors as gumcols
import matplotlib.pyplot as plt
import pydot as dot


def _buildMB(model, x: int, k: int = 1):
  """
  Build the nodes and arcs of Markov Blanket (of order k) of node x

  Parameters
  ----------
  model: pyagrum.DirectedGraphicalModel
      i.e. a class with methods parents, children, variable(i), idFromName(name)
  x : int
      the nodeId of the node for the Markov blanket
  k: int
      the order of the Markov blanket. If k=2, build the MarkovBlanket(MarkovBlanket())

  Returns
  -------
      (nodes,arcs,depth) : the set of nodes, the set of arcs of the Markov Blanket and a dict[Str,int] that gives the MB-depth of each node in nodes.
  """
  nodes = {x}
  arcs = set()
  depth = dict()

  def _internal_build_markov_blanket(bn, x: int, k: int):
    nodes.add(x)
    depth[x] = k
    if k == 0:
      return
    for y in bn.parents(x):
      visit(y, k - 1)
      arcs.add((y, x))
    for y in bn.children(x):
      visit(y, k - 1)
      arcs.add((x, y))
      for z in bn.parents(y):
        visit(z, k - 1)
        arcs.add((z, y))

  def visit(x, k):
    if x in nodes and depth[x] >= k:
      return
    _internal_build_markov_blanket(model, x, k)

  _internal_build_markov_blanket(model, x, k)
  return nodes, arcs, depth


def generalizedMarkovBlanket(bn, var: int | str, k: int = 1, cmapNode=None):
  """
  Build a pydot.Dot representation of the nested Markov Blankets (of order k) of node x

  Warnings
  --------
  It is assumed that k<=8. If not, every thing is fine except that the colorscale will change in order to accept more colors.

  Parameters
  ----------
  bn: pyagrum.DirectedGraphicalModel
      i.e. a class with methods parents, children, variable(i), idFromName(name)
  var : str|int
      the name or nodeId of the node for the Markov blanket
  k: int
      the order of the Markov blanket. If k=1, build the MarkovBlanket(MarkovBlanket())
  cmap: maplotlib.ColorMap
      the colormap used (if not, inferno is used)

  Returns
  -------
      pydotplus.Dot object
  """
  if cmapNode is None:
    cmapNode = plt.get_cmap("inferno")  # gum.config["notebook", "default_arc_cmap"])

  maxcols = max(
    8, k
  )  # It is assumed that k<=8. If not, every thing is fine except that the colorscale will change in order to accept more colors.

  mb = dot.Dot(f"MB({var},{k}", graph_type="digraph", bgcolor="transparent")

  if isinstance(var, str):
    nx = bn.idFromName(var)
  else:
    nx = var
  nodes, arcs, visited = _buildMB(bn, nx, k)
  names = dict()

  for n in nodes:
    protected_name = f'"{bn.variable(n).name()}"'
    pnode = dot.Node(protected_name, style="filled")
    if n == var:
      bgcol = "#99FF99"
      fgcol = "black"
    else:
      bgcol = gumcols.proba2bgcolor(1 - (k - visited[n]) / maxcols, cmapNode)
      fgcol = gumcols.proba2fgcolor(1 - (k - visited[n]) / maxcols, cmapNode)
    pnode.set_fillcolor(bgcol)
    pnode.set_fontcolor(fgcol)
    mb.add_node(pnode)
    names[n] = protected_name
  for n in nodes:
    for u in bn.parents(n).intersection(nodes):
      edge = dot.Edge(names[u], names[n])
      if (u, n) in arcs:
        edge.set_color("black")
      else:
        edge.set_color("#DDDDDD")
      mb.add_edge(edge)

  return mb
