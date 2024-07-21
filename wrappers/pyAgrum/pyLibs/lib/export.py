# (c) Copyright 2015-2024 by Pierre-Henri Wuillemin(@LIP6)
# (pierre-henri.wuillemin@lip6.fr)
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
Tools for exporting graphical models and inference as other formats.

For each function `pyAgrum.lib.export.to...(model,filename)`, it is assumed that the filename is complete (including the correct suffix).
"""
import sys

import pyAgrum as gum


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
