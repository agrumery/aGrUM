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
The purpose of this module is to store a Conditional Linear Gaussian (CLG) in a graph structure.
"""

import pyagrum
import random

from .GaussianVariable import GaussianVariable

import pyagrum.lib.bn_vs_bn as gcm

import numpy as np
from scipy.stats import norm
import pandas as pd


class CLG:
  _graph: pyagrum.DAG
  _id2var: dict[int, GaussianVariable]
  _arc2coef: dict[tuple[int, int], float | int]

  def __init__(self, clg: "CLG | None" = None) -> None:
    self._graph = pyagrum.DAG()
    self._id2var = {}
    self._arc2coef = {}

    if clg:
      self.copy(clg)

  def asDiscreteBN(self, domain: int = 2) -> pyagrum.BayesNet:
    """
    Return a BN with the same structure as the CLG. The variables of the BN are RangeVariable[domain].

    Parameters
    ----------
    domain : int
      The domain of the variables in the returned BN.

    Returns
    -------
    pyagrum.BayesNet
      A BN with the same structure as the CLG.

    Warning
    -------
    The returned BN is not a faithful representation of the CLG since the variables in the CLG are Gaussian and the variables in the returned BN are discrete. In particular, the CPTs are not defined in the returned BN.
    """
    bn = pyagrum.BayesNet()
    for name in self.names():
      bn.add(f"{name}[{domain}]")
    for arc in self.arcs():
      bn.addArc(self.variable(arc[0]).name(), self.variable(arc[1]).name())

    return bn

  def copy(self, clg: CLG) -> None:
    for node_id, variable in sorted(clg._id2var.items()):
      new_variable = GaussianVariable(name=variable.name(), mu=variable.mu(), sigma=variable.sigma())
      self._graph.addNodeWithId(node_id)
      self._graph.setName(node_id, variable.name())
      self._id2var[node_id] = new_variable

    for (parent, child), coef in clg._arc2coef.items():
      self._graph.addArc(parent, child)
      self._arc2coef[(parent, child)] = coef

  def __repr__(self):
    return str(self)

  def __getstate__(self) -> dict:
    return {'_id2var': self._id2var, '_arc2coef': self._arc2coef}

  def __setstate__(self, state: dict) -> None:
    self._graph = pyagrum.DAG()
    self._id2var = {}
    self._arc2coef = {}
    for node_id in sorted(state['_id2var']):
      var = state['_id2var'][node_id]
      self._graph.addNodeWithId(node_id)
      self._graph.setName(node_id, var.name())
      self._id2var[node_id] = var
    for (p, c), coef in state['_arc2coef'].items():
      self._graph.addArc(p, c)
      self._arc2coef[(p, c)] = coef

  def __eq__(self, other: object) -> bool:
    if not isinstance(other, CLG):
      return NotImplemented
    if self._graph.size() != other._graph.size() or self._graph.sizeArcs() != other._graph.sizeArcs():
      return False
    if set(self.names()) != set(other.names()):
      return False
    for name in self.names():
      v1, v2 = self.variable(name), other.variable(name)
      if v1.mu() != v2.mu() or v1.sigma() != v2.sigma():
        return False
    self_arcs = {
      (self._id2var[p].name(), self._id2var[c].name()): coef
      for (p, c), coef in self._arc2coef.items()
    }
    other_arcs = {
      (other._id2var[p].name(), other._id2var[c].name()): coef
      for (p, c), coef in other._arc2coef.items()
    }
    return self_arcs == other_arcs

  def add(self, var: GaussianVariable) -> int:
    """
    Add a new variable to the CLG.

    Parameters
    ----------
    var : GaussianVariable
      The variable to be added to the CLG.

    Returns
    -------
    int
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
    if self._graph.idFromName(var.name()) is not None:
      raise NameError(f"A variable with the same name ({var.name()}) already exists in this CLG.")

    n = self._graph.addNode()
    self._graph.setName(n, var.name())
    self._id2var[n] = var

    return n

  def setMu(self, node: int, mu: float) -> None:
    """
    Set the mean of a variable.

    Parameters
    ----------
    node : int
      The id of the variable.
    mu : float
      The new mean of the variable.

    Raises
    ------
    pyagrum.NotFound
      if the node is not found in the CLG.
    """
    self._id2var[node].setMu(mu)

  def setSigma(self, node: int, sigma: float) -> None:
    """
    Set the standard deviation of a variable.

    Parameters
    ----------
    node : int
      The id of the variable.
    sigma : float
      The new standard deviation of the variable.

    Raises
    ------
    pyagrum.NotFound
      if the node is not found in the CLG.
    """
    self._id2var[node].setSigma(sigma)

  def nameOrId(self, val: int | str) -> int:
    """
    Return the int from the name or the int.

    Parameters
    ----------
    val : str | int
      The name or the int of the variable.

    Returns
    -------
    int
      The int of the variable.
    """
    return val if isinstance(val, int) else self._graph.idFromName(val)

  def addArc(self, val1: int | str, val2: int | str, coef: float | int = 1) -> tuple[int, int]:
    """
    Add an arc val->val2 with a coefficient coef to the CLG.

    Parameters
    ----------
    val1 : str | int
      The name or the int of the parent variable.
    val2 : str | int
      The name or the int of the child variable.
    coef : float or int
      The coefficient of the arc.

    Returns
    -------
    tuple[int, int]
      The tuple of the NodeIds of the parent and the child variables.

    Raises
    ------
    pyagrum.NotFound
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

  def existsArc(self, val1: int | str, val2: int | str) -> bool:
    """
    Check if an arc val->val2 exists.

    Parameters
    ----------
    val1 : str | int
      The name or the int of the parent variable.
    val2 : str | int
      The name or the int of the child variable.

    Returns
    -------
    bool
      True if the arc exists.

    Raises
    ------
    pyagrum.NotFound
      if one of the names is not found in the CLG.
    """
    n1 = self.nameOrId(val1)
    n2 = self.nameOrId(val2)

    return self._graph.existsArc(n1, n2)

  def setCoef(self, val1: int | str, val2: int | str, coef: float | int) -> None:
    """
    Set the coefficient of an arc val1->val2.

    Parameters
    ----------
    val1 : str | int
      The name or the int of the parent variable.
    val2 : str | int
      The name or the int of the child variable.
    coef : float or int
      The new coefficient of the arc.

    Raises
    ------
    pyagrum.NotFound
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

  def dag(self) -> pyagrum.DAG:
    """
    Return the graph of the CLG (which is a DAG).

    Returns
    -------
    pyagrum.DAG
      The graph of the CLG.
    """
    return self._graph

  def eraseArc(self, val1: int, val2: int) -> None:
    """
    Erase the arc val->val2.
    """
    self._graph.eraseArc(val1, val2)
    del self._arc2coef[(val1, val2)]

  def name(self, node: int) -> str:
    """
    Return the associated name of the variable.

    Parameters
    ----------
    node : int
      The id of the variable.

    Returns
    -------
    str
      The associated name of the variable.

    Raises
    ------
    pyagrum.NotFound
      if the node is not found in the CLG.
    """
    return self._id2var[node].name()

  def idFromName(self, name: str) -> int:
    """
    Return the int from the name.

    Parameters
    ----------
    name : str
      The name of the variable.

    Returns
    -------
    int
      The int of the variable.

    Raises
    ------
    pyagrum.NotFound
      if the name is not found in the CLG.
    """
    nid = self._graph.idFromName(name)
    if nid is None:
      raise pyagrum.NotFound(f"'{name}' not found in the CLG.")
    return nid

  def variable(self, val: int | str) -> GaussianVariable:
    """
    Return the variable from the int or from the name.

    Parameters
    ----------
    val : str | int
      The name or the int of the variable.

    Returns
    -------
    GaussianVariable
      The variable.

    Raises
    ------
    pyagrum.NotFound
      if val is not Found in the CLG.
    """
    return self._id2var[self.nameOrId(val)]

  def variables(self) -> list[GaussianVariable]:
    """
    Return the list of the variables in the CLG.

    Returns
    -------
    list[GaussianVariable]
      The list of the variables in the CLG.
    """
    return [self.variable(i) for i in self.nodes()]

  def nodes(self) -> list[int]:
    """
    Return the list of NodeIds in the CLG.

    Returns
    -------
    list[int]
      The list of NodeIds in the CLG.
    """
    return list(self._id2var.keys())

  def names(self) -> list[str]:
    """
    Return the list of names in the CLG.

    Returns
    -------
    list[str]
      The list of names in the CLG.
    """
    return [self.variable(n).name() for n in self.nodes()]

  def arcs(self) -> set[tuple[int, int]]:
    """
    Return the set of arcs in the CLG.

    Returns
    -------
    set[tuple[int, int]]
      The set of arcs in the CLG.
    """
    return set(self._graph.arcs())

  def coefArc(self, val1: int | str, val2: int | str) -> float | int:
    """
    Return the coefficient of the arc val1->val2.

    Parameters
    ----------
    val1 : str | int
      The name or the int of the parent variable.
    val2 : str | int
      The name or the int of the child variable.

    Returns
    -------
    float
      The coefficient of the arc.

    Raises
    ------
    pyagrum.NotFound
      if one of the names is not found in the CLG.
    pyagrum.NotFound
      if the arc does not exist.
    """
    if not self.existsArc(val1, val2):
      raise pyagrum.NotFound("The arc does not exist.")

    n1 = self.nameOrId(val1)
    n2 = self.nameOrId(val2)

    return self._arc2coef[(n1, n2)]

  def parents(self, val: int | str) -> set[int]:
    """
    Return the list of parent ids from the name or the id of a node.

    Parameters
    ----------
    val : str | int
      The name or the int of the variable.

    Returns
    -------
    set[int]
      The set of parent nodes' ids.
    """
    return set(self._graph.parents(self.nameOrId(val)))

  def parent_names(self, val: int | str) -> list[str]:
    """
    Return the list of parents names from the name or the id of a node.

    Parameters
    ----------
    val : str | int
      The name or the int of the variable.

    Returns
    -------
    list[str]
      The list of val's parents' names.
    """
    return [self.name(n) for n in self.parents(val)]

  def children(self, val: int | str) -> set[int]:
    """
    Return the list of children ids from the name or the id of a node.

    Parameters
    ----------
    val : str | int
      The name or the int of the variable.

    Returns
    -------
    set[int]
      The set of children nodes' ids.
    """
    return set(self._graph.children(self.nameOrId(val)))

  def children_names(self, val: int | str) -> list[str]:
    """
    Return the list of children names from the name or the id of a node.

    Parameters
    ----------
    val : str | int
      The name or the int of the variable.

    Returns
    -------
    list[str]
      The list of val's children's names.
    """
    return [self.name(n) for n in self.children(val)]

  def topologicalOrder(self) -> list[int]:
    """
    Return the topological order of the CLG.

    Returns
    -------
    list[int]
      The list of NodeIds in the topological order.
    """
    return self._graph.topologicalOrder()

  def dag2dict(self) -> dict[int, set[int]]:
    """
    Return a dictionary representing the DAG of the CLG.

    Returns
    -------
    C : dict[int, set[int]]
      A directed graph DAG representing the causal structure.
    """
    V = self.nodes()
    C: dict[int, set[int]] = {v: set() for v in V}  # C is shown by a Adjacency List

    for node in V:
      for child in self.children(node):
        C[node].add(child)

    return C

  def logLikelihood(self, data: str) -> float:
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
    n_samples = len(next(iter(id2samples.values())))
    for i in range(n_samples):
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

  def structuralFScore(self, other: "CLG") -> float:
    """
    Compare the structure of two CLGs using the F-score.

    Parameters
    ----------
    other : CLG
      The CLG to compare with.

    Returns
    -------
    float
      The F-score of the structural comparison (1.0 = identical structure).
    """
    cmp = gcm.GraphicalBNComparator(self.asDiscreteBN(), other.asDiscreteBN())
    return cmp.scores()["fscore"]


  def _build_canonical_forms(self) -> dict[int, "CanonicalForm"]:
    from .canonicalForm import CanonicalForm
    cf_dict = {}
    for node in self._graph.nodes():
      var = self._id2var[node]
      parents = list(self.parents(node))
      if len(parents) == 0:
        cf = CanonicalForm.fromCLG(node, [], var.mu(), var.sigma(), [])
      else:
        B = [self._arc2coef[(parent, node)] for parent in parents]
        cf = CanonicalForm.fromCLG(node, parents, var.mu(), var.sigma(), B)
      cf_dict[node] = cf
    return cf_dict

  def __str__(self) -> str:
    from . import SEM
    return SEM.tosem(self)


