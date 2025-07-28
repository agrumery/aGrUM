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

"""
The purpose of this module is to provide tools for comaring different BNs.
"""

import os
import math
from itertools import product, combinations

import pydot as dot

import pyagrum as gum
import pyagrum.lib.bn2graph as ggr
import pyagrum.lib.utils as gutils


STRUCTURAL_HAMMING = "structural hamming"
PURE_HAMMING = "hamming"


class GraphicalBNComparator:
  """
  BNGraphicalComparator allows to compare in multiple way 2 BNs...
  The smallest assumption is that the names of the variables are the same in
  the 2 BNs. But some comparisons will have also to check the type and
  domainSize of the variables.

  The bns have not exactly the  same role : _bn1 is rather the referent model
  for the comparison whereas _bn2 is the compared one to the referent model.

  Parameters
  ----------
  bn1 : str or pyagrum.BayesNet
    a BN or a filename for reference
  bn2 : str or pyagrum.BayesNet
    another BN or antoher filename for comparison
  """

  def __init__(self, bn1: str | gum.BayesNet, bn2: str | gum.BayesNet, delta=1e-6):
    self.DELTA_ERROR = delta
    if isinstance(bn1, str):
      self._bn1 = gum.loadBN(bn1)
      self._bn1.setProperty("name", '"' + os.path.basename(self._bn1.property("name") + '"'))
    else:
      self._bn1 = bn1

    if isinstance(bn2, str):
      self._bn2 = gum.loadBN(bn2)
      self._bn2.setProperty("name", '"' + os.path.basename(self._bn2.property("name") + '"'))
    else:
      self._bn2 = bn2

    s1: set[str] = set(self._bn1.names())
    s2: set[str] = set(self._bn2.names())

    if s1 != s2:
      raise ValueError(
        "The 2 BNs are not comparable! There are names not present in the 2 BNs : " + str(s1.symmetric_difference(s2))
      )

  def _compareBNVariables(self):
    """
    Checks if the two BNs have the same set of variables

    Returns
    -------
    str
      'OK' if the BNs have composed of the same variables, indicates problematic variables otherwise

    """
    # it is assumed (checked by the constructor) that _bn1 and _bn2 share the same set of variable names
    for i in self._bn1.nodes():
      v1 = self._bn1.variable(i)
      v2 = self._bn2.variableFromName(v1.name())
      if v2.domainSize() != v1.domainSize():
        return v1.name() + " has not the same domain size in the two bns"

    return "OK"

  @staticmethod
  def _parents_name(bn, n):
    return {bn.variable(p).name() for p in bn.parents(n)}

  def _compareBNParents(self):
    """
    Returns
    -------
    str
      'OK' if _bn2 have (at least) the same variable as b1 and their parents are the same.

    """
    for id1 in self._bn1.nodes():
      id2 = self._bn2.idFromName(self._bn1.variable(id1).name())

      p1 = self._parents_name(self._bn1, id1)
      p2 = self._parents_name(self._bn2, id2)
      if p1 != p2:
        return (
          self._bn1.variable(id1).name()
          + " has different parents in the two bns whose names are in "
          + str(p1.symmetric_difference(p2))
        )

    return "OK"

  def _compareTensors(self, pot1, pot2):
    """
    Compare 2 tensors one in each Bayesian network

    Parameters
    ----------
    pot1 : pyagrum.Tensor
      one of b1's cpts
    pot2 : pyagrum.Tensor
      one of _bn2's cpts

    Returns
    -------
    str
      'OK' if CPTs are the same

    Raises
    ------
    gum.KeyError
      If cpts are not from the same variable
    """
    I1 = gum.Instantiation(pot1)
    I2 = gum.Instantiation(pot2)
    I1.setFirst()
    while not I1.end():
      I2.fromdict(I1.todict())  # copy value on the base of names
      if abs(pot1.get(I1) - pot2.get(I2)) > self.DELTA_ERROR:
        return "Different CPTs for " + pot1.variable(0).name()
      I1 += 1
    return "OK"

  def _compareBNCPT(self):
    """
    Returns
    -------
    str
      'OK' if _bn2 have (at least) the same variable as b1 and their cpts are the same
    """
    for i in self._bn1.nodes():
      res = self._compareTensors(self._bn1.cpt(i), self._bn2.cpt(self._bn1.variable(i).name()))
      if res != "OK":
        return res
    return "OK"

  def equivalentBNs(self):
    """
    Check if the 2 BNs are equivalent :

    * same variables
    * same graphical structure
    * same parameters

    Returns
    -------
    str
      "OK" if bn are the same, a description of the error otherwise

    """

    ret = self._compareBNVariables()
    if ret != "OK":
      return ret

    ret = self._compareBNParents()
    if ret != "OK":
      return ret

    ret = self._compareBNCPT()
    return ret

  def dotDiff(self):
    """Return a pydot graph that compares the arcs of _bn1 (reference) with those of self._bn2.
    full black line: the arc is common for both
    full red line: the arc is common but inverted in _bn2
    dotted black line: the arc is added in _bn2
    dotted red line: the arc is removed in _bn2

    Warning
    -------
    if pydot is not installed, this function just returns None

    Returns
    -------
    pydot.Dot
      the result dot graph or None if pydot can not be imported
    """
    return graphDiff(self._bn1, self._bn2)

  def skeletonScores(self):
    """
    Compute Precision, Recall, F-score for skeletons of self._bn2 compared to self._bn1

    precision and recall are computed considering BN1 as the reference

    Fscor is 2*(recall* precision)/(recall+precision) and is the weighted average of Precision and Recall.

    dist2opt=square root of (1-precision)^2+(1-recall)^2 and represents the euclidian distance to the ideal point (precision=1, recall=1)

    Returns
    -------
    dict[str,double]
      A dictionnary containing 'precision', 'recall', 'fscore', 'dist2opt' and so on.
    """
    # t: True, f: False, p: Positive, n: Negative
    count = {"tp": 0, "tn": 0, "fp": 0, "fn": 0}

    # We look at all combination
    listVariables = self._bn1.names()

    # Loop on pairs of variables
    for head, tail in combinations(listVariables, 2):
      idHead_1 = self._bn1.idFromName(head)
      idTail_1 = self._bn1.idFromName(tail)

      idHead_2 = self._bn2.idFromName(head)
      idTail_2 = self._bn2.idFromName(tail)

      if self._bn1.dag().existsArc(idHead_1, idTail_1) or self._bn1.dag().existsArc(
        idTail_1, idHead_1
      ):  # Check edge node1-node2
        if self._bn2.dag().existsArc(idHead_2, idTail_2) or self._bn2.dag().existsArc(idTail_2, idHead_2):  # if edge:
          count["tp"] += 1
        else:  # If no edge:
          count["fn"] += 1
      else:  # Check if no edge
        if self._bn2.dag().existsArc(idHead_2, idTail_2) or self._bn2.dag().existsArc(idTail_2, idHead_2):  # If edge
          count["fp"] += 1
        else:  # If no arc
          count["tn"] += 1

    # Compute the scores
    if count["tp"] + count["fn"] != 0:
      recall = (1.0 * count["tp"]) / (count["tp"] + count["fn"])
    else:
      recall = 0.0

    if count["tp"] + count["fp"] != 0:
      precision = (1.0 * count["tp"]) / (count["tp"] + count["fp"])
    else:
      precision = 0.0

    if precision + recall != 0.0:
      Fscore = (2 * recall * precision) / (recall + precision)
    else:
      Fscore = 0.0

    return {
      "count": count,
      "recall": recall,
      "precision": precision,
      "fscore": Fscore,
      "dist2opt": math.sqrt((1 - precision) ** 2 + (1 - recall) ** 2),
    }

  def scores(self):
    """
    Compute Precision, Recall, F-score for self._bn2 compared to self._bn1

    precision and recall are computed considering BN1 as the reference

    Fscore is 2*(recall* precision)/(recall+precision) and is the weighted average of Precision and Recall.

    dist2opt=square root of (1-precision)^2+(1-recall)^2 and represents the euclidian distance to the ideal point (precision=1, recall=1)

    Returns
    -------
    dict[str,double]
      A dictionnary containing 'precision', 'recall', 'fscore', 'dist2opt' and so on.
    """
    # t: True, f: False, p: Positive, n: Negative
    count = {"tp": 0, "tn": 0, "fp": 0, "fn": 0}

    # We look at all combination
    listVariables = self._bn1.names()

    # Loop on oriented pairs of variables
    for head, tail in product(listVariables, listVariables):
      if head != tail:
        idHead_1 = self._bn1.idFromName(head)
        idTail_1 = self._bn1.idFromName(tail)

        idHead_2 = self._bn2.idFromName(head)
        idTail_2 = self._bn2.idFromName(tail)

        if self._bn1.dag().existsArc(idHead_1, idTail_1):  # Check arcs head->tail
          if self._bn2.dag().existsArc(idHead_2, idTail_2):  # if arc:
            count["tp"] += 1
          else:  # If no arc:
            count["fn"] += 1
        else:  # Check if no arc
          if self._bn2.dag().existsArc(idHead_2, idTail_2):  # If arc
            count["fp"] += 1
          else:  # If no arc
            count["tn"] += 1

    # Compute the scores
    if count["tp"] + count["fn"] != 0:
      recall = (1.0 * count["tp"]) / (count["tp"] + count["fn"])
    else:
      recall = 0.0

    if count["tp"] + count["fp"] != 0:
      precision = (1.0 * count["tp"]) / (count["tp"] + count["fp"])
    else:
      precision = 0.0

    if precision + recall != 0.0:
      Fscore = (2 * recall * precision) / (recall + precision)
    else:
      Fscore = 0.0

    return {
      "count": count,
      "recall": recall,
      "precision": precision,
      "fscore": Fscore,
      "dist2opt": math.sqrt((1 - precision) ** 2 + (1 - recall) ** 2),
    }

  def hamming(self):
    """
    Compute hamming and structural hamming distance

    Hamming distance is the difference of edges comparing the 2 skeletons, and Structural Hamming difference is the
    difference comparing the cpdags,	including the arcs' orientation.

    Returns
    -------
    dict[double,double]
      A dictionary containing PURE_HAMMING,STRUCTURAL_HAMMING
    """
    # convert graphs to cpdags
    cpdag1 = gum.EssentialGraph(self._bn1).pdag()
    cpdag2 = gum.EssentialGraph(self._bn2).pdag()

    # We look at all combinations
    listVariables = self._bn1.names()
    hamming_dico = {PURE_HAMMING: 0, STRUCTURAL_HAMMING: 0}

    for head, tail in combinations(listVariables, 2):
      idHead_1 = self._bn1.idFromName(head)
      idTail_1 = self._bn1.idFromName(tail)

      idHead_2 = self._bn2.idFromName(head)
      idTail_2 = self._bn2.idFromName(tail)

      if cpdag1.existsArc(idHead_1, idTail_1):  # Check arcs head->tail
        if cpdag2.existsArc(idTail_2, idHead_2) or cpdag2.existsEdge(idTail_2, idHead_2):
          hamming_dico[STRUCTURAL_HAMMING] += 1
        elif (
          not cpdag2.existsArc(idTail_2, idHead_2)
          and not cpdag2.existsArc(idHead_2, idTail_2)
          and not cpdag2.existsEdge(idTail_2, idHead_2)
        ):
          hamming_dico[STRUCTURAL_HAMMING] += 1
          hamming_dico[PURE_HAMMING] += 1

      elif cpdag1.existsArc(idTail_1, idHead_1):  # Check arcs tail->head
        if cpdag2.existsArc(idHead_2, idTail_2) or cpdag2.existsEdge(idTail_2, idHead_2):
          hamming_dico[STRUCTURAL_HAMMING] += 1
        elif (
          not cpdag2.existsArc(idTail_2, idHead_2)
          and not cpdag2.existsArc(idHead_2, idTail_2)
          and not cpdag2.existsEdge(idTail_2, idHead_2)
        ):
          hamming_dico[STRUCTURAL_HAMMING] += 1
          hamming_dico[PURE_HAMMING] += 1

      elif cpdag1.existsEdge(idTail_1, idHead_1):  # Check edge
        if cpdag2.existsArc(idHead_2, idTail_2) or cpdag2.existsArc(idTail_2, idHead_2):
          hamming_dico[STRUCTURAL_HAMMING] += 1
        elif (
          not cpdag2.existsArc(idTail_2, idHead_2)
          and not cpdag2.existsArc(idHead_2, idTail_2)
          and not cpdag2.existsEdge(idTail_2, idHead_2)
        ):
          hamming_dico[STRUCTURAL_HAMMING] += 1
          hamming_dico[PURE_HAMMING] += 1
          # check no edge or arc on the ref graph, and yes on the other graph

      elif (
        cpdag2.existsArc(idHead_2, idTail_2)
        or cpdag2.existsEdge(idHead_2, idTail_2)
        or cpdag2.existsArc(idTail_2, idHead_2)
      ):
        hamming_dico[STRUCTURAL_HAMMING] += 1
        hamming_dico[PURE_HAMMING] += 1

    return hamming_dico


