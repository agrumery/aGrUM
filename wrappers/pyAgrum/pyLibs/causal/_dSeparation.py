# -*- coding: utf-8 -*-
# (c) Copyright 2020-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)

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
This file defines functions for dSeparation crtieria
"""
from typing import Set

import pyAgrum

from pyAgrum.causal._types import NodeId, DirectedModel, NodeSet

# pylint: disable=unused-import
import pyAgrum.causal  # for annotations


def isParent(a: NodeId, b: NodeId, g: "pyAgrum.BayesNet") -> bool:
  """
  Predicate on whether ``a`` is parent of ``b`` in the graph ``g``, the graph must be a DAG
  """
  return g.existsArc(a, b)


def ancester(x: NodeId, dm: DirectedModel, anc: NodeSet):
  """
  Adds the ancestors of ``x`` in the Bayesian network ``bn`` to the set ``P``
  """
  for parent in dm.parents(x):
    if parent not in anc:
      anc.add(parent)
      ancester(parent, dm, anc)


def _reduce_moralize(bn: "pyAgrum.BayesNet", x: NodeSet, y: NodeSet, zset: NodeSet) -> "pyAgrum.UndiGraph":
  """
  Returns the undirected graph obtained by reducing (ancestor graph) and moralizing the Bayesian network ``bn``

  Parameters
  ----------
  bn: pyAgrum.BayesNet
      the BayesNet
  x: Set[int|str]
      NodeSet generating the ancestor graph
  y: Set[int|str]
      Second NodeSet generating the ancestor graph
  zset: Set[int|str]
      Third NodeSet generating the ancestor graph

  Returns
  -------
  pyAgrum.UndiGraph
      The reduced moralized graph
  """
  G = pyAgrum.UndiGraph()

  Ancetre = x | y
  anc = frozenset(Ancetre)
  for i in anc:
    ancester(i, bn, Ancetre)

  for i in zset:
    Ancetre.add(i)
    ancester(i, bn, Ancetre)
  for i in Ancetre:
    G.addNodeWithId(i)

  for b in G.nodes():
    for a in bn.parents(b):
      G.addEdge(a, b)

  for nod in G.nodes():
    parents_nod = bn.parents(nod)
    for par in parents_nod:
      for par2 in parents_nod:
        if par2 != par:
          G.addEdge(par, par2)

  return G


def _removeZ(g_undi: "pyAgrum.UndiGraph", zset: NodeSet):
  for node in g_undi.nodes():
    if node in zset:
      g_undi.eraseNode(node)


def _is_path_x_y(g_undi: "pyAgrum.UndiGraph", sx: NodeSet, sy: NodeSet, marked: NodeSet = None) -> bool:
  """
  Predicate asserting the existence of a path between ``x`` and ``y`` in the non-oriented graph
  ``g_undi``, without going through the optional marking set ``mark``

  Parameters
  ----------
  g_undi: PyAgrum.UndiGraph
      The graph
  x: int
      first node
  y: int
      second node

  marked: Set[int]
      forbidden nodes

  :return:
  bool
      True if a path has been found
  """

  def inner_ec(g_und: "pyAgrum.UndiGraph", a: NodeId, b: NodeSet, m: NodeSet) -> bool:
    if a in b:
      return True

    m.add(a)

    for n in g_und.neighbours(a):
      if n not in m:
        if inner_ec(g_und, n, b, m):
          return True

    return False

  if len(sx) > len(sy):
    ssx, ssy = sy, sx
  else:
    ssx, ssy = sx, sy

  if marked is None:
    marked = set()

  ma = set(marked)
  for i in ssx:
    ma.add(i)
    if inner_ec(g_undi, i, ssy, ma):
      return True

  return False


def isDSep_tech2(bn: "pyAgrum.BayesNet", sx: NodeSet, sy: NodeSet, zset: NodeSet) -> bool:
  """
  Test of d-separation for ``x`` and ``y``, given ``zset`` using the graph-moralization method

  Parameters
  ----------
  bn: pyAgrum.BayesNet
      the bayesian network
  sx: Set[int]
      source nodes
  sy: Set[int]
      destinantion nodes
  zset: Set[int]
      blocking set

  Returns
  -------
  bool
      True if ``Z`` d-separates ``x`` and ``y``
  """
  g_undi = _reduce_moralize(bn, sx, sy, zset)

  _removeZ(g_undi, zset)

  if _is_path_x_y(g_undi, sx, sy):
    return False

  return True


def isDSep_parents(bn: "pyAgrum.BayesNet", sx: NodeSet, sy: NodeSet, zset: NodeSet) -> bool:
  """Test of d-separation of ``sx`` and ``sy`` given ``Z``, considering only the paths with an arc coming into ``x``
  using the graph-moralization method


  Parameters
  ----------
  bn: pyAgrum.BayesNet
      the bayesian network
  sx: Set[int]
      source nodes
  sy: Set[int]
      destinantion nodes
  zset: Set[int]
      blocking set

  Returns
  -------
  bool
      True if ``Z`` d-separates ``x`` and ``y``
  """
  return _isDSep_tech2_parents(bn, sx, sy, zset)


def _isDSep_tech2_parents(bn: "pyAgrum.BayesNet", sx: NodeSet, sy: NodeSet, zset: NodeSet) -> bool:
  """Test of d-separation of ``sx`` and ``sy`` given ``Z``, considering only the paths with an arc coming into ``x``
  using the graph-moralization method

  Parameters
  ----------
  bn: pyAgrum.BayesNet
      the bayesian network
  sx: Set[int]
      source nodes
  sy: Set[int]
      destinantion nodes
  zset: Set[int]
      blocking set

  Returns
  -------
  bool
  """
  G = pyAgrum.UndiGraph()
  ancesters = sx | sy
  anc = frozenset(ancesters)
  for i in anc:
    ancester(i, bn, ancesters)

  for i in zset:
    ancesters.add(i)
    ancester(i, bn, ancesters)
  for i in ancesters:
    G.addNodeWithId(i)

  for b in G.nodes():
    for a in (set(bn.parents(b)) - sx):
      G.addEdge(a, b)

  for nod in G.nodes():
    parents_nod = set(bn.parents(nod)) - sx
    for par in parents_nod:
      for par2 in parents_nod:
        if par2 != par:
          G.addEdge(par, par2)

  _removeZ(G, zset)

  if _is_path_x_y(G, sx, sy):
    return False

  return True


def _isDSep_tech2_children(bn: "pyAgrum.BayesNet", sx: NodeSet, sy: NodeSet, zset: NodeSet) -> bool:
  """Test of d-separation of ``x`` and ``y`` given ``zset``, considering only the paths with an arc coming from ``x``
  using the graph-moralization method

  Parameters
  ----------
  bn: pyAgrum.BayesNet
      the bayesian network
  sx: Set[int]
      source nodes
  sy: Set[int]
      destinantion nodes
  zset: Set[int]
      blocking set

  Returns
  -------
  bool
  """
  G = pyAgrum.UndiGraph()
  ancesters = sx | sy
  for i in sy:
    ancester(i, bn, ancesters)
  # sx's ancesters will not be added since sx already is in ancesters
  for i in zset:
    ancesters.add(i)
    ancester(i, bn, ancesters)
  for i in ancesters:
    G.addNodeWithId(i)
  se = set(G.nodes()) - sx
  for b in se:
    for a in bn.parents(b):
      G.addEdge(a, b)

  for nod in se:
    parents_nod = bn.parents(nod)
    for par in parents_nod:
      for par2 in parents_nod:
        if par2 != par:
          G.addEdge(par, par2)
  _removeZ(G, zset)

  if _is_path_x_y(G, sx, sy):
    return False

  return True


def _is_descendant(bn: "pyAgrum.BayesNet", x: NodeId, y: NodeId, marked: NodeSet = None) -> bool:
  """ Asserts whether or not ``x`` is a descendant of ``y`` in ``bn``"""

  if marked is None:
    marked = set()

  if isParent(y, x, bn):
    return True

  for c in bn.children(y):
    if c not in marked:
      marked.add(c)
      if _is_descendant(bn, x, c, marked):
        return True

  return False


def _is_ascendant(bn: "pyAgrum.BayesNet", x: NodeId, y: NodeId, marquage: Set[int] = None) -> bool:
  """Predicate on whether ``x`` is an ancestor of ``y`` in the Bayesian network ``bn``"""

  if isParent(x, y, bn):
    return True

  if marquage is None:
    marquage = set()

  for p in bn.parents(y):
    if p not in marquage:
      marquage.add(p)
      if _is_ascendant(bn, x, p, marquage):
        return True

  return False


def descendants(bn: "pyAgrum.BayesNet", x: NodeId, marked: NodeSet = None, ensdesc: NodeSet = None) -> NodeSet:
  """ Returns a set composed by all the descendents of ``x`` in ``bn`` """
  if marked is None:
    marked = set()
  if ensdesc is None:
    ensdesc = set()

  ensdesc = ensdesc | set(bn.children(x))

  for c in bn.children(x):
    if c not in marked:
      marked.add(c)
      ensdesc = ensdesc | descendants(bn, c, marked)

  return ensdesc


def _filaires(bn: DirectedModel, interest: NodeSet = None, inf: bool = True) -> NodeSet:
  s = set()

  if interest is None:
    interest = set()

  for x in bn.nodes():
    if len(set(bn.parents(x)) - s) == 0 and len(bn.children(x)) == 1 and x not in interest:
      a = x
      while True:
        s.add(a)
        for a in bn.children(a):
          break  # take the first elt
        if len(bn.children(a)) != 1 or len(set(bn.parents(a)) - s) != 0 or a in interest:
          break

    if inf and len(bn.children(x)) == 0 and len(bn.parents(x)) == 1 and x not in interest:
      a = x
      while True:
        s.add(a)
        for a in bn.parents(a):
          break  # take the first elt
        if len(bn.children(a)) != 1 or a in interest:
          break
        if len(bn.parents(a)) != 1:
          s.add(a)
          break
  return s


def _barren_nodes(bn: "pyAgrum.BayesNet", interest: NodeSet = None) -> NodeSet:
  """Returns the set of recursively determined barren nodes in ``bn`` relatively to the set of nodes ``interest`` (if
  ``interest`` is void, then the whole set of nodes in the graph will be returned)"""
  s = set()

  if interest is None:
    interest = set()

  def inner_rec(a):
    if a in interest | s:
      return
    s.add(a)
    for b in bn.parents(a):
      if len(set(bn.children(b)) - s) == 0 and (b not in s):
        inner_rec(b)

  for x in bn.nodes():
    if len(bn.children(x)) == 0:
      inner_rec(x)

  return s


def partialDAGFromBN(bn: "pyAgrum.BayesNet", Nexcl: NodeSet = None) -> "pyAgrum.DAG":
  """
  Creates and returns a duplicate DAG of the given Bayesian network

  Parameters
  ----------
  bn : pyAgrum.BayesNet
    the source
  Nexcl : NodeSet
    the nodes

  Returns
  -------
  pyAgrum.DAG
  """
  if Nexcl is None:
    Nexcl = set()
  d = pyAgrum.DAG()

  nodes = set(bn.nodes()) - (Nexcl)
  for n in nodes:
    d.addNodeWithId(n)

  for x, y in bn.arcs():
    if x in nodes and y in nodes:
      d.addArc(x, y)

  return d


def dSep_reduce(g: "pyAgrum.BayesNet", interest: NodeSet = None) -> "pyAgrum.DAG":
  """
  Reduce a BN by removing barren nodes w.r.t a set of nodes.

  Parameters
  ----------
  g : pyAgrum.BayesNet
    the source
  interest: NodeSet
    the nodes of interest

  Returns
  -------
  pyAgrum.DAG
    the reduced DAG
  """
  if interest is None:
    interest = set()

  barren = _barren_nodes(g, interest)

  reduced_g = partialDAGFromBN(g, barren)

  for f in _filaires(reduced_g, interest, False):
    reduced_g.eraseNode(f)

  return reduced_g


def _blocked(bn: "pyAgrum.BayesNet", pht: bool, x: NodeSet, y: NodeSet, setz: NodeSet,
             marquage0: Set[int],
             marquage1: Set[int]) -> bool:
  """
  internal method to check if a path is blocked
  """
  if x in y:
    return False

  isInxZ = x in setz
  wasIn = x in marquage0 or x in marquage1

  if pht:
    marquage1.add(x)
  else:
    marquage0.add(x)

  if not isInxZ and not wasIn:
    for c in bn.children(x):
      if c not in marquage1 and not _blocked(bn, True, c, y, setz, marquage0, marquage1):
        return False

  if pht:
    if isInxZ or len(setz & descendants(bn, x)) != 0:
      for p in bn.parents(x):
        if p not in marquage0 and not _blocked(bn, False, p, y, setz, marquage0, marquage1):
          return False

  else:
    if not isInxZ:
      for p in bn.parents(x):
        if p not in marquage0 and not _blocked(bn, False, p, y, setz, marquage0, marquage1):
          return False

  return True


def _isDSep_tech1_parents(bn: "pyAgrum.BayesNet", x: NodeId, sy: NodeSet, zset: NodeSet, reduced: bool = False) -> bool:
  """ Test of d-separation of ``x`` and ``y`` given ``Z``, considering only the paths with an arc coming into ``x``
  using the usual paths method"""

  if not reduced and len(bn.nodes()) > 170:
    g = dSep_reduce(bn, sy | zset | {x})
  else:
    g = bn

  for p in g.parents(x):
    if not _blocked(g, False, p, sy, zset, {x}, {x}):
      return False
  return True


def _isDSep_tech1_children(bn: "pyAgrum.BayesNet", x: NodeId, sy: NodeSet, setz: NodeSet, reduced=False) -> bool:
  """ Test of d-separation of ``x`` and ``y`` given ``Z``, considering only the paths with an arc coming from ``x``
  using the usual paths method"""

  if not reduced and len(bn.nodes()) > 170:
    g = dSep_reduce(bn, sy | setz | {x})
  else:
    g = bn

  for c in g.children(x):
    if not _blocked(g, True, c, sy, setz, {x}, {x}):
      return False
  return True


def isDSep_tech1(bn: "pyAgrum.BayesNet", sx: NodeSet, sy: NodeSet, setz: NodeSet, reduced=False) -> bool:
  """ Test of d-separation for ``x`` and ``y``, given ``Z`` using the usual paths method """

  if len(sx) > len(sy):
    sx, sy = sy, sx

  if not reduced and len(bn.nodes()) > 170:
    g = dSep_reduce(bn, sx | sy | setz)
  else:
    g = bn
  for i in sx:
    if not _isDSep_tech1_parents(g, i, sy, setz, True):
      return False
    if not _isDSep_tech1_children(g, i, sy, setz, True):
      return False
  return True


def isDSep(bn: "pyAgrum.BayesNet", x: NodeSet, y: NodeSet, z: NodeSet) -> bool:
  """
  Check if x and y are d-separated by z in the BN

  Parameters
  ----------
  bn: pyAgrum.BayesNet

  x : NodeSet

  y : NodeSet

  z : NodeSet

  Returns
  -------
  bool
    whether x and y are d-separated by z
  """
  return isDSep_tech2(bn, x, y, z)
