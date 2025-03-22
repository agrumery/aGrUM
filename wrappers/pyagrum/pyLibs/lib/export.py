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

"""
Tools for exporting graphical models and inference as other formats.

For each function `pyagrum.lib.export.to...(model,filename)`, it is assumed that the filename is complete (including the correct suffix).
"""
import sys

import pyagrum as gum


def toGML(model, filename: str = None):
  """
  Export directed graphical models as a graph to the graph GML format (https://gephi.org/users/supported-graph-formats/gml-format/)

  Parameters
  ----------
  model :
    the directed graphical model to export
  filename : Optional[str]
    the name of the file (including the prefix), if None , use sys.stdout
  """

  def _toGML(model, gmlfile):
    print("graph", file=gmlfile)
    print("[", file=gmlfile)
    for i in model.nodes():
      print("  node", file=gmlfile)
      print("  [", file=gmlfile)
      print(f"    id X{i}", file=gmlfile)
      print(f"    label \"{model.variable(i).name()}\"", file=gmlfile)
      print("  ]", file=gmlfile)
    print("", file=gmlfile)
    for i, j in model.arcs():
      print("  edge", file=gmlfile)
      print("  [", file=gmlfile)
      print(f"    source X{i}", file=gmlfile)
      print(f"    target X{j}", file=gmlfile)
      print("  ]", file=gmlfile)
    print("]", file=gmlfile)

  if filename is None:
    _toGML(model, sys.stdout)
  else:
    with open(filename, "w") as gmlfile:
      _toGML(model, gmlfile)