def graphDiff(bnref, bncmp, noStyle=False):
  """Return a pydot graph that compares the arcs of bnref to bncmp.
  graphDiff allows bncmp to have less nodes than bnref. (this is not the case in GraphicalBNComparator.dotDiff())

  if noStyle is False use 4 styles (fixed in pyagrum.config) :
    - the arc is common for both
    - the arc is common but inverted in _bn2
    - the arc is added in _bn2
    - the arc is removed in _bn2

  See graphDiffLegend() to add a legend to the graph.
  Warning
  -------
  if pydot is not installed, this function just returns None

  Returns
  -------
  pydot.Dot
    the result dot graph or None if pydot can not be imported
  """
  g = ggr.BN2dot(bnref)
  positions = gutils.dot_layout(g)

  res = dot.Dot(graph_type="digraph", bgcolor="transparent", layout="fdp", splines=True)
  for i1 in bnref.nodes():
    if bnref.variable(i1).name() in bncmp.names():
      res.add_node(
        dot.Node(
          f'"{bnref.variable(i1).name()}"',
          style="filled",
          fillcolor=gum.config["notebook", "graphdiff_correct_color"],
          color=gutils.getBlackInTheme(),
        )
      )
    else:
      if not noStyle:
        res.add_node(
          dot.Node(
            f'"{bnref.variable(i1).name()}"',
            style="dashed",
            fillcolor=gum.config["notebook", "graphdiff_correct_color"],
            color=gutils.getBlackInTheme(),
          )
        )
  if noStyle:
    for i1, i2 in bncmp.arcs():
      n1 = bncmp.variable(i1).name()
      n2 = bncmp.variable(i2).name()
      res.add_edge(
        dot.Edge(
          f'"{n1}"',
          f'"{n2}"',
          style=gum.config["notebook", "graphdiff_correct_style"],
          color=gum.config["notebook", "graphdiff_correct_color"],
        )
      )

  else:
    for i1, i2 in bnref.arcs():
      n1 = bnref.variable(i1).name()
      n2 = bnref.variable(i2).name()

      # a node is missing
      if not (n1 in bncmp.names() and n2 in bncmp.names()):
        res.add_edge(
          dot.Edge(
            f'"{n1}"',
            f'"{n2}"',
            style=gum.config["notebook", "graphdiff_missing_style"],
            color=gum.config["notebook", "graphdiff_missing_color"],
          )
        )
        continue

      if bncmp.existsArc(n1, n2):  # arc is OK in BN2
        res.add_edge(
          dot.Edge(
            f'"{n1}"',
            f'"{n2}"',
            style=gum.config["notebook", "graphdiff_correct_style"],
            color=gum.config["notebook", "graphdiff_correct_color"],
          )
        )
      elif bncmp.existsArc(n2, n1):  # arc is reversed in BN2
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"', style="invis"))
        res.add_edge(
          dot.Edge(
            f'"{n2}"',
            f'"{n1}"',
            style=gum.config["notebook", "graphdiff_reversed_style"],
            color=gum.config["notebook", "graphdiff_reversed_color"],
          )
        )
      else:  # arc is missing in BN2
        res.add_edge(
          dot.Edge(
            f'"{n1}"',
            f'"{n2}"',
            style=gum.config["notebook", "graphdiff_missing_style"],
            color=gum.config["notebook", "graphdiff_missing_color"],
          )
        )

    for i1, i2 in bncmp.arcs():
      n1 = bncmp.variable(i1).name()
      n2 = bncmp.variable(i2).name()
      if not bnref.existsArc(n1, n2) and not bnref.existsArc(n2, n1):  # arc only in BN2
        res.add_edge(
          dot.Edge(
            f'"{n1}"',
            f'"{n2}"',
            style=gum.config["notebook", "graphdiff_overflow_style"],
            color=gum.config["notebook", "graphdiff_overflow_color"],
          )
        )

    gutils.apply_dot_layout(res, positions)

  return res


