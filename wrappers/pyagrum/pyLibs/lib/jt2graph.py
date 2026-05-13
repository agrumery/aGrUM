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


def _junctionTreeMapDot(
  bn: pyagrum.BayesNet,
  scaleClique: float | None,
  scaleSep: float | None,
  lenEdge: float | None,
  colorClique: str | None,
  colorSep: str | None,
) -> dot.Dot:
  """Build junction tree map as a pydot.Dot using config defaults for None parameters."""
  if scaleClique is None:
    scaleClique = float(pyagrum.config["notebook", "junctiontree_map_cliquescale"])
  if scaleSep is None:
    scaleSep = float(pyagrum.config["notebook", "junctiontree_map_sepscale"])
  if lenEdge is None:
    lenEdge = float(pyagrum.config["notebook", "junctiontree_map_edgelen"])
  if colorClique is None:
    colorClique = pyagrum.config["notebook", "junctiontree_clique_bgcolor"]
  if colorSep is None:
    colorSep = pyagrum.config["notebook", "junctiontree_separator_bgcolor"]
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  dotstring = jt.__map_str__(scaleClique, scaleSep, lenEdge, colorClique, colorSep)
  graphs = dot.graph_from_dot_data(dotstring)
  assert graphs is not None
  return graphs[0]


def _junctionTreeDotStr(bn: pyagrum.BayesNet, withNames: bool) -> str:
  """Build junction tree as a dot string with transparent background."""
  jtg = pyagrum.JunctionTreeGenerator()
  jt = jtg.junctionTree(bn)
  jt._engine = jtg
  jtg._model = bn
  dotstring = jt.toDotWithNames(bn) if withNames else jt.toDot()
  graphs = dot.graph_from_dot_data(dotstring)
  assert graphs is not None
  g = graphs[0]
  g.set_bgcolor("transparent")
  return g.to_string()
