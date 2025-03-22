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
This file defines a representation for causal model
"""

import itertools as it
from typing import Union, Dict, Tuple

import pyagrum

from pyagrum.causal._types import LatentDescriptorList, NodeSet, NodeId, ArcSet, NameSet
from pyagrum.causal._doorCriteria import backdoor_generator, frontdoor_generator

# pylint: disable=unused-import
import pyagrum.causal  # for annotations


class CausalModel:
  """
  From an observational BNs and the description of latent variables, this class represent a complet causal model
  obtained by adding the latent variables specified in ``latentVarsDescriptor`` to the Bayesian network ``bn``.

  Parameters
  ----------
  bn: pyagrum.BayesNet
      an observational Bayesian network
  latentVarsDescriptor:  List[(str,List[int])]
      list of couples (<latent variable name>, <list of affected variables' ids>).
  keepArcs: bool
      By default, the arcs between variables affected by a common latent variable will be removed but this can be avoided by setting ``keepArcs`` to ``True``
  """

  def __init__(self, bn: "pyagrum.BayesNet",
               latentVarsDescriptor: LatentDescriptorList = None,
               keepArcs: bool = False):
    self.__observationalBN = bn
    self.__latentVarsDescriptor = latentVarsDescriptor
    self.__keepArcs = keepArcs

    if latentVarsDescriptor is None:
      latentVarsDescriptor = []

    # we have to redefine those attributes since the __observationalBN may be augmented by latent variables
    self.__causalBN = pyagrum.BayesNet()

    # nodes of BN
    for n in bn.nodes():
      self.__causalBN.add(bn.variable(n), n)

    # arcs on BN
    for x, y in bn.arcs():
      self.__causalBN.addArc(x, y)

    # latent variables and arcs from latent variables
    self.__lat: NodeSet = set()

    self.__names = {nId: self.__causalBN.variable(
      nId).name() for nId in self.__causalBN.nodes()}

    for n, ls in latentVarsDescriptor:
      self.addLatentVariable(n, ls, keepArcs)

  def clone(self) -> "pyagrum.causal.CausalModel":
    """
    Copy a causal model

    :return: the copy
    """
    return CausalModel(pyagrum.BayesNet(self.__observationalBN),
                       self.__latentVarsDescriptor,
                       self.__keepArcs)

  def addLatentVariable(self, name: str, lchild: Tuple[str, str], keepArcs: bool = False) -> None:
    """
    Add a new latent variable with a name, a tuple of children and replacing (or not) correlations between children.

    Parameters
    ----------
    name: str
        the name of the latent variable
    lchild: Tuple[str,str]
        the tuple of (2) children
    keepArcs: bool
        do wee keep (or not) the arc between the children ?
    """
    # simplest variable to add : only 2 modalities for latent variables
    id_latent = self.__causalBN.add(name, 2)
    self.__lat.add(id_latent)
    self.__names[id_latent] = name

    for item in lchild:
      j = self.__observationalBN.idFromName(
        item) if isinstance(item, str) else item
      self.addCausalArc(id_latent, j)

    if not keepArcs:
      ils = {self.__observationalBN.idFromName(x) for x in lchild}
      for ix, iy in it.combinations(ils, 2):
        if ix in self.__causalBN.parents(iy):
          self.eraseCausalArc(ix, iy)
        elif iy in self.__causalBN.parents(ix):
          self.eraseCausalArc(iy, ix)

  def toDot(self) -> str:
    """
    Create a dot representation of the causal model

    :return: the dot representation in a string
    """
    res = "digraph {"

    # latent variables
    if pyagrum.config.asBool['causal', 'show_latent_names']:
      shap = "ellipse"
    else:
      shap = "point"

    for n in self.nodes():
      if n in self.latentVariablesIds():
        res +=f'''
    "{self.names()[n]}" [fillcolor="{pyagrum.config['causal', 'default_node_bgcolor']}",
          fontcolor="{pyagrum.config['causal', 'default_node_fgcolor']}",
          style=filled,shape={shap}];
          
      '''

    # not latent variables
    for n in self.nodes():
      if n not in self.latentVariablesIds():
        res +=f'''
    "{self.names()[n]}" [fillcolor="{pyagrum.config['causal', 'default_node_bgcolor']}",
          fontcolor="{pyagrum.config['causal', 'default_node_fgcolor']}",
          style=filled,shape="ellipse"];
          
      '''

    for a, b in self.arcs():
      res += '   "' + self.names()[a] + '"->"' + self.names()[b] + '" '
      if a in self.latentVariablesIds() or b in self.latentVariablesIds():
        res += ' [style="dashed"];'
      else:
        black_color = pyagrum.config['notebook', 'default_arc_color']
        res += ' [color="' + black_color + ':' + black_color + '"];'
      res += "\n"

    res += "\n};"
    return res

  def causalBN(self) -> "pyagrum.BayesNet":
    """
    :return: the causal Bayesian network

    :warning: do not infer any computations in this model. It is strictly a structural model
    """
    return self.__causalBN

  def observationalBN(self) -> "pyagrum.BayesNet":
    """
    :return: the observational Bayesian network
    """
    return self.__observationalBN

  def connectedComponents(self) -> Dict[int,NodeSet]:
    """
    Return a map of connected components and their nodes.

    Returns
    -------
    Dict[int,NodeSet]:
      thedisc of connected components
    """
    return self.__causalBN.connectedComponents()

  def parents(self, x: Union[NodeId, str]) -> NodeSet:
    """
    From a NodeId, returns its parent (as a set of NodeId)

    Parameters
    ----------
    x : int
      the node

    Returns
    -------
    Set[int]
      the set of parents
    """
    return self.__causalBN.parents(self.__causalBN.idFromName(x) if isinstance(x, str) else x)

  def children(self, x: Union[NodeId, str]) -> NodeSet:
    """
    From a NodeId, returns its children (as a set of NodeId)

    Parameters
    ----------
    x : int
      the node

    Returns
    -------
    Set[int]
      the set of children
    """
    return self.__causalBN.children(self.__causalBN.idFromName(x) if isinstance(x, str) else x)

  def names(self) -> Dict[NodeId, str]:
    """
    Returns
    -------
    Dict[int,str]
      the map NodeId,Name
    """
    return self.__names

  def idFromName(self, name: str) -> NodeId:
    """

    Parameters
    ----------
    name: str
      the name of the variable

    Returns
    -------
    int
      the id of the variable
    """
    return self.__causalBN.idFromName(name)

  def latentVariablesIds(self) -> NodeSet:
    """
    Returns
    -------
    NodeSet
      the set of ids of latent variables in the causal model
    """
    return self.__lat

  def eraseCausalArc(self, x: Union[NodeId, str], y: Union[NodeId, str]) -> None:
    """
    Erase the arc x->y

    Parameters
    ----------
    x : int|str
      the nodeId or the name of the first node
    y : int|str
      the nodeId or the name of the second node
    """
    ix = self.__observationalBN.idFromName(x) if isinstance(x, str) else x
    iy = self.__observationalBN.idFromName(y) if isinstance(y, str) else y
    self.__causalBN.eraseArc(pyagrum.Arc(ix, iy))

  def addCausalArc(self, x: Union[NodeId, str], y: Union[NodeId, str]) -> None:
    """
    Add an arc x->y

    Parameters
    ----------
    x : int|str
      the nodeId or the name of the first node
    y : int|str
      the nodeId or the name of the second node
    """
    ix = self.__observationalBN.idFromName(x) if isinstance(x, str) else x
    iy = self.__observationalBN.idFromName(y) if isinstance(y, str) else y
    self.__causalBN.addArc(ix, iy)

  def existsArc(self, x: Union[NodeId, str], y: Union[NodeId, str]) -> bool:
    """
    Does the arc x->y exist ?

    Parameters
    ----------
    x : int|str
      the nodeId or the name of the first node
    y : int|str
      the nodeId or the name of the second node

    Returns
    -------
    bool
      True if the arc exists.
    """
    ix = self.__observationalBN.idFromName(x) if isinstance(x, str) else x
    iy = self.__observationalBN.idFromName(y) if isinstance(y, str) else y
    return self.__causalBN.dag().existsArc(ix, iy)

  def nodes(self) -> NodeSet:
    """
    :return: the set of nodes
    """

    return self.__causalBN.nodes()

  def arcs(self) -> ArcSet:
    """
    :return: the set of arcs
    """
    return self.__causalBN.arcs()

  def backDoor(self, cause: Union[NodeId, str], effect: Union[NodeId, str], withNames: bool = True) -> Union[
    None, NameSet, NodeSet]:
    """
    Check if a backdoor exists between `cause` and `effect`

    Parameters
    ----------
    cause: int|str
      the nodeId or the name of the cause
    effect: int|str
      the nodeId or the name of the effect
    withNames: bool
      wether we use ids (int) or names (str)

    Returns
    -------
    None|Set[str]|Set[int]
      None if no found backdoor. Otherwise return the found backdoors as set of ids or set of names.
    """
    icause = self.__observationalBN.idFromName(cause) if isinstance(cause, str) else cause
    ieffect = self.__observationalBN.idFromName(effect) if isinstance(effect, str) else effect

    for bd in backdoor_generator(self, icause, ieffect, self.latentVariablesIds()):
      if withNames:
        return {self.__observationalBN.variable(i).name() for i in bd}

      return bd

    return None

  def frontDoor(self, cause: Union[NodeId, str], effect: Union[NodeId, str], withNames: bool = True) -> Union[
    None, NameSet, NodeSet]:
    """
    Check if a frontdoor exists between cause and effet

    Parameters
    ----------
    cause: int|str
      the nodeId or the name of the cause
    effect: int|str
      the nodeId or the name of the effect
    withNames: bool
      wether we use ids (int) or names (str)

    Returns
    -------
    None|Set[str]|Set[int]
      None if no found frontdoot. Otherwise return the found frontdoors as set of ids or set of names.
    """
    icause = self.__observationalBN.idFromName(cause) if isinstance(cause, str) else cause
    ieffect = self.__observationalBN.idFromName(effect) if isinstance(effect, str) else effect

    for fd in frontdoor_generator(self, icause, ieffect, self.latentVariablesIds()):
      if withNames:
        return {self.__observationalBN.variable(i).name() for i in fd}

      return fd

    return None


def inducedCausalSubModel(cm: CausalModel, sns: NodeSet = None) -> CausalModel:
  """
  Create an causal model induced by a subset of nodes.

  Parameters
  ----------
  cm: CausalModel
    the causal model
  sns: Set[int]
    the set of nodes

  Returns
  -------
  CausalModel
    the induced sub-causal model
  """
  if sns is None:
    sns = cm.nodes()
  nodes = sns - cm.latentVariablesIds()

  bn = pyagrum.BayesNet()

  for n in nodes:
    bn.add(cm.observationalBN().variable(n), n)

  for x, y in cm.arcs():
    if y in nodes:
      if x in nodes:
        bn.addArc(x, y)

  names = cm.names()
  latentVarsDescriptor = []
  lats = cm.latentVariablesIds()
  for latentVar in lats:
    inters = cm.children(latentVar) & nodes
    if len(inters) > 0:
      latentVarsDescriptor.append((names[latentVar],
                                   list(inters)))

  return CausalModel(bn, latentVarsDescriptor, True)
