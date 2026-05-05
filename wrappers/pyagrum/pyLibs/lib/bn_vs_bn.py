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
The purpose of this module is to provide tools for comaring different BNs.
"""

import os
import math
from itertools import product, combinations

import pydot as dot

import pyagrum
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

  def __init__(self, bn1: str | pyagrum.BayesNet, bn2: str | pyagrum.BayesNet, delta=1e-6):
    self.DELTA_ERROR = delta
    if isinstance(bn1, str):
      self._bn1 = pyagrum.loadBN(bn1)
      self._bn1.setProperty("name", '"' + os.path.basename(self._bn1.property("name") + '"'))
    else:
      self._bn1 = bn1

    if isinstance(bn2, str):
      self._bn2 = pyagrum.loadBN(bn2)
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
    pyagrum.KeyError
      If cpts are not from the same variable
    """
    I1 = pyagrum.Instantiation(pot1)
    I2 = pyagrum.Instantiation(pot2)
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

  def _bn2_aligned_with_bn1_ids(self):
    """
    Return a BayesNet with self._bn1's variables and NodeIds, but self._bn2's
    arc structure. Used to align NodeIds before delegating structural comparisons
    to pyagrum.StructuralMetrics, since aGrUM C++ comparisons match nodes by ID,
    not by name.
    """
    aligned = pyagrum.BayesNet()
    for nid in self._bn1.nodes():
      aligned.add(self._bn1.variable(nid), nid)
    for tail, head in self._bn2.arcs():
      ttail = self._bn1.idFromName(self._bn2.variable(tail).name())
      thead = self._bn1.idFromName(self._bn2.variable(head).name())
      aligned.addArc(ttail, thead)
    return aligned

  def equivalentBNs(self) -> str:
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

  def dotDiff(self) -> dot.Dot:
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

  def skeletonScores(self) -> dict[str, float]:
    """
    Compute Precision, Recall, F-score and dist2opt for the skeleton of
    self._bn2 compared to self._bn1 (orientations are ignored).

    precision and recall are computed considering BN1 as the reference.

    Fscore is 2*(recall*precision)/(recall+precision) and is the weighted average
    of Precision and Recall.

    dist2opt=square root of (1-precision)^2+(1-recall)^2 and represents the
    euclidian distance to the ideal point (precision=1, recall=1).

    Note
    ----
    Now delegates to pyagrum.StructuralMetrics (aGrUM C++). The skeleton metrics
    are independent of orientation, so the values match the previous Python
    implementation exactly.

    Returns
    -------
    dict[str,double]
      A dictionnary containing 'precision', 'recall', 'fscore', 'dist2opt' and so on.
    """
    aligned_bn2 = self._bn2_aligned_with_bn1_ids()
    comp = pyagrum.StructuralMetrics()
    comp.compare(self._bn1.dag(), aligned_bn2.dag())

    count = {
      "tp": int(comp.tp_skeleton()),
      "tn": int(comp.tn_skeleton()),
      "fp": int(comp.fp_skeleton()),
      "fn": int(comp.fn_skeleton()),
    }
    precision = comp.precision_skeleton()
    recall = comp.recall_skeleton()
    fscore = comp.f_score_skeleton()

    return {
      "count": count,
      "recall": recall,
      "precision": precision,
      "fscore": fscore,
      "dist2opt": math.sqrt((1 - precision) ** 2 + (1 - recall) ** 2),
    }

  def scores(self) -> dict[str, float]:
    """
    Compute Precision, Recall, F-score, dist2opt and SID for self._bn2 compared
    to self._bn1 at the DAG level.

    precision and recall are computed considering BN1 as the reference.

    Fscore is 2*(recall*precision)/(recall+precision) and is the weighted average
    of Precision and Recall.

    dist2opt=square root of (1-precision)^2+(1-recall)^2 and represents the
    euclidian distance to the ideal point (precision=1, recall=1).

    SID (Structural Intervention Distance, Peters & Bühlmann 2015) counts the
    number of ordered pairs (i, j) for which the parent-adjustment formula in
    self._bn2 gives a wrong intervention distribution relative to self._bn1.

    Note
    ----
    The metrics are now computed by pyagrum.StructuralMetrics (aGrUM C++).
    Misoriented arcs are counted once (in fp, not in fn) instead of being
    double-counted as both a missing arc and an extra arc.

    Returns
    -------
    dict[str,double]
      A dictionary containing 'count', 'precision', 'recall', 'fscore',
      'dist2opt', 'sid'.
    """
    aligned_bn2 = self._bn2_aligned_with_bn1_ids()
    comp = pyagrum.StructuralMetrics()
    comp.compare(self._bn1.dag(), aligned_bn2.dag())

    count = {
      "tp": int(comp.tp()),
      "tn": int(comp.tn()),
      "fp": int(comp.fp()),
      "fn": int(comp.fn()),
    }
    precision = comp.precision()
    recall = comp.recall()
    fscore = comp.f_score()

    return {
      "count": count,
      "recall": recall,
      "precision": precision,
      "fscore": fscore,
      "dist2opt": math.sqrt((1 - precision) ** 2 + (1 - recall) ** 2),
      "sid": int(comp.sid(self._bn1.dag(), aligned_bn2.dag())),
    }

  def hamming(self) -> dict[float, float]:
    """
    Compute hamming and structural hamming distance.

    Hamming distance is the difference of edges comparing the 2 skeletons (CPDAGs),
    and Structural Hamming distance is the difference comparing the CPDAGs
    including the arcs' orientation.

    Note
    ----
    Now delegates to pyagrum.StructuralMetrics (aGrUM C++). The essential graphs
    (CPDAGs) are still built in Python via pyagrum.EssentialGraph (since the
    BayesNet overload of compare is not exposed via SWIG), then the actual
    counting happens in C++ via compare(PDAG, PDAG).

    Returns
    -------
    dict[str,int]
      A dictionary containing PURE_HAMMING and STRUCTURAL_HAMMING.
    """
    aligned_bn2 = self._bn2_aligned_with_bn1_ids()
    cpdag1 = pyagrum.EssentialGraph(self._bn1).pdag()
    cpdag2 = pyagrum.EssentialGraph(aligned_bn2).pdag()

    comp = pyagrum.StructuralMetrics()
    comp.compare(cpdag1, cpdag2)
    return {
      PURE_HAMMING: int(comp.shd_skeleton()),
      STRUCTURAL_HAMMING: int(comp.shd()),
    }


