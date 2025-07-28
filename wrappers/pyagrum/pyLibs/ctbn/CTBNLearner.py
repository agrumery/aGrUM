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
#   See LICENCES for more details.                                         #
#                                                                          #
#   SPDX-FileCopyrightText: Copyright 2005-2025                            #
#       - Pierre-Henri WUILLEMIN(_at_LIP6)                                 #
#       - Christophe GONZALES(_at_AMU)                                     #
#   SPDX-License-Identifier: LGPL-3.0-or-later OR MIT                      #
#                                                                          #
#   Contact  : info_at_agrum_dot_org                                       #
#   homepage : http://agrum.gitlab.io                                      #
#   gitlab   : https://gitlab.com/agrumery/agrum                           #
#                                                                          #
############################################################################

from itertools import combinations

import pyagrum

from pyagrum.ctbn import CTBN
from pyagrum.ctbn import Trajectory

"""
This file contains the learning algorithm of a CTBN (i.e of its CIMs and dependency graph). 
"""


class Learner:
  """
  Class used to learn a CTBN (independence between variables and CIMs) using samples.

  Parameters
  ----------
  source : str|Dict[int, List[Tuple[float, str, str]]]
          Path to the csv file containing the samples(trajectories). Or directly the trajectories in a python dict.
  """

  def __init__(self, source):
    if isinstance(source, str):
      self.data = pyagrum.ctbn.readTrajectoryCSV(source)
    else:
      self.data = source

  def fitParameters(self, ctbn: CTBN):
    """
    Learns the parameters of ``ctbn``'s CIMs.

    Parameters
    ----------
    ctbn : CTBN
        CTBN containing the CIMs to learn.
    """
    traj = Trajectory(self.data, ctbn)
    traj.computeAllCIMs()

  def learnCTBN(self, template: CTBN = None) -> CTBN:
    """
    Learns a CTBN, using the CTPC(continuous-time PC) algorithm.
    Reference : A. Bregoli, M. Scutari, F. Stella, Constraint-Based Learning for Continuous-Time Bayesian Networks,
    arXiv:2007.03248, 2020.

    Parameters
    ----------
    template : CTBN
        CTBN used to find variables. If not given, variables are searched inside the trajectories.
        (if the trajectory is very short, some variables can be missed).

    Returns
    -------
    CTBN
        The learned ctbn.
    """
    test = pyagrum.ctbn.FChi2Test(Trajectory(self.data))
    # List[pyagrum.DiscreteVariable] : list of all the variables of our Ctbn
    newVariableList = []

    newGraph = pyagrum.DiGraph()
    newCtbn = CTBN()

    if template is None:
      tmpCtbn = pyagrum.ctbn.CTBNFromData(self.data)
      newVariableList = tmpCtbn.variables()

    else:
      newVariableList = [pyagrum.LabelizedVariable(var) for var in template.variables()]

    for var in newVariableList:
      newGraph.addNodeWithId(
        newCtbn.add(var)
      )  # makes the link between the variable and its corresponding node in the DiGraph

    # List[int] : list of the indexes of all the nodes
    variableIdList = newGraph.nodes()

    # We add all the possible arcs first (initialisation of the Ctbn)
    # logging.debug(f"variables id list : {variableIdList}")
    for id1 in variableIdList:
      for id2 in variableIdList:
        if id1 != id2:
          newGraph.addArc(id1, id2)  # arc from var1 to var2

    for Xi in variableIdList:  # (2.)
      # List[int] : "U" in the Continuous-time PC Algorithm (2.1)
      parentsIdList = list(newGraph.parents(Xi))
      length = len(parentsIdList)

      parents_done = {id: False for id in parentsIdList}

      for b in range(length):  # +1  # (2.2)
        for Xj in parentsIdList:  # (2.2.1 / 2.2.2)
          if parents_done[Xj]:
            continue

          UXj = parentsIdList.copy()
          UXj.remove(Xj)

          if b == 0:
            loop = [[]]
          elif b <= len(UXj):
            loop = combinations(UXj, b)

          for SXiXj in loop:
            SXiXj = list(SXiXj)
            independance_test = test.testIndep(newCtbn.name(Xi), newCtbn.name(Xj), [newCtbn.name(id) for id in SXiXj])

            if independance_test:  # if the test is True
              newGraph.eraseArc(Xj, Xi)
              parents_done[Xj] = True
              break

          parentsIdList = [var for var in parents_done.keys() if not parents_done[var]]

    for arc in newGraph.arcs():
      newCtbn.addArc(arc[0], arc[1])

    return newCtbn
