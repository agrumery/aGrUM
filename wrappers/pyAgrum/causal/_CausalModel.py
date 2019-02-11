# -*- coding: utf-8 -*-
#(c) Copyright by Pierre-Henri Wuillemin, UPMC, 2011  (pierre-henri.wuillemin@lip6.fr)

#Permission to use, copy, modify, and distribute this
#software and its documentation for any purpose and
#without fee or royalty is hereby granted, provided
#that the above copyright notice appear in all copies
#and that both that copyright notice and this permission
#notice appear in supporting documentation or portions
#thereof, including modifications, that you make.

#THE AUTHOR P.H. WUILLEMIN  DISCLAIMS ALL WARRANTIES
#WITH REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
#WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT
#SHALL THE AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT
#OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
#RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
#IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS
#ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
#OR PERFORMANCE OF THIS SOFTWARE!

"""
This file defines a representation for causal model
"""
from __future__ import annotations

import itertools as it

from ._types import *


class CausalModel:
  """
  From an observational BNs and the description of latent variables, this class represent a complet causal model
  obtained by adding the latent variables specified in ``latentVarsDescriptor`` to the Bayesian network ``bn``.

  :param bn: a observational bayesian network
  :param latentVarsDescriptor:  list of couples (<latent variable name>, <list of affected variables' ids>).
  :param keepArcs: By default, the arcs between variables affected by a common latent variable will be removed but
         this can be avoided by setting ``keepArcs`` to ``True``
  """

  def __init__(self, bn: gum.BayesNet,
               latentVarsDescriptor: LatentDescriptorList = None,
               keepArcs: bool = False):
    self.__observationalBN = bn

    if latentVarsDescriptor is None:
      latentVarsDescriptor = []

    firstIdForLatentVariable = max(bn.nodes()) + 1

    # we have to redefine those attributes since the __observationalBN may be augmented by latent variables
    self.__dag = gum.DAG()
    self.__names: Dict[NodeId, str] = {}
    self.__ids: Dict[str, NodeId] = {}

    # nodes of BN
    for n in bn.nodes():
      self.__dag.addNodeWithId(n)
      self.__names.update({n: self.bn().variable(n).name()})
      self.__ids.update({self.bn().variable(n).name(): n})

    # arcs on BN
    for x, y in bn.arcs():
      if keepArcs or (not _sameLatency(bn.variable(x).name(),
                                       bn.variable(y).name(),
                                       latentVarsDescriptor)):
        self.__dag.addArc(x, y)

    # latent variables and arcs from latent variables
    self.__lat: NodeSet = set()
    for i in range(len(latentVarsDescriptor)):
      n, ls = latentVarsDescriptor[i]
      self.__dag.addNodeWithId(firstIdForLatentVariable + i)
      self.__names.update({i + firstIdForLatentVariable: n})
      self.__ids.update({n: +firstIdForLatentVariable})
      self.__lat.add(firstIdForLatentVariable + i)

      for item in ls:
        j = bn.idFromName(item) if isinstance(item, str) else item
        self.__dag.addArc(i + firstIdForLatentVariable, j)

    self.__biArcs = {i for lt in self.__lat for i in it.combinations(self.children(lt), 2)}

  def bn(self) -> gum.BayesNet:
    """
    :return: the observational Bayesian network
    """
    return self.__observationalBN

  def parents(self, x: NodeId) -> NodeSet:
    """
    From a NodeId, returns its parent (as a set of NodeId)

    :param x: the node
    :return:
    """
    return self.__dag.parents(x)

  def children(self, x: NodeId) -> NodeSet:
    """
    :param x: the node
    :return: 
    """
    return self.__dag.children(x)

  def names(self) -> Dict[NodeId, str]:
    """
    :return: the map NodeId,Name
    """
    return self.__names

  def idFromName(self, name: str) -> NodeId:
    """
    :param name: the name of the variable
    :return: the id of the variable
    """
    return self.__ids[name]

  def biArcs(self) -> LatentDescriptorList:
    """
    :return: list of descriptor of latent variables
    """
    return self.__biArcs

  def latentVariablesIds(self) -> NodeSet:
    """
    :return: the set of ids of latent variables in the causal model
    """
    return self.__lat

  def eraseCausalArc(self, x, y):
    self.__dag.eraseArc(x, y)

  def addCausalArc(self, x, y):
    self.__dag.addArc(x, y)

  def existsArc(self, a, b) -> bool:
    return self.__dag.existsArc(a, b)

  def nodes(self):
    return self.__dag.nodes()

  def arcs(self) -> ArcSet:
    return self.__dag.arcs()


def inducedCausalSubModel(cm: CausalModel, sns: NodeSet = None) -> CausalModel:
  if sns is None:
    sns = cm.nodes()
  nodes = sns - cm.latentVariablesIds()

  bn = gum.BayesNet()

  for n in nodes:
    bn.add(cm.bn().variable(n), n)

  for x, y in cm.arcs():
    if y in nodes:
      if x in nodes:
        bn.addArc(x, y)

  names = cm.names()
  latentVarsDescriptor = list()
  for latentVar in cm.latentVariablesIds():
    if cm.children(latentVar).issubset(nodes):
      latentVarsDescriptor.append((names[latentVar],
                                   list(cm.children(latentVar))))

  return CausalModel(bn, latentVarsDescriptor, True)


def _sameLatency(xxx: NodeId, yyy: NodeId, latent_set: LatentDescriptorList) -> bool:
  for _, ll in latent_set:
    if xxx in ll:
      if yyy in ll:
        return True
  return False