def graphDiffLegend():
  try:
    # pydot is optional
    # pylint: disable=import-outside-toplevel
    import pydot as dot
  except ImportError:
    return None

  res = dot.Dot(graph_type="digraph", bgcolor="transparent", rankdir="LR")
  for i in "abcdefgh":
    res.add_node(dot.Node(i, style="invis"))
  res.add_edge(
    dot.Edge(
      "a",
      "b",
      label="overflow",
      style=gum.config["notebook", "graphdiff_overflow_style"],
      color=gum.config["notebook", "graphdiff_overflow_color"],
    )
  )
  res.add_edge(
    dot.Edge(
      "c",
      "d",
      label="Missing",
      style=gum.config["notebook", "graphdiff_missing_style"],
      color=gum.config["notebook", "graphdiff_missing_color"],
    )
  )
  res.add_edge(
    dot.Edge(
      "e",
      "f",
      label="reversed",
      style=gum.config["notebook", "graphdiff_reversed_style"],
      color=gum.config["notebook", "graphdiff_reversed_color"],
    )
  )
  res.add_edge(
    dot.Edge(
      "g",
      "h",
      label="Correct",
      style=gum.config["notebook", "graphdiff_correct_style"],
      color=gum.config["notebook", "graphdiff_correct_color"],
    )
  )

  return res
