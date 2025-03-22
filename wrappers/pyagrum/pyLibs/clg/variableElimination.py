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
This module implements the inference algorithms for CLG.
"""
import math

import numpy as np
from pyagrum import JunctionTreeGenerator
from .canonicalForm import CanonicalForm
from .GaussianVariable import GaussianVariable

class CLGVariableElimination:

  def __init__(self, clg):
    self._clg = clg
    self._evidence = {}
    self._cf_dict = self._constructCanonicalForms()

  def updateEvidence(self, evidence):
    """
    Update evidences.

    Parameters
    ----------
    evidence : dict
      A dictionary of evidence.
    """
    self._evidence.update(evidence)

  def hasEvidence(self, variable):
    """
    Check if a variable has an evidence.

    Parameters
    ----------
    variable : str
      The variable name.

    Returns
    -------
    bool
      True if the variable has an evidence, False otherwise.
    """
    return variable in self._evidence

  def eraseEvidence(self, variable):
    """
    Remove the evidence corresponding to the variable name.

    Parameters
    ----------
    variable : str
      The variable name.
    """
    del self._evidence[variable]

  def eraseAllEvidence(self):
    """
    Remove all the evidences.

    Parameters
    ----------
    variable : str
      The variable name.
    """
    self._evidence = {}

  def nbrEvidence(self):
    """
    Returns the number of evidence.

    Returns
    -------
    int
      The number of evidence.
    """
    return len(self._evidence)

  def canonicalPosterior(self, variables, normalized=True):
    """
    Returns the posterior density as a canonical form.

    Parameters
    ----------
    variables : list
      The list of target variable names.
    normalized : bool, optional
      Optional parameter to normalize the returned canonical form.

    Returns
    -------
    CanonicalForm
      The posterior canonical form.
    """
    if type(variables) is not list:
      variables = [variables]

    for l in variables:
      if l in self._evidence:
        raise ValueError(f"The variable {l} is observed.")

    # Converting list of name to list of ids
    variables = [self._clg._name2id[v] for v in variables]

    # Finding the elimination order
    jtg = JunctionTreeGenerator()
    elimination_order = jtg.eliminationOrder(self._clg._graph)

    # Removing posterior variables from elimination order
    elimination_order_removed = []
    elimination_order_kept = []
    for var in elimination_order:
      if var not in variables:
        elimination_order_removed.append(var)
      else:
        elimination_order_kept.append(var)

    cf_list = self._sum_product_ve(elimination_order_removed,
                                   list(self._cf_dict.values()),
                                   self._evidence)
    posterior = np.prod(cf_list)
    if normalized:
      normalization_cf = self._sum_product_ve(elimination_order_kept,
                                              cf_list,
                                              {})
      posterior = posterior / np.prod(normalization_cf)
    return posterior

  def posterior(self, variable:str)->GaussianVariable:
    """
    Returns the posterior density as a Gaussian variable.

    Parameters
    ----------
    variable : str
      The target variable name.

    Returns
    -------
    GaussianVariable
      The posterior Gaussian variable.
    """
    if variable in self._evidence:
      return GaussianVariable(variable,self._evidence[variable],0)

    posterior_cf = self.canonicalPosterior([variable])
    _,t_mu,t_var= posterior_cf.toGaussian()

    return GaussianVariable(variable,t_mu[0][0],math.sqrt(t_var[0][0]))

  def _constructCanonicalForms(self):
    """
    Construct the canonical forms associated with the CLG.
    """
    cf_dict = {}
    for node in self._clg._graph.nodes():
      var = self._clg._id2var[node]
      parents = list(self._clg.parents(node))
      if len(parents) == 0:
        cf = CanonicalForm.fromCLG(node, [], var.mu(), var.sigma(), [])
      else:
        B = []
        for parent in parents:
          B.append(self._clg._arc2coef[(parent, node)])

        cf = CanonicalForm.fromCLG(node, parents, var.mu(), var.sigma(), B)
      cf_dict[node] = cf
    return cf_dict

  def _sum_product_ve(self, elimination_order, cf_list, evidence):
    """
    The variable elimination algorithm for CLG.

    Parameters
    ----------
    elimination_order : list
      The elimination order.
    cf_list : list
      A list of canonical forms.
    evidence : dict
      A dictionary of evidences.

    Returns
    -------
    list
      A list of canonical forms with variables in elimination_order eliminated.
    """

    # Converting from dict[str, float] to dict[NodeId, float]
    evidence = {self._clg._name2id[name]: evidence[name]
                for name in evidence.keys()}

    # Reducing the canonical forms containing observed variables
    if len(evidence) != 0:
      for (i, cf) in enumerate(cf_list):
        cf_list[i] = cf.reduce(evidence)

    if len(elimination_order) != 0:
      for variable in elimination_order:
        cf_list = self._sum_product_eliminate_var(cf_list, variable)

    return cf_list

  def _sum_product_eliminate_var(self, cf_list, variable):
    """
    Remove a variable from a set of canonical forms.

    Parameters
    ----------
    cf_list : list
      A list of canonical form.
    variable : int
      The variable id to eliminate.

    Returns
    -------
    list
      The list of canonical form with the variable removed.
    """
    contain_var_cfs = []  # CF containing the variable
    id_to_remove = []
    for (i, cf) in enumerate(cf_list):
      if variable in cf:
        contain_var_cfs.append(cf)
        id_to_remove.append(i)

    if len(contain_var_cfs) == 0:
      return cf_list

    for i in reversed(id_to_remove):
      cf_list.pop(i)
    cf_product = np.prod(contain_var_cfs)
    cf_marg = cf_product.marginalize([variable])
    cf_list.append(cf_marg)

    return cf_list
