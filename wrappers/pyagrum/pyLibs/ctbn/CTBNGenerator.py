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

from typing import Tuple
import pyagrum
import random
import logging

from pyagrum.ctbn import CIM
from pyagrum.ctbn import CTBN
from pyagrum.ctbn.constants import NodeId

"""
This file is used to generate random CTBNs using Prufer code
"""


def directTree(graph: pyagrum.MixedGraph, root: NodeId):
  """
  Direct a undirected tree.

  Parameters
  ----------
  graph : pyagrum.MixedGraph
      A graph that contains directed/undirected arcs.
  root : NodeId
      Root of the tree to direct.
  """
  for i in graph.neighbours(root):
    graph.eraseEdge(i, root)
    graph.addArc(root, i)
    # CTBN.addArc(root, i)
    directTree(graph, i)


def randomCIMs(ctbn: CTBN, valueRange: Tuple[float, float]):
  """
  Fills the ``ctbn``'s CIMs with random values in ``valueRange``.
  Note : as diagonal coefficents are the negative sum of the coefficients one the line,
  their value does not necessarily belong to ``valueRange``.

  Parameters
  ----------
  ctbn : CTBN
      The ctbn to fill.
  valueRange : Tuple[float, float]
      Range to choose values from when filling the cims.
  """

  for v in ctbn.names():
    v_i = ctbn.CIM(v).findVar(CIM.varI(v))
    v_j = ctbn.CIM(v).findVar(CIM.varJ(v))

    I1 = pyagrum.Instantiation(ctbn.CIM(v)._pot)
    I2 = pyagrum.Instantiation()
    ind = I1.pos(v_j)
    I2.add(I1.variable(ind))

    I1.setFirst()
    while not I1.end():
      I2.setFirst()
      sumCIM = 0
      while not I2.end():
        I1.setVals(I2)
        if I1.val(v_i) != I1.val(v_j):
          draw = random.uniform(valueRange[0], valueRange[1])
          sumCIM += draw
          ctbn.CIM(v)._pot.set(I1, draw)
        else:
          ctbn.CIM(v)._pot.set(I1, 0)

        I2.inc()

      tmp = pyagrum.Instantiation(I1)
      tmp.chgVal(v_j, tmp.val(v_i))
      ctbn.CIM(v)._pot.set(tmp, -sumCIM)
      I1.incOut(I2)


def randomCTBN(valueRange: Tuple[float, float], n: int = 1, parMax: int = 1, modal: int = 2) -> CTBN:
  """
  Generates a random CTBN using Prufer's sequence.
  Note : as diagonal coefficents are the negative sum of the coefficients one the line in a CIM,
  their value does not necessarily belong to ``valueRange``.

  Parameters
  ----------
  valueRange : Tuple[float, float]
      Range to choose values from when filling the CIMs.
  n : int
      Number of variables.
  parMax : int
      Maximum number of parents a variable can have.
  modal : int
      Number of states a variable has (domain size).

  Returns
  -------
  CTBN
      A randomly generated ctbn.
  """
  ctbn = CTBN()

  graph = pyagrum.MixedGraph()

  for i in range(n):
    name = f"V{i + 1}"
    labels = [f"v{i + 1}_{t}" for t in range(1, modal + 1)]
    var = pyagrum.LabelizedVariable(name, name, labels)
    ctbn.add(var)
    graph.addNode()

  # generating Prufer's sequence and associated tree
  if n == 2:
    if random.random() > 0.5:
      graph.addArc(0, 1)
    else:
      graph.addArc(1, 0)

  elif n > 2:
    sequence = [random.randint(0, n - 1) for _ in range(n - 2)]
    degree = {id: 1 for id in range(0, n)}
    par = {id: 0 for id in range(0, n)}
    for i in range(n - 2):
      degree[sequence[i]] += 1

    for i in range(n - 2):
      for j in range(n):
        if degree[j] == 1:
          graph.addEdge(j, sequence[i])
          par[j] += 1
          par[sequence[i]] += 1
          degree[sequence[i]] -= 1
          degree[j] -= 1
          break

    u = 0
    v = 0
    for i in range(n):
      if degree[i] == 1:
        if u == 0:
          u = i
        else:
          v = i
          break

    graph.addEdge(u, v)

    # direct it
    directTree(graph, random.randint(0, n - 1))

  # add/remove/invert arcs at random

  for i in range(n * 3):
    event = random.sample(range(0, n), k=2)
    draw = random.randint(1, 3)

    if draw == 1:
      if len(graph.parents(event[1])) < parMax:
        graph.addArc(event[0], event[1])

    elif draw == 2:
      if graph.existsArc(event[0], event[1]):
        graph.eraseArc(event[0], event[1])
        if len(graph.connectedComponents()) > 1:
          graph.addArc(event[0], event[1])

    else:
      if graph.existsArc(event[0], event[1]):
        if len(graph.parents(event[0])) < parMax:
          graph.eraseArc(event[0], event[1])
          graph.addArc(event[1], event[0])

  # add arcs to the CTBN
  for arc in graph.arcs():
    ctbn.addArc(arc[0], arc[1])

  randomCIMs(ctbn, valueRange)

  return ctbn
