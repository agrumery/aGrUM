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
#   useful, but WITHOUT ANY WARRANTY; without even the implied warranty    #
#   of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the        #
#   GNU General Public License for more details.                           #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

"""Generate the images at the top of the graph pages of the documentation (_static/graph<type>.png).

Run from this directory: ``python graphTypes.py`` (needs pyagrum and graphviz's ``dot``).
"""

import subprocess

import pyagrum as gum

# E-A and F-B are reversible in the CPDAG; the MPDAG imposes E->A and keeps F-B
dag = gum.fastDAG("E->A->C;B->C;C->D;F->B")
cpdag = gum.CPDAG.fromDAG(dag)

GRAPHS = {
  "DiGraph": gum.fastDiGraph("A->B->C->A;C->D"),
  "DAG": gum.fastDAG("A->B->D;A->C->D;C->E"),
  "UndiGraph": gum.fastUndiGraph("A-B-C-A;C-D"),
  "MixedGraph": gum.fastMixedGraph("A->B-C->D;A-D"),
  "PDAG": gum.fastPDAG("A->B-C-A;D->C;B->E"),  # A->B-C-A: a partially directed cycle (not a chain graph)
  "ChainGraph": gum.fastChainGraph("A->B-C;D->C;C->E;E-F"),
  "CPDAG": cpdag,
  "MPDAG": gum.MPDAG.fromCPDAG(cpdag, [(0, 1)]),  # background knowledge: E->A (ids in fastDAG's order)
  "MarkedGraph": gum.fastMarkedGraph("A->B;B<->C;C o-> D;D o-o E;E-F"),
  "PAG": gum.fastPAG("X o-o Y;Y o-> Z;W o-> Z"),
  "MAG": gum.MAG.fromMarkedGraph(gum.fastMarkedGraph("E-F;F->A;A->B;B<->C;C->D")),
}

for name, graph in GRAPHS.items():
  print(f"{name:12}: {graph}")
  subprocess.run(["dot", "-Tpng", "-Gdpi=96", "-o", f"../graph{name}.png"], input=graph.toDot(), text=True, check=True)
