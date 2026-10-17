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
Helper functions for Junction Tree / CliqueGraph → dot representations.
Used internally by notebook.py.
"""

import pydot as dot

import pyagrum
import pyagrum.lib.dotstyle as gds


def _junctionTreeOptions(
  jt: pyagrum.CliqueGraph, model: pyagrum.BayesNet, withNames: bool, size: float | str | None = None
) -> pyagrum.DotOptions:
  """The options of the toDot of a junction tree of a model: the cliques and the separators are labelled by the names (or the ids) of their variables."""
  options = pyagrum.DotOptions()
  if size is not None:
    options.setSize(str(size))

  def names(nodes) -> list[str]:
    return sorted(model.variable(n).name() for n in nodes) if withNames else [str(n) for n in sorted(nodes)]

  for c in jt.nodes():
    options.setNodeLabel(c, f"({c}):" + ",".join(names(jt.clique(c))))
  for c1, c2 in jt.edges():
    options.setArcLabel(c1, c2, ",".join(names(jt.separator(c1, c2))))
  return options


def _junctionTreeDot(jt: pyagrum.CliqueGraph, size: float | str | None = None) -> dot.Dot:
  """The pydot graph of a junction tree built for a model (the attribute `_engine` of the junction tree)."""
  model = jt._engine._model
  withNames = pyagrum.config.typed["notebook", "junctionTreeWithNames"]
  return gds.dotFromAgrum(jt.toDot(_junctionTreeOptions(jt, model, withNames, size)))


def _mapDotString(
  jt: pyagrum.CliqueGraph,
  scaleClique: float | None,
  scaleSep: float | None,
  lenEdge: float | None,
  colorClique: str | None,
  colorSep: str | None,
) -> str:
  """The dot string of the map of a junction tree; the values which are None are the ones of the configuration."""
  if scaleClique is None and scaleSep is None and lenEdge is None and colorClique is None and colorSep is None:
    return jt.__map_str__()
  if scaleClique is None:
    scaleClique = float(pyagrum.config["notebook", "junctionTreeMapCliqueScale"])
  if scaleSep is None:
    scaleSep = float(pyagrum.config["notebook", "junctionTreeMapSepScale"])
  if lenEdge is None:
    lenEdge = float(pyagrum.config["notebook", "junctionTreeMapEdgeLen"])
  return jt.__map_str__(scaleClique, scaleSep, lenEdge, colorClique or "", colorSep or "")


def _junctionTreeMapDot(
  bn: pyagrum.BayesNet,
  scaleClique: float | None,
  scaleSep: float | None,
  lenEdge: float | None,
  colorClique: str | None,
  colorSep: str | None,
) -> dot.Dot:
  """Build junction tree map as a pydot.Dot using config defaults for None parameters."""
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  return gds.dotFromAgrum(_mapDotString(jt, scaleClique, scaleSep, lenEdge, colorClique, colorSep))


def _junctionTreeDotStr(bn: pyagrum.BayesNet, withNames: bool) -> str:
  """Build junction tree as a dot string."""
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  return jt.toDot(_junctionTreeOptions(jt, bn, withNames))