def randomCLG(
   nb_variables: int,
   names: list[str],
   max_parents: int | None = None,
   ratio_arc: float = 1.2,
   MuMin: float = -5,
   MuMax: float = 5,
   SigmaMin: float = 1,
   SigmaMax: float = 10,
   ArcCoefMin: float = 1,
   ArcCoefMax: float = 10,
) -> CLG:
  """
  Generate a random CLG with ``nb_variables`` variables.

  Parameters
  ----------
  nb_variables : int
    Number of variables. Must be >= 4.
  names : list[str]
    Names of the variables. Must satisfy ``len(names) == nb_variables``.
  max_parents : int or None
    Maximum number of parents per node. ``None`` means no constraint.
  ratio_arc : float
    Target number of arcs expressed as a multiple of ``nb_variables``
    (passed to :func:`pyagrum.randomBN`). Must be > 0.
  MuMin : float
    Lower bound for the uniform draw of each node's mean. Must be <= ``MuMax``.
  MuMax : float
    Upper bound for the uniform draw of each node's mean.
  SigmaMin : float
    Lower bound for the uniform draw of each node's std deviation.
    Must be > 0 and <= ``SigmaMax``.
  SigmaMax : float
    Upper bound for the uniform draw of each node's std deviation.
  ArcCoefMin : float
    Minimum absolute value of arc coefficients. Must be > 0 and <= ``ArcCoefMax``.
    Coefficients are drawn uniformly from ``[-ArcCoefMax, -ArcCoefMin] ∪ [ArcCoefMin, ArcCoefMax]``.
  ArcCoefMax : float
    Maximum absolute value of arc coefficients.

  Returns
  -------
  CLG
    A random CLG.

  Raises
  ------
  ValueError
    If any parameter violates its constraint.
  """
  if len(names) != nb_variables:
    raise ValueError(f"len(names) ({len(names)}) must equal nb_variables ({nb_variables}).")
  if nb_variables < 4:
    raise ValueError(f"nb_variables must be >= 4, got {nb_variables}.")
  if ratio_arc <= 0:
    raise ValueError(f"ratio_arc must be strictly positive, got {ratio_arc}.")
  if max_parents is not None and max_parents < 1:
    raise ValueError(f"max_parents must be >= 1, got {max_parents}.")
  if MuMin > MuMax:
    raise ValueError(f"MuMin ({MuMin}) must be <= MuMax ({MuMax}).")
  if SigmaMin <= 0:
    raise ValueError(f"SigmaMin must be strictly positive, got {SigmaMin}.")
  if SigmaMin > SigmaMax:
    raise ValueError(f"SigmaMin ({SigmaMin}) must be <= SigmaMax ({SigmaMax}).")
  if ArcCoefMin <= 0:
    raise ValueError(f"ArcCoefMin must be strictly positive, got {ArcCoefMin}.")
  if ArcCoefMin > ArcCoefMax:
    raise ValueError(f"ArcCoefMin ({ArcCoefMin}) must be <= ArcCoefMax ({ArcCoefMax}).")

  bn = pyagrum.randomBN(names=names, n=nb_variables, ratio_arc=ratio_arc)

  # Order names by their NodeIds in the generated BN
  ordered_names = [""] * nb_variables
  for name in bn.names():
    ordered_names[bn.idFromName(name)] = name

  clg = CLG()
  for name in ordered_names:
    clg.add(
      GaussianVariable(
        name=name,
        mu=random.uniform(MuMin, MuMax),
        sigma=random.uniform(SigmaMin, SigmaMax),
      )
    )

  # Build arc list, trimming excess parents per node when max_parents is set
  if max_parents is not None:
    arcs_by_child: dict[int, list[int]] = {}
    for arc in bn.arcs():
      arcs_by_child.setdefault(arc[1], []).append(arc[0])
    arcs_to_add: list[tuple[int, int]] = []
    for child, parents in arcs_by_child.items():
      if len(parents) > max_parents:
        parents = random.sample(parents, max_parents)
      for parent in parents:
        arcs_to_add.append((parent, child))
  else:
    arcs_to_add = list(bn.arcs())

  for arc in arcs_to_add:
    clg.addArc(
      val1=arc[0],
      val2=arc[1],
      coef=random.uniform(-ArcCoefMax, -ArcCoefMin)
      if random.random() < 0.5
      else random.uniform(ArcCoefMin, ArcCoefMax),
    )

  return clg