def graphDiff(bnref, bncmp, noStyle: bool = False) -> dot.Dot:
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
          fillcolor=pyagrum.config["notebook", "graphdiff_correct_color"],
          color=gutils.getBlackInTheme(),
        )
      )
    else:
      if not noStyle:
        res.add_node(
          dot.Node(
            f'"{bnref.variable(i1).name()}"',
            style="dashed",
            fillcolor=pyagrum.config["notebook", "graphdiff_correct_color"],
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
          style=pyagrum.config["notebook", "graphdiff_correct_style"],
          color=pyagrum.config["notebook", "graphdiff_correct_color"],
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
            style=pyagrum.config["notebook", "graphdiff_missing_style"],
            color=pyagrum.config["notebook", "graphdiff_missing_color"],
          )
        )
        continue

      if bncmp.existsArc(n1, n2):  # arc is OK in BN2
        res.add_edge(
          dot.Edge(
            f'"{n1}"',
            f'"{n2}"',
            style=pyagrum.config["notebook", "graphdiff_correct_style"],
            color=pyagrum.config["notebook", "graphdiff_correct_color"],
          )
        )
      elif bncmp.existsArc(n2, n1):  # arc is reversed in BN2
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"', style="invis"))
        res.add_edge(
          dot.Edge(
            f'"{n2}"',
            f'"{n1}"',
            style=pyagrum.config["notebook", "graphdiff_reversed_style"],
            color=pyagrum.config["notebook", "graphdiff_reversed_color"],
          )
        )
      else:  # arc is missing in BN2
        res.add_edge(
          dot.Edge(
            f'"{n1}"',
            f'"{n2}"',
            style=pyagrum.config["notebook", "graphdiff_missing_style"],
            color=pyagrum.config["notebook", "graphdiff_missing_color"],
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
            style=pyagrum.config["notebook", "graphdiff_overflow_style"],
            color=pyagrum.config["notebook", "graphdiff_overflow_color"],
          )
        )

    gutils.apply_dot_layout(res, positions)

  return res


def graphDiffLegend() -> dot.Dot | None:
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
      style=pyagrum.config["notebook", "graphdiff_overflow_style"],
      color=pyagrum.config["notebook", "graphdiff_overflow_color"],
    )
  )
  res.add_edge(
    dot.Edge(
      "c",
      "d",
      label="Missing",
      style=pyagrum.config["notebook", "graphdiff_missing_style"],
      color=pyagrum.config["notebook", "graphdiff_missing_color"],
    )
  )
  res.add_edge(
    dot.Edge(
      "e",
      "f",
      label="reversed",
      style=pyagrum.config["notebook", "graphdiff_reversed_style"],
      color=pyagrum.config["notebook", "graphdiff_reversed_color"],
    )
  )
  res.add_edge(
    dot.Edge(
      "g",
      "h",
      label="Correct",
      style=pyagrum.config["notebook", "graphdiff_correct_style"],
      color=pyagrum.config["notebook", "graphdiff_correct_color"],
    )
  )

  return res
