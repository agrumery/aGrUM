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
Algorithms on the graphs of pyAgrum, one function per algorithm whatever the kind of graph.

Each function accepts a **family** of graphs, given by what the algorithm reads:

======  ================================  ============================================================
family  reads                             accepted graphs
======  ================================  ============================================================
A       the arcs                          DiGraph, DAG, MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG,
                                          EssentialGraph
E       the (undirected) edges            UndiGraph, MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG,
                                          EssentialGraph
AE      the arcs and the edges            MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph
K       the marked edges                  MarkedGraph, PAG, MAG
======  ================================  ============================================================

An algorithm reads only what its family designates: a function of the family A ignores the edges of a
PDAG, a function of the family E ignores its arcs. A graph out of the family raises a ``TypeError``.

The nodes are given by their id or their name, one node or any iterable of nodes. As everywhere in
pyAgrum, the ancestors (or descendants, or anteriors) of a set of nodes start from their parents (or
children): a node of the set belongs to the result only if it is reached by a path of at least one
arc, on a directed cycle or from another node of the set.

:func:`separation` is the only function whose meaning depends on the class of the graph:
d-separation for DAG, CPDAG, MPDAG and EssentialGraph, c-separation for PDAG and ChainGraph,
m-separation for MAG. The three criteria are equal on a DAG.

Examples
--------
.. code-block:: python

  import pyagrum as gum
  import pyagrum.graph as gg

  pdag = gum.fastPDAG("A->B-C;C-D")
  gg.descendants(pdag, "A")  # the arcs only
  # {1}
  gg.isChordal(pdag)  # the edges only
  # True
