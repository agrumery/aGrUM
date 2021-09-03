# -*- coding: utf-8 -*-
# (c) Copyright by Pierre-Henri Wuillemin (LIP6), 2020  (pierre-henri.wuillemin@lip6.fr)

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
This files define some routines for front and back door
"""

from typing import Iterator, Optional
import itertools as it

import pyAgrum

from pyAgrum.causal._types import NameSet, NodeId, NodeSet, NodeList
from pyAgrum.causal._dSeparation import isParent, dSep_reduce, descendants, isDSep_parents

# pylint: disable=unused-import
import pyAgrum.causal  # for annotations

def backdoor_path(bn: "pyAgrum.BayesNet", x: str, y: str, zset: NameSet = None)->bool:
  """
  Predicate on the existence of an open back door path from ``x`` to ``y``,
  conditioning on the set of variables ``zset``

  :param bn: the DAG model
  :param x: name of source node
  :param y: name of destination node
  :param zset: names of conditioning nodes
  :return: True if such backdoor exists
  """
  if zset is None:
    zset = set()
  return not isDSep_parents(bn, x, y, zset)


def exists_unblocked_directed_path(bn: "pyAgrum.BayesNet", x: str, y: str, zset: NameSet = None) -> bool:
  """
  Predicate on the existence of a directed path from ``x`` to ``y`` in the Bayesian network ``bn``
  not blocked by nodes of ``zset``

  :param bn:
  :param x:
  :param y:
  :param zset:
  :return:
  """
  if zset is None:
    zset = set()
  if y in bn.children(x):
    return True

  for c in bn.children(x):
    if (c not in zset) and exists_unblocked_directed_path(bn, c, y, zset):
      return True
  return False


def is_frontdoor(bn: "pyAgrum.BayesNet", x: str, y: str, zset: NameSet) -> bool:
  """
  Tests whether or not ``zset`` satisifies the front door criterion for ``x`` and ``y``, in the Bayesian network ``bn``

  :param bn:
  :param x:
  :param y:
  :param zset:
  :return:
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


def is_backdoor(bn: "pyAgrum.BayesNet", x: str, y: str, zset: NameSet) -> bool:
  """
  Tests whether or not ``zset`` satisifies the back door criterion for ``x`` and ``y``, in the Bayesian network ``bn``

  :param bn:
  :param x:
  :param y:
  :param zset:
  :return:
  """

  desc_x = descendants(bn, x)
  if len(desc_x & zset) != 0:
    return False

  return isDSep_parents(bn, x, y, zset)


def backdoor_reach(bn: "pyAgrum.BayesNet", a: NodeId) -> NodeSet:
  """
  Returns the set of nodes that can be reached through a backdoor path from ``a`` in the graph ``bn``

  :param bn:
  :param a:
  :return:
  """

  def inner_br(bn: "pyAgrum.BayesNet", x: NodeId, pht: bool, reach0: NodeSet, reach1: NodeSet):
    for c in bn.children(x):
      if c not in reach0 and c not in reach1:
        reach1.add(c)
        inner_br(bn, c, True, reach0, reach1)

    if not pht:
      for p in bn.parents(x):
        if p not in reach0:
          reach0.add(p)
          inner_br(bn, p, False, reach0, reach1)

  r = set([a])
  r.union(bn.parents(a))
  l = set([a])
  for pa in bn.parents(a):
    inner_br(bn, pa, False, r, l)
  s = r | l
  if a in s:
    s.remove(a)
  return s


def nodes_on_dipath(bn: "pyAgrum.BayesNet", x: NodeId, y: NodeId) -> Optional[NodeSet]:
  """
  Returns the set of nodes through which there is a directed path from ``x`` to ``y`` in the graph ``bn``

  :param bn:
  :param x:
  :param y:
  :return:
  """

  def inner_nod(g: "pyAgrum.BayesNet", a: NodeId, b: NodeId) -> Optional[NodeSet]:
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


def backdoor_generator(bn: "pyAgrum.BayesNet", cause: NodeId, effect: NodeId, not_bd: NodeSet = None) -> Iterator[
  NodeList]:
  """
  Generates backdoor sets for the pair of nodes ``(x, y)`` in the graph ``bn`` excluding the nodes in the set
  ``not_bd`` (optional)

  :param bn:
  :param cause:
  :param effect:
  :param not_bd:
  :return:
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
  GG = pyAgrum.DiGraph(G)
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


def frontdoor_generator(bn: "pyAgrum.BayesNet", x: NodeId, y: NodeId, not_fd: NodeSet = None) -> Iterator[NodeList]:
  """
  Generates frontdoor sets for the pair of nodes ``(x, y)`` in the graph ``bn`` excluding the nodes in the set
  ``not_fd`` (optional)

  :param bn:
  :param x:
  :param y:
  :param not_fd:
  :return:
  """
  if isParent(x, y, bn):
    return

  if not_fd is None:
    not_fd = set()

  possible = nodes_on_dipath(bn, x, y)
  nodiPath = False
  if possible is None:
    nodiPath = True
    possible = set(bn.nodes()) - {x, y}
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
