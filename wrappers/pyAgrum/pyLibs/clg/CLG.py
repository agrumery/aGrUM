# -*- coding: utf-8 -*-
# (c) Copyright 2022-2023 by Pierre-Henri Wuillemin(@LIP6)  (pierre-henri.wuillemin@lip6.fr)
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
The purpose of this module is to store a Conditional Linear Gaussian (CLG) in a graph structure.
"""

import pyAgrum as gum
from typing import Dict, Tuple
import random

from .constants import NodeId
from .GaussianVariable import GaussianVariable

import pyAgrum.lib.bn_vs_bn as gcm

import numpy as np
from scipy.stats import norm
import pandas as pd


class CLG:
  _graph: gum.DAG
  _id2var: Dict[NodeId, GaussianVariable]
  _name2id: Dict[str, NodeId]
  _arc2coef: Dict[Tuple[NodeId, NodeId], float or int]

  def __init__(self, clg=None):
    self._graph = gum.DAG()
    self._id2var = {}
    self._name2id = {}
    self._arc2coef = {}

    if clg:
      self.copy(clg)

  def copy(self, clg):
    for _, variable in clg._id2var.items():
      new_variable = GaussianVariable(name=variable.name(), mu=variable.mu(), sigma=variable.sigma())
      self.add(new_variable)

    for (parent, child), coef in clg._arc2coef.items():
      self.addArc(parent, child, coef)

  def __str__(self):
    return (f"CLG{{nodes: {len(self.nodes())}, "
            f"arcs: {len(self.arcs())}, "
            f"parameters: {len(self.arcs()) + 2 * len(self.nodes())}}}")

  def __repr__(self):
    return str(self)

  def add(self, var):
    """
    Add a new variable to the CLG.

    Parameters
    ----------
    var : GaussianVariable
      The variable to be added to the CLG.

    Returns
    -------
    NodeId
      The id of the added variable.

    Raises
    ------
    ValueError
      if the argument is None.

    NameError
      if the name of the variable is empty.

    NameError
      if a variable with the same name already exists in the CLG.
    """
    if var is None:
      raise ValueError("The argument cannot be None.")
    if var.name() == "":
      raise NameError(f"The name cannot be '{var.name()}'.")
    if var.name() in self._name2id:
      raise NameError(f"A variable with the same name ({var.name()}) already exists in this CLG.")

    n = NodeId(self._graph.addNode())
    self._id2var[n] = var
    self._name2id[var.name()] = n

    return n

  def setMu(self, node, mu):
    """
    Set the mean of a variable.

    Parameters
    ----------
    node : NodeId
      The id of the variable.
    mu : float
      The new mean of the variable.

    Raises
    ------
    gum.NotFound
      if the node is not found in the CLG.
    """
    self._id2var[node].setMu(mu)

  def setSigma(self, node, sigma):
    """
    Set the standard deviation of a variable.

    Parameters
    ----------
    node : NodeId
      The id of the variable.
    sigma : float
      The new standard deviation of the variable.

    Raises
    ------
    gum.NotFound
      if the node is not found in the CLG.
    """
    self._id2var[node].setSigma(sigma)

  def nameOrId(self, val):
    """
    Return the NodeId from the name or the NodeId.

    Parameters
    ----------
    val : NameOrId
      The name or the NodeId of the variable.

    Returns
    -------
    NodeId
      The NodeId of the variable.
    """
    return val if isinstance(val, int) else self._name2id[val]

  def addArc(self, val1, val2, coef=1):
    """
    Add an arc val->val2 with a coefficient coef to the CLG.

    Parameters
    ----------
    val1 : NameOrId
      The name or the NodeId of the parent variable.
    val2 : NameOrId
      The name or the NodeId of the child variable.
    coef : float or int
      The coefficient of the arc.

    Returns
    -------
    Tuple[NodeId, NodeId]
      The tuple of the NodeIds of the parent and the child variables.

    Raises
    ------
    gum.NotFound
      if one of the names is not found in the CLG.
    ValueError
      if the coefficient is 0.
    """
    if coef == 0:
      raise ValueError("The coefficient of an arc cannot be 0.")

    n1 = self.nameOrId(val1)
    n2 = self.nameOrId(val2)

    self._graph.addArc(n1, n2)
    self._arc2coef[(n1, n2)] = coef

    return (n1, n2)

  def existsArc(self, val1, val2):
    """
    Check if an arc val->val2 exists.

    Parameters
    ----------
    val1 : NameOrId
      The name or the NodeId of the parent variable.
    val2 : NameOrId
      The name or the NodeId of the child variable.

    Returns
    -------
    bool
      True if the arc exists.

    Raises
    ------
    gum.NotFound
      if one of the names is not found in the CLG.
    """
    n1 = self.nameOrId(val1)
    n2 = self.nameOrId(val2)

    return self._graph.existsArc(n1, n2)

  def setCoef(self, val1, val2, coef):
    """
    Set the coefficient of an arc val1->val2.

    Parameters
    ----------
    val1 : NameOrId
      The name or the NodeId of the parent variable.
    val2 : NameOrId
      The name or the NodeId of the child variable.
    coef : float or int
      The new coefficient of the arc.

    Raises
    ------
    gum.NotFound
      if one of the names is not found in the CLG.
    ValueError
      if the coefficient is 0.
    ValueError
      if the arc does not exist.
    """
    if coef == 0:
      raise ValueError("The coefficient of an arc cannot be 0.")

    if not self.existsArc(val1, val2):
      raise ValueError("The arc does not exist.")

    n1 = self.nameOrId(val1)
    n2 = self.nameOrId(val2)

    self._arc2coef[(n1, n2)] = coef

  def dag(self):
    """
    Return the graph of the CLG (which is a DAG).

    Returns
    -------
    gum.DAG
      The graph of the CLG.
    """
    return self._graph

  def eraseArc(self, val1, val2):
    """
    Erase the arc val->val2.
    """
    self._graph.eraseArc(val1, val2)

  def name(self, node):
    """
    Return the associated name of the variable.

    Parameters
    ----------
    node : NodeId
      The id of the variable.

    Returns
    -------
    str
      The associated name of the variable.

    Raises
    ------
    gum.NotFound
      if the node is not found in the CLG.
    """
    return self._id2var[node].name()

  def idFromName(self, name):
    """
    Return the NodeId from the name.

    Parameters
    ----------
    name : str
      The name of the variable.

    Returns
    -------
    NodeId
      The NodeId of the variable.

    Raises
    ------
    gum.NotFound
      if the name is not found in the CLG.
    """
    return self._name2id[name]

  def variable(self, val):
    """
    Return the variable from the NodeId or from the name.

    Parameters
    ----------
    val : NameOrId
      The name or the NodeId of the variable.

    Returns
    -------
    GaussianVariable
      The variable.

    Raises
    ------
    gum.NotFound
      if val is not Found in the CLG.
    """
    return self._id2var[self.nameOrId(val)]

  def variables(self):
    """
    Return the list of the variables in the CLG.

    Returns
    -------
    List[GaussianVariable]
      The list of the variables in the CLG.
    """
    return [self.variable(i) for i in self.nodes()]

  def nodes(self):
    """
    Return the list of NodeIds in the CLG.

    Returns
    -------
    List[NodeId]
      The list of NodeIds in the CLG.
    """
    return list(self._id2var.keys())

  def names(self):
    """
    Return the list of names in the CLG.

    Returns
    -------
    List[str]
      The list of names in the CLG.
    """
    return list(self._name2id.keys())

  def arcs(self):
    """
    Return the list of arcs in the CLG.

    Returns
    -------
    List[Tuple[NodeId, NodeId]]
      The list of arcs in the CLG.
    """
    return self._graph.arcs()

  def coefArc(self, val1, val2):
    """
    Return the coefficient of the arc val1->val2.

    Parameters
    ----------
    val1 : NameOrId
      The name or the NodeId of the parent variable.
    val2 : NameOrId
      The name or the NodeId of the child variable.

    Returns
    -------
    float
      The coefficient of the arc.

    Raises
    ------
    pyAgrum.NotFound
      if one of the names is not found in the CLG.
    pyAgrum.NotFound
      if the arc does not exist.
    """
    if not self.existsArc(val1, val2):
      raise gum.NotFound("The arc does not exist.")

    n1 = self.nameOrId(val1)
    n2 = self.nameOrId(val2)

    return self._arc2coef[(n1, n2)]

  def parents(self, val):
    """
    Return the list of parent ids from the name or the id of a node.

    Parameters
    ----------
    val : NameOrId
      The name or the NodeId of the variable.

    Returns
    -------
    Set[NodeId]
      The set of parent nodes' ids.
    """
    return self._graph.parents(self.nameOrId(val))

  def parent_names(self, val):
    """
    Return the list of parents names from the name or the id of a node.

    Parameters
    ----------
    val : NameOrId
      The name or the NodeId of the variable.

    Returns
    -------
    List[str]
      The list of val's parents' names.
    """
    return [self.name(n) for n in self.parents(val)]

  def children(self, val):
    """
    Return the list of children ids from the name or the id of a node.

    Parameters
    ----------
    val : NameOrId
      The name or the NodeId of the variable.

    Returns
    -------
    Set[NodeId]
      The set of children nodes' ids.
    """
    return self._graph.children(self.nameOrId(val))

  def children_names(self, val):
    """
    Return the list of children names from the name or the id of a node.

    Parameters
    ----------
    val : NameOrId
      The name or the NodeId of the variable.

    Returns
    -------
    List[str]
      The list of val's children's names.
    """
    return [self.name(n) for n in self.children(val)]

  def topologicalOrder(self):
    """
    Return the topological order of the CLG.

    Returns
    -------
    List[NodeId]
      The list of NodeIds in the topological order.
    """
    return self._graph.topologicalOrder()

  def dag2dict(self):
    """
    Return a dictionary representing the DAG of the CLG.

    Returns
    -------
    C : Dict[NodeId, Set[NodeId]]
      A directed graph DAG representing the causal structure.
    """
    V = self.nodes()
    C = {v: set() for v in V}  # C is shown by a Adjacency List

    for node in V:
      for child in self.children(node):
        C[node].add(child)

    return C

  def logLikelihood(self, data):
    """
    Return the log-likelihood of the data.

    Parameters
    ----------
    data : csv file
      The data.

    Returns
    -------
    float
      The log-likelihood of the data for the CLG.
    """
    id2samples = {}
    log_likelihood = 0

    # collect the samples to a dict whose keys are NodeID
    df = pd.read_csv(data)
    for node in self.nodes():
      id2samples[node] = df[self.name(node)].tolist()

    # for each sample, compute the pdf of each node
    for i in range(len(id2samples[0])):
      joint_pdf = 1
      for node in self.nodes():
        # compute the real mu of node considering its parents
        loc_mu = self._id2var[node].mu()
        for parent in self.parents(node):
          loc_mu += id2samples[parent][i] * self._arc2coef[(parent, node)]

        # use norm.pdf to compute the pdf of each node
        joint_pdf *= norm(loc_mu, self._id2var[node].sigma()).pdf(id2samples[node][i])

      log_likelihood += np.log(joint_pdf)

    return log_likelihood


    return clg

  def CompareStructure(self, clg_to_compare):
    """
    We use the f-score to compare the causal structure of the two CLGs.
    We create two BNs with the same structure as the two CLGs and then compare the two BNs.

    Parameters
    ----------
    clg_to_compare : CLG
      The CLG to compare with.

    Returns
    -------
    float
      The f-score of the comparison.
    """
    # Create a BN with the same structure as the CLG
    bn = gum.BayesNet()
    # add variables
    for name in self.names():
      new_variable = gum.LabelizedVariable(name, 'a labelized variable', 2)
      bn.add(new_variable)
    # add arcs
    for arc in self.arcs():
      bn.addArc(arc[0], arc[1])

    # Create a BN with the same structure as the clg_to_compare
    bn_to_compare = gum.BayesNet()
    # add variables
    for name in clg_to_compare.names():
      new_variable = gum.LabelizedVariable(name, 'a labelized variable', 2)
      bn_to_compare.add(new_variable)
    # add arcs and edges
    for arc in clg_to_compare.arcs():
      bn_to_compare.addArc(arc[0], arc[1])

    # We use the f-score to compare graphs of the two created BNs
    cmp = gcm.GraphicalBNComparator(bn, bn_to_compare)

    return cmp.scores()['fscore']

def randomCLG(nb_variables, names, MuMax=5, MuMin=-5, SigmaMax=10, SigmaMin=1, ArcCoefMax=10, ArcCoefMin=5):
  """
  This function generates a random CLG with nb_variables variables.

  Parameters
  ----------
  nb_variables : int
    The number of variables in the CLG.
  names : str
    The list of names of the variables.
  MuMax : float
    The maximum value of mu.
  MuMin : float
    The minimum value of mu.
  SigmaMax : float
    The maximum value of sigma.
  SigmaMin : float
    The minimum value of sigma.
  ArcCoefMax : float
    The maximum value of the coefficient of the arc.
  ArcCoefMin : float
    The minimum value of the coefficient of the arc.

  Returns
  -------
  CLG
    The random CLG.
  """
  # Create a random BN with nb_variables variables
  bn = gum.randomBN(names=names, n=nb_variables)

  # Order names by their NodeIds
  ordered_names = [''] * nb_variables
  for name in bn.names():
    ordered_names[bn.idFromName(name)] = name

  # Create a random CLG with nb_variables variables(The CLG is created with the same structure as the BN)
  clg = CLG()
  # Add variables
  for name in ordered_names:
    new_variable = GaussianVariable(name=name, mu=random.uniform(MuMin, MuMax),
                                    sigma=random.uniform(SigmaMin, SigmaMax))
    clg.add(new_variable)
  # Add arcs
  for arc in bn.arcs():
    clg.addArc(val1=arc[0], val2=arc[1],
               coef=random.uniform(-1 * ArcCoefMax, -1 * ArcCoefMin) if random.random() < 0.5 else random.uniform(
                 ArcCoefMin, ArcCoefMax))

  return clg
