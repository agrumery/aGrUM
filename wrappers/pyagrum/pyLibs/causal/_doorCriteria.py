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
This files define some routines for front and back door
"""

from typing import Iterator, Optional
import itertools as it

import pyagrum

from pyagrum.causal._types import NameSet, NodeId, NodeSet, NodeList
from pyagrum.causal._dSeparation import isParent, dSep_reduce, descendants, isDSep_parents

# pylint: disable=unused-import
import pyagrum.causal  # for annotations


def backdoor_path(bn: "pyagrum.BayesNet", x: str, y: str, zset: NameSet = None) -> bool:
  """
  Predicate on the existence of an open back door path from ``x`` to ``y``,
  conditioning on the set of variables ``zset``

  Parameters
  ----------
  bn: pyagrum.BayesNez
      the DAG model
  x: str
      name of source node
  y: str
      name of destination node
  zset: Set[str]
      names of conditioning nodes

  Returns
  -------
  bool
      True if such backdoor exists
  """
  if zset is None:
    zset = set()
  return not isDSep_parents(bn, x, y, zset)


def exists_unblocked_directed_path(bn: "pyagrum.BayesNet", x: str, y: str, zset: NameSet = None) -> bool:
  """
  Predicate on the existence of a directed path from ``x`` to ``y`` in the Bayesian network ``bn``
  not blocked by nodes of ``zset``

  Parameters
  ----------
  bn: pyagrum.BayesNez
      the DAG model
  x: str
      name of source node
  y: str
      name of destination node
  zset: Set[str]
      names of conditioning nodes

  Returns
  -------
  bool
  """
  if zset is None:
    zset = set()
  if y in bn.children(x):
    return True

  for c in bn.children(x):
    if (c not in zset) and exists_unblocked_directed_path(bn, c, y, zset):
      return True
  return False


def is_frontdoor(bn: "pyagrum.BayesNet", x: str, y: str, zset: NameSet) -> bool:
  """
  Tests whether or not ``zset`` satisifies the front door criterion for ``x`` and ``y``, in the Bayesian network ``bn``

  Parameters
  ----------
  bn: pyagrum.BayesNez
      the DAG model
  x: str
      name of source node
  y: str
      name of destination node
  zset: Set[str]
      names of conditioning nodes

  Returns
  -------
  bool
  """
  if exists_unblocked_directed_path(bn, x, y, zset):
    return False

  if len(zset & backdoor_reach(bn, x)) != 0:
    return False

  bn_reduit = dSep_reduce(bn, zset | {x, y})
  for i in zset:
    if backdoor_path(bn_reduit, i, y, {x}):
      return False

  return True


def is_backdoor(bn: "pyagrum.BayesNet", x: str, y: str, zset: NameSet) -> bool:
  """
  Tests whether or not ``zset`` satisifies the back door criterion for ``x`` and ``y``, in the Bayesian network ``bn``

  Parameters
  ----------
  bn: pyagrum.BayesNez
      the DAG model
  x: str
      name of source node
  y: str
      name of destination node
  zset: Set[str]
      names of conditioning nodes

  Returns
  -------
  bool
  """

  desc_x = descendants(bn, x)
  if len(desc_x & zset) != 0:
    return False

  return isDSep_parents(bn, x, y, zset)


def backdoor_reach(bn: "pyagrum.BayesNet", a: NodeId) -> NodeSet:
  """
  Returns the set of nodes that can be reached through a backdoor path from ``a`` in the graph ``bn``

  Parameters
  ----------
  bn: pyagrum.BayesNet
    the model
  a: int
    the backdoor-ed node

  Returns
  -------
  Set[int]
    set of nodes
  """

  def inner_br(bn: "pyagrum.BayesNet", x: NodeId, pht: bool, reach0: NodeSet, reach1: NodeSet):
    for c in bn.children(x):
      if c not in reach0 and c not in reach1:
        reach1.add(c)
        inner_br(bn, c, True, reach0, reach1)

    if not pht:
      for p in bn.parents(x):
        if p not in reach0:
          reach0.add(p)
          inner_br(bn, p, False, reach0, reach1)

  r = {a}
  r.union(bn.parents(a))
  l = {a}
  for pa in bn.parents(a):
    inner_br(bn, pa, False, r, l)
  s = r | l
  if a in s:
    s.remove(a)
  return s


def nodes_on_dipath(bn: "pyagrum.BayesNet", x: NodeId, y: NodeId) -> Optional[NodeSet]:
  """
  Returns the set of nodes through which there is a directed path from `x` to `y` in the graph `bn`

  Parameters
  ----------
  bn: pyagrum.BayesNet
  x  int
  y: int

  Returns
  -------
  Set[int] (maybe None)
  """

  def inner_nod(g: "pyagrum.BayesNet", a: NodeId, b: NodeId) -> Optional[NodeSet]:
    if b == a:
      return set()

    inners = {a}
    children = g.children(a)
    if len(children) == 0:
      return None

    found = False
    for c in children:
      s = inner_nod(g, c, b)
      if s is not None:
        found = True
        inners |= s
    if found:
      return inners
    return None

  r = inner_nod(bn, x, y)
  if r:
    r.remove(x)

  return r


def backdoor_generator(bn: "pyagrum.BayesNet", cause: NodeId, effect: NodeId, not_bd: NodeSet = None):
  """
  Generates backdoor sets for the pair of nodes `(cause, effect)` in the graph `bn` excluding the nodes in the set
  `not_bd` (optional)

  Parameters
  ----------
  bn: pyagrum.BayesNet
  cause: int
  effect: int
  not_bd: Set[int] default=None

  Yields
  -------
  List[int]
    the different backdoors
  """
  if len(bn.parents(cause)) == 0:  # no parent of cause, no backdoor
    return
  if isParent(effect, cause, bn):  # causalDagFromBN(bn)):
    return

  if not_bd is None:
    not_bd = set()

  # simplifying the graph
  interest = {cause, effect}
  G = dSep_reduce(bn, interest)

  # removing the non connected in G without descendants
  # GG is a trash graph just to find the disjointed nodes in G
  GG = pyagrum.DiGraph(G)
  for i in descendants(bn, cause, set()):
    GG.eraseNode(i)

  # we only keep interesting connex components
  for nodes in GG.connectedComponents().values():
    if nodes.isdisjoint(interest):
      for n in nodes:
        G.eraseNode(n)

  possible = set(G.nodes()) - (descendants(bn, cause, set()) | interest | not_bd)
  if len(possible) == 0:
    return

  backdoors = set()

  for i in range(len(possible)):
    for subset in it.combinations(possible, i + 1):
      sub = frozenset(subset)
      worth_testing = True
      for s in backdoors:
        if s <= sub:
          worth_testing = False
      if worth_testing and isDSep_parents(G, {cause}, {effect}, sub):
        backdoors.add(sub)
        yield list(subset)


def frontdoor_generator(bn: "pyagrum.BayesNet", x: NodeId, y: NodeId, not_fd: NodeSet = None):
  """
  Generates frontdoor sets for the pair of nodes `(x, y)` in the graph `bn` excluding the nodes in the set
  `not_fd` (optional)

  Parameters
  ----------
  bn: pyagrum.BayesNet
  x: int
  y: int
  not_fd: Set[int] default=None

  Yields
  -------
  List[int]
    the different frontdoors
  """
  if isParent(x, y, bn):
    return

  if not_fd is None:
    not_fd = set()

  possible = nodes_on_dipath(bn, x, y)

  nodiPath = False
  if possible is None:
    nodiPath = True
    possible=None
    for _,cc in bn.connectedComponents().items():
      if x in cc and y in cc:
        possible=cc
    if possible is None:
      return
    possible -= {x, y}

  possible -= backdoor_reach(bn, x)
  possible -= not_fd
  impossible = set()
  g = dSep_reduce(bn, {x, y} | possible)
  for z in possible:
    if backdoor_path(g, {z}, {y}, {x}):
      impossible.add(z)
  possible -= impossible
  frontdoors = set()

  if nodiPath:
    for s in possible:
      yield [s]
    return

  for i in range(len(possible)):
    for subset in it.combinations(possible, i + 1):
      sub = frozenset(subset)
      worth_testing = True
      for s in frontdoors:
        if s <= sub:
          worth_testing = False
      if worth_testing and not exists_unblocked_directed_path(bn, x, y, sub):
        frontdoors.add(sub)
        yield list(subset)
