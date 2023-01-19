"""
The purpose of this module is to provide tools for comaring different BNs.
"""

# (c) Copyright by Pierre-Henri Wuillemin, 2022
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


import os
import math
from itertools import product, combinations

import pyAgrum as gum
import pyAgrum.lib._colors as gumcols


class GraphicalBNComparator:
  """
  BNGraphicalComparator allows to compare in multiple way 2 BNs...The smallest assumption is that the names of the variables are the same in the 2 BNs. But some comparisons will have also to check the type and domainSize of the variables. The bns have not exactly the  same role : _bn1 is rather the referent model for the comparison whereas _bn2 is the compared one to the referent model.

  Parameters
  ----------
  name1 : str or pyAgrum.BayesNet
    a BN or a filename for reference
  name2 : str or pyAgrum.BayesNet
    another BN or antoher filename for comparison
  """

  def __init__(self, name1, name2, delta=1e-6):
    self.DELTA_ERROR = delta
    if isinstance(name1, str):
      self._bn1 = gum.loadBN(name1)
      self._bn1.setProperty('name', '"' + os.path.basename(self._bn1.property('name') + '"'))
    else:
      self._bn1 = name1

    if isinstance(name2, str):
      self._bn2 = gum.loadBN(name2)
      self._bn2.setProperty('name', '"' + os.path.basename(self._bn2.property('name') + '"'))
    else:
      self._bn2 = name2

    s1 = set(self._bn1.names())
    s2 = set(self._bn2.names())

    if s1 != s2:
      raise ValueError(
        "The 2 BNs are not comparable! There are names not present in the 2 BNs : " + str(
          s1.symmetric_difference(s2)))

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
        return self._bn1.variable(id1).name() + " has different parents in the two bns whose names are in " + str(
          p1.symmetric_difference(p2))

    return "OK"

  def _comparePotentials(self, pot1, pot2):

    """
    Compare 2 potentials one in each Bayesian network

    Parameters
    ----------
    pot1 : pyAgrum.Potential
      one of b1's cpts
    pot2 : pyAgrum.Potential
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
      res = self._comparePotentials(self._bn1.cpt(i), self._bn2.cpt(self._bn1.variable(i).name()))
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
    """ Return a pydot graph that compares the arcs of _bn1 (reference) with those of self._bn2.
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

      if (self._bn1.dag().existsArc(idHead_1, idTail_1) or
         self._bn1.dag().existsArc(idTail_1, idHead_1)):  # Check edge node1-node2
        if (self._bn2.dag().existsArc(idHead_2, idTail_2) or
           self._bn2.dag().existsArc(idTail_2, idHead_2)):  # if edge:
          count["tp"] += 1
        else:  # If no edge:
          count["fn"] += 1
      else:  # Check if no edge
        if (self._bn2.dag().existsArc(idHead_2, idTail_2) or
           self._bn2.dag().existsArc(idTail_2, idHead_2)):  # If edge
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
      'count': count,
      'recall': recall,
      'precision': precision,
      'fscore': Fscore,
      'dist2opt': math.sqrt((1 - precision) ** 2 + (1 - recall) ** 2)
    }

  def scores(self):
    """
    Compute Precision, Recall, F-score for self._bn2 compared to self._bn1

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
      'count': count,
      'recall': recall,
      'precision': precision,
      'fscore': Fscore,
      'dist2opt': math.sqrt((1 - precision) ** 2 + (1 - recall) ** 2)
    }

  def hamming(self):
    """
    Compute hamming and structural hamming distance

    Hamming distance is the difference of edges comparing the 2 skeletons, and Structural Hamming difference is the
    difference comparing the cpdags,	including the arcs' orientation.

    Returns
    -------
    dict[double,double]
      A dictionnary containing 'hamming','structural hamming'
    """
    # convert graphs to cpdags

    cpdag1 = gum.EssentialGraph(self._bn1).pdag()
    cpdag2 = gum.EssentialGraph(self._bn2).pdag()

    # We look at all combinations
    listVariables = self._bn1.names()
    hamming_dico = {'hamming': 0, 'structural hamming': 0}

    for head, tail in combinations(listVariables, 2):
      idHead_1 = self._bn1.idFromName(head)
      idTail_1 = self._bn1.idFromName(tail)

      idHead_2 = self._bn2.idFromName(head)
      idTail_2 = self._bn2.idFromName(tail)

      if cpdag1.existsArc(idHead_1, idTail_1):  # Check arcs head->tail
        if cpdag2.existsArc(idTail_2, idHead_2) or cpdag2.existsEdge(idTail_2, idHead_2):
          hamming_dico["structural hamming"] += 1
        elif not cpdag2.existsArc(idTail_2, idHead_2) and not cpdag2.existsArc(idHead_2,
                                                                               idTail_2) and not cpdag2.existsEdge(
          idTail_2, idHead_2):

          hamming_dico["structural hamming"] += 1
          hamming_dico["hamming"] += 1

      elif cpdag1.existsArc(idTail_1, idHead_1):  # Check arcs tail->head
        if cpdag2.existsArc(idHead_2, idTail_2) or cpdag2.existsEdge(idTail_2, idHead_2):
          hamming_dico["structural hamming"] += 1
        elif not cpdag2.existsArc(idTail_2, idHead_2) and \
           not cpdag2.existsArc(idHead_2, idTail_2) and \
           not cpdag2.existsEdge(idTail_2, idHead_2):
          hamming_dico["structural hamming"] += 1
          hamming_dico["hamming"] += 1

      elif cpdag1.existsEdge(idTail_1, idHead_1):  # Check edge
        if cpdag2.existsArc(idHead_2, idTail_2) or cpdag2.existsArc(idTail_2, idHead_2):
          hamming_dico["structural hamming"] += 1
        elif not cpdag2.existsArc(idTail_2, idHead_2) and \
           not cpdag2.existsArc(idHead_2, idTail_2) and \
           not cpdag2.existsEdge(idTail_2, idHead_2):
          hamming_dico["structural hamming"] += 1
          hamming_dico["hamming"] += 1
          # check no edge or arc on the ref graph, and yes on the other graph

      elif cpdag2.existsArc(idHead_2, idTail_2) or \
         cpdag2.existsEdge(idHead_2, idTail_2) or \
         cpdag2.existsArc(idTail_2, idHead_2):
        hamming_dico["structural hamming"] += 1
        hamming_dico["hamming"] += 1

    return hamming_dico


def graphDiff(bnref, bncmp,noStyle=False):
  """ Return a pydot graph that compares the arcs of bnref to bncmp.
  graphDiff allows bncmp to have less nodes than bnref. (this is not the case in GraphicalBNComparator.dotDiff())

  if noStyle is False use 4 styles (fixed in pyAgrum.config) :
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

  try:
    # pydot is optional
    # pylint: disable=import-outside-toplevel
    import pydot as dot
    import csv
    import pyAgrum.lib.bn2graph as ggr
  except ImportError:
    return None

  g = ggr.BN2dot(bnref)
  positions = {l[1]: f"{l[2]},{l[3]}!"
               for l in csv.reader(g.create(format="plain")
                                   .decode("utf8")
                                   .split("\n")
                                   , delimiter=' ', quotechar='"')
               if len(l) > 3 and l[0] == "node"}

  res = dot.Dot(graph_type='digraph', bgcolor="transparent", layout="fdp", splines=True)
  for i1 in bnref.nodes():
    if bnref.variable(i1).name() in bncmp.names():
      res.add_node(dot.Node(f'"{bnref.variable(i1).name()}"',
                            style="filled",
                            fillcolor=gum.config["notebook", "graphdiff_correct_color"],
                            color=gumcols.getBlackInTheme(),
                            pos=positions[bnref.variable(i1).name()]
                            )
                   )
    else:
      if not noStyle:
        res.add_node(dot.Node(f'"{bnref.variable(i1).name()}"',
                              style="dashed",
                              fillcolor=gum.config["notebook", "graphdiff_correct_color"],
                              color=gumcols.getBlackInTheme(),
                              pos=positions[bnref.variable(i1).name()]
                              )
                     )
  if noStyle:
    for (i1, i2) in bncmp.arcs():
      n1 = bncmp.variable(i1).name()
      n2 = bncmp.variable(i2).name()
      res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                            style=gum.config["notebook", "graphdiff_correct_style"],
                            color=gum.config["notebook", "graphdiff_correct_color"],
                            constraint="false"))

  else:
    for (i1, i2) in bnref.arcs():
      n1 = bnref.variable(i1).name()
      n2 = bnref.variable(i2).name()

      # a node is missing
      if not (n1 in bncmp.names() and n2 in bncmp.names()):
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                              style=gum.config["notebook", "graphdiff_missing_style"],
                              color=gum.config["notebook", "graphdiff_missing_color"]))
        continue

      keyarc = "graphdiff_correct"
      if bncmp.existsArc(n1, n2):  # arc is OK in BN2
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                              style=gum.config["notebook", "graphdiff_correct_style"],
                              color=gum.config["notebook", "graphdiff_correct_color"]))
      elif bncmp.existsArc(n2, n1):  # arc is reversed in BN2
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                              style="invis"))
        res.add_edge(dot.Edge(f'"{n2}"', f'"{n1}"',
                              style=gum.config["notebook", "graphdiff_reversed_style"],
                              color=gum.config["notebook", "graphdiff_reversed_color"],
                              constraint="false"))
      else:  # arc is missing in BN2
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                              style=gum.config["notebook", "graphdiff_missing_style"],
                              color=gum.config["notebook", "graphdiff_missing_color"]))

    for (i1, i2) in bncmp.arcs():
      n1 = bncmp.variable(i1).name()
      n2 = bncmp.variable(i2).name()
      if not bnref.existsArc(n1, n2) and not bnref.existsArc(n2, n1):  # arc only in BN2
        res.add_edge(dot.Edge(f'"{n1}"', f'"{n2}"',
                              style=gum.config["notebook", "graphdiff_overflow_style"],
                              color=gum.config["notebook", "graphdiff_overflow_color"],
                              constraint="false"))

  return res

def graphDiffLegend():
  try:
    # pydot is optional
    # pylint: disable=import-outside-toplevel
    import pydot as dot
  except ImportError:
    return None

  res = dot.Dot(graph_type='digraph', bgcolor="transparent", rankdir="LR")
  for i in "abcdefgh":
    res.add_node(dot.Node(i, style="invis"))
  res.add_edge(dot.Edge("a", "b", label="overflow",
                        style=gum.config["notebook", "graphdiff_overflow_style"],
                        color=gum.config["notebook", "graphdiff_overflow_color"]))
  res.add_edge(dot.Edge("c", "d", label="Missing",
                        style=gum.config["notebook", "graphdiff_missing_style"],
                        color=gum.config["notebook", "graphdiff_missing_color"]))
  res.add_edge(dot.Edge("e", "f", label="reversed",
                        style=gum.config["notebook", "graphdiff_reversed_style"],
                        color=gum.config["notebook", "graphdiff_reversed_color"]))
  res.add_edge(dot.Edge("g", "h", label="Correct",
                        style=gum.config["notebook", "graphdiff_correct_style"],
                        color=gum.config["notebook", "graphdiff_correct_color"]))

  return res