"""

__author__ = "Pierre-Henri Wuillemin"
__copyright__ = "(c) 2026 PARIS"

from collections.abc import Iterable

import pyagrum
import pyagrum.pyagrumcpp as _cpp  # the private _graph_* functions are not exported by pyagrum

_FAMILIES = {
  "A": ("a graph with arcs", (pyagrum.DiGraph, pyagrum.CPDAG, pyagrum.MPDAG)),
  "E": ("a graph with edges", (pyagrum.UndiGraph, pyagrum.CPDAG, pyagrum.MPDAG)),
  "AE": ("a graph with arcs and edges", (pyagrum.MixedGraph, pyagrum.CPDAG, pyagrum.MPDAG)),
  "K": ("a marked graph", (pyagrum.MarkedGraph, pyagrum.MAG)),
}
_FAMILY_NAMES = {
  "A": "DiGraph, DAG, MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph",
  "E": "UndiGraph, MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph",
  "AE": "MixedGraph, PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph",
  "K": "MarkedGraph, PAG, MAG",
}


def _check(name: str, g, *families: str) -> None:
  """Raise a TypeError if ``g`` belongs to none of ``families``."""
  for family in families:
    if isinstance(g, _FAMILIES[family][1]):
      return
  what = " or ".join(_FAMILIES[family][0] for family in families)
  accepted = ", ".join(_FAMILY_NAMES[family] for family in families)
  raise TypeError(f"{name} needs {what} ({accepted}), got {type(g).__name__}")


def _node(g, node) -> int:
  """The id of ``node``, given by its id or its name."""
  if isinstance(node, str):
    nid = g.idFromName(node)
    if nid is None:
      raise pyagrum.NotFound(f"no node named '{node}' in the graph")
    return nid
  return int(node)


def _nodes(g, nodes) -> set[int]:
  """The ids of ``nodes``: one node (id or name) or an iterable of nodes."""
  if nodes is None:
    return set()
  if isinstance(nodes, (int, str)):
    return {_node(g, nodes)}
  if isinstance(nodes, Iterable):
    return {_node(g, node) for node in nodes}
  return {_node(g, nodes)}


def _ordering(g, nodes) -> list[int]:
  """The ids of ``nodes`` in their order."""
  return [_node(g, node) for node in nodes]


# ======================================================================================================
# A: the arcs
# ======================================================================================================
def topologicalOrder(g) -> list[int]:
  """
  A topological order of the arcs of a graph (family A): every node comes after its parents.

  Parameters
  ----------
  g : DiGraph | DAG | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its edges are ignored)

  Returns
  -------
  list[int]
    the nodes in a topological order

  Raises
  ------
  pyagrum.InvalidDirectedCycle
    if the arcs of the graph contain a directed cycle
  """
  _check("topologicalOrder", g, "A")
  return list(_cpp._graph_topologicalOrder(g))


def hasDirectedCycle(g) -> bool:
  """
  Whether the arcs of a graph (family A) contain a directed cycle.

  Parameters
  ----------
  g : DiGraph | DAG | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its edges are ignored)

  Returns
  -------
  bool
    True if a directed cycle exists
  """
  _check("hasDirectedCycle", g, "A")
  return _cpp._graph_hasDirectedCycle(g)


def ancestors(g, nodes) -> set[int]:
  """
  The ancestors of nodes (family A, or a marked graph): the nodes with a directed path to one of them.

  The search starts from the parents: a node of ``nodes`` belongs to the result only if it has a
  directed path to one of them (on a directed cycle, or ancestor of another one). The set an(X) of
  Richardson and Spirtes (2002), which contains X, is ``ancestors(g, X) | X``.

  Parameters
  ----------
  g : DiGraph | DAG | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph | MarkedGraph | PAG | MAG
    the graph (the edges of a mixed graph are ignored; in a marked graph, the directed paths are made
    of edges ``->``)
  nodes : int | str | Iterable[int | str]
    a node or several nodes, by id or by name

  Returns
  -------
  set[int]
    the ancestors
  """
  _check("ancestors", g, "A", "K")
  return set(_cpp._graph_ancestors(g, _nodes(g, nodes)))


def descendants(g, nodes) -> set[int]:
  """
  The descendants of nodes (family A, or a marked graph): the nodes with a directed path from one of
  them.

  The search starts from the children: a node of ``nodes`` belongs to the result only if it has a
  directed path from one of them (on a directed cycle, or descendant of another one).

  Parameters
  ----------
  g : DiGraph | DAG | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph | MarkedGraph | PAG | MAG
    the graph (the edges of a mixed graph are ignored; in a marked graph, the directed paths are made
    of edges ``->``)
  nodes : int | str | Iterable[int | str]
    a node or several nodes, by id or by name

  Returns
  -------
  set[int]
    the descendants
  """
  _check("descendants", g, "A", "K")
  return set(_cpp._graph_descendants(g, _nodes(g, nodes)))


def directedPath(g, source, target) -> list[int] | None:
  """
  A shortest directed path between two nodes (family A).

  Parameters
  ----------
  g : DiGraph | DAG | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its edges are ignored)
  source : int | str
    the first node of the path
  target : int | str
    the last node of the path

  Returns
  -------
  list[int] | None
    the nodes of the path, from ``source`` to ``target``, or None if there is no directed path
  """
  _check("directedPath", g, "A")
  return _cpp._graph_directedPath(g, _node(g, source), _node(g, target))


def hasDirectedPath(g, source, target) -> bool:
  """
  Whether a directed path exists between two nodes (family A).

  Parameters
  ----------
  g : DiGraph | DAG | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its edges are ignored)
  source : int | str
    the first node of the path
  target : int | str
    the last node of the path

  Returns
  -------
  bool
    True if there is a directed path from ``source`` to ``target``
  """
  _check("hasDirectedPath", g, "A")
  return _cpp._graph_hasDirectedPath(g, _node(g, source), _node(g, target))


# ======================================================================================================
# E: the edges
# ======================================================================================================
def hasUndirectedCycle(g) -> bool:
  """
  Whether the edges of a graph (family E) contain a cycle.

  Parameters
  ----------
  g : UndiGraph | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its arcs are ignored)

  Returns
  -------
  bool
    True if a cycle of edges exists
  """
  _check("hasUndirectedCycle", g, "E")
  return _cpp._graph_hasUndirectedCycle(g)


def isChordal(g) -> bool:
  """
  Whether the edges of a graph (family E) form a chordal graph: every cycle of length 4 or more has a
  chord.

  The chain components of a CPDAG are chordal (Andersson, Madigan and Perlman, 1997).

  Parameters
  ----------
  g : UndiGraph | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its arcs are ignored)

  Returns
  -------
  bool
    True if the graph is chordal

  References
  ----------
  D. J. Rose, R. E. Tarjan, G. S. Lueker. Algorithmic aspects of vertex elimination on graphs. SIAM
  Journal on Computing 5(2):266-283, 1976.
  """
  _check("isChordal", g, "E")
  return _cpp._graph_isChordal(g)


def lexBFS(g, first=None) -> list[int]:
  """
  An ordering of the nodes by lexicographic breadth-first search on the edges of a graph (family E).

  On a chordal graph, the reverse of this ordering is a perfect elimination ordering.

  Parameters
  ----------
  g : UndiGraph | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its arcs are ignored)
  first : Iterable[int | str] | None
    nodes to visit first, in this order; they must form a clique

  Returns
  -------
  list[int]
    the nodes in the order of their visit

  Raises
  ------
  pyagrum.InvalidArgument
    if the nodes of ``first`` are not a clique of the graph

  References
  ----------
  D. J. Rose, R. E. Tarjan, G. S. Lueker. Algorithmic aspects of vertex elimination on graphs. SIAM
  Journal on Computing 5(2):266-283, 1976.
  """
  _check("lexBFS", g, "E")
  return list(_cpp._graph_lexBFS(g, [] if first is None else _ordering(g, first)))


def maximumCardinalitySearch(g) -> list[int]:
  """
  An ordering of the nodes by maximum cardinality search on the edges of a graph (family E): each node
  has the largest number of already visited neighbours.

  On a chordal graph, the reverse of this ordering is a perfect elimination ordering.

  Parameters
  ----------
  g : UndiGraph | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its arcs are ignored)

  Returns
  -------
  list[int]
    the nodes in the order of their visit

  References
  ----------
  R. E. Tarjan, M. Yannakakis. Simple linear-time algorithms to test chordality of graphs, test
  acyclicity of hypergraphs, and selectively reduce acyclic hypergraphs. SIAM Journal on Computing
  13(3):566-579, 1984.
  """
  _check("maximumCardinalitySearch", g, "E")
  return list(_cpp._graph_maximumCardinalitySearch(g))


def isPerfectEliminationOrdering(g, order) -> bool:
  """
  Whether an ordering of all the nodes is a perfect elimination ordering of the edges of a graph
  (family E): the neighbours of each node that come after it form a clique.

  Parameters
  ----------
  g : UndiGraph | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its arcs are ignored)
  order : Iterable[int | str]
    all the nodes of the graph, once each

  Returns
  -------
  bool
    True if ``order`` is a perfect elimination ordering

  Raises
  ------
  pyagrum.InvalidArgument
    if ``order`` does not contain exactly the nodes of the graph
  """
  _check("isPerfectEliminationOrdering", g, "E")
  return _cpp._graph_isPerfectEliminationOrdering(g, _ordering(g, order))


def undirectedPath(g, source, target) -> list[int] | None:
  """
  A shortest path of edges between two nodes (family E).

  Parameters
  ----------
  g : UndiGraph | MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph (its arcs are ignored)
  source : int | str
    the first node of the path
  target : int | str
    the last node of the path

  Returns
  -------
  list[int] | None
    the nodes of the path, from ``source`` to ``target``, or None if there is no path
  """
  _check("undirectedPath", g, "E")
  return _cpp._graph_undirectedPath(g, _node(g, source), _node(g, target))


# ======================================================================================================
# AE: the arcs and the edges
# ======================================================================================================
def hasPartiallyDirectedCycle(g) -> bool:
  """
  Whether a graph with arcs and edges (family AE) has a partially directed cycle: a cycle made of arcs
  (followed in their direction) and edges, with at least one arc.

  A mixed graph without such a cycle is a chain graph.

  Parameters
  ----------
  g : MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph

  Returns
  -------
  bool
    True if a partially directed cycle exists
  """
  _check("hasPartiallyDirectedCycle", g, "AE")
  return _cpp._graph_hasPartiallyDirectedCycle(g)


def mixedOrientedPath(g, source, target) -> list[int] | None:
  """
  A shortest path made of arcs (followed in their direction) and edges between two nodes (family AE).

  Parameters
  ----------
  g : MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph
  source : int | str
    the first node of the path
  target : int | str
    the last node of the path

  Returns
  -------
  list[int] | None
    the nodes of the path, from ``source`` to ``target``, or None if there is no such path
  """
  _check("mixedOrientedPath", g, "AE")
  return _cpp._graph_mixedOrientedPath(g, _node(g, source), _node(g, target))


def degeneracyOrdering(g) -> list[int]:
  """
  A degeneracy (smallest-last) ordering of the skeleton of a graph with arcs and edges (family AE).

  Every node has at most d neighbours (by an arc or an edge) before it in the ordering, where d is the
  degeneracy of the skeleton, the smallest such bound.

  Parameters
  ----------
  g : MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph

  Returns
  -------
  list[int]
    the nodes in a degeneracy ordering

  References
  ----------
  D. W. Matula, L. L. Beck. Smallest-last ordering and clustering and graph coloring algorithms.
  Journal of the ACM 30(3):417-427, 1983.
  """
  _check("degeneracyOrdering", g, "AE")
  return list(_cpp._graph_degeneracyOrdering(g))


# ======================================================================================================
# K: the marked graphs
# ======================================================================================================
def anteriors(g, nodes) -> set[int]:
  """
  The anterior nodes of nodes (family AE, or a marked graph): the nodes with a path made of arcs
  (followed in their direction) and edges to one of them; in a marked graph, of edges ``->``
  (pointing towards them) and ``-``.

  As for :func:`ancestors`, a node of ``nodes`` belongs to the result only if it has such a path to
  another one or lies on such a cycle; going back and forth through the same edge is not a cycle. The
  set ant(X) of Richardson and Spirtes (2002), which contains X, is ``anteriors(g, X) | X``.

  Parameters
  ----------
  g : MixedGraph | PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph | MarkedGraph | PAG | MAG
    the graph
  nodes : int | str | Iterable[int | str]
    a node or several nodes, by id or by name

  Returns
  -------
  set[int]
    the anterior nodes

  References
  ----------
  T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics 30(4):962-1030,
  2002.
  """
  _check("anteriors", g, "AE", "K")
  return set(_cpp._graph_anteriors(g, _nodes(g, nodes)))


def isAncestral(g) -> bool:
  """
  Whether a marked graph (family K) is an ancestral graph: no circle, no node anterior to one of its
  parents or spouses, and no parent or spouse for a node with an undirected edge.

  Parameters
  ----------
  g : MarkedGraph | PAG | MAG
    the graph

  Returns
  -------
  bool
    True if the graph is ancestral (always True for a MAG)

  References
  ----------
  T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics 30(4):962-1030,
  2002.
  """
  _check("isAncestral", g, "K")
  return _cpp._graph_isAncestral(g)


def isMaximal(g) -> bool:
  """
  Whether an ancestral graph (family K) is maximal: two non adjacent nodes are always m-separated by
  some set of nodes.

  Parameters
  ----------
  g : MarkedGraph | PAG | MAG
    the graph, supposed ancestral

  Returns
  -------
  bool
    True if the graph is maximal (always True for a MAG)

  References
  ----------
  T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics 30(4):962-1030,
  2002.
  """
  _check("isMaximal", g, "K")
  return _cpp._graph_isMaximal(g)


# ======================================================================================================
# orientation
# ======================================================================================================
def cpdag(dag) -> "pyagrum.CPDAG":
  """
  The CPDAG (completed PDAG, or essential graph) of the Markov equivalence class of a DAG.

  Same as :meth:`pyagrum.CPDAG.fromDAG`.

  Parameters
  ----------
  dag : DAG
    the DAG

  Returns
  -------
  pyagrum.CPDAG
    the CPDAG: the compelled arcs of ``dag``, its reversible arcs as edges

  References
  ----------
  D. M. Chickering. A Transformational Characterization of Equivalent Bayesian Network Structures.
  Proceedings of the 11th Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 87-98.
  """
  if not isinstance(dag, pyagrum.DAG):
    raise TypeError(f"cpdag needs a DAG, got {type(dag).__name__}")
  return _cpp._graph_cpdag(dag)


def consistentExtension(g) -> "pyagrum.DAG | None":
  """
  A consistent extension of a partially directed graph: a DAG with the same skeleton, the same arcs
  and the same v-structures.

  Parameters
  ----------
  g : PDAG | ChainGraph | CPDAG | MPDAG | EssentialGraph
    the graph

  Returns
  -------
  pyagrum.DAG | None
    a consistent extension, or None if there is none (never None for a CPDAG or an MPDAG)

  References
  ----------
  D. Dor, M. Tarsi. A simple algorithm to construct a consistent extension of a partially oriented
  graph. UCLA Cognitive Systems Laboratory, Technical Report R-185, 1992.
  """
  if not isinstance(g, (pyagrum.PDAG, pyagrum.CPDAG, pyagrum.MPDAG)):
    raise TypeError(
      f"consistentExtension needs a PDAG (PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph), got {type(g).__name__}"
    )
  return _cpp._graph_consistentExtension(g)


def meekClosure(pdag) -> "pyagrum.PDAG | None":
  """
  The closure of a PDAG under the orientation rules of Meek, if the PDAG has a consistent extension.

  The closure is the maximally oriented PDAG (MPDAG) of the extensions: its arcs are the arcs shared by
  all of them. :meth:`pyagrum.MPDAG.fromPDAG` gives it as an MPDAG.

  Parameters
  ----------
  pdag : PDAG | ChainGraph
    the PDAG

  Returns
  -------
  pyagrum.PDAG | None
    the closure, or None if ``pdag`` has no consistent extension

  References
  ----------
  C. Meek. Causal inference and causal explanation with background knowledge. Proceedings of the 11th
  Conference on Uncertainty in Artificial Intelligence (UAI 1995), pp. 403-410.
  """
  if not isinstance(pdag, pyagrum.PDAG):
    raise TypeError(f"meekClosure needs a PDAG (PDAG, ChainGraph), got {type(pdag).__name__}")
  return _cpp._graph_meekClosure(pdag)


def orientAsDAG(pdag, domainSizes: dict[int, int]) -> "pyagrum.DAG":
  """
  A DAG orienting all the edges of a PDAG, preferring the extensions with the smallest parameter count.

  Parameters
  ----------
  pdag : PDAG | ChainGraph
    the PDAG
  domainSizes : dict[int, int]
    the domain size of each node, by id

  Returns
  -------
  pyagrum.DAG
    a DAG containing the arcs of ``pdag``

  Raises
  ------
  pyagrum.InvalidArgument
    if a node has no domain size, or a domain size 0
  """
  if not isinstance(pdag, pyagrum.PDAG):
    raise TypeError(f"orientAsDAG needs a PDAG (PDAG, ChainGraph), got {type(pdag).__name__}")
  return _cpp._graph_orientAsDAG(pdag, {_node(pdag, n): int(s) for n, s in domainSizes.items()})


# ======================================================================================================
# separation
# ======================================================================================================
def separation(g, X, Y, Z=None) -> bool:
  """
  Whether two sets of nodes are separated by a third one, with the criterion of the class of the graph.

  =============================================  ============================================
  graph                                          criterion
  =============================================  ============================================
  DAG, CPDAG, MPDAG, EssentialGraph              d-separation (in the DAGs of the class)
  PDAG, ChainGraph                               c-separation
  MAG                                            m-separation
  =============================================  ============================================

  The three criteria are equal on a DAG. The nodes of ``X`` and ``Y`` which belong to ``Z`` are
  observed, hence separated from any node.

  Parameters
  ----------
  g : DAG | CPDAG | MPDAG | EssentialGraph | PDAG | ChainGraph | MAG
    the graph
  X : int | str | Iterable[int | str]
    the first node or set of nodes
  Y : int | str | Iterable[int | str]
    the second node or set of nodes, disjoint from ``X``
  Z : int | str | Iterable[int | str] | None
    the separating node or set of nodes (None: the empty set)

  Returns
  -------
  bool
    True if ``X`` and ``Y`` are separated by ``Z``

  Raises
  ------
  pyagrum.InvalidArgument
    if ``X`` and ``Y`` are not disjoint

  References
  ----------
  - J. Pearl. Probabilistic Reasoning in Intelligent Systems. Morgan Kaufmann, 1988 (d-separation).
  - R. R. Bouckaert, M. Studený. Chain graphs: semantics and expressiveness. ECSQARU 1995, LNCS 946,
    pp. 69-76 (c-separation).
  - T. Richardson, P. Spirtes. Ancestral graph Markov models. The Annals of Statistics
    30(4):962-1030, 2002 (m-separation).
  """
  if not isinstance(g, (pyagrum.DAG, pyagrum.PDAG, pyagrum.CPDAG, pyagrum.MPDAG, pyagrum.MAG)):
    raise TypeError(
      f"separation needs a DAG, PDAG, ChainGraph, CPDAG, MPDAG, EssentialGraph or MAG, got {type(g).__name__}"
    )
  return _cpp._graph_separation(g, _nodes(g, X), _nodes(g, Y), _nodes(g, Z))


# ======================================================================================================
# input/output
# ======================================================================================================
fromGUMString = pyagrum.graphFromGUMString
load = pyagrum.loadGraph
save = pyagrum.saveGraph

__all__ = [
  "topologicalOrder",
  "hasDirectedCycle",
  "ancestors",
  "descendants",
  "directedPath",
  "hasDirectedPath",
  "hasUndirectedCycle",
  "isChordal",
  "lexBFS",
  "maximumCardinalitySearch",
  "isPerfectEliminationOrdering",
  "undirectedPath",
  "hasPartiallyDirectedCycle",
  "mixedOrientedPath",
  "degeneracyOrdering",
  "anteriors",
  "isAncestral",
  "isMaximal",
  "cpdag",
  "consistentExtension",
  "meekClosure",
  "orientAsDAG",
  "separation",
  "fromGUMString",
  "load",
  "save",
]
